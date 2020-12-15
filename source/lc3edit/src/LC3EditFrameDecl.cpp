///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "LC3EditFrameDecl.h"

///////////////////////////////////////////////////////////////////////////

LC3EditFrameDecl::LC3EditFrameDecl( wxDocManager* manager, wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDocMDIParentFrame( manager, parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	fileNotebook = new wxFlatNotebook(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFNB_FANCY_TABS|wxFNB_NO_X_BUTTON|wxFNB_SMART_TABS|wxFNB_X_ON_TAB);

	fileNotebook->SetCustomizeOptions( wxFNB_CUSTOM_ALL );
	fileNotebook->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );


	bSizer2->Add( fileNotebook, 1, wxEXPAND | wxALL, 5 );


	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	statusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	m_menubar1 = new wxMenuBar( 0 );
	file = new wxMenu();
	wxMenuItem* fileNew;
	fileNew = new wxMenuItem( file, wxID_NEW, wxString( _("&New") ) + wxT('\t') + wxT("Ctrl+N"), wxEmptyString, wxITEM_NORMAL );
	file->Append( fileNew );

	wxMenuItem* fileOpen;
	fileOpen = new wxMenuItem( file, wxID_OPEN, wxString( _("&Open") ) + wxT('\t') + wxT("Ctrl+O"), wxEmptyString, wxITEM_NORMAL );
	file->Append( fileOpen );

	file->AppendSeparator();

	wxMenuItem* fileSave;
	fileSave = new wxMenuItem( file, wxID_SAVE, wxString( _("&Save") ) + wxT('\t') + wxT("Ctrl+S"), wxEmptyString, wxITEM_NORMAL );
	file->Append( fileSave );

	wxMenuItem* fileSaveAs;
	fileSaveAs = new wxMenuItem( file, wxID_SAVEAS, wxString( _("Save As...") ) + wxT('\t') + wxT("F12"), wxEmptyString, wxITEM_NORMAL );
	file->Append( fileSaveAs );

	file->AppendSeparator();

	wxMenuItem* fileExit;
	fileExit = new wxMenuItem( file, wxID_EXIT, wxString( _("Exit") ) + wxT('\t') + wxT("Alt+F4"), wxEmptyString, wxITEM_NORMAL );
	file->Append( fileExit );

	m_menubar1->Append( file, _("&File") );

	edit = new wxMenu();
	wxMenuItem* editUndo;
	editUndo = new wxMenuItem( edit, wxID_UNDO, wxString( _("&Undo") ) + wxT('\t') + wxT("Ctrl+Z"), wxEmptyString, wxITEM_NORMAL );
	edit->Append( editUndo );

	wxMenuItem* editRedo;
	editRedo = new wxMenuItem( edit, wxID_REDO, wxString( _("&Redo") ) + wxT('\t') + wxT("Ctrl+Y"), wxEmptyString, wxITEM_NORMAL );
	edit->Append( editRedo );

	m_menubar1->Append( edit, _("&Edit") );

	view = new wxMenu();
	m_menubar1->Append( view, _("&View") );

	run = new wxMenu();
	m_menubar1->Append( run, _("&Run") );

	help = new wxMenu();
	m_menubar1->Append( help, _("&Help") );

	this->SetMenuBar( m_menubar1 );


	this->Centre( wxBOTH );
}

LC3EditFrameDecl::~LC3EditFrameDecl()
{
}
