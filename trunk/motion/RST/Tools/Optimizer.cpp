/*
 * Optimizer.cpp
 *
 *  Created on: Oct 31, 2009
 *      Author: alexgc
 */

#include <iostream>
#include "Optimizer.h"

using namespace std;

bool Optimizer::optimize(const std::vector<std::vector<double> >& init_path) {
	if (init_path.size() == 0) {
		cout << "Failure: trying to optimize an empty path!" << endl;
		return false;
	}

	// copy initial path into local store
	original_ = init_path;

	// insert optimization code here and write optimized path to optimized_
	//TODO  write this code
	// placeholder for now
	optimized_ = init_path;
}

std::vector<std::vector<double> >  Optimizer::getOriginal() const {
	return original_;
}

std::vector<std::vector<double> >  Optimizer::getOptimized() const {
	return optimized_;
}
