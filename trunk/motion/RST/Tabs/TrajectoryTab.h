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

#ifndef __TRAJECTORY_TAB__
#define __TRAJECTORY_TAB__

#include "RSTTab.h"

class TrajectoryTab : public RSTTab {
public:
    TrajectoryTab(){};
    TrajectoryTab(wxWindow * parent, wxWindowID id = -1,
                const wxPoint & pos = wxDefaultPosition,
                const wxSize & size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL);
    virtual ~TrajectoryTab(){};
    
	void OnListBox(wxCommandEvent &evt);
    void OnSlider(wxCommandEvent &evt);
    void OnButton(wxCommandEvent &evt);
    void RSTStateChange();
    
	void addConfig();
    void removeConfig();

    void generateTrajectory();
    
    DECLARE_DYNAMIC_CLASS(TrajectoryTab)
    DECLARE_EVENT_TABLE()
    
    wxBoxSizer *overallSizer;
    
    wxStaticBox *leftBox;
    wxStaticBoxSizer *leftBoxSizer;
    
    wxStaticBox *rightBox;
    wxStaticBoxSizer *rightBoxSizer;
    
	wxListBox *configListBox;
	RSTSlider *timeSlider;
	RSTSlider *incrementSlider;
	wxListBox *splineType;
	
	// appears to be like a path - should check
	vector< vector<double> > configurationlist;
	vector<double> timelist;
	
    vector<double> linksPos;
    Robot* thisrobot; 
    Link* thislink;
	
	int configListSize;
	int splineTypeID;
	
	double sliderTime;
	double sliderIncrement;
	
};

#endif
