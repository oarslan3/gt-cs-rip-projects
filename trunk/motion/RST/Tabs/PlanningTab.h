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

#ifndef PLANNING_TAB
#define PLANNING_TAB

#include "../GUI/GUI.h"
#include "RSTTab.h"
#include "../Tools/Matrix.h"
#include "../Tools/Robot.h"
#include "../Tools/Planner.h"

class PlanningTab : public RSTTab
{
public:
	PlanningTab(){};
    PlanningTab(wxWindow * parent, wxWindowID id = -1,
             const wxPoint & pos = wxDefaultPosition,
             const wxSize & size = wxDefaultSize,
             long style = wxTAB_TRAVERSAL);
	virtual ~PlanningTab(){}

	int rrtStyle;
	bool greedyMode;
	bool connectMode;
	bool showProg;
	//Planner *planner;

	int robotID;

	wxTextCtrl *timeText;

	rstate startConf;
	rstate goalConf;

	// public vars to capture external selection stuff (should move these higher somethime)
	Object* selectedObject;
	Robot* selectedRobot;
	Link* selectedLink;

	void OnSlider(wxCommandEvent &evt);
	void OnRadio(wxCommandEvent &evt);
	void OnButton(wxCommandEvent &evt);
	void OnCheckBox(wxCommandEvent &evt);
	void SetTimeline();
	void RSTStateChange();

	DECLARE_DYNAMIC_CLASS(PlanningTab)
	DECLARE_EVENT_TABLE()
};

#endif
