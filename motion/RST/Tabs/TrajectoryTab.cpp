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

#include "../GUI/Viewer.h"
#include "../GUI/GUI.h"
#include "../GUI/RSTSlider.h"
#include "../GUI/RSTFrame.h"
#include "../Tools/World.h"
#include "../Tools/Robot.h"
#include "../Tools/Link.h"
#include "../Tools/Object.h"
#include "../Tools/Constants.h"
#include "../Tools/Trajectory.h"

#include "TrajectoryTab.h"

#include <wx/wx.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

// Control IDs
enum planTabEvents {
    button_AddConfig,
    button_RemoveConfig,
    button_GenerateTrajectory,
    listBox_ConfigListBox,
    listBox_SplineType,
    slider_Time,
    slider_Increment
};

// Handle events
BEGIN_EVENT_TABLE(TrajectoryTab, wxPanel)
    EVT_COMMAND (wxID_ANY, wxEVT_COMMAND_LISTBOX_SELECTED, TrajectoryTab::OnListBox)
    EVT_COMMAND (wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, TrajectoryTab::OnButton)
    EVT_COMMAND (wxID_ANY, wxEVT_RST_SLIDER_CHANGE, TrajectoryTab::OnSlider)
END_EVENT_TABLE()

// Class constructor for the tab
IMPLEMENT_DYNAMIC_CLASS(TrajectoryTab, RSTTab)

TrajectoryTab::TrajectoryTab(wxWindow *parent, const wxWindowID id, 
                             const wxPoint& pos, const wxSize& size, 
                             long style) : 
RSTTab(parent, id, pos, size, style) {
    
    configListSize = 0;
	
    // Left and right box
    leftBox = new wxStaticBox(this, -1, wxT("Configure"));
    leftBoxSizer = new wxStaticBoxSizer(leftBox, wxHORIZONTAL);
    rightBox = new wxStaticBox(this, -1, wxT("Execute"));
    rightBoxSizer = new wxStaticBoxSizer(rightBox, wxVERTICAL);
	
    // Overall sizer
    overallSizer = new wxBoxSizer(wxHORIZONTAL);
    overallSizer->Add(leftBoxSizer, 2, wxEXPAND | wxALL, 6);
    overallSizer->Add(rightBoxSizer, 1, wxEXPAND | wxALL, 6);
	
    // Configurations list
    wxBoxSizer *col1Sizer = new wxBoxSizer(wxVERTICAL);
    configListBox = new wxListBox(this, listBox_ConfigListBox, wxDefaultPosition, wxSize(130, 110)); 
    col1Sizer->Add(configListBox, 0, wxALL, 1);
    leftBoxSizer->Add(col1Sizer, 1, wxALIGN_NOT);
	
    // Create sizer for start buttons in 2nd column
    wxBoxSizer *col2Sizer = new wxBoxSizer(wxVERTICAL);
	
    // Time slider
    timeSlider = new RSTSlider( "Time", 0, 5, 30, 0, .2, 0, this, slider_Time);
    col2Sizer->Add(timeSlider, 2, wxGROW, 0);
	
    // Increment slider
	
    incrementSlider = new RSTSlider( "Increment", 0, 1, 30, 0, .2, 0, this, slider_Time);
    col2Sizer->Add(incrementSlider, 2, wxGROW, 0);
	
    // Add / remove buttons
	
    col2Sizer->Add(new wxButton(this, button_AddConfig, wxT("&Add Config")), 0, wxALL, 1);
    col2Sizer->Add(new wxButton(this, button_RemoveConfig, wxT("&Remove Config")), 0, wxALL, 1);
    leftBoxSizer->Add(col2Sizer, 2, wxALIGN_NOT);

    // Create sizer for goal buttons in 3rd column
    wxBoxSizer *col3Sizer = new wxBoxSizer(wxVERTICAL);
    leftBoxSizer->Add(col3Sizer, 1, wxALIGN_NOT);

    // Right box
    splineType = new wxListBox(this, listBox_SplineType, wxDefaultPosition, wxDefaultSize); 
    wxString normal_str("Normal", wxConvUTF8);
    wxString clamped_str("Clamped", wxConvUTF8);
    splineType->InsertItems(1, &normal_str, 0);
    splineType->InsertItems(1, &clamped_str, 1);
	
    rightBoxSizer->Add(splineType, 1, wxGROW, 0);
	
    rightBoxSizer->Add(new wxButton(this, button_GenerateTrajectory, wxT("&Generate Trajectory")), 1, wxGROW);

    SetSizer(overallSizer);
}

//Handle ListBox events
void TrajectoryTab::OnListBox(wxCommandEvent &evt) {
    int slnum = evt.GetId();
    int index;
    switch (slnum) {
        case listBox_ConfigListBox:
            index = configListBox->GetSelection();
            for(size_t i = 0; i < thisrobot->activeLinks.size(); i++) {
                thisrobot->activeLinks[i]->jVal = configurationlist.at(index).at(i);
            }
            world->updateRobot(thisrobot);
            viewer->UpdateCamera();
            break;
        case listBox_SplineType:
            splineTypeID = splineType->GetSelection();
            cout << "Spline type " << splineTypeID << "  was selected" << endl;
            break;
        default:
            return;
    }
}

//Handle Slider Events
void TrajectoryTab::OnSlider(wxCommandEvent &evt) {
    int slnum = evt.GetId();
    double pos = *(double*) evt.GetClientData();
    char numBuf[1000];

    vector< vector <double> > tmp(3);
    
    switch (slnum) {
        case slider_Time:
            sprintf(numBuf, "X Change: %7.4f", pos);
            sliderTime = pos;
            //	cout << "TIme slider output: " << numBuf << endl;
            break;
        case slider_Increment:
            sprintf(numBuf, "X Change: %7.4f", pos);
            sliderIncrement = pos;
            //	cout << "TIme slider output: " << numBuf << endl;
            break;
        default:
            return;
    }

    if (frame != NULL)
        frame->SetStatusText(wxString(numBuf, wxConvUTF8));
}

// Handle Button Events
void TrajectoryTab::OnButton(wxCommandEvent &evt) {
    int button_num = evt.GetId();
    
    switch (button_num) {
        case button_AddConfig:
            addConfig();
            break;
        case button_RemoveConfig:
            removeConfig();
            break;

        case button_GenerateTrajectory:
            generateTrajectory();
            break;
    }
    
}

// Handle RST events
void TrajectoryTab::RSTStateChange() {
    if(selectedTreeNode == NULL){
        return;
    }

    Object* o;
    Robot* r;
    Link* l;
    string statusBuf;
    string buf, buf2;
    switch (selectedTreeNode->dType) {
        case Return_Type_Object:
            o = (Object*) (selectedTreeNode->data);
            statusBuf = " Selected Object: " + o->name;
            buf = "You clicked on object: " + o->name;
            break;
        case Return_Type_Robot:
            r = (Robot*) (selectedTreeNode->data);
            statusBuf = " Selected Robot: " + r->name;
            buf = "You clicked on robot: " + r->name;
            break;
        case Return_Type_Link:
            l = (Link*) (selectedTreeNode->data);
            statusBuf = " Selected Link: " + l->name + " of Robot: "
            + l->robot->name;
            buf = " Link: " + l->name + " of Robot: " + l->robot->name;
            // Do something here if you want to.  you get the idea...

            break;
        default:
            fprintf(stderr, "Someone else's problem\n");
            assert(0);
            exit(1);
    }
    //frame->SetStatusText(wxString(statusBuf.c_str(), wxConvUTF8));
    //sizerFull->Layout();
}

void TrajectoryTab::addConfig() {
    if (world != NULL && selectedTreeNode != NULL) {
        switch (selectedTreeNode->dType) {
            case Return_Type_Object:
                cout << "Sorry. This is a robot simulator, not an artillery simulator." << endl;
                return;
                break;
            case Return_Type_Robot:
                thisrobot = (Robot*)selectedTreeNode->data;
                break;
            case Return_Type_Link:
                thisrobot = ((Link*)selectedTreeNode->data)->robot;
                break;
            default:
                fprintf(stderr, "Someone else's problem!\n");
                assert(0);
                exit(1);
        }

        linksPos.resize(thisrobot->activeLinks.size());
        for(size_t i=0; i<thisrobot->activeLinks.size(); i++) {
            linksPos[i] = thisrobot->activeLinks[i]->jVal;
        }
		
        configurationlist.push_back(linksPos);
        timelist.push_back(sliderTime);

        cout << "Add Configuration: ";
        for(size_t i=0; i<thisrobot->activeLinks.size(); i++) {
            cout << configurationlist.back().at(i) << " ";
        }
        cout << endl;
		
		
        string name = "Config ";
        stringstream out;
        out << configListSize << " (" << sliderTime << ")";
        name.append(out.str());

        wxString item(name.c_str(), wxConvUTF8);
        configListBox->InsertItems(1, &item, configListSize);
        ++configListSize;
    } else {
        cout << "Must load a world and select a link first before adding a configuration!" << endl;
    }
}

void TrajectoryTab::removeConfig() {	
    int index = configListBox->GetSelection();

    if (configListSize == 0) {
        cout << "There's no configuration to delete" << endl;
    } else if (index == -1) {
        cout << "Must select a configuration first!" << endl;
    } else {
        configListBox->Delete(index);
        configurationlist.erase(configurationlist.begin() + index);
        timelist.erase(timelist.begin() + index);
        --configListSize;
    }
	
    cout << "Configuration " << index << " removed" << endl;
}

void TrajectoryTab::generateTrajectory() {
    double increment = .001;
    double duration = 5.0;

    // create base trajectory from the current robot
    trajectory = new CubicViaSpline(thisrobot->activeLinks.size(), NORMAL_SPLINE, duration);
	
    if (configurationlist.size() >= 3) {
        trajectory->setStart(configurationlist.at(0));
        trajectory->setEnd(configurationlist.at(configListSize - 1));
				
        for (unsigned int x = 0; x < configurationlist.size(); ++x) {
            reinterpret_cast<CubicViaSpline*>(trajectory)->add(timelist.at(x), configurationlist.at(x));
        }

        trajectory->generate_trajectory(5);

        // Mike added

        if ( world != NULL && thisrobot != NULL && trajectory != NULL && 
             trajectory->start.size() != 0 && trajectory->end.size() != 0) {
            frame->InitTimer(string("Traj"),increment);
            for(double i=0; i<duration; i+=increment){
                vector< vector <double> > tmp(3);
                tmp = trajectory->evaluate(i);
                for(size_t i = 0; i < thisrobot->activeLinks.size(); i++) {
                    thisrobot->activeLinks[i]->jVal = tmp[0][i];
                }
                world->updateRobot(thisrobot);
                frame->AddWorld(world);
            }
        }
    } else {
        cout << "Must add 3 configurations first before generating trajectory!" << endl;
    }
}
