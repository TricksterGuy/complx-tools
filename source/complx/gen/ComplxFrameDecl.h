///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
class MemoryView;

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <wx/aui/auibook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/filepicker.h>
#include <wx/choice.h>
#include <wx/valgen.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define ID_ADVANCED_LOAD 1000
#define ID_NEW_VIEW 1001
#define ID_GOTO_ADDRESS 1002
#define ID_SHOW_ALL 1003
#define ID_SHOW_NON_ZERO 1004
#define ID_SHOW_ONLY_CODEDATA 1005
#define ID_CUSTOM 1006
#define ID_BASIC 1007
#define ID_NORMAL 1008
#define ID_HIGH_LEVEL 1009
#define ID_INSTRUCTION_HIGHLIGHTING 1010
#define ID_FATAL 1011
#define ID_DEBUG 1012
#define ID_WARNING 1013
#define ID_INFO 1014
#define ID_VERBOSE 1015
#define ID_STEP 1016
#define ID_BACK 1017
#define ID_NEXT_LINE 1018
#define ID_PREVIOUS_LINE 1019
#define ID_RUN 1020
#define ID_RUN_FOR 1021
#define ID_RUN_AGAIN 1022
#define ID_REWIND 1023
#define ID_FINISH 1024
#define ID_SIMPLE 1025
#define ID_ADVANCED 1026
#define ID_TRUE_TRAPS 1027
#define ID_INTERRUPTS 1028
#define ID_STRICT_EXECUTION_MODE 1029
#define ID_CLEAR_CONSOLE 1030
#define ID_CLEAR_WARNINGS 1031
#define ID_UNDO_STACK 1032
#define ID_CALL_STACK 1033
#define ID_SIMULATE_SUBROUTINE_CALL 1034
#define ID_BREAKPOINTS_AND_WATCHPOINTS 1035
#define ID_ADD_TEMPORARY_BREAKPOINT 1036
#define ID_ADD_BREAKPOINT 1037
#define ID_ADD_WATCHPOINT 1038
#define ID_ADVANCED_BREAKPOINT 1039
#define ID_TRACE_LOGGING 1040
#define ID_SETUP_REPLAY_STRING 1041
#define ID_RELOAD_REPLAY_STRING 1042
#define ID_CHANGE_LOG 1043
#define ID_CREATE_BUG_REPORT 1044
#define ID_TIPS 1045
#define ID_ABOUT 1046

///////////////////////////////////////////////////////////////////////////////
/// Class ComplxFrameDecl
///////////////////////////////////////////////////////////////////////////////
class ComplxFrameDecl : public wxFrame
{
	private:

	protected:
		wxMenuBar* menuBar;
		wxMenu* menuFile;
		wxMenu* menuView;
		wxMenu* menuViewShowAddresses;
		wxMenuItem* menuViewShowAddressesShowAll;
		wxMenuItem* menuViewShowAddressesShowNonZero;
		wxMenuItem* menuViewShowAddressesShowOnlyCodeData;
		wxMenuItem* menuViewShowAddressesCustom;
		wxMenu* menuViewDisassemble;
		wxMenuItem* menuViewBasic;
		wxMenuItem* menuViewNormal;
		wxMenuItem* menuViewHighLevel;
		wxMenu* menuViewLogLevel;
		wxMenuItem* menuViewLogLevelFatal;
		wxMenuItem* menuViewLogLevelDebug;
		wxMenuItem* menuViewLogLevelWarning;
		wxMenuItem* menuViewLogLevelInfo;
		wxMenuItem* menuViewLogLevelVerbose;
		wxMenu* menuControl;
		wxMenu* menuCycleSpeed;
		wxMenu* menuState;
		wxMenu* menuStateControl;
		wxMenu* menuStateControlMode;
		wxMenuItem* menuStateTrueTraps;
		wxMenuItem* menuStateInterrupts;
		wxMenuItem* menuStateStrictExecution;
		wxMenu* menuDebug;
		wxMenu* menuTest;
		wxMenu* menuHelp;
		wxPanel* toolbarPanel;
		wxAuiToolBar* m_auiToolBar1;
		wxButton* m_button1;
		wxButton* m_button2;
		wxButton* m_button3;
		wxButton* m_button4;
		wxButton* m_button5;
		wxButton* m_button6;
		wxButton* m_button7;
		wxButton* stopButton;
		wxAuiToolBar* m_auiToolBar2;
		wxButton* m_button8;
		wxButton* m_button9;
		wxPanel* memoryPanel;
		MemoryView* memoryView;
		wxPanel* statePanel;
		wxPropertyGridManager* statePropGridManager;
		wxPropertyGridPage* statePropGrid;
		wxPGProperty* registers;
		wxAuiNotebook* infoNotebook;
		wxPanel* consolePanel;
		wxTextCtrl* consoleText;
		wxTextCtrl* consoleInputText;
		wxPanel* warningPanel;
		wxTextCtrl* warningText;
		wxPanel* tracePanel;
		wxTextCtrl* traceText;
		wxPanel* loggingPanel;
		wxTextCtrl* loggingText;
		wxStatusBar* statusBar;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnLoad( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReload( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAdvancedLoad( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNewView( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateShowAddresses( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShowAddressesCustom( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDisassemble( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNormalDisassemble( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCDisassemble( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnInstructionHighlighting( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLogLevel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCycleSpeedCustom( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStep( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBack( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBackStep( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNextLine( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPrevLine( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRun( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRunFor( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRunAgain( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRewind( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFinish( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnControlModeSimple( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnControlModeAdvanced( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTrueTraps( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnInterrupts( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStrictExecution( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearConsole( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearConsoleInput( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUndoStack( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCallStack( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSubroutineCall( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBreakAndWatchpoints( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTemppoint( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBreakpoint( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnWatchpoint( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAdvancedBreakpoint( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTraceFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSetupReplayString( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReloadReplayString( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDocs( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChangeLog( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCreateBugReport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTips( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStepOver( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBackOver( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStepOut( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStop( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStateChange( wxPropertyGridEvent& event ) { event.Skip(); }


	public:
		wxString consoleInput;

		ComplxFrameDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Complx2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1024,800 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		wxAuiManager m_mgr;

		~ComplxFrameDecl();

};

///////////////////////////////////////////////////////////////////////////////
/// Class AdvancedLoadDialogDecl
///////////////////////////////////////////////////////////////////////////////
class AdvancedLoadDialogDecl : public wxDialog
{
	private:

	protected:
		wxPanel* m_panel2;
		wxStaticText* m_staticText1;
		wxFilePickerCtrl* assemblyFileCtrl;
		wxStaticText* m_staticText9;
		wxTextCtrl* replayStringCtrl;
		wxStaticText* m_staticText6;
		wxTextCtrl* consoleInputCtrl;
		wxStaticText* m_staticText0;
		wxChoice* regInitializerCtrl;
		wxTextCtrl* regFillCtrl;
		wxStaticText* m_staticText4;
		wxChoice* memInitializerCtrl;
		wxTextCtrl* memFillCtrl;
		wxStaticText* m_staticText5;
		wxTextCtrl* pcCtrl;
		wxStaticText* m_staticText7;
		wxCheckBox* trueTrapsCtrl;
		wxStaticText* m_staticText8;
		wxCheckBox* interruptsCtrl;
		wxStaticText* m_staticText81;
		wxCheckBox* strictExecutionCtrl;
		wxButton* m_button1;
		wxButton* m_button2;

		// Virtual event handlers, overide them in your derived class
		virtual void OnRegChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMemChoice( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxString replayString;
		wxString consoleInput;
		int regInitializer;
		wxString regFill;
		int memInitializer;
		wxString memFill;
		wxString pc;
		bool trueTraps;
		bool interrupts;
		bool strictExecution;

		AdvancedLoadDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Advanced Load"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 540,420 ), long style = wxDEFAULT_DIALOG_STYLE );
		~AdvancedLoadDialogDecl();

};

///////////////////////////////////////////////////////////////////////////////
/// Class MemoryViewFrameDecl
///////////////////////////////////////////////////////////////////////////////
class MemoryViewFrameDecl : public wxFrame
{
	private:

	protected:
		wxMenuBar* menu;
		wxMenu* menuView;
		wxMenu* menuViewShowAddresses;
		wxMenuItem* menuViewShowAddressesShowAll;
		wxMenuItem* menuViewShowAddressesShowNonZero;
		wxMenuItem* menuViewShowAddressesShowOnlyCodeData;
		wxMenuItem* menuViewShowAddressesCustom;
		wxMenu* menuViewDisassemble;
		wxMenuItem* menuViewBasic;
		wxMenuItem* menuViewNormal;
		wxMenuItem* menuViewHighLevel;
		wxPanel* memoryPanel;
		wxPanel* m_panel12;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_textCtrl11;
		wxStaticText* m_staticText10;
		wxChoice* m_choice3;
		MemoryView* memoryView;

		// Virtual event handlers, overide them in your derived class
		virtual void OnUpdateShowAddresses( wxCommandEvent& event ) = 0;
		virtual void OnShowAddressesCustom( wxCommandEvent& event ) = 0;
		virtual void OnDisassemble( wxCommandEvent& event ) = 0;
		virtual void OnNormalDisassemble( wxCommandEvent& event ) = 0;
		virtual void OnCDisassemble( wxCommandEvent& event ) = 0;
		virtual void OnInstructionHighlighting( wxCommandEvent& event ) = 0;
		virtual void OnName( wxCommandEvent& event ) = 0;
		virtual void OnTracking( wxCommandEvent& event ) = 0;


	public:
		wxString name;
		int tracking;

		MemoryViewFrameDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Memory View"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MemoryViewFrameDecl();

};

