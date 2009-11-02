/*
 * Optimizer.cpp
 *
 *  Created on: Oct 31, 2009
 *      Author: alexgc
 */

#include <iostream>
#include <cmath>
#include <boost/foreach.hpp>
#include <boost/assign/std/vector.hpp> // for operator +=
using namespace boost::assign;
#include "../GUI/GUI.h"
#include "World.h"
#include "Robot.h"
#include "Optimizer.h"
#include "Trajectory.h"

using namespace std;
typedef std::vector<double> rstate;

void printPath(const Path_t& path, string str="") {
	cout << "Path: " << str << endl;
	int i = 0;
	BOOST_FOREACH(rstate state, path) {
		cout << " Config " << i++ << "[ ";
		BOOST_FOREACH(double d, state) {
			cout << d << " ";
		}
		cout << "]" << endl;
	}
}

bool Optimizer::optimize(const std::vector<std::vector<double> >& init_path) {
	if (init_path.size() == 0) {
		cout << "Failure: trying to optimize an empty path!" << endl;
		return false;
	}

	// copy initial path into local store
	original_ = init_path;
	optimized_ = init_path;

	// insert optimization code here
	simpleSearchOptimize();
	splineSmooting();
	return true;
}

std::vector<std::vector<double> >  Optimizer::getOriginal() const {
	return original_;
}

std::vector<std::vector<double> >  Optimizer::getOptimized() const {
	return optimized_;
}

// Counts the length of an existing path
unsigned int countDist(Path_iterator a, Path_iterator b) {
	unsigned int length = 0;
	Path_iterator cur = a;
	while (cur != b) {
		++length;
		++cur;
	}
	return length;
}


void Optimizer::simpleSearchOptimize() {
	cout << "Starting path shortening with simple search..." << endl;
	cout << "start path length: " << optimized_.size() << endl;

	// init start point at beginning of path
	Path_iterator start_point = optimized_.begin();
	// init end point at end of path
	Path_iterator end_point = optimized_.end()-1; // ensure looking at actual config

	// loop while start has not reached the end of the path
	while (start_point != optimized_.end()) {
		// if end = start :
		// the there is no shorter path from end to start,
		// move to next start
		if (start_point == end_point) {
			++start_point;
			end_point = optimized_.end()-1;
		} else {
			//cout << "checking path" << endl;
			// check path
			Path_t segment = evalPath(start_point, end_point);
			unsigned int curDist =  countDist(start_point, end_point);
			cout << "Shortcut length: " << segment.size() << endl;
			cout << "Current distance: " << curDist << endl;
			if (segment.size() > 0 && segment.size() < curDist) {
				cout << "Found shortcut!" << endl;
				// reconstruct path
				// first segment
				Path_t new_path(optimized_.begin(), start_point);
				// middle segment
				new_path.insert(new_path.end(), segment.begin(), segment.end());
				// last segment
				new_path.insert(new_path.end(), end_point, optimized_.end());

				// replace optimized
				optimized_ = new_path;

				// reset counters to start process over again
				start_point = optimized_.begin();
				end_point = optimized_.end()-1;
			} else {
				// no path found, continuing search
				--end_point;
			}
		}
	}
	cout << "Finished Optimizing!  Final path length: " << optimized_.size() << endl;
}

rstate findDelta(rstate start, rstate end, double stepsize) {
	rstate delta(start.size(),0.0);

	// find the norm
	double dist = 0.0;
	for (unsigned int i = 0; i < start.size(); ++i)
		dist += (end[i]-start[i])*(end[i]-start[i]);
	dist = sqrt(dist);

	// scale the difference vector
	double scale = stepsize/dist;
	for (unsigned int i = 0; i<start.size(); ++i)
		delta[i] = scale*(end[i]-start[i]);

	return delta;
}

double diffDist(rstate start, rstate end) {
	// find the norm
	double dist = 0.0;
	for (unsigned int i = 0; i < start.size(); ++i)
		dist += (end[i]-start[i])*(end[i]-start[i]);
	dist = sqrt(dist);
	return dist;
}

rstate operator+(rstate a, rstate b) {
	rstate sum(a);
	for (unsigned int i=0; i<a.size(); ++i)
		sum[i] = a[i]+b[i];
	return sum;
}

Path_t Optimizer::evalPath(Path_iterator it_start, Path_iterator it_end) {
	// find the direct line between the start and end configurations
	rstate start = *it_start;
	rstate end = *it_end;
	rstate delta = findDelta(start, end, stepSize_);

	// continue moving points towards the end until within range
	bool successful = false;
	Path_t segment;
	segment.push_back(start);
	unsigned int max_steps = diffDist(start, end)/stepSize_;
	// gets within stepsize of the target
	for (unsigned int i =0; i < max_steps; ++i) {
		// determine a new point
		rstate testPt = *(segment.end()-1) + delta;
		// check if it collides
		world->robots[robotID_]->setConf(testPt);
		world->updateRobot(world->robots[robotID_]);
		if (!world->checkCollisions())
			break;
		else {
			successful=true;
			segment.push_back(testPt);
		}
	}

	// handle return
	if (successful) return segment;
	else return Path_t();
}

void Optimizer::splineSmooting() {
	printPath(optimized_, "Initial Optimized Path");

	// create a trajectory
	int numlinks = optimized_[0].size();
	double duration = 5.0;
	CubicViaSpline spline(numlinks, NORMAL_SPLINE, duration);

	// add the start and end points to the path
	unsigned int nrInitPts = optimized_.size();
	spline.setStart(optimized_[0]);
	spline.setEnd(optimized_[nrInitPts-1]);

	// insert all of the configs at a fixed interval
	double increment = duration/nrInitPts;
	for (unsigned int i=1; i<nrInitPts-1; ++i)
		spline.add(increment*i, optimized_[i]);

	cout << "Generating trajectory with "
		 << "\n  nrInitPts = " << nrInitPts
		 << "\n  increment = " << increment
		 << "\n  duration  = " << duration << endl;

	// generate the trajectory
	spline.generate_trajectory(duration);

	// perform subsampling
	// subsampling for even interpolation is twice the n-1 points (leaving out endpoint)
	unsigned int interp_rate = 2; // interpolation factor
	unsigned int nrSamples = interp_rate*(nrInitPts-1); // total number of output samples
	double inc = duration/nrSamples;
	Path_t smoothed_path;
	for (int i=0; i<=nrSamples;i++){
		vector<vector<double> > res = spline.evaluate(inc*i);
		smoothed_path += res[0];
	}

	// output for verification
	cout << "Subsampled trajectory"
	     << "\n  interpolation rate = " << interp_rate
	     << "\n  total samples      = " << nrSamples
	     << "\n  time increment     = " << inc << endl;
	printPath(smoothed_path, "Full smoothed path");

	// determine if there are collisions
	// TODO

	// write back to store and visualize
	optimized_ = smoothed_path;
}
