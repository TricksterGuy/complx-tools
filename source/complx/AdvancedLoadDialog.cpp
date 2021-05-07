#include "AdvancedLoadDialog.hpp"
#include "util/ValidationHelper.hpp"
#include <lc3.hpp>
#include <logging.hpp>

AdvancedLoadDialog::AdvancedLoadDialog(wxWindow* parent, const LoadingOptions& opts) : AdvancedLoadDialogDecl(parent)
{
    assemblyFileCtrl->SetPath(opts.file);
	replayString = opts.replay_string;
	consoleInput = opts.console_input;
	regInitializer = opts.registers == RANDOMIZE ? 0 : (opts.registers == ZEROED ? 1 : 2);
	regFill = wxString::Format("x%04x", static_cast<unsigned short>(opts.registers));
	memInitializer = opts.memory == RANDOMIZE ? 0 : (opts.memory == ZEROED ? 1 : 2);
	memFill = wxString::Format("x%04x", static_cast<unsigned short>(opts.memory));
	pc = wxString::Format("x%04x", opts.pc);
	trueTraps = opts.true_traps;
	interrupts = opts.interrupts;
	strictExecution = opts.strict_execution;

    auto* validator = dynamic_cast<wxTextValidator*>(replayStringCtrl->GetValidator());
    wxASSERT(validator);
    validator->SetCharIncludes(Base64Chars);
#ifndef ENABLE_LC3_REPLAY
    replayStringCtrl->Enable(false);
    replayStringCtrl->SetValue("");
#endif

    if (opts.registers != RANDOMIZE && opts.registers != ZEROED)
        regFillCtrl->Enable();
    validator = dynamic_cast<wxTextValidator*>(regFillCtrl->GetValidator());
    wxASSERT(validator);
    validator->SetCharIncludes(IntegralChars);

    if (opts.memory != RANDOMIZE && opts.memory != ZEROED)
        memFillCtrl->Enable();
    validator = dynamic_cast<wxTextValidator*>(memFillCtrl->GetValidator());
    wxASSERT(validator);
    validator->SetCharIncludes(IntegralChars);

    validator = dynamic_cast<wxTextValidator*>(pcCtrl->GetValidator());
    wxASSERT(validator);
    validator->SetCharIncludes(HexadecimalChars);
}

LoadingOptions AdvancedLoadDialog::GetOptions()
{
    LoadingOptions options;

    options.file = assemblyFileCtrl->GetPath().ToStdString();
#ifdef ENABLE_LC3_REPLAY
    options.replay_string = replayString.ToStdString();
#endif
    options.console_input = consoleInput.ToStdString();


    switch(regInitializer)
    {
        case 0:
            options.registers = RANDOMIZE;
            break;
        case 1:
            options.registers = ZEROED;
            break;
        case 2:
            options.registers = static_cast<ValueInitializationMethod>(ParseIntegralValueOrDie(regFill));
            break;
        default:
            WarnLog("Unknown register initialization option given %d", regInitializer);
            options.registers = RANDOMIZE;
            break;
    }

    switch(memInitializer)
    {
        case 0:
            options.memory = RANDOMIZE;
            break;
        case 1:
            options.memory = ZEROED;
            break;
        case 2:
            options.memory = static_cast<ValueInitializationMethod>(ParseIntegralValueOrDie(memFill));
            break;
        default:
            WarnLog("Unknown memory initialization option given %d", regInitializer);
            options.memory = RANDOMIZE;
            break;
    }

    options.pc = ParseIntegralValueOrDie(pc);
    options.true_traps = trueTraps;
    options.interrupts = interrupts;
    options.strict_execution = strictExecution;

    return options;
}

bool AdvancedLoadDialog::Validate()
{
    if (!AdvancedLoadDialogDecl::Validate())
        return false;

    TransferDataFromWindow();

    if (assemblyFileCtrl->GetPath().IsEmpty())
        return false;
    if (!(regInitializer == RANDOMIZE && regInitializer == ZEROED) && !ValidateHexValue(regFill) && !ValidateDecimalValue(regFill))
        return false;
    if (!(memInitializer == RANDOMIZE && memInitializer == ZEROED) && !ValidateHexValue(memFill) && !ValidateDecimalValue(memFill))
        return false;
    if (!pc.IsEmpty() && !ValidateHexValue(pc))
        return false;
    if (!replayString.IsEmpty() && !ValidateBase64Value(replayString))
        return false;
    return true;
}

void AdvancedLoadDialog::OnRegChoice(wxCommandEvent& event)
{
    regFillCtrl->Enable(event.GetSelection() == 2);
}

void AdvancedLoadDialog::OnMemChoice(wxCommandEvent& event)
{
    memFillCtrl->Enable(event.GetSelection() == 2);
}
