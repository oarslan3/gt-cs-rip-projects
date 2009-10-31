/*
 * OptimizationTab.cpp
 *
 *  Created on: Oct 30, 2009
 *      Author: alexgc
 */

#include <wx/wx.h>
#include "../GUI/Viewer.h"
#include "../GUI/GUI.h"
#include "../GUI/RSTSlider.h"
#include "../GUI/RSTFrame.h"
#include <iostream>
using namespace std;
#include "../Tools/World.h"
#include "../Tools/Robot.h"
#include "../Tools/Link.h"
#include "../Tools/Object.h"
#include "../Tools/Constants.h"
#include "PlanningTab.h"

#include "OptimizationTab.h"

// Control IDs (used for event handling - be sure to start with a non-conflicted id)
enum optimizationTabEvents {
	button_ExecuteOptimization = 45100,
			button_SetOriginalPath,
			button_SetOptimizedPath
};

//Add a handler for slider changes
BEGIN_EVENT_TABLE(OptimizationTab, wxPanel)
EVT_COMMAND (wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, OptimizationTab::OnButton)
END_EVENT_TABLE ()

// Class constructor for the tab: Each tab will be a subclass of RSTTab
IMPLEMENT_DYNAMIC_CLASS(OptimizationTab, RSTTab)

OptimizationTab::OptimizationTab(wxWindow *parent, const wxWindowID id,
		const wxPoint& pos, const wxSize& size, long style) :
			RSTTab(parent, id, pos, size, style) {
	sizerFull = new wxBoxSizer(wxHORIZONTAL);

	// add the three buttons
	sizerFull->Add(new wxButton(this, button_ExecuteOptimization, wxT("Execute Optimization")), 0, wxALL, 1);
	sizerFull->Add(new wxButton(this, button_SetOriginalPath, wxT("Set Original Path")), 0, wxALL, 1);
	sizerFull->Add(new wxButton(this, button_SetOptimizedPath, wxT("Set Optimized Path")), 0, wxALL, 1);

	SetSizer(sizerFull);
}


void OptimizationTab::OnButton(wxCommandEvent &evt) {
	int button_num = evt.GetId();
	switch (button_num) {
	case button_ExecuteOptimization:
		// perform optimization
		executeOptimize();
		break;
	case button_SetOptimizedPath:
		// swap the path in the planner with the optimized path
		setOptimizedPath();
		break;
	case button_SetOriginalPath:
		// swap the current path with the optimized path
		setOriginalPath();
		break;
	}
}

void OptimizationTab::executeOptimize() {
	cout << "Performing Optimization..." << endl;
	if (planner == NULL || planner->rrt == NULL) {
		cout << "Must perform planning before attempting to optimize!" << endl;
		return;
	}
	// fetch the path from planner
	vector<vector<double> >& path = planner->rrt->path;

	// use optimization function
	if (optimizer.optimize(path))
		cout << "Optimization complete!" << endl;
	else
		cout << "Optimization failed!" << endl;
}

void OptimizationTab::setOriginalPath() {
	if (planner == NULL || planner->rrt == NULL) {
		cout << "Must perform planning before attempting to optimize!" << endl;
		return;
	}
	planner->rrt->path = optimizer.getOriginal();
	cout << "Current path is now the original path" << endl;
}

void OptimizationTab::setOptimizedPath() {
	if (planner == NULL || planner->rrt == NULL) {
		cout << "Must perform planning before attempting to optimize!" << endl;
		return;
	}
	planner->rrt->path = optimizer.getOptimized();
	cout << "Current path is now the optimized path" << endl;
}


// This function is called when an object is selected in the Tree View or other
// global changes to the RST world. Use this to capture events from outside the tab.
void OptimizationTab::RSTStateChange() {
	//	if(selectedTreeNode==NULL){
	//		return;
	//	}
	//
	//	Object* o;
	//	Robot* r;
	//	Link* l;
	//	string statusBuf;
	//	string buf, buf2;
	//	switch (selectedTreeNode->dType) {
	//	case Return_Type_Object:
	//		o = (Object*) (selectedTreeNode->data);
	//		statusBuf = " Selected Object: " + o->name;
	//		buf = "You clicked on object: " + o->name;
	//		sampleText1->SetLabel(wxString(buf.c_str(), wxConvUTF8));
	//		sampleText2->Hide();
	//
	//		break;
	//	case Return_Type_Robot:
	//		r = (Robot*) (selectedTreeNode->data);
	//		statusBuf = " Selected Robot: " + r->name;
	//		buf = "You clicked on robot: " + r->name;
	//		sampleText2->SetLabel(wxString(buf.c_str(), wxConvUTF8));
	//		sampleText1->Hide();
	//
	//		break;
	//	case Return_Type_Link:
	//		l = (Link*) (selectedTreeNode->data);
	//		statusBuf = " Selected Link: " + l->name + " of Robot: "
	//				+ l->robot->name;
	//		buf = " Link: " + l->name + " of Robot: " + l->robot->name;
	//		// Do something here if you want to.  you get the idea...
	//
	//		break;
	//    default:
	//        fprintf(stderr, "someone else's problem.");
	//        assert(0);
	//        exit(1);
	//	}
	//frame->SetStatusText(wxString(statusBuf.c_str(), wxConvUTF8));
	//sizerFull->Layout();
}


