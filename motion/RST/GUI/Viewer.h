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

#ifndef VIEWER_H
#define VIEWER_H

#include <wx/wx.h>
#include <wx/glcanvas.h>

#include "../Tools/GL/glcommon.h"



#include "../Tools/Transform.h"
#include "../Tools/Model3DS.h"




class Viewer : public wxGLCanvas
{

public:
	Viewer(wxWindow * parent, wxWindowID id,
	          const wxPoint & pos, const wxSize& size, long style=0,
	          const wxString & name = _("GLCanvas"), int * attribList = 0,
	          const wxPalette & palette = wxNullPalette)
			  : wxGLCanvas(parent, id, pos, size, style, name, attribList, palette){}

	virtual ~Viewer() { }

	void OnIdle(wxIdleEvent & evt)	{
		//draw();
		evt.RequestMore();
	}

	void InitGL();
	void setClearColor(double r,double g, double b, double a);
	void UpdateCamera();
	int DrawGLScene();
	void ResetGL();
	void addGrid();

	long x,y,xInit,yInit;

	int w,h;

	bool existsUpdate;
	bool doCollisions;
	bool Move;
	bool pflag;
	int threadCounter;
	bool    gridActive;
	double   camRadius;
	bool	keys[256];
	bool	active;
	bool	renderLoopActive;
	bool	redrawFlag;
	bool	showSpheres;
	bool	fullscreen;
	///////////////////////
	bool	mouseLDown;
	bool	mouseRDown;
	bool	mouseMDown;
	int		redrawCount;

	bool mouseCaptured;

	Transform camT,prevCamT,camRotT;
	Transform targT,prevTargT;
	Transform worldT,prevWorldT;
	Vec3	upV;
	Model3DS* model;

	void resized(wxSizeEvent& evt);
	void shown(wxShowEvent& evt);

	int getWidth() { return GetSize().x; }
	int getHeight() { return GetSize().y; }

	void render(wxPaintEvent& evt);

	// events
	void OnMouse(wxMouseEvent& evt);
	void OnCaptureLost(wxMouseCaptureLostEvent& evt);
	void mouseWheelMoved(wxMouseEvent& evt);


	DECLARE_EVENT_TABLE()
};





#endif
