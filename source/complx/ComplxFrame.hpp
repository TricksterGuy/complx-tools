#ifndef COMPLX_FRAME_HPP
#define COMPLX_FRAME_HPP

#include <array>
#include <optional>
#include <vector>

#include <lc3.hpp>

#include "ExecuteOptions.hpp"
#include "LoadingOptions.hpp"
#include "MemoryView.hpp"
#include "MemoryViewFrame.hpp"
#include "data/MemoryViewDataModel.hpp"
#include "data/RegisterProperty.hpp"
#include "data/ProcessStatusRegisterProperty.hpp"
#include "gen/ComplxFrameDecl.h"
#include <wx/stopwatch.h>
#include <wx/timer.h>

#define ID_CYCLE_SPEED 6000

class ComplxFrame : public ComplxFrameDecl
{
public:
    ComplxFrame();
    ~ComplxFrame() override;

    void OnClose(wxCloseEvent& event) override;
    void OnExit(wxCommandEvent& event) override;

    // File Menu Event Handlers
    void OnLoad(wxCommandEvent& event) override;
    void OnReload(wxCommandEvent& event) override;
    void OnAdvancedLoad(wxCommandEvent& event) override;

    // View Menu Event Handlers
    void OnNewView(wxCommandEvent& event) override;
    void OnDestroyView(wxCloseEvent& event);
    void OnLogLevel(wxCommandEvent& event) override;

    // Control Menu Event Handlers
    void OnCycleSpeed(wxCommandEvent& event);
    void OnCycleSpeedCustom(wxCommandEvent& event) override;
    void OnStep(wxCommandEvent& event) override;
    void OnBack(wxCommandEvent& event) override;
	void OnRun(wxCommandEvent& event) override;
	void OnStepOver(wxCommandEvent& event) override;
	void OnBackOver(wxCommandEvent& event) override;
    void OnStepOut(wxCommandEvent& event) override;
	void OnRewind(wxCommandEvent& event) override;
	void OnStop(wxCommandEvent& event) override;

    // State Event Handling
    void OnStateChange(wxPropertyGridEvent& event) override;

    // Misc Event Handlers
    // Actually handles executing instructions.
    void OnTimer(wxTimerEvent& event);

private:
    // Initialization
    void InitializeMenus();
    void InitializeLC3State();
    void InitializeMemoryView();
    void InitializeStatePropGrid();
    void InitializeStatusBar();
    void InitializeOutput();

    void DoExit();

    /** Do the work of assembling a file. */
    bool DoLoadFile(const LoadingOptions& opts);
    /** Updates all objects referring to the now stale lc3_state object */
    void PostLoadFile();

    void CancelRunningExecution();
    /** Called to read data from textctrls before executing instructions. */
    void PreExecute();
    /** Called to setup execution of instructions */
    void Execute(RunMode mode, long instructions, int depth = 0);
    /** Called when the display needs to be updated after executing instructions. */
    void PostExecute();
    /** Called when execution is over */
    void EndExecution();

    /** Console input functions */
    int ConsoleRead(lc3_state& state, std::istream& file);
    int ConsolePeek(lc3_state& state, std::istream& file);

    /** Get Instructions per second */
    long GetIps() const;

    std::unique_ptr<lc3_state> state;

    std::list<MemoryViewFrame*> memory_views;

    /** Options used when reloading assembly files */
    LoadingOptions reload_options;
    /** Current execution info*/
    std::optional<ExecutionInfo> execution;

    /** Backing data for Memory View */
    wxObjectDataPtr<MemoryViewDataModel> memory_view_model;

    /** Extra Menu options */
    std::array<wxMenuItem*, 17> cycle_speed_menu_items;

    /** State Property Grid Properties */
    RegisterProperty* pc_property;
    ProcessStatusRegisterProperty* cc_property;
    std::array<RegisterProperty*, 8> register_properties;

    /** Custom Instructions Per Second set */
    long custom_ips = 1024;

    /** Streams for output */
    std::unique_ptr<std::ostream> output;
    std::unique_ptr<std::ostream> warning;
    std::unique_ptr<std::ostream> trace;
    std::unique_ptr<std::ostream> logging;

    wxStopWatch watch;
    wxTimer timer;
};


#endif
