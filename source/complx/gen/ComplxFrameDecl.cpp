///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "../MemoryView.hpp"

#include "ComplxFrameDecl.h"

///////////////////////////////////////////////////////////////////////////

ComplxFrameDecl::ComplxFrameDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetExtraStyle( wxWS_EX_PROCESS_IDLE|wxWS_EX_VALIDATE_RECURSIVELY );
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);

	menuBar = new wxMenuBar( 0 );
	menuFile = new wxMenu();
	wxMenuItem* menuFileLoad;
	menuFileLoad = new wxMenuItem( menuFile, wxID_OPEN, wxString( wxT("&Load") ) + wxT('\t') + wxT("Ctrl+O"), wxT("Loads an assembly file"), wxITEM_NORMAL );
	menuFile->Append( menuFileLoad );

	wxMenuItem* menuFileReload;
	menuFileReload = new wxMenuItem( menuFile, wxID_REFRESH, wxString( wxT("&Reload") ) + wxT('\t') + wxT("Ctrl+R"), wxT("Reloads current asm file under same conditions"), wxITEM_NORMAL );
	menuFile->Append( menuFileReload );

	wxMenuItem* menuFileAdvancedLoad;
	menuFileAdvancedLoad = new wxMenuItem( menuFile, ID_ADVANCED_LOAD, wxString( wxT("&Advanced Load...") ) , wxT("Opens dialog for loading program under special conditions."), wxITEM_NORMAL );
	menuFile->Append( menuFileAdvancedLoad );

	wxMenuItem* menuFileExit;
	menuFileExit = new wxMenuItem( menuFile, wxID_EXIT, wxString( wxT("&Exit") ) + wxT('\t') + wxT("Alt+F4"), wxT("Exits the Program"), wxITEM_NORMAL );
	menuFile->Append( menuFileExit );

	menuBar->Append( menuFile, wxT("File") );

	menuView = new wxMenu();
	wxMenuItem* menuViewNew;
	menuViewNew = new wxMenuItem( menuView, ID_NEW_VIEW, wxString( wxT("&New View") ) + wxT('\t') + wxT("Ctrl+V"), wxT("Shows a new view of main memory."), wxITEM_NORMAL );
	menuView->Append( menuViewNew );

	wxMenuItem* menuViewGoto;
	menuViewGoto = new wxMenuItem( menuView, ID_GOTO_ADDRESS, wxString( wxT("&Goto Address") ) + wxT('\t') + wxT("Ctrl+G"), wxT("Moves memory to a specific address."), wxITEM_NORMAL );
	menuView->Append( menuViewGoto );

	menuViewShowAddresses = new wxMenu();
	wxMenuItem* menuViewShowAddressesItem = new wxMenuItem( menuView, wxID_ANY, wxT("Hide Addresses"), wxEmptyString, wxITEM_NORMAL, menuViewShowAddresses );
	menuViewShowAddressesShowAll = new wxMenuItem( menuViewShowAddresses, ID_SHOW_ALL, wxString( wxT("Show All") ) , wxT("Shows all memory addresses"), wxITEM_RADIO );
	menuViewShowAddresses->Append( menuViewShowAddressesShowAll );
	menuViewShowAddressesShowAll->Check( true );

	menuViewShowAddressesShowNonZero = new wxMenuItem( menuViewShowAddresses, ID_SHOW_NON_ZERO, wxString( wxT("Show Non Zero") ) , wxT("Show all memory addresses that have a value other than 0"), wxITEM_RADIO );
	menuViewShowAddresses->Append( menuViewShowAddressesShowNonZero );

	menuViewShowAddressesShowOnlyCodeData = new wxMenuItem( menuViewShowAddresses, ID_SHOW_ONLY_CODEDATA, wxString( wxT("Show Only Code/Data") ) , wxT("Shows addresses modified when your program was loaded."), wxITEM_RADIO );
	menuViewShowAddresses->Append( menuViewShowAddressesShowOnlyCodeData );

	menuViewShowAddressesCustom = new wxMenuItem( menuViewShowAddresses, ID_CUSTOM, wxString( wxT("Custom...") ) + wxT('\t') + wxT("Ctrl+H"), wxT("Allows you to show/hide memory address ranges"), wxITEM_RADIO );
	menuViewShowAddresses->Append( menuViewShowAddressesCustom );

	menuView->Append( menuViewShowAddressesItem );

	menuViewDisassemble = new wxMenu();
	wxMenuItem* menuViewDisassembleItem = new wxMenuItem( menuView, wxID_ANY, wxT("Disassemble"), wxEmptyString, wxITEM_NORMAL, menuViewDisassemble );
	menuViewBasic = new wxMenuItem( menuViewDisassemble, ID_BASIC, wxString( wxT("Basic") ) , wxT("Disassembles ignoring symbols"), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewBasic );

	menuViewNormal = new wxMenuItem( menuViewDisassemble, ID_NORMAL, wxString( wxT("Normal") ) , wxT("Disassembles with symbol info."), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewNormal );
	menuViewNormal->Check( true );

	menuViewHighLevel = new wxMenuItem( menuViewDisassemble, ID_HIGH_LEVEL, wxString( wxT("High Level") ) , wxT("Disassembles into C-like statements"), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewHighLevel );

	menuView->Append( menuViewDisassembleItem );

	wxMenuItem* menuViewInstructionHighlighting;
	menuViewInstructionHighlighting = new wxMenuItem( menuView, ID_INSTRUCTION_HIGHLIGHTING, wxString( wxT("&Instruction Highlighting") ) , wxT("Highlights bits of the instruction."), wxITEM_CHECK );
	menuView->Append( menuViewInstructionHighlighting );

	menuView->AppendSeparator();

	menuViewLogLevel = new wxMenu();
	wxMenuItem* menuViewLogLevelItem = new wxMenuItem( menuView, wxID_ANY, wxT("&Log Level"), wxEmptyString, wxITEM_NORMAL, menuViewLogLevel );
	menuViewLogLevelFatal = new wxMenuItem( menuViewLogLevel, ID_FATAL, wxString( wxT("Fatal") ) , wxEmptyString, wxITEM_RADIO );
	menuViewLogLevel->Append( menuViewLogLevelFatal );

	menuViewLogLevelDebug = new wxMenuItem( menuViewLogLevel, ID_DEBUG, wxString( wxT("Debug") ) , wxEmptyString, wxITEM_RADIO );
	menuViewLogLevel->Append( menuViewLogLevelDebug );

	menuViewLogLevelWarning = new wxMenuItem( menuViewLogLevel, ID_WARNING, wxString( wxT("Warning") ) , wxEmptyString, wxITEM_RADIO );
	menuViewLogLevel->Append( menuViewLogLevelWarning );

	menuViewLogLevelInfo = new wxMenuItem( menuViewLogLevel, ID_INFO, wxString( wxT("Info") ) , wxEmptyString, wxITEM_RADIO );
	menuViewLogLevel->Append( menuViewLogLevelInfo );

	menuViewLogLevelVerbose = new wxMenuItem( menuViewLogLevel, ID_VERBOSE, wxString( wxT("Verbose") ) , wxEmptyString, wxITEM_RADIO );
	menuViewLogLevel->Append( menuViewLogLevelVerbose );
	menuViewLogLevelVerbose->Check( true );

	menuView->Append( menuViewLogLevelItem );

	menuBar->Append( menuView, wxT("View") );

	menuControl = new wxMenu();
	menuCycleSpeed = new wxMenu();
	wxMenuItem* menuCycleSpeedItem = new wxMenuItem( menuControl, wxID_ANY, wxT("Cycle Speed"), wxEmptyString, wxITEM_NORMAL, menuCycleSpeed );
	wxMenuItem* menuCycleSpeedCustom;
	menuCycleSpeedCustom = new wxMenuItem( menuCycleSpeed, ID_CUSTOM, wxString( wxT("Custom...") ) , wxT("Sets a custom number of instructions to execute per second."), wxITEM_RADIO );
	menuCycleSpeed->Append( menuCycleSpeedCustom );

	menuControl->Append( menuCycleSpeedItem );

	wxMenuItem* menuControlStep;
	menuControlStep = new wxMenuItem( menuControl, ID_STEP, wxString( wxT("&Step") ) + wxT('\t') + wxT("F2"), wxT("Executes one instruction"), wxITEM_NORMAL );
	menuControl->Append( menuControlStep );

	wxMenuItem* menuControlBack;
	menuControlBack = new wxMenuItem( menuControl, ID_BACK, wxString( wxT("&Back") ) + wxT('\t') + wxT("F3"), wxT("Undo one instruction"), wxITEM_NORMAL );
	menuControl->Append( menuControlBack );

	menuBar->Append( menuControl, wxT("Control") );

	menuState = new wxMenu();
	menuStateControl = new wxMenu();
	wxMenuItem* menuStateControlItem = new wxMenuItem( menuState, wxID_ANY, wxT("Control"), wxEmptyString, wxITEM_NORMAL, menuStateControl );
	wxMenuItem* menuStateControlStep;
	menuStateControlStep = new wxMenuItem( menuStateControl, ID_STEP, wxString( wxT("Step") ) + wxT('\t') + wxT("F2"), wxT("Runs one instruction"), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlStep );

	wxMenuItem* menuStateControlBack;
	menuStateControlBack = new wxMenuItem( menuStateControl, ID_BACK, wxString( wxT("Back") ) + wxT('\t') + wxT("Shift+F2"), wxT("Undoes one instruction"), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlBack );

	wxMenuItem* menuStateControlNextLine;
	menuStateControlNextLine = new wxMenuItem( menuStateControl, ID_NEXT_LINE, wxString( wxT("Next Line") ) + wxT('\t') + wxT("F3"), wxT("Steps over the next instruction."), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlNextLine );

	wxMenuItem* menuStateControlPreviousLine;
	menuStateControlPreviousLine = new wxMenuItem( menuStateControl, ID_PREVIOUS_LINE, wxString( wxT("Previous Line") ) + wxT('\t') + wxT("Shift+F3"), wxT("Steps over the previous instruction."), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlPreviousLine );

	wxMenuItem* menuStateControlRun;
	menuStateControlRun = new wxMenuItem( menuStateControl, ID_RUN, wxString( wxT("Run") ) + wxT('\t') + wxT("F4"), wxT("Runs until program halts."), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlRun );

	wxMenuItem* menuStateControlRunFor;
	menuStateControlRunFor = new wxMenuItem( menuStateControl, ID_RUN_FOR, wxString( wxT("Run For...") ) + wxT('\t') + wxT("Ctrl+F4"), wxT("Runs for a specified number of steps"), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlRunFor );

	wxMenuItem* menuStateControlRunAgain;
	menuStateControlRunAgain = new wxMenuItem( menuStateControl, ID_RUN_AGAIN, wxString( wxT("Run Again...") ) + wxT('\t') + wxT("Ctrl+Space"), wxT("Runs the default number of instructions (see RunFor)"), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlRunAgain );

	wxMenuItem* menuStateControlRewind;
	menuStateControlRewind = new wxMenuItem( menuStateControl, ID_REWIND, wxString( wxT("Rewind") ) + wxT('\t') + wxT("Shift+F4"), wxT("Undoes all instructions."), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlRewind );

	wxMenuItem* menuStateControlFinish;
	menuStateControlFinish = new wxMenuItem( menuStateControl, ID_FINISH, wxString( wxT("Finish") ) + wxT('\t') + wxT("Shift+F5"), wxT("Finishes current subroutine."), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlFinish );

	menuState->Append( menuStateControlItem );

	menuStateControlMode = new wxMenu();
	wxMenuItem* menuStateControlModeItem = new wxMenuItem( menuState, wxID_ANY, wxT("Control Mode"), wxEmptyString, wxITEM_NORMAL, menuStateControlMode );
	wxMenuItem* menuStateControlModeSimple;
	menuStateControlModeSimple = new wxMenuItem( menuStateControlMode, ID_SIMPLE, wxString( wxT("Simple") ) , wxT("Show commonly used control buttons."), wxITEM_RADIO );
	menuStateControlMode->Append( menuStateControlModeSimple );

	wxMenuItem* menuStateControlModeAdvanced;
	menuStateControlModeAdvanced = new wxMenuItem( menuStateControlMode, ID_ADVANCED, wxString( wxT("Advanced") ) , wxT("Show control buttons related to subroutines."), wxITEM_RADIO );
	menuStateControlMode->Append( menuStateControlModeAdvanced );

	menuState->Append( menuStateControlModeItem );

	menuState->AppendSeparator();

	menuStateTrueTraps = new wxMenuItem( menuState, ID_TRUE_TRAPS, wxString( wxT("&True Traps") ) , wxT("Enables True Traps"), wxITEM_CHECK );
	menuState->Append( menuStateTrueTraps );

	menuStateInterrupts = new wxMenuItem( menuState, ID_INTERRUPTS, wxString( wxT("&Interrupts") ) , wxT("Enables Interrupts"), wxITEM_CHECK );
	menuState->Append( menuStateInterrupts );

	menuStateStrictExecution = new wxMenuItem( menuState, ID_STRICT_EXECUTION_MODE, wxString( wxT("&Strict Execution Mode") ) + wxT('\t') + wxT("Ctrl+S"), wxT("Enables strict execution mode."), wxITEM_CHECK );
	menuState->Append( menuStateStrictExecution );
	menuStateStrictExecution->Check( true );

	menuState->AppendSeparator();

	wxMenuItem* menuStateClearConsole;
	menuStateClearConsole = new wxMenuItem( menuState, ID_CLEAR_CONSOLE, wxString( wxT("&Clear Console") ) , wxT("Clears the console output."), wxITEM_NORMAL );
	menuState->Append( menuStateClearConsole );

	wxMenuItem* menuStateClearConsoleInput;
	menuStateClearConsoleInput = new wxMenuItem( menuState, wxID_ANY, wxString( wxT("Clear Console Input") ) , wxT("Clears the console input."), wxITEM_NORMAL );
	menuState->Append( menuStateClearConsoleInput );

	wxMenuItem* menuStateClearWarnings;
	menuStateClearWarnings = new wxMenuItem( menuState, ID_CLEAR_WARNINGS, wxString( wxT("Clear Warnings") ) , wxT("Clears the warning output."), wxITEM_NORMAL );
	menuState->Append( menuStateClearWarnings );

	menuBar->Append( menuState, wxT("State") );

	menuDebug = new wxMenu();
	wxMenuItem* menuDebugUndoStack;
	menuDebugUndoStack = new wxMenuItem( menuDebug, ID_UNDO_STACK, wxString( wxT("&Undo Stack") ) + wxT('\t') + wxT("F6"), wxT("Modifies the undo stack size"), wxITEM_NORMAL );
	menuDebug->Append( menuDebugUndoStack );

	wxMenuItem* menuDebugCallStack;
	menuDebugCallStack = new wxMenuItem( menuDebug, ID_CALL_STACK, wxString( wxT("&Call Stack") ) , wxT("Views the call stack."), wxITEM_NORMAL );
	menuDebug->Append( menuDebugCallStack );

	menuDebug->AppendSeparator();

	wxMenuItem* menuDebugSimulateSubroutineCall;
	menuDebugSimulateSubroutineCall = new wxMenuItem( menuDebug, ID_SIMULATE_SUBROUTINE_CALL, wxString( wxT("Simulate Subroutine &Call...") ) , wxT("Simulates a subroutine call."), wxITEM_NORMAL );
	menuDebug->Append( menuDebugSimulateSubroutineCall );

	menuDebug->AppendSeparator();

	wxMenuItem* menuDebugBreakWatchpoints;
	menuDebugBreakWatchpoints = new wxMenuItem( menuDebug, ID_BREAKPOINTS_AND_WATCHPOINTS, wxString( wxT("Breakpoints &and Watchpoints") ) + wxT('\t') + wxT("F7"), wxT("View all breakpoints and watchpoints"), wxITEM_NORMAL );
	menuDebug->Append( menuDebugBreakWatchpoints );

	wxMenuItem* menuDebugTemporary;
	menuDebugTemporary = new wxMenuItem( menuDebug, ID_ADD_TEMPORARY_BREAKPOINT, wxString( wxT("Add &Temporary Breakpoint") ) + wxT('\t') + wxT("F8"), wxT("Adds/Removes a one time breakpoint"), wxITEM_NORMAL );
	menuDebug->Append( menuDebugTemporary );

	wxMenuItem* menuDebugBreakpoint;
	menuDebugBreakpoint = new wxMenuItem( menuDebug, ID_ADD_BREAKPOINT, wxString( wxT("Add &Breakpoint") ) + wxT('\t') + wxT("F9"), wxT("Adds/Removes a breakpoint"), wxITEM_NORMAL );
	menuDebug->Append( menuDebugBreakpoint );

	wxMenuItem* menuDebugWatchpoint;
	menuDebugWatchpoint = new wxMenuItem( menuDebug, ID_ADD_WATCHPOINT, wxString( wxT("Add &Watchpoint") ) + wxT('\t') + wxT("F11"), wxT("Adds a watchpoint."), wxITEM_NORMAL );
	menuDebug->Append( menuDebugWatchpoint );

	wxMenuItem* menuDebugAdvanced;
	menuDebugAdvanced = new wxMenuItem( menuDebug, ID_ADVANCED_BREAKPOINT, wxString( wxT("Ad&vanced Breakpoint") ) + wxT('\t') + wxT("F12"), wxT("Adds a customizable breakpoint."), wxITEM_NORMAL );
	menuDebug->Append( menuDebugAdvanced );

	menuDebug->AppendSeparator();

	wxMenuItem* menuDebugTraceLogging;
	menuDebugTraceLogging = new wxMenuItem( menuDebug, ID_TRACE_LOGGING, wxString( wxT("Trace Logging") ) , wxT("Enable trace logging."), wxITEM_CHECK );
	menuDebug->Append( menuDebugTraceLogging );

	menuBar->Append( menuDebug, wxT("Debug") );

	menuTest = new wxMenu();
	wxMenuItem* setupReplayString;
	setupReplayString = new wxMenuItem( menuTest, ID_SETUP_REPLAY_STRING, wxString( wxT("&Setup Replay String...") ) + wxT('\t') + wxT("Ctrl+T"), wxT("Loads a replay string from the pylc3 unit test framework."), wxITEM_NORMAL );
	menuTest->Append( setupReplayString );

	wxMenuItem* menuTestReloadReplayString;
	menuTestReloadReplayString = new wxMenuItem( menuTest, ID_RELOAD_REPLAY_STRING, wxString( wxT("&Reload Replay String") ) + wxT('\t') + wxT("Ctrl+Alt+T"), wxT("Reloads a replay string from the pylc3 unit test framework."), wxITEM_NORMAL );
	menuTest->Append( menuTestReloadReplayString );

	menuBar->Append( menuTest, wxT("Test") );

	menuHelp = new wxMenu();
	wxMenuItem* menuHelpDocs;
	menuHelpDocs = new wxMenuItem( menuHelp, wxID_ANY, wxString( wxT("&Documentation") ) + wxT('\t') + wxT("F1"), wxT("Opens up the Complx Documentation."), wxITEM_NORMAL );
	menuHelp->Append( menuHelpDocs );

	wxMenuItem* menuHelpChangeLog;
	menuHelpChangeLog = new wxMenuItem( menuHelp, ID_CHANGE_LOG, wxString( wxT("&Change Log") ) , wxT("Shows the change log"), wxITEM_NORMAL );
	menuHelp->Append( menuHelpChangeLog );

	wxMenuItem* menuHelpCreateBugReport;
	menuHelpCreateBugReport = new wxMenuItem( menuHelp, ID_CREATE_BUG_REPORT, wxString( wxT("Create &Bug Report") ) , wxT("Create a bug report"), wxITEM_NORMAL );
	menuHelp->Append( menuHelpCreateBugReport );

	wxMenuItem* menuHelpTips;
	menuHelpTips = new wxMenuItem( menuHelp, ID_TIPS, wxString( wxT("Tips") ) , wxT("Displays the tips"), wxITEM_NORMAL );
	menuHelp->Append( menuHelpTips );

	wxMenuItem* menuHelpAbout;
	menuHelpAbout = new wxMenuItem( menuHelp, ID_ABOUT, wxString( wxT("&About") ) , wxT("Shows info about this program"), wxITEM_NORMAL );
	menuHelp->Append( menuHelpAbout );

	menuBar->Append( menuHelp, wxT("Help") );

	this->SetMenuBar( menuBar );

	toolbarPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( toolbarPanel, wxAuiPaneInfo() .Top() .CaptionVisible( false ).CloseButton( false ).PaneBorder( false ).Dock().Resizable().FloatingSize( wxDefaultSize ).BestSize( wxSize( -1,48 ) ).Layer( 10 ).ToolbarPane() );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxHORIZONTAL );

	m_auiToolBar1 = new wxAuiToolBar( toolbarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT );
	m_button1 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button1 );
	m_button2 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Step"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button2 );
	m_button3 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Back"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button3 );
	m_button4 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Step Over"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button4 );
	m_button5 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Back Over"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button5 );
	m_button6 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Step Out"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button6 );
	m_button7 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Rewind"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button7 );
	stopButton = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	stopButton->Enable( false );

	m_auiToolBar1->AddControl( stopButton );
	m_auiToolBar1->Realize();

	bSizer111->Add( m_auiToolBar1, 1, wxALL|wxEXPAND, 5 );

	m_auiToolBar2 = new wxAuiToolBar( toolbarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT );
	m_button8 = new wxButton( m_auiToolBar2, wxID_ANY, wxT("Breakpoint"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar2->AddControl( m_button8 );
	m_button9 = new wxButton( m_auiToolBar2, wxID_ANY, wxT("Watchpoint"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar2->AddControl( m_button9 );
	m_auiToolBar2->Realize();

	bSizer111->Add( m_auiToolBar2, 1, wxALL|wxEXPAND, 5 );


	toolbarPanel->SetSizer( bSizer111 );
	toolbarPanel->Layout();
	bSizer111->Fit( toolbarPanel );
	memoryPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( memoryPanel, wxAuiPaneInfo() .Name( wxT("memory") ).Center() .Caption( wxT("Memory") ).CloseButton( false ).MaximizeButton( true ).MinimizeButton( true ).PinButton( true ).Gripper().Dock().Resizable().FloatingSize( wxDefaultSize ).CentrePane() );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	memoryView = new MemoryView( memoryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES|wxDV_ROW_LINES|wxDV_SINGLE|wxDV_VERT_RULES );
	bSizer6->Add( memoryView, 1, wxEXPAND, 5 );


	memoryPanel->SetSizer( bSizer6 );
	memoryPanel->Layout();
	bSizer6->Fit( memoryPanel );
	statePanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( statePanel, wxAuiPaneInfo() .Name( wxT("state") ).Right() .Caption( wxT("State") ).MaximizeButton( true ).MinimizeButton( true ).PinButton( true ).Gripper().Dock().Resizable().FloatingSize( wxDefaultSize ).BestSize( wxSize( 256,-1 ) ) );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	statePropGridManager = new wxPropertyGridManager(statePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPGMAN_DEFAULT_STYLE);
	statePropGridManager->SetExtraStyle( wxPG_EX_MODE_BUTTONS );

	statePropGrid = statePropGridManager->AddPage( wxEmptyString, wxNullBitmap );
	registers = statePropGrid->Append( new wxPropertyCategory( wxT("Registers"), wxT("Registers") ) );
	bSizer8->Add( statePropGridManager, 1, wxALL|wxEXPAND, 5 );


	statePanel->SetSizer( bSizer8 );
	statePanel->Layout();
	bSizer8->Fit( statePanel );
	infoNotebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE );
	m_mgr.AddPane( infoNotebook, wxAuiPaneInfo() .Bottom() .PinButton( true ).Dock().Resizable().FloatingSize( wxDefaultSize ).BestSize( wxSize( -1,256 ) ) );

	consolePanel = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	consoleText = new wxTextCtrl( consolePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	bSizer11->Add( consoleText, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	consoleInputText = new wxTextCtrl( consolePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	consoleInputText->SetValidator( wxTextValidator( wxFILTER_ASCII, &consoleInput ) );

	bSizer11->Add( consoleInputText, 0, wxALL|wxEXPAND, 5 );


	consolePanel->SetSizer( bSizer11 );
	consolePanel->Layout();
	bSizer11->Fit( consolePanel );
	infoNotebook->AddPage( consolePanel, wxT("Console"), true, wxNullBitmap );
	warningPanel = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	warningText = new wxTextCtrl( warningPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	bSizer13->Add( warningText, 1, wxALL|wxEXPAND, 5 );


	warningPanel->SetSizer( bSizer13 );
	warningPanel->Layout();
	bSizer13->Fit( warningPanel );
	infoNotebook->AddPage( warningPanel, wxT("Warnings"), false, wxNullBitmap );
	tracePanel = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	traceText = new wxTextCtrl( tracePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	bSizer14->Add( traceText, 1, wxEXPAND, 5 );


	tracePanel->SetSizer( bSizer14 );
	tracePanel->Layout();
	bSizer14->Fit( tracePanel );
	infoNotebook->AddPage( tracePanel, wxT("Trace"), false, wxNullBitmap );
	loggingPanel = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );

	loggingText = new wxTextCtrl( loggingPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	bSizer15->Add( loggingText, 1, wxEXPAND, 5 );


	loggingPanel->SetSizer( bSizer15 );
	loggingPanel->Layout();
	bSizer15->Fit( loggingPanel );
	infoNotebook->AddPage( loggingPanel, wxT("Logging"), false, wxNullBitmap );

	statusBar = this->CreateStatusBar( 5, wxSTB_SHOW_TIPS|wxSTB_SIZEGRIP, wxID_ANY );

	m_mgr.Update();
	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ComplxFrameDecl::OnClose ) );
	menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLoad ), this, menuFileLoad->GetId());
	menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnReload ), this, menuFileReload->GetId());
	menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnAdvancedLoad ), this, menuFileAdvancedLoad->GetId());
	menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnExit ), this, menuFileExit->GetId());
	menuView->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNewView ), this, menuViewNew->GetId());
	menuViewShowAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUpdateShowAddresses ), this, menuViewShowAddressesShowAll->GetId());
	menuViewShowAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUpdateShowAddresses ), this, menuViewShowAddressesShowNonZero->GetId());
	menuViewShowAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUpdateShowAddresses ), this, menuViewShowAddressesShowOnlyCodeData->GetId());
	menuViewShowAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnShowAddressesCustom ), this, menuViewShowAddressesCustom->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnDisassemble ), this, menuViewBasic->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNormalDisassemble ), this, menuViewNormal->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCDisassemble ), this, menuViewHighLevel->GetId());
	menuView->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnInstructionHighlighting ), this, menuViewInstructionHighlighting->GetId());
	menuViewLogLevel->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLogLevel ), this, menuViewLogLevelFatal->GetId());
	menuViewLogLevel->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLogLevel ), this, menuViewLogLevelDebug->GetId());
	menuViewLogLevel->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLogLevel ), this, menuViewLogLevelWarning->GetId());
	menuViewLogLevel->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLogLevel ), this, menuViewLogLevelInfo->GetId());
	menuViewLogLevel->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLogLevel ), this, menuViewLogLevelVerbose->GetId());
	menuCycleSpeed->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCycleSpeedCustom ), this, menuCycleSpeedCustom->GetId());
	menuControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnStep ), this, menuControlStep->GetId());
	menuControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBack ), this, menuControlBack->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnStep ), this, menuStateControlStep->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBackStep ), this, menuStateControlBack->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNextLine ), this, menuStateControlNextLine->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnPrevLine ), this, menuStateControlPreviousLine->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRun ), this, menuStateControlRun->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRunFor ), this, menuStateControlRunFor->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRunAgain ), this, menuStateControlRunAgain->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRewind ), this, menuStateControlRewind->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnFinish ), this, menuStateControlFinish->GetId());
	menuStateControlMode->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnControlModeSimple ), this, menuStateControlModeSimple->GetId());
	menuStateControlMode->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnControlModeAdvanced ), this, menuStateControlModeAdvanced->GetId());
	menuState->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTrueTraps ), this, menuStateTrueTraps->GetId());
	menuState->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnInterrupts ), this, menuStateInterrupts->GetId());
	menuState->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnStrictExecution ), this, menuStateStrictExecution->GetId());
	menuState->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnClearConsole ), this, menuStateClearConsole->GetId());
	menuState->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnClearConsoleInput ), this, menuStateClearConsoleInput->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUndoStack ), this, menuDebugUndoStack->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCallStack ), this, menuDebugCallStack->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnSubroutineCall ), this, menuDebugSimulateSubroutineCall->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBreakAndWatchpoints ), this, menuDebugBreakWatchpoints->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTemppoint ), this, menuDebugTemporary->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBreakpoint ), this, menuDebugBreakpoint->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnWatchpoint ), this, menuDebugWatchpoint->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnAdvancedBreakpoint ), this, menuDebugAdvanced->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTraceFile ), this, menuDebugTraceLogging->GetId());
	menuTest->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnSetupReplayString ), this, setupReplayString->GetId());
	menuTest->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnReloadReplayString ), this, menuTestReloadReplayString->GetId());
	menuHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnDocs ), this, menuHelpDocs->GetId());
	menuHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnChangeLog ), this, menuHelpChangeLog->GetId());
	menuHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCreateBugReport ), this, menuHelpCreateBugReport->GetId());
	menuHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTips ), this, menuHelpTips->GetId());
	menuHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnAbout ), this, menuHelpAbout->GetId());
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRun ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStep ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnBack ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStepOver ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnBackOver ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStepOut ), NULL, this );
	m_button7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRewind ), NULL, this );
	stopButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStop ), NULL, this );
	statePropGridManager->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( ComplxFrameDecl::OnStateChange ), NULL, this );
}

ComplxFrameDecl::~ComplxFrameDecl()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ComplxFrameDecl::OnClose ) );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRun ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStep ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnBack ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStepOver ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnBackOver ), NULL, this );
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStepOut ), NULL, this );
	m_button7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRewind ), NULL, this );
	stopButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStop ), NULL, this );
	statePropGridManager->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( ComplxFrameDecl::OnStateChange ), NULL, this );

	m_mgr.UnInit();

}

AdvancedLoadDialogDecl::AdvancedLoadDialogDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetExtraStyle( this->GetExtraStyle() | wxWS_EX_VALIDATE_RECURSIVELY );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 9, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( m_panel2, wxID_ANY, wxT("Assembly File:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_staticText1->SetForegroundColour( wxColour( 255, 0, 0 ) );

	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	assemblyFileCtrl = new wxFilePickerCtrl( m_panel2, wxID_ANY, wxEmptyString, wxT("Select an assembly file"), wxT("*.asm"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_SMALL|wxFLP_USE_TEXTCTRL );
	assemblyFileCtrl->SetToolTip( wxT("(Required) Assembly File to load.") );

	fgSizer1->Add( assemblyFileCtrl, 1, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	m_staticText9 = new wxStaticText( m_panel2, wxID_ANY, wxT("Replay (Test) String:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer1->Add( m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	replayStringCtrl = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	replayStringCtrl->SetToolTip( wxT("Replay string given by pyLC3.  Note that setting this may override other settings.") );

	replayStringCtrl->SetValidator( wxTextValidator( wxFILTER_INCLUDE_CHAR_LIST, &replayString ) );

	fgSizer1->Add( replayStringCtrl, 1, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	m_staticText6 = new wxStaticText( m_panel2, wxID_ANY, wxT("Console Input:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	consoleInputCtrl = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	consoleInputCtrl->SetToolTip( wxT("Input present in LC3 console when program is loaded") );

	consoleInputCtrl->SetValidator( wxTextValidator( wxFILTER_ASCII, &consoleInput ) );

	fgSizer1->Add( consoleInputCtrl, 1, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	m_staticText0 = new wxStaticText( m_panel2, wxID_ANY, wxT("Registers?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText0->Wrap( -1 );
	fgSizer1->Add( m_staticText0, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	wxString regInitializerCtrlChoices[] = { wxT("RANDOMIZE"), wxT("ZERO"), wxT("FILL WITH...") };
	int regInitializerCtrlNChoices = sizeof( regInitializerCtrlChoices ) / sizeof( wxString );
	regInitializerCtrl = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, regInitializerCtrlNChoices, regInitializerCtrlChoices, 0 );
	regInitializerCtrl->SetSelection( 0 );
	regInitializerCtrl->SetToolTip( wxT("Whether to randomize, zero out, or fill with a specific value") );

	regInitializerCtrl->SetValidator( wxGenericValidator( &regInitializer ) );

	bSizer5->Add( regInitializerCtrl, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );

	regFillCtrl = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	regFillCtrl->Enable( false );

	regFillCtrl->SetValidator( wxTextValidator( wxFILTER_INCLUDE_CHAR_LIST, &regFill ) );

	bSizer5->Add( regFillCtrl, 0, wxALL, 5 );


	fgSizer1->Add( bSizer5, 1, wxEXPAND, 5 );

	m_staticText4 = new wxStaticText( m_panel2, wxID_ANY, wxT("Memory?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );

	wxString memInitializerCtrlChoices[] = { wxT("RANDOMIZE"), wxT("ZERO"), wxT("FILL WITH...") };
	int memInitializerCtrlNChoices = sizeof( memInitializerCtrlChoices ) / sizeof( wxString );
	memInitializerCtrl = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, memInitializerCtrlNChoices, memInitializerCtrlChoices, 0 );
	memInitializerCtrl->SetSelection( 0 );
	memInitializerCtrl->SetToolTip( wxT("Whether to randomize, zero out, or fill with a specific value") );

	memInitializerCtrl->SetValidator( wxGenericValidator( &memInitializer ) );

	bSizer51->Add( memInitializerCtrl, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );

	memFillCtrl = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	memFillCtrl->Enable( false );

	memFillCtrl->SetValidator( wxTextValidator( wxFILTER_INCLUDE_CHAR_LIST, &memFill ) );

	bSizer51->Add( memFillCtrl, 0, wxALL, 5 );


	fgSizer1->Add( bSizer51, 1, wxEXPAND, 5 );

	m_staticText5 = new wxStaticText( m_panel2, wxID_ANY, wxT("PC"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	pcCtrl = new wxTextCtrl( m_panel2, wxID_ANY, wxT("x3000"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	pcCtrl->SetValidator( wxTextValidator( wxFILTER_INCLUDE_CHAR_LIST, &pc ) );

	fgSizer1->Add( pcCtrl, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );

	m_staticText7 = new wxStaticText( m_panel2, wxID_ANY, wxT("True Traps?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer1->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	trueTrapsCtrl = new wxCheckBox( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	trueTrapsCtrl->SetToolTip( wxT("Enable executing of trap handler routines in asssembly instead of C.  (This is automatically determined by your assembly code).") );

	trueTrapsCtrl->SetValidator( wxGenericValidator( &trueTraps ) );

	fgSizer1->Add( trueTrapsCtrl, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText8 = new wxStaticText( m_panel2, wxID_ANY, wxT("Interrupts?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer1->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	interruptsCtrl = new wxCheckBox( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	interruptsCtrl->SetToolTip( wxT("Enable processing of lc3 interrupts.  This is automatically configued based on your assembly code.") );

	interruptsCtrl->SetValidator( wxGenericValidator( &interrupts ) );

	fgSizer1->Add( interruptsCtrl, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText81 = new wxStaticText( m_panel2, wxID_ANY, wxT("Strict Execution?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText81->Wrap( -1 );
	fgSizer1->Add( m_staticText81, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	strictExecutionCtrl = new wxCheckBox( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	strictExecutionCtrl->SetValue(true);
	strictExecutionCtrl->SetToolTip( wxT("Enables strict execution mode. Instructions that are malformed will immediately halt the simulator.") );

	strictExecutionCtrl->SetValidator( wxGenericValidator( &strictExecution ) );

	fgSizer1->Add( strictExecutionCtrl, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer7->Add( fgSizer1, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	m_button1 = new wxButton( m_panel2, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_button1, 0, wxALL, 5 );

	m_button2 = new wxButton( m_panel2, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_button2, 0, wxALL, 5 );


	bSizer7->Add( bSizer9, 0, wxALIGN_RIGHT, 5 );


	m_panel2->SetSizer( bSizer7 );
	m_panel2->Layout();
	bSizer7->Fit( m_panel2 );
	bSizer6->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( bSizer6 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	regInitializerCtrl->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AdvancedLoadDialogDecl::OnRegChoice ), NULL, this );
	memInitializerCtrl->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AdvancedLoadDialogDecl::OnMemChoice ), NULL, this );
}

AdvancedLoadDialogDecl::~AdvancedLoadDialogDecl()
{
	// Disconnect Events
	regInitializerCtrl->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AdvancedLoadDialogDecl::OnRegChoice ), NULL, this );
	memInitializerCtrl->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AdvancedLoadDialogDecl::OnMemChoice ), NULL, this );

}

MemoryViewFrameDecl::MemoryViewFrameDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	menu = new wxMenuBar( 0 );
	menuView = new wxMenu();
	wxMenuItem* menuViewGoto;
	menuViewGoto = new wxMenuItem( menuView, ID_GOTO_ADDRESS, wxString( wxT("&Goto Address") ) + wxT('\t') + wxT("Ctrl+G"), wxT("Moves memory to a specific address."), wxITEM_NORMAL );
	menuView->Append( menuViewGoto );

	menuViewShowAddresses = new wxMenu();
	wxMenuItem* menuViewShowAddressesItem = new wxMenuItem( menuView, wxID_ANY, wxT("Hide Addresses"), wxEmptyString, wxITEM_NORMAL, menuViewShowAddresses );
	menuViewShowAddressesShowAll = new wxMenuItem( menuViewShowAddresses, ID_SHOW_ALL, wxString( wxT("Show All") ) , wxT("Shows all memory addresses"), wxITEM_RADIO );
	menuViewShowAddresses->Append( menuViewShowAddressesShowAll );
	menuViewShowAddressesShowAll->Check( true );

	menuViewShowAddressesShowNonZero = new wxMenuItem( menuViewShowAddresses, ID_SHOW_NON_ZERO, wxString( wxT("Show Non Zero") ) , wxT("Show all memory addresses that have a value other than 0"), wxITEM_RADIO );
	menuViewShowAddresses->Append( menuViewShowAddressesShowNonZero );

	menuViewShowAddressesShowOnlyCodeData = new wxMenuItem( menuViewShowAddresses, ID_SHOW_ONLY_CODEDATA, wxString( wxT("Show Only Code/Data") ) , wxT("Shows addresses modified when your program was loaded."), wxITEM_RADIO );
	menuViewShowAddresses->Append( menuViewShowAddressesShowOnlyCodeData );

	menuViewShowAddressesCustom = new wxMenuItem( menuViewShowAddresses, ID_CUSTOM, wxString( wxT("Custom...") ) + wxT('\t') + wxT("Ctrl+H"), wxT("Allows you to show/hide memory address ranges"), wxITEM_RADIO );
	menuViewShowAddresses->Append( menuViewShowAddressesCustom );

	menuView->Append( menuViewShowAddressesItem );

	menuViewDisassemble = new wxMenu();
	wxMenuItem* menuViewDisassembleItem = new wxMenuItem( menuView, wxID_ANY, wxT("Disassemble"), wxEmptyString, wxITEM_NORMAL, menuViewDisassemble );
	menuViewBasic = new wxMenuItem( menuViewDisassemble, ID_BASIC, wxString( wxT("Basic") ) , wxT("Disassembles ignoring symbols"), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewBasic );

	menuViewNormal = new wxMenuItem( menuViewDisassemble, ID_NORMAL, wxString( wxT("Normal") ) , wxT("Disassembles with symbol info."), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewNormal );
	menuViewNormal->Check( true );

	menuViewHighLevel = new wxMenuItem( menuViewDisassemble, ID_HIGH_LEVEL, wxString( wxT("High Level") ) , wxT("Disassembles into C-like statements"), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewHighLevel );

	menuView->Append( menuViewDisassembleItem );

	wxMenuItem* menuViewInstructionHighlighting;
	menuViewInstructionHighlighting = new wxMenuItem( menuView, ID_INSTRUCTION_HIGHLIGHTING, wxString( wxT("&Instruction Highlighting") ) , wxT("Highlights bits of the instruction."), wxITEM_CHECK );
	menuView->Append( menuViewInstructionHighlighting );

	menu->Append( menuView, wxT("View") );

	this->SetMenuBar( menu );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	memoryPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_panel12 = new wxPanel( memoryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText11 = new wxStaticText( m_panel12, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer16->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl11 = new wxTextCtrl( m_panel12, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl11->SetValidator( wxTextValidator( wxFILTER_ASCII, &name ) );

	bSizer16->Add( m_textCtrl11, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText10 = new wxStaticText( m_panel12, wxID_ANY, wxT("Tracking:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer16->Add( m_staticText10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxString m_choice3Choices[] = { wxT("NONE"), wxT("R0"), wxT("R1"), wxT("R2"), wxT("R3"), wxT("R4"), wxT("R5"), wxT("R6"), wxT("R7"), wxT("USP"), wxT("SSP") };
	int m_choice3NChoices = sizeof( m_choice3Choices ) / sizeof( wxString );
	m_choice3 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice3NChoices, m_choice3Choices, 0 );
	m_choice3->SetSelection( 0 );
	m_choice3->SetValidator( wxGenericValidator( &tracking ) );

	bSizer16->Add( m_choice3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );


	m_panel12->SetSizer( bSizer16 );
	m_panel12->Layout();
	bSizer16->Fit( m_panel12 );
	bSizer6->Add( m_panel12, 0, wxALL, 5 );

	memoryView = new MemoryView( memoryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES|wxDV_ROW_LINES|wxDV_SINGLE|wxDV_VERT_RULES );
	bSizer6->Add( memoryView, 1, wxEXPAND, 5 );


	memoryPanel->SetSizer( bSizer6 );
	memoryPanel->Layout();
	bSizer6->Fit( memoryPanel );
	bSizer13->Add( memoryPanel, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer13 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	menuViewShowAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnUpdateShowAddresses ), this, menuViewShowAddressesShowAll->GetId());
	menuViewShowAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnUpdateShowAddresses ), this, menuViewShowAddressesShowNonZero->GetId());
	menuViewShowAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnUpdateShowAddresses ), this, menuViewShowAddressesShowOnlyCodeData->GetId());
	menuViewShowAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnShowAddressesCustom ), this, menuViewShowAddressesCustom->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnDisassemble ), this, menuViewBasic->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnNormalDisassemble ), this, menuViewNormal->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnCDisassemble ), this, menuViewHighLevel->GetId());
	menuView->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnInstructionHighlighting ), this, menuViewInstructionHighlighting->GetId());
	m_textCtrl11->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MemoryViewFrameDecl::OnName ), NULL, this );
	m_choice3->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnTracking ), NULL, this );
}

MemoryViewFrameDecl::~MemoryViewFrameDecl()
{
	// Disconnect Events
	m_textCtrl11->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MemoryViewFrameDecl::OnName ), NULL, this );
	m_choice3->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnTracking ), NULL, this );

}
