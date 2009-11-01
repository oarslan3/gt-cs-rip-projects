/*
 * RRT.h
 *
 *	This is a super-simple library for planning using an RRT.
 *	It currently only implements a single tree with random and greedy step
 *	methods.  It was built for speed first and simplicity second, so if
 *	you're pissed at the lack of features or elegance, bite me.
 *
 *	This may be used in stock form for finding a valid path n-dimensions,
 *	e.g. to create a trajectory for a robot arm controller.  Here, you
 *	only need the following:
 *		- start and end points expressible as a vector of doubles ("config")
 *		- lower and upper bounds on the search space, also expressible as configs
 *		- a collision function which returns true for invalid rstate configurations
 *
 * Applying should be as simple as implementing this collision function, either
 * by changing the actual code or just deriving from RRT as I did in the Splinter project
 *
 *  Created on: Aug 27, 2009
 *      Author: jscholz
 */

#ifndef RRT_H_
#define RRT_H_

#include "ANN/ANN.h"
#include <vector>
#include <stdlib.h>

using namespace std;

#define RANDNM(N,M) N + ((M-N) * (rand() / ((double)RAND_MAX + 1))) // random # between N&M

// For representing and manipulating rstate configurations:
#define rstate vector<double>

rstate operator-(const rstate& v1, const rstate& v2);
ostream& operator<<(ostream& os,const rstate& l);

static inline double rstateSub(rstate &a, rstate &b, rstate &c){
	/* Computes difference vector and its magnitude WITHOUT
	 * taking sqrt (for speed)
	 */
	//if ((a.size() != b.size()) && (a.size() != c.size()))
		//perror("vector sizes do not match");

	double sd=0;
	for (int i = 0; i < (int)a.size(); ++i) {
		double d = a[i] - b[i];
		c[i] = d;
		sd += d * d;
	}
	return sd;
}

static inline double rstateSD(rstate &a, rstate &b){
	/* Computes squared difference between vectors (WITHOUT
	 * taking sqrt, for speed)
	 */

	//if (a.size() != b.size())
		//perror("vector sizes do not match");

	double sd = 0;
	for (int i = 0; i < (int)a.size(); ++i) {
		double d = a[i] - b[i];
		sd += d * d;
	}
	return sd;
}

class RRT {
public:
	RRT();
	~RRT();

	// Fixed initialization code
	void initialize(rstate &ic, rstate &gc, rstate &mins, rstate &maxs, double ss = 0.05, int mn = 100000, int ll = 1000, double AE = 0);

	// A place to add additional instructions in derived classes;
	virtual void Init_extras();

	void Init_ANN();

	virtual void cleanup();

	int ndim;
	double step_size;
	int max_nodes;
	int linear_limit;
	double ANNeps;

	bool extended; // true if last extend operation was successfull (created a new node)
	int frontierNodeIDX; // index of "freshest" extension of tree

	rstate initRstate; // Container for starting configuration
	rstate goalRstate; // Container for goal configuration

	rstate qtmp; // Container for random configuration

	rstate minRstate; // Container for minimum configuration values
	rstate maxRstate; // Container for maximum configuration values

	rstate bestRstate;  // actual best configuration params
	int bestRstateIDX;	// refers to an index in rstateVector
	double bestSD;		// squared distance to best rstate

	vector<int> parentVector;		// vector of indices to relate rstates in RRT
	vector<rstate> rstateVector; 	// vector of all visited rstates
	vector<rstate> path; 			// vector of rstates between initRstate and bestConf

	// ANN stuff
	int 				linearNNstart;
	int					nPts;					// actual number of data points
	ANNpointArray		dataPts;				// data points
	ANNpoint			queryPt;				// query point
	ANNidxArray			nnIdx;					// near neighbor indices
	ANNdistArray		dists;					// near neighbor distances
	ANNkd_tree*			kdTree;					// search structure


	void stepRandom();

	void stepGreedy(rstate &target);

	// Connect algorithm (repeated greedy steps)
	void connect(rstate &target);

	// Tries to extend tree towards provided sample (virtual, if you need custom transition model)
	virtual void tryStep(rstate qtry, int NNidx);

	// Adds qnew to the tree
	void addNode(rstate &qnew, int parentID);

	// Sets qsamp a random configuration (may be overridden you want to do something fancy with sampled rstates)
	virtual rstate& getRandomRstate();

	// Returns NN to query point
	int getNearestNeighbor(rstate &qsamp);

	// traces the path from bestConf to initRstate node
	void tracePath();

	// Traces the path and dumps to stdout
	void showPath();

	// Implementation-specific function for checking collisions
	virtual bool checkCollisions(rstate &s);

};



#endif /* RRT_H_ */
