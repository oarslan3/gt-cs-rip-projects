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

class Optimizer {
public:
	Optimizer(){}
	virtual ~Optimizer(){}

	/**
	 * Performs actual optimization of the path
	 * @param path is the original path, which will be copied
	 * @return true if path could be optimized
	 */
	bool optimize(const std::vector<std::vector<double> >& init_path);

	// get functions - will throw errors for null paths
	std::vector<std::vector<double> > getOriginal() const;
	std::vector<std::vector<double> > getOptimized() const;

private:
	// keep original and optimized path
	std::vector<std::vector<double> > original_;
	std::vector<std::vector<double> > optimized_;
};

#endif /* OPTIMIZER_H_ */
