/*
 * RRT.cpp
 *
 *  Created on: Aug 27, 2009
 *      Author: jscholz
 */

#include "RRT.h"

#include "../GUI/Viewer.h"
extern Viewer* viewer; // only needed for visualization of configs

// for collision detection in RST:
#include "World.h"
#include "Robot.h"
#include "Matrix.h"
extern World* world;

rstate operator-(const rstate& v1, const rstate& v2) {
	rstate result(v1);
	for (unsigned int i=0; i < v1.size(); ++i) {
		result[i] = v1[i] - v2[i];
	}
	return result;
}

ostream& operator<<(ostream& os, const rstate& c) {
	for (unsigned int i=0; i<c.size(); ++i){
		os << c[i];
		if (i<c.size()-1)
			os << " ";
	}
	return os;
}

RRT::RRT() {

}

void RRT::initialize(rstate &ic, rstate &gc, rstate &mins, rstate &maxs, double ss, int mn, int ll, double AE)
{
	// clean-up from previous
	cleanup();

	ndim = ic.size();
	step_size = ss;
	max_nodes = mn;
	linear_limit = ll;
	ANNeps = AE;

	initRstate = rstate(ic);
	goalRstate = rstate(gc);

	qtmp = rstate(ic);

	minRstate = rstate(mins);
	maxRstate = rstate(maxs);

	parentVector.clear();
	rstateVector.clear();

	bestRstateIDX = -1;
	bestSD = DBL_MAX;

	srand(time(NULL));

	Init_extras();

	Init_ANN();
}

void RRT::Init_extras()
{
	// Empty Function
}

RRT::~RRT() {
	cleanup();
}

void RRT::cleanup()
{
	parentVector.clear();
	rstateVector.clear();
	parentVector.resize(0);
	rstateVector.resize(0);

// ANN stuff (commented out b/c they blow shit up)
//		annDeallocPts(dataPts);
//		delete [] nnIdx;
//		delete [] dists;
//		delete kdTree;
//		annClose();
}

void RRT::Init_ANN()
{
	//int maxPts = max_nodes;
	int k = 1;

	linearNNstart=0;

	queryPt = annAllocPt(ndim);					// allocate query point
	for(int i=0; i<ndim; i++)
		queryPt[i] = 0.1*i;

	dataPts = annAllocPts(max_nodes, ndim);		// allocate data points
	nnIdx = new ANNidx[k];						// allocate near neighbor indices
	dists = new ANNdist[k];						// allocate near neighbor dists

	nPts = 0;									// counter for number of data points

	addNode(initRstate, bestRstateIDX); 				// Add initRstate rstate and "-1" to rstate vectors and ANN

	kdTree = new ANNkd_tree(		// build search structure
		dataPts,					// the data points
		nPts,						// current number of points
		ndim);						// dimension of space
}

void RRT::stepRandom()
{
	/*
	 * Take a step in a random direction (hint: wraps getRandomRstate,
	 * getNearestNeighbor, and tryStep)
	 */
	
	/*
	 Your code here
	 */

}

void RRT::stepGreedy(rstate &target)
{
	/*
	 * Implement this to take a step towards a specific configuration 
	 */
	
	/*
	 Your code here
	 */

}

void RRT::connect(rstate &target) {
	/* The RRT connect algorithm 
	 * Grow along a single vector until a collision is hit
	 */

	/*
	 Your code here
	 */
}


void RRT::tryStep(rstate qtry, int NNidx)
{
	/*
	 * Calculates a new node to grow towards qtry, checks for collisions, and adds
	 * * also maintains distance to goalRstate
	 */

	rstate qnear(ndim);
	rstate qnew(ndim);
	qnear = rstateVector[NNidx];	// sets qnear to the closest configuration to qsamp

	// Compute direction and magnitude of vector from qnear to qtry (save in qnew)
	double sumsq = rstateSub(qtry, qnear, qnew);
	double edist = sqrt(sumsq);

	// Scale this vector to step_size and add to end of qnear
	double scale = (double)step_size / edist;
	for (int i=0; i<ndim; ++i){
		qnew[i] = qnear[i] + qnew[i] * scale;
	}

	if (!checkCollisions(qnew)) {

		addNode(qnew, NNidx);

		double sd = rstateSD(qnew, goalRstate);

		if (sd < bestSD) {
			bestRstateIDX = rstateVector.size()-1;	// if last node is closest, mark idx as greatestConf
			bestSD = sd;
			bestRstate = rstateVector[bestRstateIDX];
			cout << "achieved best SD: " << bestSD << " (treesize=" << rstateVector.size() << ")" << endl;
		}
	}
}

void RRT::addNode(rstate &qnew, int parentID)
{
	/*
	 * Expands RRT by attaching qnew at parentID (checks collisions and
	 * balances tree)
	 */

	// Update graph vectors
		rstateVector.push_back(qnew);
		parentVector.push_back(parentID);

		// add points to ANN data set
		for(int i=0; i<ndim; i++)
			dataPts[nPts][i] = qnew[i];
		nPts++;

		// after "linear_limit" steps build new tree
		if((int)rstateVector.size()-linearNNstart > linear_limit){
			delete kdTree;
			kdTree = new ANNkd_tree(dataPts,nPts,ndim);
			linearNNstart = rstateVector.size();
		}
}

rstate& RRT::getRandomRstate()
{
	/*
	 * Samples a random point for qtmp in the configuration space,
	 * bounded by the provided configuration vectors (and returns ref to it)
	 */

	for (int i = 0; i < ndim; ++i) {
		qtmp[i] = RANDNM(minRstate[i], maxRstate[i]);
	}

	return qtmp;
}

int RRT::getNearestNeighbor(rstate &qsamp)
{
	/*
	 * Returns ID of rstate node nearest to qsamp
	 */

	double min = DBL_MAX;
	double sd = 0.0;
	int nearest=0;

	//First search the linear vector
	for(unsigned int i = linearNNstart; i < rstateVector.size(); ++i){
		sd = rstateSD(qsamp, rstateVector[i]); // joint space version

		if(sd < min) {
			min = sd;
			nearest = i;
		}
	}

	//Then search the ANN kd-tree
	if(nPts>linear_limit){
		for(int i = 0; i < ndim; ++i)
			queryPt[i] = qsamp[i];

		kdTree->annkSearch(queryPt, 1, nnIdx, dists, ANNeps);

		// take best result from ANN & list
		if (dists[0] < min)
			nearest = nnIdx[0];
	}
	return nearest;
}

void RRT::tracePath()
{
	path.clear();

	int x = bestRstateIDX;

	while(parentVector[x] != -1){
		path.insert(path.begin(), rstateVector[x]);
		x = parentVector[x];
	}
}

void RRT::showPath()
{
	tracePath();

	for (unsigned int i = 0; i < path.size(); ++i) {
		cout << path[i] << endl;
	}
}

bool RRT::checkCollisions(rstate &s)
{
	return false;
}

