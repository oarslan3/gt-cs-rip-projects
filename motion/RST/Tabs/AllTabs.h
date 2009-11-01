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
#ifndef ALLTABS_H
#define ALLTABS_H
// To avoid repetition, here's a macro for adding tabs.
// DO NOT CHANGE THIS:
#define ADD_TAB(className, tabLabel)								\
	{className* classNameObject = new className(tabView, wxID_ANY);	\
	tabView->AddPage(classNameObject, tabLabel, true);}
// Standard include for all graphical elements
#include "../GUI/GUI.h"


// TO ADD A TAB: just include the header file and the corresponding
// line in the function below.

#include "InspectorTab.h"
#include "PlanningTab.h"
#include "TrajectoryTab.h"
#include "TemplateTab.h"
#include "OptimizationTab.h"

void addAllTabs() {
	ADD_TAB(InspectorTab,wxT("Inspector"))
	ADD_TAB(PlanningTab,wxT("Planning"))
	ADD_TAB(TrajectoryTab,wxT("Trajectory"))
	ADD_TAB(TemplateTab,wxT("Tab Template"))
	ADD_TAB(OptimizationTab, wxT("Optimization"))

	tabView->SetSelection(0);
}

#endif
