/*
 *  Planner.h
 *  RST
 *
 *  Created by Jonathan Scholz on 9/28/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PLANNER_H
#define PLANNER_H

#include "../Tools/RRT.h"
class Robot;
class World;

using namespace std;

#define GREEDINESS 5
#define MAX_POINTS 50000

class RSTRRT : public RRT {
public:
	RSTRRT();
	~RSTRRT();

	int robotID;
	World* pworld;

	void setWorld(World* w, int rid);
	bool checkCollisions(rstate &s); // uses local world for collision checking
};

class Planner {
public:
	Planner(World* w, rstate start, rstate goal, int whichRobot=0, bool beGreedy=0, bool useConnect=0, bool showProg=0, int rrt_style=0);
	~Planner();  //TODO implement this to cleanup worlds

	// Planning stuff
	int rrtStyle;
	bool greedyMode;
	bool connectMode;
	bool showProgress;
	int numLinks;
	bool solved;
	rstate lowBounds;
	rstate highBounds;
	rstate startConf;
	rstate goalConf;
	Robot* probot;
	World* pworld;
	RSTRRT* rrt;
	RSTRRT* rrtb; // for bidirectional RRT

	void setStart(World* world);
	void setGoal(World* world);
	void getBestConf();
	void getCurrentConf();
	bool Plan();

};

#endif
