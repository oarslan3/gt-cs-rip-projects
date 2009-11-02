//---------------------------------------------------------------------
//  Copyright (c) 2009 Mike Stilman
//  All Rights Reserved.
//
//  Permission to duplicate or use this software in whole or in part
//  is only granted by consultation with the author.
//
//    Mike Stilman              mstilman@cc.gatech.edu
//
//	  Robotics and Intelligent Machines
//    Georgia Tech
//--------------------------------------------------------------------

#include "GUI.h"
#include "RSTFrame.h"
#include "Viewer.h"
#include "TreeView.h"

#include "../Tabs/AllTabs.h"
#include "../Tabs/RSTTab.h"
#include "RSTSlider.h"
#include "../Tools/World.h"
#include "RSTimeSlice.h"
#include "RSTFrame.h"

#include "icons/open.xpm"
#include "icons/save.xpm"
#include "icons/redo.xpm"
#include "icons/up.xpm"
#include "icons/anchor.xpm"
#include "icons/asterisk.xpm"
#include "icons/camera.xpm"
#include "icons/film.xpm"
#include "icons/clock.xpm"


#include <iostream>
#include <fstream>

using namespace std;
#define ID_TOOLBAR 1257
#define ID_TIMESLIDER 1258

enum toolNums{
    Tool_open= 1262,
    Tool_save= 1263,
    Tool_quickload = 1264,
	Tool_linkorder = 1265,
	Tool_checkcollisions = 1266,
	Tool_screenshot = 1267,
	Tool_movie = 1268
};

extern bool check_for_collisions;

//wxSTD_MDIPARENTFRAME ICON wxICON(ROBOT_xpm)

RSTFrame::RSTFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title)
{
	tPrecision = 1000;
	tMax = 5;
	tMax = 0;
	InitTimer("",0);

    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
	wxMenu *settingsMenu = new wxMenu;
	wxMenu *bgMenu = new wxMenu;
	//wxMenu *saveMenu = new wxMenu;
	fileMenu->Append(MenuLoad, wxT("L&oad\tAlt-O"));
	fileMenu->Append(MenuQuickLoad, wxT("Q&uickLoad\tAlt-Shift-Q"));
	fileMenu->Append(MenuSaveScene, wxT("Save Scene"));
	//saveMenu->Append(MenuSaveRobot, wxT("Save Robot"));
	//fileMenu->AppendSubMenu(saveMenu, wxT("S&ave\tAlt-S"));
	fileMenu->Append(MenuClose, wxT("C&lose\tAlt-C"));
	fileMenu->AppendSeparator();
	fileMenu->Append(MenuQuit, wxT("E&xit\tAlt-Q"));

	bgMenu->Append(MenuBgWhite, wxT("White"));
	bgMenu->Append(MenuBgBlack, wxT("Black"));
	settingsMenu->AppendSubMenu(bgMenu, wxT("Background"));

	helpMenu->Append(MenuAbout, wxT("&About...\tF1"), wxT("Show about dialog"));
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, wxT("&File"));
	menuBar->Append(settingsMenu, wxT("&Settings"));
    menuBar->Append(helpMenu, wxT("&Help"));
    SetMenuBar(menuBar);

	toolBarBitmaps[0] = wxIcon(open_xpm);
	toolBarBitmaps[1] = wxIcon(save_xpm);
	toolBarBitmaps[2] = wxIcon(redo_xpm);
	toolBarBitmaps[3] = wxIcon(anchor_xpm);
	toolBarBitmaps[4] = wxIcon(asterisk_xpm);
	toolBarBitmaps[5] = wxIcon(camera_xpm);
	toolBarBitmaps[6] = wxIcon(film_xpm);

	wxBitmap clockBmp = wxBitmap(clock_xpm);

	filebar = new wxToolBar(this,ID_TOOLBAR,wxPoint(0, 0), wxSize(prefTreeViewWidth+50, toolBarHeight), wxTB_HORIZONTAL);

	filebar->SetToolBitmapSize(wxSize(16, 16));
    filebar->AddTool(wxID_OPEN, _T("Open"),toolBarBitmaps[0], toolBarBitmaps[0], wxITEM_NORMAL, _T("Open .rscene file (Alt-O)"));
	filebar->AddTool(wxID_SAVE, _T("Save"),toolBarBitmaps[1], toolBarBitmaps[1], wxITEM_NORMAL,  _T("Save world to .rscene file (Alt-S)"));
	filebar->AddSeparator();
	filebar->AddTool(Tool_quickload, _T("Quick Load"),toolBarBitmaps[2], toolBarBitmaps[2], wxITEM_NORMAL, _T("Load last viewed scene (Alt-Shift-Q)"));
	filebar->AddSeparator();
	filebar->AddTool(Tool_screenshot, _T("Screenshot"),toolBarBitmaps[5], toolBarBitmaps[5], wxITEM_NORMAL, _T("Export screenshot"));
	filebar->AddTool(Tool_movie, _T("Movie"),toolBarBitmaps[6], toolBarBitmaps[6], wxITEM_NORMAL, _T("Export film sequence"));


	//timeSlider = new RSTSlider(clockBmp,0,1000,100,0,100,500,this,ID_TIMESLIDER,true);
	wxPanel* timePanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, 0);

#ifdef WIN32 // For windows use a thicker slider - it looks nice
	timeTrack = new wxSlider(timePanel,1009,0,0,1000,wxDefaultPosition, wxSize(30,100), wxSL_BOTH | wxSL_VERTICAL | wxALIGN_CENTRE);
#else
	timeTrack = new wxSlider(timePanel,1009,0,0,1000,wxDefaultPosition, wxSize(16,100), wxSL_BOTH | wxSL_VERTICAL | wxALIGN_CENTRE);
#endif
	wxStaticBitmap *timeButton = new wxStaticBitmap(timePanel, -1, clockBmp, wxDefaultPosition, wxSize(16,16), wxALIGN_CENTRE);
	wxSizer *sizerTime = new wxBoxSizer(wxVERTICAL);
	sizerTime->Add(timeButton,0 , wxALIGN_CENTRE | wxEXPAND | wxALL, 2);
	sizerTime->Add(timeTrack,1 , wxALIGN_CENTRE | wxEXPAND | wxALL, 2);


	optionbar = new wxToolBar(this,ID_TOOLBAR,wxPoint(0, 0), wxSize(prefTreeViewWidth, toolBarHeight), wxTB_HORIZONTAL);
	wxBitmap optionBarBitmaps[2];
	optionbar->SetToolBitmapSize(wxSize(16, 16));
    optionbar->AddTool(Tool_linkorder, _T("Link Order"),toolBarBitmaps[3], toolBarBitmaps[3], wxITEM_CHECK, _T("Fix link position"));
    optionbar->AddTool(Tool_checkcollisions, _T("Check Collisions"),toolBarBitmaps[4], toolBarBitmaps[4], wxITEM_CHECK, _T("Click to toggle collision detection"));



	timeText = new wxTextCtrl(optionbar,1008,wxT(" 0.00"),wxDefaultPosition,wxSize(50,20),wxTE_PROCESS_ENTER | wxTE_RIGHT);
	optionbar->AddSeparator();
	optionbar->AddControl(timeText);

    CreateStatusBar(2);
    SetStatusText(wxT("RST Loading..."));

	// Create sizers - these will manage the layout/resizing of the frame elements
	wxSizer *sizerFull = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerRightH = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *sizerBottom = new wxBoxSizer(wxHORIZONTAL);

	treeView = new TreeView(this, TreeViewHandle, wxPoint(0, 0), wxSize(prefTreeViewWidth, prefViewerHeight-2*toolBarHeight),
                            wxTR_LINES_AT_ROOT | wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT | wxSUNKEN_BORDER);

	// Adding a backPanel to the lower half of the window covers the generic "inner grey"
	// with a forms-like control color. The tabView is added to the backPanel
	backPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, 0);
	tabView = new wxNotebook(backPanel, wxID_ANY, wxPoint(0, 0), wxSize(prefTreeViewWidth+prefViewerWidth, prefTabsHeight),  wxNB_NOPAGETHEME | wxNB_TOP);

	addAllTabs();


	// Start OpenGL and do various hacks to make it work cross platform
	#ifndef WIN32 // Weird hack to make wxWidgets work in Linux
	Show();
	#endif
	viewer = new Viewer(this, -1, wxPoint(0, 0), wxSize(prefViewerWidth, prefViewerHeight), wxFULL_REPAINT_ON_RESIZE | wxSUNKEN_BORDER);
	#ifdef WIN32  // Weird hack to make wxWidgets work with VC++ debug
	viewer->MSWSetOldWndProc((WXFARPROC)DefWindowProc);
	#endif


	// Add elements to the sizers, setting the proportion flag to 1 and using the
    // wxEXPAND flag to ensure that the Viewer fills the entire sizer (subject to constraints)
	sizerFull->Add(sizerTop, 1,  wxEXPAND | wxALL,  0 );
	sizerTop->Add(viewer, 1, wxALIGN_LEFT | wxEXPAND | wxALL, 0);

	// Set the proportion flag to 0 (for wxHORIZONTAL sizer) to fix the width to its minimal size
	sizerTop->Add(sizerRight, 0, wxALIGN_RIGHT | wxEXPAND | wxALL, 0);

	sizerRight->Add(filebar,0, wxALL, 0);
	sizerRight->Add(sizerRightH, 1 , wxEXPAND | wxALL, 0);
	sizerRight->Add(optionbar,0, wxALIGN_LEFT | wxEXPAND | wxALL, 0);

	sizerRightH->Add((wxTreeCtrl*)treeView, 1, wxALIGN_CENTRE | wxALL | wxEXPAND, 0);
	sizerRightH->Add(timePanel,0, wxALIGN_LEFT | wxEXPAND | wxALL, 0);

	// Place the back panel on the lower part of the window (0 fixes the height for wxVERTICAL sizer)
	sizerFull->Add(backPanel, 0, wxEXPAND | wxALL, 0);

	// Add another sizer to stretch the tabs over the back panel while keeping a 3 pixel border
	sizerBottom->Add(tabView, 1, wxALIGN_LEFT | wxEXPAND | wxALL, 3);
	backPanel->SetSizer(sizerBottom);
	timePanel->SetSizer(sizerTime);

	SetSizer(sizerFull);
	sizerFull->SetSizeHints( this );
	filebar->Realize();
	optionbar->Realize();

	Show();
	viewer->Freeze();
	viewer->InitGL();
	viewer->Thaw();

	viewer->setClearColor(1,1,1,1);
	viewer->UpdateCamera();
}

// Checks if the input file is a .rscene file or not and then writes the scene to it.
void RSTFrame::OnSaveScene(wxCommandEvent& event) {
	wxString filepath;
	string filename;
	size_t endpath;
	wxFileDialog *SaveDialog = new wxFileDialog(this, _("Save File As"), wxT("../scene/"), wxT(""),
			_("*.rscene"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	if (SaveDialog->ShowModal() == wxID_OK) {
			filepath = SaveDialog->GetPath();
			filename = string(filepath.mb_str());
			endpath = filename.find(".rscene");
			if(endpath == -1) filename += ".rscene";
			world->Save(filename);
			wxString filename_string(filename.c_str(), wxConvUTF8);
			saveText(filename_string,".lastload");
		}
}



void RSTFrame::OnSaveRobot(wxCommandEvent& event) {

}



void RSTFrame::OnLoad(wxCommandEvent& event){
	wxString filename = wxFileSelector(wxT("Choose a file to open"),wxT("../scene/"),wxT(""),wxT(""), // -- default extension
                                       wxT("*.rscene"), 0);
	if ( !filename.empty() )
		DoLoad(string(filename.mb_str()));
}

void RSTFrame::OnQuickLoad(wxCommandEvent& event){
	ifstream lastloadFile;
	lastloadFile.open(".lastload", ios::in);
	if(lastloadFile.fail()){
		cout << "No previously loaded files" << endl;
		return;
	}
	string line;
	getline(lastloadFile,line);
	lastloadFile.close();
	DoLoad(line);
}

void RSTFrame::DoLoad(string filename){
	DeleteWorld();
	world = new World();
	world->Load(string(filename));

	//UpdateTreeView();
	cout << "Done Loading." << endl;
	treeView->CreateFromWorld();
	cout << "Done Updating TreeView." << endl;
	viewer->ResetGL();
	SetStatusText(wxT("Done Loading"));

	//Extract path to executable & save "lastload" there
	cout << "Saving " << filename << " to .lastload file" << endl;
    wxString filename_string(filename.c_str(), wxConvUTF8);
	saveText(filename_string,".lastload");
	viewer->ResetGL();
	selectedTreeNode = 0;
	treeView->ExpandAll();
	updateAllTabs();
}

void RSTFrame::DeleteWorld(){
	InitTimer("",0);
	for(size_t i=0; i<timeVector.size(); i++){
		if(timeVector[i]!=NULL)
			delete timeVector[i];
	}
	timeVector.clear();
	if(world !=NULL){
		World* w = world;
		world = 0;
		selectedTreeNode = 0;
		treeView->DeleteAllItems();
		w->DeleteModels();
		delete w;
	}
}


void RSTFrame::OnToolOrder(wxCommandEvent& event){
	reverseLinkOrder = !reverseLinkOrder;
}

void RSTFrame::OnToolCheckColl(wxCommandEvent& event){
	// toggle collision detection
	check_for_collisions = !check_for_collisions;
}

void RSTFrame::OnToolMovie(wxCommandEvent& event){
    wxString dirname = wxDirSelector(wxT("Choose input directory")); // , "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST

    if ( dirname.empty() ){ // filename
            cout << "No Directory Selected" << endl;
            return;
    }

    string path = string(dirname.mb_str());

    char *buf = new char[1000];
    int w,h;

    double step = .03333/tIncrement;
    int count = 0;

#ifdef WIN32
        ::CreateDirectory(path.c_str(),NULL);
#else

#endif

        for(double s=0; s<timeVector.size(); s+= step){
                int i = (int)s;

                timeVector[i]->SetToWorld(world);
                viewer->UpdateCamera();
                wxYield();

                wxClientDC dc(viewer);
                dc.GetSize(&w, &h);
                wxMemoryDC memDC;
                wxBitmap memBmp(w, h);
                memDC.SelectObject(memBmp);

#ifdef WIN32
                // The fast way (currently only works in windowsXP) :
                memDC.Blit(0,0, w,h, &dc, 0,0);
#else
                // The slow way:
                unsigned char* imageData = (unsigned char*)malloc(w*h*3);
                glReadPixels(0, 0, w-1, h-1, GL_RGB, GL_UNSIGNED_BYTE, imageData);

                int wIndex = 0;
                int hIndex = 0;
                for (int i=0; i < w*h*3; i+=3, ++wIndex) {
                        if(wIndex >= w) {
                                wIndex=0;
                                hIndex++;
                        }
                        // vertically flip the image
                        int hout = -hIndex+h-1;

                        memDC.SetPen(wxPen(wxColor((int)imageData[i], (int)imageData[i+1], (int)imageData[i+2]), 1));
                        memDC.DrawCircle(wIndex, hout, 1);
                }
                free(imageData);
#endif

                memDC.SelectObject(wxNullBitmap);
                sprintf(buf, "%s/%06d.png",path.c_str(),count);

                wxString fname = wxString(buf,wxConvUTF8);
                cout << "Saving:" << buf << ":" << endl;
                memBmp.SaveFile(fname, wxBITMAP_TYPE_PNG);

                count++;
        }

    delete buf;
        event.Skip();
}

void RSTFrame::OnToolScreenshot(wxCommandEvent& event){
	wxYield();
	int w,h;

	wxClientDC dc(viewer);
	dc.GetSize(&w, &h);
	wxMemoryDC memDC;
	wxBitmap memBmp(w, h);
	memDC.SelectObject(memBmp);
	memDC.Blit(0,0, w,h, &dc, 0,0);
    memDC.SelectObject(wxNullBitmap);
	wxString fname(wxT("screenGL.png"));
	memBmp.SaveFile(fname, wxBITMAP_TYPE_PNG);

	wxClientDC dc2(this);
	dc2.GetSize(&w, &h);
	wxMemoryDC memDC2;
	wxBitmap memBmp2(w, h);
	memDC2.SelectObject(memBmp2);
	memDC2.Blit(0,0, w,h, &dc2, 0,0);
	memDC2.SelectObject(wxNullBitmap);
	wxString fname2(wxT("screen.png"));
	memBmp2.SaveFile(fname2, wxBITMAP_TYPE_PNG);

	SetStatusText(wxT("Screenshot saved in: screen.png, screenGL.png"));
	event.Skip();
}

int RSTFrame::saveText(wxString scenepath, const char* llfile)
{
     try {
		 ofstream lastloadFile(llfile, ios::out);
		 if (lastloadFile) {
			 lastloadFile  << string(scenepath.mb_str()) << endl;
			 lastloadFile.close();
			 cout << "Saved" << endl;
			 return 0;
		 } else {
			 cout << "Error opening file: " << llfile << endl;
			 return -1;
		 }
     }catch (const std::exception& e) {
		 cout << "Shouldn't see this catch after migrating to wxWidgets... tell jon: " << e.what() << endl;
		 return 0;
     }
	 return 1;
}


void RSTFrame::OnClose(wxCommandEvent& event){
	DeleteWorld();
	viewer->UpdateCamera();
	//exit(0);
	//DeleteWorld();
}


void RSTFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	exit(0);
    //Close(true);
}

void RSTFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox(wxString::Format(wxT("RST: Humanoids @ GT \
                                      \n\n Mike Stilman\
                                      \n Saul Reynolds-Haertl\
                                      \n Jon Scholz\
									  \n Pushkar Kolhe\
                                      \n Venkat Subramanian\
                                      \n Jiuguang Wang\
                                      \n Misha Novitzky\
                                      \n Neil Dantam"),
                                  wxVERSION_STRING,
                                  wxGetOsDescription().c_str()
                                  ),wxT("Info"), wxOK | wxICON_INFORMATION,this);
}

void RSTFrame::onTVChange(wxTreeEvent& event){
	updateAllTabs();
}

// Go through all the tabs and indicate a state change
void RSTFrame::updateAllTabs(){
	int type = 0;
	wxCommandEvent evt(wxEVT_RST_STATE_CHANGE,GetId());
	evt.SetEventObject(this);
	evt.SetClientData((void*)&type);
	size_t numPages = tabView->GetPageCount();
	for(size_t i=0; i< numPages; i++){
		RSTTab* tab = (RSTTab*)tabView->GetPage(i);
		tab->RSTStateChange();
	}
}



void RSTFrame::setTimeValue(double value, bool sendSignal){
	tCurrent = value;
	timeTrack->SetValue(value * tPrecision);
	updateTimeValue(value, sendSignal);
}

void RSTFrame::updateTimeValue(double value, bool sendSignal){
	if(tIncrement == 0) return;
	char buf[100];
	sprintf(buf, "%6.2f", tCurrent);

	wxString posString = wxString(buf,wxConvUTF8);
	timeText->ChangeValue(posString);

	unsigned int timeIndex = (int)((tCurrent/tMax)*((double)timeVector.size()));
	if(timeIndex > timeVector.size()-1) timeIndex = timeVector.size()-1;
	timeVector[timeIndex]->SetToWorld(world);
	viewer->UpdateCamera();

	if(sendSignal) updateAllTabs();
}

void RSTFrame::OnTimeScroll(wxScrollEvent &evt){
	tCurrent = (double)(evt.GetPosition())/(double)tPrecision;
	//updateTimeValue(tCurrent);
	updateTimeValue(tCurrent,true);
}

void RSTFrame::AddWorld(World* world){
	RSTimeSlice* tsnew = new RSTimeSlice(world);
	timeVector.push_back(tsnew);
	tMax += tIncrement;
	timeTrack->SetRange(0, tMax * tPrecision);
}

void RSTFrame::InitTimer(string title, double period){
	for(size_t i=0; i<timeVector.size(); i++){
		delete timeVector[i];
	}
	tMax = 0;
	timeVector.clear();
	tIncrement = period;
}

void RSTFrame::OnTimeEnter(wxCommandEvent &evt){
	double p;
	timeText->GetValue().ToDouble(&p);
	if(p < 0) p = 0;
	if(p > tMax) p = tMax;
	setTimeValue(p);
	setTimeValue(p,true);
}

void RSTFrame::OnWhite(wxCommandEvent &evt){
	viewer->setClearColor(0,0,0,1);
	viewer->UpdateCamera();
}

void RSTFrame::OnBlack(wxCommandEvent &evt){
	viewer->setClearColor(1,1,1,1);
	viewer->UpdateCamera();
}



BEGIN_EVENT_TABLE(RSTFrame, wxFrame)
EVT_COMMAND_SCROLL(1009, RSTFrame::OnTimeScroll)
EVT_TEXT_ENTER(1008, RSTFrame::OnTimeEnter)

EVT_MENU(MenuSaveScene,  RSTFrame::OnSaveScene)
EVT_MENU(MenuSaveRobot,  RSTFrame::OnSaveRobot)
EVT_MENU(MenuLoad,  RSTFrame::OnLoad)
EVT_MENU(MenuQuickLoad,  RSTFrame::OnQuickLoad)
EVT_MENU(wxID_CLOSE,  RSTFrame::OnClose)
EVT_MENU(MenuClose,  RSTFrame::OnClose)
EVT_MENU(MenuQuit,  RSTFrame::OnQuit)
EVT_MENU(MenuAbout, RSTFrame::OnAbout)

EVT_MENU(MenuBgWhite,  RSTFrame::OnBlack)
EVT_MENU(MenuBgBlack, RSTFrame::OnWhite)

EVT_MENU(wxID_OPEN, RSTFrame::OnLoad)
EVT_MENU(Tool_quickload, RSTFrame::OnQuickLoad)
EVT_MENU(wxID_SAVE, RSTFrame::OnSaveScene)

EVT_MENU(Tool_linkorder, RSTFrame::OnToolOrder)
EVT_MENU(Tool_checkcollisions, RSTFrame::OnToolCheckColl)
EVT_MENU(Tool_screenshot, RSTFrame::OnToolScreenshot)
EVT_MENU(Tool_movie, RSTFrame::OnToolMovie)

EVT_TREE_SEL_CHANGED(TreeViewHandle,RSTFrame::onTVChange)

//	EVT_BUTTON (BUTTON_Hello, RSTFrame::OnQuit )
END_EVENT_TABLE()


