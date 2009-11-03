/*
 * Optimizer.h
 *
 *  Created on: Oct 31, 2009
 *      Author: alexgc
 */

#ifndef OPTIMIZER_H_
#define OPTIMIZER_H_

#include <vector>

typedef std::vector<std::vector<double> > Path_t;
typedef std::vector<std::vector<double> >::iterator Path_iterator;

class Optimizer {
public:
	Optimizer(double stepSize=0.01, unsigned int robotID=0) : stepSize_(stepSize), robotID_(robotID) {}
	virtual ~Optimizer(){}

	/**
	 * Performs actual optimization of the path
	 * @param path is the original path, which will be copied
	 * @return true if path could be optimized
	 */
	bool optimize(const std::vector<std::vector<double> >& init_path);

	/**
	 * Simple search optimizer just does a linear search
	 * and tries to make shortcuts whereever possible
	 */
	void simpleSearchOptimize();

	/**
	 * Uses splines to smooth the path by interpolating, currently set at
	 * doubling the samples in the path.  This can be increased as necessary
	 */
	void splineSmooting();

	/**
	 * evaluate a path via the same process as RRT-connect
	 * @param initial point
	 * @param final point
	 * @return a path segment if successful, empty path otherwise
	 */
	Path_t evalPath(Path_iterator start, Path_iterator end);

	// get functions - will throw errors for null paths
	std::vector<std::vector<double> > getOriginal() const;
	std::vector<std::vector<double> > getOptimized() const;

private:
	// keep original and optimized path
	std::vector<std::vector<double> > original_;
	std::vector<std::vector<double> > optimized_;

	// shortening parameters
	double stepSize_; // used when creating shortcuts
	unsigned int robotID_;
};

#endif /* OPTIMIZER_H_ */
