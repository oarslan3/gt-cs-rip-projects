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
#ifndef TRAJECTORY_H
#define TRAJECTORY_H
#include <vector>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include "Constants.h"

using namespace std;

typedef enum {NORMAL_SPLINE, CLAMPED_SPLINE} spline_type;

class Trajectory {
public:
	int numLinks;
	double duration;
	bool initialized;

	vector<double> start;
	vector<double> end;
	void test();
	Trajectory(int numLinks, double duration=30);
	void setStart(vector<double> start);
	void setEnd(vector<double> end);
    virtual void add(vector < vector<double> > point);
	//void getLinksPos(vector<double> &v);
	virtual void generate_trajectory(double duration=-1)=0;
	virtual vector < vector < double > > evaluate(double time_offset)=0;

};
class CubicSpline: public Trajectory {
public:
	vector <vector <double> > coefficients;
	CubicSpline(int numLinks, double duration=30);
	virtual void generate_trajectory(double duration=-1);
	virtual vector < vector < double > > evaluate(double time_offset);
};


class Blends: public Trajectory {
//trajectory  with parabolic blends. question is - will it blend?
public:
    vector<double> blend_times;
    //TODO: diff blend_accel for each param
    vector<double> blend_accel;
    //blend accel will be % over min_possible_accel
    double auto_smooth;

    Blends(int numLinks, vector<double> blend_accel, double duration=30, double auto_smooth=0.0);
    virtual void generate_trajectory(double duration=-1);
    virtual vector <vector <double > > evaluate(double time_offset);

    double min_duration_for_joint(double start, double end, double max_vel, double max_acc);
    void set_min_duration_acc(double max_vel, double max_acc);

};

class CubicViaSpline: public Trajectory {
public:
    vector<double> start_vels;
    vector<double> end_vels;
    vector < vector < vector <double> > > joint_via_points;
    spline_type st;

    vector < vector <double> > all_a_coeffs;
    vector < vector <double> > all_b_coeffs;
    vector < vector <double> > all_c_coeffs;
    vector < vector <double> > all_d_coeffs;

    CubicViaSpline(int numLinks, spline_type st, double duration=30, vector<double> start_vels= (vector<double>)NULL, vector<double> end_vels=(vector<double>)NULL);
	void setStart(vector<double> start);
    void setEnd(vector<double> end);
    void add(vector < vector <double> > via_points);
	//By Misha: this overloaded add function is for newbie friendliness
	//adds a via point to the CubicViaSpline Trajectory at specified time
	void add(double time, vector<double> via_point);
    virtual void generate_trajectory(double duration=-1);
    vector < vector <double> > generate_trajectory_for_joint(int joint);

    /**
     * Calculates the position at a given time offset
     * Also gives the velocity and acceleration in output vectors
     * @return vector with result[0] = position, result[1] = vel, result[2] = accel
     */
    virtual vector <vector <double> > evaluate(double time_offset);

    vector <double> evaluate_for_joint(int joint, double time_offset);
    void print_via_points();

};

void print_2d_matrix(vector <vector <double> > matrix);
void print_1d_matrix(vector<double> matrix);
void init_h(vector<double> &h, vector < vector <double> > via_points);
void TridiagonalSolve(const vector <double> &a, const vector <double> &b, vector <double> &c, vector <double> &d, vector <double> &x, int &n);
void init_C_normal_spline(vector<double> &C, vector < vector <double> > via_points, const vector<double> h, vector <double> &d);
void init_C_clamped_spline(vector<double> &C, vector < vector <double> > via_points, const vector<double> h, vector <double> &d, double start_vel, double end_vel);
void empty_S(vector < vector <double> > &S);
void init_S_normal_spline(vector < vector <double> > &S, const vector<double> h, vector <double> &a, vector <double> &b, vector <double> &c);
void init_S_clamped_spline(vector < vector <double> > &S, const vector<double> h, vector <double> &a, vector <double> &b, vector <double> &c);
void find_coeffs(vector <double> &a_coeff, vector <double> &b_coeff, vector <double> &c_coeff, vector <double> &d_coeff, vector <double> &y_acc, vector < vector <double> > &via_points, const vector<double> &h);
void empty_coeffs(vector < vector <double> > &coeffs, int numLinks);


#endif
