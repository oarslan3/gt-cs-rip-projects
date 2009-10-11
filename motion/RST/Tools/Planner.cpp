/*
 *  Planner.cpp
 *  RST
 *
 *  Created by Jonathan Scholz on 9/28/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Planner.h"

// pointer to the viewer (only needed for showProgress)
#include "../GUI/Viewer.h"
extern Viewer* viewer;	
#include "Matrix.h"

#include "Robot.h"
#include "World.h"
extern World* world; 	// pointer to the real world (needed for showProgress)

#include <vector>
#include <iostream>
#include <sstream>

RSTRRT::RSTRRT() {

}

RSTRRT::~RSTRRT() {
	cleanup();
}

void RSTRRT::setWorld(World *w, int rid)
{
	robotID = rid;
	pworld = w;
}

bool RSTRRT::checkCollisions(rstate &s)
{
	/*
	 * This is what we're supposed be doing, but it doesn't work yet
	 * despite all the stuff I already fixed...
	 */
	//plannerWorld->robots[robotID]->setConf(s);
	//plannerWorld->updateRobot(plannerWorld->robots[robotID]);
	//return !plannerWorld->checkCollisions();

	world->robots[robotID]->setConf(s);
	world->updateRobot(world->robots[robotID]);
	// viewer->UpdateCamera();
	return !world->checkCollisions();
}

Planner::Planner(World* w, rstate start, rstate goal, int whichRobot, bool beGreedy, bool useConnect, bool showProg, int rrt_style)
{
 	pworld = new World(*w);
 	probot = pworld->robots[whichRobot];

 	rrtStyle = rrt_style;
 	greedyMode = beGreedy;
 	connectMode = useConnect;
 	showProgress = showProg;

 	numLinks = probot->activeLinks.size();//robot->activeLinks.size();

 	startConf = start; // sets the size of the vector to no more than numLinks...
 	goalConf = goal;
 	lowBounds.resize(numLinks);
 	highBounds.resize(numLinks);

 	for(int i=0; i<numLinks; i++){
 		lowBounds[i] = probot->activeLinks[i]->jMin;
 		highBounds[i] = probot->activeLinks[i]->jMax;
 	}

 	solved = false;
 	rrt = NULL;
}

Planner::~Planner()
{
//	delete pworld;
	if (rrt != NULL)
		delete rrt;
}

 void Planner::getBestConf()
{
 	cout << "Best configuration: ";
 	for(int i=0; i < numLinks; i++){
 		cout << rrt->bestRstate[i] << " ";
 		//pworld->robots[0]->setConf(rrt->bestRstate); // uncomment to move arm to this config
 	}
 	// pworld->updateRobot(pworld->robots[0],true);
	// UpdateCamera();
 }

void Planner::getCurrentConf()
{
 	cout << "Current configuration: ";
 	for(int i=0; i<numLinks; i++){
 		cout << rrt->bestRstate[i] << " ";
 		// pworld->robots[0]->setConf(rrt->configVector.back()); // uncomment to move arm to this config
 	}
 	// pworld->updateRobot(pworld->robots[0],true);
	// UpdateCamera();
}

bool Planner::Plan()
{
	// Initialize the RRT
	rrt = new RSTRRT();
	rrt->setWorld(pworld, pworld->findRobot(probot->name));
	rrt->initialize(startConf, goalConf, lowBounds, highBounds);

	int greediness = GREEDINESS;

	int pass;
	for(pass = 0; pass < MAX_POINTS; ++pass){

		if (greedyMode && (pass % greediness == 0)) { //greediness
			if (connectMode)
				rrt->connect(goalConf);
			else
				rrt->stepGreedy(goalConf);
		} else {
			rrt->stepRandom();
		}

		// Check if goal was reached
		if ( rrt->bestSD <= pow(rrt->step_size,2) ) {
			solved = true;
			break;
		}

		// To visualize arm states as they're added:
		if (showProgress) {
			static double bestSD = DBL_MAX;

			if (rrt->bestSD < bestSD) {
				bestSD = rrt->bestSD;

				int worldR = world->findRobot(probot->name);
				world->robots[worldR]->setConf(rrt->bestRstate);
				world->updateRobot(world->robots[worldR]);
				viewer->UpdateCamera();
			}
		}

		if (pass%1000 == 0) {
			cout << pass << " ";
			getBestConf();
			// report the distance b/t this config and goal
			cout << " Dist: " << rrt->bestSD << "(tree size = " << rrt->rstateVector.size() << ")" << endl;	
		}
	}


	if(!solved){
		cerr << "OUT OF SPACE" << endl;
		return false;
	}

	cerr << "SOLVED!" << endl;
	// Read tree forward:
	rrt->tracePath();

	cout << "Path Length = " << rrt->path.size();
	cout << "(" << pass << " iterations, ";
	cout << rrt->rstateVector.size() << " nodes in tree)" << endl;
	return true;
}
