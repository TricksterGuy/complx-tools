#include "ComplxFrame.hpp"
#include "data/PropertyTypes.hpp"

#include <sstream>

#include <wx/filedlg.h>
#include <wx/numdlg.h>
#include <wx/valnum.h>

#include <logging.hpp>

namespace
{

wxString AskForAssemblyFile()
{
    std::unique_ptr<wxFileDialog> dialog(new wxFileDialog(nullptr, _("Load .asm file"), wxEmptyString, wxEmptyString, _("LC-3 Assembly Files (*.asm)|*.asm"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR));
    if (dialog->ShowModal() == wxID_OK)
        return dialog->GetPath();
    return wxEmptyString;
}

}

ComplxFrame::ComplxFrame() : ComplxFrameDecl(nullptr), state(new lc3_state()), memory_view_model(new MemoryViewDataModel(std::ref(*state))), timer(this, wxID_ANY)
{
    EventLog l(__func__);

    InitializeMenus();
    InitializeOutput();
    InitializeLC3State();
    InitializeMemoryView();
    InitializeStatePropGrid();
    Connect(timer.GetId(), wxEVT_TIMER, wxTimerEventHandler(ComplxFrame::OnTimer), nullptr, this);
}

ComplxFrame::~ComplxFrame()
{
    EventLog l(__func__);
    logger->SetLogTarget(std::cerr);
}

void ComplxFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    DoExit();
}

void ComplxFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    DoExit();
}

void ComplxFrame::DoExit()
{
    timer.Stop();
    Disconnect(timer.GetId(), wxEVT_TIMER, wxTimerEventHandler(ComplxFrame::OnTimer), nullptr, this);
    Destroy();
}

void ComplxFrame::InitializeMenus()
{
    for (auto i = 0U; i < cycle_speed_menu_items.size(); i++)
    {
        auto id = cycle_speed_menu_items.size() - i - 1;
        auto menu_id = ID_CYCLE_SPEED + id;
        cycle_speed_menu_items[id] = menuCycleSpeed->PrependRadioItem(menu_id, wxString::Format("%d Instruction%s", 1 << id, id == 0 ? "" : "s"));
        menuCycleSpeed->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ComplxFrame::OnCycleSpeed), this, menu_id);
    }
}

void ComplxFrame::InitializeLC3State()
{
    state->default_seed = time(nullptr);

    InfoLog("Random Seed %u", state->default_seed);

    // TODO change back to false, false
    lc3_init(*state, false, false, 0x1000, 0x1000);

    state->output = output.get();
    state->warning = warning.get();
    //state->trace = trace.get();
    state->reader = std::bind(&ComplxFrame::ConsoleRead, this, std::placeholders::_1, std::placeholders::_2);
    state->peek = std::bind(&ComplxFrame::ConsolePeek, this, std::placeholders::_1, std::placeholders::_2);
}

void ComplxFrame::InitializeMemoryView()
{
    memoryView->UpdateRef(std::ref(*state));
    memoryView->AssociateModel(memory_view_model.get());
    memoryView->ScrollTo(state->pc);
}

void ComplxFrame::InitializeStatePropGrid()
{
    // Setup PC Property.
    pc_property = new RegisterProperty("PC", std::ref(reinterpret_cast<int16_t&>(state->pc)), RegisterProperty::Hexadecimal, RegisterProperty::NoBaseProperty | RegisterProperty::AllowHexadecimal);
    statePropGrid->Append(pc_property);
    // xFFFF
    statePropGrid->SetPropertyMaxLength(pc_property, 5);

    // Setup CC property.
    cc_property = new ProcessStatusRegisterProperty(std::ref(*state), ProcessStatusRegisterProperty::DisplayAsCC);
    statePropGrid->Append(cc_property);
    statePropGrid->SetPropertyMaxLength(cc_property, cc_property->GetPropertyMaximumLength());

    for (unsigned int i = 0; i < 8; i++)
    {
        auto base = i < 5 ? RegisterProperty::Decimal : RegisterProperty::Hexadecimal;

        auto* property = new RegisterProperty(wxString::Format("R%d", i), std::ref(state->regs[i]), base);
        statePropGrid->Append(property);
        // -32768 is 6 characters.
        statePropGrid->SetPropertyMaxLength(property, 6);
        statePropGrid->Collapse(property);

        register_properties[i] = property;
    }

    statePropGridManager->GetGrid()->CenterSplitter();
}

void ComplxFrame::InitializeOutput()
{
    // TODO Buffer the output, uses up a lot of CPU on verbose mode.
    output = std::make_unique<std::ostream>(consoleText);
    warning = std::make_unique<std::ostream>(warningText);
    //trace = std::make_unique<std::ostream>(traceText);
    logging = std::make_unique<std::ostream>(loggingText);

    // Copy initial logs to the logging textctrl. And switch to logging to the textctrl.
    const auto& old = dynamic_cast<const std::stringstream&>(logger->GetLogTarget());
    *logging << (old.str());
    logger->SetLogTarget(*logging);
}

void ComplxFrame::OnLoad(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    if (execution)
    {
        WarnLog("Currently executing instructions, please stop execution first.");
        return;
    }

    wxString file = AskForAssemblyFile();
    if (file.IsEmpty())
    {
        WarnLog("No file loaded.");
        return;
    }

    InfoLog("File selected: %s", static_cast<const char*>(file));

    LoadingOptions options;
    options.file = file;

    // Success/failure logs handled in DoLoadFile.
    if (DoLoadFile(options))
        reload_options = options;
}

void ComplxFrame::OnReload(wxCommandEvent& event)
{
    EventLog l(__func__);
    if (execution)
    {
        WarnLog("Currently executing instructions, please stop execution first.");
        return;
    }

    if (reload_options.file.IsEmpty())
    {
        OnLoad(event);
        return;
    }

    InfoLog("Reloading: %s", static_cast<const char*>(reload_options.file));
    DoLoadFile(reload_options);
}

bool ComplxFrame::DoLoadFile(const LoadingOptions& opts)
{
    std::unique_ptr<lc3_state> new_state(new lc3_state());

    wxFileName filename(opts.file);
    bool randomize_registers = opts.registers == RANDOMIZE;
    bool randomize_memory = opts.memory == RANDOMIZE;
    int16_t fill_registers = opts.registers;
    int16_t fill_memory = opts.memory;

    new_state->default_seed = (opts.has_random_seed) ? opts.random_seed : state->default_seed;
    lc3_init(*new_state, randomize_registers, randomize_memory, fill_registers, fill_memory);
    new_state->pc = opts.pc;

    try
    {
        std::vector<code_range> ranges;
        LC3AssembleOptions options;
        options.multiple_errors = true;
        options.warnings_as_errors = false;
        options.process_debug_comments = true;
        options.enable_warnings = false;
        options.disable_plugins = false;
        lc3_assemble(*new_state, filename.GetFullPath().ToStdString(), ranges, options);
    }
    catch (const LC3AssembleException& e)
    {
        WarnLog("Assembling file %s failed. Reason: %s", static_cast<const char*>(filename.GetFullName()), e.what());
        return false;
    }

    InfoLog("Successfully loaded file: %s", static_cast<const char*>(opts.file));

    /// TODO should really have an lc3_destroy function.
    // Without this plugins will be dangling.
    lc3_init(*state);

    state = std::move(new_state);
    PostLoadFile();
    reload_options.file_modification_time = filename.GetModificationTime();

    return true;
}

void ComplxFrame::PostLoadFile()
{
    memoryView->UpdateRef(std::ref(*state));
    cc_property->UpdateRef(std::ref(*state));
    pc_property->UpdateRef(std::ref(reinterpret_cast<int16_t&>(state->pc)));
    for (unsigned int i = 0; i < 8; i++)
        register_properties[i]->UpdateRef(std::ref(state->regs[i]));
    memoryView->Refresh();
    memoryView->ScrollTo(state->pc);
}

void ComplxFrame::OnLogLevel(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);

    if (menuViewLogLevelVerbose->IsChecked())
        logger->SetLogLevel(LogLevel::VERBOSE);
    if (menuViewLogLevelInfo->IsChecked())
        logger->SetLogLevel(LogLevel::INFO);
    if (menuViewLogLevelWarning->IsChecked())
        logger->SetLogLevel(LogLevel::WARNING);
    if (menuViewLogLevelDebug->IsChecked())
        logger->SetLogLevel(LogLevel::DEBUG);
    if (menuViewLogLevelFatal->IsChecked())
        logger->SetLogLevel(LogLevel::FATAL);

    InfoLog("Log level set to %d", logger->GetLogLevel());
}

void ComplxFrame::OnCycleSpeed(wxCommandEvent& event)
{
    EventLog l(__func__);
    auto speed = event.GetId() - ID_CYCLE_SPEED;
    InfoLog("Setting instructions per second to %d", 1 << speed);

    if (execution)
    {
        VerboseLog("Updating current execution speed");
        execution->options.ips = 1 << speed;
    }
}

void ComplxFrame::OnCycleSpeedCustom(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    auto speed = wxGetNumberFromUser("Enter new instructions per second (1-1000000)", wxEmptyString, "Set Custom Instruction Cycle Speed", custom_ips, 1, 1000000, this);
    if (speed == -1)
    {
        WarnLog("User canceled dialog, not setting speed.");
        return;
    }
    InfoLog("Setting instructions per second to %d", speed);
    custom_ips = speed;

    if (execution)
    {
        VerboseLog("Updating current execution speed");
        execution->options.ips = custom_ips;
    }
}

void ComplxFrame::OnStep(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    InfoLog("Stepping 1 instruction.");
    PreExecute();
    Execute(RunMode::STEP, 1);
}

void ComplxFrame::OnBack(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    InfoLog("Stepping back 1 instruction.");
    PreExecute();
    Execute(RunMode::BACK, 1);
}

void ComplxFrame::OnRun(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    InfoLog("Running until HALT.");
    PreExecute();
    Execute(RunMode::RUN, -1);
}

void ComplxFrame::OnStepOver(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    InfoLog("Stepping over Subroutines/Traps.");
    PreExecute();
    Execute(RunMode::NEXT_LINE, -1);
}

void ComplxFrame::OnBackOver(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    InfoLog("Stepping back over Subroutines/Traps.");
    PreExecute();
    Execute(RunMode::PREV_LINE, -1);
}

void ComplxFrame::OnStepOut(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    InfoLog("Stepping out of current Subroutine/Trap.");
    PreExecute();
    Execute(RunMode::FINISH, -1, 1);
}

void ComplxFrame::OnRewind(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    InfoLog("Rewinding to start of program.");
    PreExecute();
    Execute(RunMode::REWIND, -1);
}

void ComplxFrame::OnStateChange(wxPropertyGridEvent& event)
{
    EventLog l(__func__);
    auto* property = event.GetProperty();
    auto property_type = reinterpret_cast<uintptr_t>(property->GetClientData());

    if (property_type == PropertyType::Register)
    {
        auto* register_property = dynamic_cast<RegisterProperty*>(property);
        wxASSERT(register_property);
        register_property->UpdateRegisterValue();
    }

    if (property_type == PropertyType::RegisterDisplayBase)
    {
        auto* register_property = dynamic_cast<RegisterProperty*>(property->GetParent());
        wxASSERT(register_property);
        register_property->UpdateDisplayBase();
    }

    // This means they changed the value of the PC.
    if (property == pc_property)
        memoryView->Refresh();

    if (property == cc_property)
        cc_property->UpdateRegisterValue();
}

void ComplxFrame::PreExecute()
{
    TransferDataFromWindow();

    if (execution)
    {
        WarnLog("Canceling previous execution command.");
        EndExecution();
    }
}

void ComplxFrame::Execute(RunMode mode, long instructions, int depth)
{
    if (execution)
        DFatalLog("Called execute twice? this shouldn't happen");

    state->halted = false;

    ExecuteOptions opts;
    opts.mode = mode;
    opts.instructions = instructions;
    opts.fps = 60;
    opts.ips = GetIps();

    execution = ExecutionInfo(opts);
    execution->depth = depth;
    timer.Start(1000 / opts.fps);
    watch.Start();
}

long ComplxFrame::GetIps() const
{
    long speed = custom_ips;
    long item_speed = 1;
    for (const auto& item : cycle_speed_menu_items)
    {
        if (item->IsChecked())
            speed = item_speed;
        item_speed = item_speed << 1;
    }
    return speed;
}

void ComplxFrame::PostExecute()
{
    memoryView->Refresh();
    memoryView->ScrollTo(state->pc);

    for (auto& property : register_properties)
        property->RefreshDisplayedValue();
    pc_property->RefreshDisplayedValue();
    cc_property->RefreshDisplayedValue();

    for (auto* plugin : state->plugins)
        plugin->Refresh(*state);
}

void ComplxFrame::EndExecution()
{
    EventLog l(__func__);
    execution = std::nullopt;
    PostExecute();
    timer.Stop();

    InfoLog("Finished previous execution command");
}

int ComplxFrame::ConsoleRead(lc3_state& state, std::istream&)
{
    if (consoleInput.IsEmpty())
    {
        lc3_warning(state, LC3_OUT_OF_INPUT, 0);
        state.pc--;
        state.halted = true;
        return -1;
    }
    int ret = consoleInput[0];
    consoleInput = consoleInput.Mid(1);
    InfoLog("Read Character %c value: %d", static_cast<char>(ret), ret);
    TransferDataToWindow();
    return ret;
}

int ComplxFrame::ConsolePeek(lc3_state& state, std::istream&)
{
    if (consoleInput.IsEmpty())
    {
        lc3_warning(state, LC3_OUT_OF_INPUT, 0);
        state.pc--;
        state.halted = true;
        return -1;
    }
    VerboseLog("Peek Character %c value: %d", static_cast<char>(consoleInput[0]), consoleInput[0]);
    return consoleInput[0];
}

void ComplxFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    EventLog l(__func__);

    if (!execution)
        return;

    watch.Pause();

    auto time_elapsed_ms = watch.Time();
    unsigned long icount = static_cast<unsigned long>(execution->count);
    double execute = execution->options.ips * time_elapsed_ms / 1000.0;
    execution->count = std::min(execution->count + execute, static_cast<double>(execution->options.instructions));
    unsigned long fcount = static_cast<unsigned long>(execution->count);
    bool ended = false;

    if (fcount > icount)
    {
        unsigned long to_execute = fcount - icount;

        VerboseLog("Running %d instructions", to_execute);

        switch (execution->options.mode)
        {
            case RunMode::STEP:
                [[fallthrough]];
            case RunMode::RUN:
                [[fallthrough]];
            case RunMode::RUNX:
                lc3_run(*state, to_execute);
                ended = state->halted;
                break;

            case RunMode::BACK:
                [[fallthrough]];
            case RunMode::REWIND:
                lc3_rewind(*state, to_execute);
                ended = state->undo_stack.empty();
                break;

            case RunMode::FINISH:
                [[fallthrough]];
            case RunMode::NEXT_LINE:
                execution->depth = lc3_next_line(*state, to_execute, execution->depth);
                ended = state->halted || execution->depth == -1;
                break;

            case RunMode::PREV_LINE:
                execution->depth = lc3_prev_line(*state, to_execute, execution->depth);
                ended = state->undo_stack.empty() || execution->depth == -1;
                break;

            default:
                WarnLog("Invalid run mode %s set... Ending Execution", execution->options.mode);
                EndExecution();
                return;
        }
    }

    // Halted or matched number of instructions or finished next/prev line / finish.
    if (ended || (execution && execution->count == execution->options.instructions))
    {
        EndExecution();
        return;
    }

    watch.Start();
    PostExecute();
}
