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

// true if data is available
bool checkPlanner() {
	if (planner == NULL) {
		cout << "Planner is NULL" << endl;
		return false;
	} else if (planner->rrt == NULL) {
		cout << "RRT is NULL!" << endl;
		return false;
	}
	return true;
}

void OptimizationTab::executeOptimize() {
	cout << "Performing Optimization..." << endl;
	if(!checkPlanner()) return;
	// fetch the path from planner
	vector<vector<double> >& path = planner->rrt->path;

	// use optimization function
	if (optimizer.optimize(path))
		cout << "Optimization complete!" << endl;
	else
		cout << "Optimization failed!" << endl;
}

void OptimizationTab::setOriginalPath() {
	if(!checkPlanner()) return;
	//planner->rrt->path = optimizer.getOriginal();
	SetTimeline(optimizer.getOriginal());
	cout << "Current path is now the original path" << endl;
}

void OptimizationTab::setOptimizedPath() {
	if(!checkPlanner()) return;
	//planner->rrt->path = optimizer.getOptimized();
	SetTimeline(optimizer.getOptimized());
	cout << "Current path is now the optimized path" << endl;
}

// sets the value of a path to a particular value
void OptimizationTab::SetTimeline(const Path_t& path) {

	// set the timeline size - just default, rather than pulling from the GUI
	double T = 5.0;

	// find the number of steps
	int numsteps = path.size();

	// find the time increments between steps
	double increment = T/(double)numsteps;

	cout << "Updating Timeline - Increment: " << increment << " Total T: " << T << " Steps: " << numsteps << endl;

	// initialize the RSTFrame timer
	frame->InitTimer(string("RRT_Plan"),increment);

	int robotID = 0; // just assume this is robot zero, no other robots available

	// copy in the solved path to the main timeline
	for(int i=0; i < numsteps; i++){
		// loop over number of links and copy them in one by one
		for(int l=0; l < planner->numLinks; l++){
			world->robots[robotID]->activeLinks[l]->jVal = path[i][l];
		}
		// update the world
		world->updateRobot(world->robots[robotID]);
		// add a time slice
		frame->AddWorld(world);
	}
}


// This function is called when an object is selected in the Tree View or other
// global changes to the RST world. Use this to capture events from outside the tab.
void OptimizationTab::RSTStateChange() {
}


