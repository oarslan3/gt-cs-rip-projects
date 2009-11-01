//---------------------------------------------------------------------
//  Copyright (c) 2008 Mike Stilman
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

#include "Viewer.h"
#include "../Tools/World.h"
#include "wx/sizer.h"
#include "GUI.h"
#include <wx/glcanvas.h>

void Viewer::shown(wxShowEvent& evt){
    int w, h;
    GetClientSize(&w, &h);
	#ifndef __WXMOTIF__
    if (GetContext())
	#endif
    {
        //SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }
	UpdateCamera();
}

void Viewer::UpdateCamera(void){

	SetCurrent();
	Transform camTransT;
	camTransT.pos.set(camRadius,0,0);
	camT = camRotT*camTransT;
	DrawGLScene();
}

int Viewer::DrawGLScene()
{
	redrawFlag = true;
	glLoadIdentity();
	glPolygonMode (GL_FRONT, GL_FILL);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	Vec3 zvec = Vec3(0,0,1);
	upV = camT*zvec;

	gluLookAt(camT.pos[0],camT.pos[1],camT.pos[2],
			  targT.pos[0],targT.pos[1],targT.pos[2],
			  upV[0],upV[1],upV[2]);

	float position[]= {camT.pos.x, camT.pos.y, camT.pos.z, 1.0};
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f,1.0f,1.0f,0.0f);

	glPushMatrix();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glPushMatrix();
	//float direction[] = {0.0, 0.5, -10.0, 0.0};
	float no_mat[] = {0.0, 0.0, 0.0, 1.0};
	float diffuse[] = {1.0, 1.0, 1.0, 1.0};
	float specular[] = {0.5, 0.5, 0.5, 1.0f};
	//float yellowAmbientDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT1, GL_POSITION, position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, no_mat);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glPopMatrix();

	glTranslated(worldT.pos.x,worldT.pos.y,worldT.pos.z);

	float ambRefl[] = {0.2f, 0.2f, 0.2f, 1.0f}; // default
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambRefl);
	float diffRefl[] = {0.8f, 0.8f, 0.8f, 1.0f}; // default
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffRefl);
	float specRefl[] = {0.0f, 0.0f, 0.0f, 1.0f}; // default
	glMaterialfv(GL_FRONT, GL_SPECULAR, specRefl);

	glPushMatrix();
	if (gridActive){  addGrid(); }
	glPopMatrix();

	glPushMatrix();
	if(world!=NULL) world->Draw();
	glPopMatrix();

	glPopMatrix();

	glFlush();
	SwapBuffers();

	return TRUE;
}


void Viewer::resized(wxSizeEvent& evt){
	if(!IsShown() || !GetParent()->IsShown()) return;
	wxGLCanvas::OnSize(evt);
    int w, h;
    GetClientSize(&w, &h);
	#ifndef __WXMOTIF__
    if (GetContext())
	#endif
    {
        SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glPolygonMode (GL_FRONT, GL_FILL);
	gluPerspective(45.0f,(GLdouble)w/(GLdouble)h,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	DrawGLScene();
}

void Viewer::mouseWheelMoved(wxMouseEvent& evt){
	SetFocus();
	int wheelRot = evt.GetWheelRotation();

	double camR = camRadius - (double)wheelRot / 300.f;
	if(camR > .05){
		camRadius = camR;
		UpdateCamera();
	}
}

void Viewer::OnCaptureLost(wxMouseCaptureLostEvent& evt){
	mouseCaptured = false;
}

void Viewer::OnMouse(wxMouseEvent& evt){
	evt.GetPosition(&x,&y);

	if(evt.ButtonUp() && mouseCaptured){
		ReleaseMouse();
		mouseCaptured = false;
	}

	if(evt.ButtonDown() && !mouseCaptured){
		prevCamT = camT;
		prevWorldT = worldT;
		xInit = x;
		yInit = y;
		SetFocus();
		CaptureMouse();
		mouseCaptured = true;
		return;
	}
	double dx = (double)(x-xInit);
	double dy = (double)(y-yInit);

	if (evt.LeftIsDown() && evt.RightIsDown()){
		xInit = x;
		yInit = y;
		double camR = camRadius + dy *CAMERASPEED* 2.0f;
		if(camR > .05)
			camRadius = camR;
		UpdateCamera();
	}else if(evt.LeftIsDown()){
		double theta,phi;
		theta = -(dx/90.f);
		phi = -(dy/90.f);
		Transform drot,dtrans,dT;
		Mat33 tm,pm;
		tm.zRot(theta);
		pm.yRot(phi);
		drot.rot = prevCamT.rot;
		drot.rot = drot.rot*tm;
		drot.rot = drot.rot*pm;
		camRotT = drot;
		existsUpdate = true;
		UpdateCamera();
	}else if(evt.MiddleIsDown() || evt.RightIsDown()){
		Vec3 dispV = camRotT*Vec3(0,(double)dx * CAMERASPEED , -(double)dy * CAMERASPEED);
		worldT.pos = prevWorldT.pos+dispV;
		existsUpdate = true;
		UpdateCamera();
	}
}

void Viewer::render(wxPaintEvent& evt){
    if(!IsShown()) return;
    wxGLCanvas::SetCurrent();
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
	DrawGLScene();
}

void Viewer::InitGL(){
	existsUpdate=false;
	doCollisions=false;
	Move=false;
	pflag=false;
	threadCounter=0;
	gridActive=true;
	redrawFlag = true;
	gridActive = true;
	mouseLDown = false;
	mouseRDown = false;
	mouseMDown = false;
	mouseCaptured = false;

	xInit=0; yInit=0; x=0; y=0;

	redrawCount=0;


    GetClientSize(&w, &h);
	#ifndef __WXMOTIF__
    if (GetContext())
	#endif
    {
		Show();
		SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(100.0f);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_FOG);
	float FogCol[3]={0.0f,0.0f,0.0f};
	glFogfv(GL_FOG_COLOR,FogCol);
	glFogf(GL_FOG_START, 5.f);
	glFogf(GL_FOG_END, 15.f);
	glFogi(GL_FOG_MODE, GL_LINEAR);


	camT.identity();
	worldT.identity();
	targT.identity();
	camRotT.identity();
	prevCamT.identity();
	prevWorldT.identity();
	camRadius = defaultCamRadius;


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glPolygonMode (GL_FRONT, GL_FILL);
	gluPerspective(45.0f,(GLdouble)w/(GLdouble)h,0.1f,15.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	UpdateCamera();
}

void Viewer::ResetGL(){
    GetClientSize(&w, &h);
	#ifndef __WXMOTIF__
    if (GetContext())
	#endif
    {
		SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }

	camT.identity();
	worldT.identity();
	targT.identity();
	camRotT.identity();
	prevCamT.identity();
	prevWorldT.identity();
	camRadius = defaultCamRadius;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glPolygonMode (GL_FRONT, GL_FILL);
	gluPerspective(45.0f,(GLdouble)w/(GLdouble)h,0.1f,15.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	UpdateCamera();
}

void Viewer::setClearColor(double r,double g, double b, double a){ 
	glClearColor(r,g,b,a); 
	float FogCol[3]={r,g,b};
	glFogfv(GL_FOG_COLOR,FogCol);
}

void Viewer::addGrid(){
	glEnable(GL_FOG);
	double sizeX=100.0f;
	double sizeY=100.0f;
	double inc=.5f;

	//int n = (int)(sizeX/inc);  //unused
	//int m = (int)(sizeY/inc);  //unused
	//int ctr=0;  //unused

	double startX=-sizeX/2.f;
	double endX  =sizeX/2.f;
	double startY=-sizeY/2.f;
	double endY  =sizeY/2.f;

	glLineWidth(.9f);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(.2f,.2f,0.0f);
	for(double i=startX; i<=endX; i+=inc){ glVertex3d( i, startY, 0.0f); glVertex3d( i, endY, 0.0f);	}
	for(double i=startY; i<=endY; i+=inc){ glVertex3d( startX, i, 0.0f); glVertex3d( endX, i, 0.0f);	}
	glEnd();
	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_LIGHTING);
	glDisable(GL_FOG);
}

BEGIN_EVENT_TABLE(Viewer, wxGLCanvas)
	EVT_SHOW(Viewer::shown)
    EVT_SIZE(Viewer::resized)
    EVT_PAINT(Viewer::render)
	EVT_MOUSE_CAPTURE_LOST(Viewer::OnCaptureLost)
	EVT_MOUSEWHEEL(Viewer::mouseWheelMoved)
    EVT_MOTION(Viewer::OnMouse)
	EVT_RIGHT_DOWN(Viewer::OnMouse)
	EVT_MIDDLE_DOWN(Viewer::OnMouse)
	EVT_LEFT_DOWN(Viewer::OnMouse)
	EVT_LEFT_UP(Viewer::OnMouse)
	EVT_RIGHT_UP(Viewer::OnMouse)
	EVT_MIDDLE_UP(Viewer::OnMouse)
END_EVENT_TABLE()

