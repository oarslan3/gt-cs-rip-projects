//---------------------------------------------------------------------
//  Copyright (c) 2008 Jon Scholz & Mike Stilman 
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

#ifndef TABVIEW_H
#define TABVIEW_H

#include "Slider.h"

class Slider;

extern HWND inspectorPage;
extern HWND planningPage;
extern HWND planedetectPage;
extern HWND teleOpPage;
extern HWND planeDetect;
extern HWND comPage;

static const int windowsSideBarSize = 10;
static const int windowsTopBarSize = 10;

//const int slider_height = 50; // sets the yVal for all sliders (effectively the row height)
//const int slider_width = 100; // leave 'em all the same for now
//const int label_width = 50; // sets the xValfor slider label
//const int box_height = 25; // base on text size later if desired
//const int box_width = 25;

extern int teleOpMethod;

extern bool individualCOM;
extern bool treeCOMS;
extern bool robotFloorCOM;
extern bool showPrimitive;

enum buttontypes {
	pushButton,
	checkBox,
	radioButton
};
enum teleOpMethods {
	jacobian,
	analytical
};
static double joint_angle;
static const int header_height = 100; // base on text size later if desired
static const int decPrecision = 1000; // multiplies all slider values by this to get better resolution

HWND CreateTabs(HWND hwndParent);
void UpdateTabs();
void TabSelect(int curSel);


//struct angleslider {
//	HWND hwndTrack;
//	HWND hwndLeftLabel;
//	HWND hwndRightEdit;
//};

//void makeSlider(angleslider *angleslider, LPCTSTR name, int xDim=35, float min=-PI, float max=PI, 
//				float pagesize=5, float ticfreq=PI / 5, int pos=0, HWND page=inspectorPage);
void makeStaticText(HWND *textitem, LPCSTR text, HFONT font, HWND page=inspectorPage);
void makeButton(HWND *button, LPCSTR text, HFONT font, HWND page=planningPage, int buttontype=pushButton, bool new_radio_group=false);

void handleTimeSlider(float realpos);
void handleTrajectorySlider(float realpos);

void PerformButtonAction(DWORD lParam);
int whichSlider(DWORD lParam);
void PerformSliderAction(int slidernum, float realpos);
void setSliderPositions();

void setSliderPositions(void);

HWND createPage(char* name, int num, HWND hParent);
LRESULT PageWndProc(HWND hWnd,unsigned int uMsg,WPARAM wParam, LPARAM lParam);

// Handles & Stuff we need available elsewhere:
extern HWND statictext[17];
//extern angleslider slideritem[9];
extern Slider sliders[9];

#endif TABVIEW_H