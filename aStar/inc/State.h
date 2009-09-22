/*
 *
 * State.h
 *
 * The header file of the example class State.
 * The purpose of this class is to show how to
 * use heritage from State_Base class to find
 * the direction to the target block on a block
 * world.
 *
 * */

#ifndef STATE_H
#define STATE_H

#include <vector>

struct Location
	{ int x,y; };

struct Parameter
{
	int **array;
	int size_x;
	int size_y;
	int numTargets;
	int type;
	Location startSokoban;
	std::vector<Location> startBoxes;
	std::vector<Location> targetBoxes;


};



enum {EMPTY=0,OBSTACLE,TOUCHED,START,TARGET};

/* Environment class is used to keep the map of the world, start and target
 * locations.
 * */
class Environment{
	private:
		int **map;					/* the map of the world */
		int size_x, size_y;			/* the size of the world. x is the horizontal axis */
		int numTargets;				/* the number of the targets */
		int type;					/* type of the problem : RANDOM | ORDERED */

		Location startSokoban;				/* the initial position of the sokoban */
		std::vector<Location> startBoxes;
		std::vector<Location> targetBoxes;	/* the destination of the boxes */




	public:
		Environment(int **array, Parameter &param);
		~Environment();
		void touch(Location);	/* used to mark a location as TOUCHED */
		void print();			/* prints the world as START (s), TARGET (t), TOUCHED (x), EMPTY (-) and OBSTACLE (O) */

		/* get functions */
		int ** get_map();
		int get_size_x() const;
		int get_size_y() const;
		int get_numTargets() const;
		int get_type() const;

		Location get_startSokoban() const;
		std::vector <Location> getStartBoxes() const;
		std::vector <Location> getTargetBoxes() const;

};

class State{

    private:
		static Environment *env;	/* Since the environment is unique, it is kept as static variable */
		Location coorSokoban;				/* The current location of the sokoban and boxes defines the state */
		std::vector<Location> coorBoxes;


		typedef enum {START,UP,LEFT,DOWN,RIGHT} Direction;
		Direction last_movement;	/* the last movement is kept */
		int f,g,h;

	public:
		State();

		/* this is the for the first state, and it config is initialized with initial_map and other variables */
		State(int **initial_map, Parameter &param);

		/* operation denotes the direction of the movement */
		State(State & parent, Direction operation);

		~State();

		/* branch new states from the state */
		State** Branch(int &nodes_n);

		/* print, calculate_g and calculate_h are pure virtual functions in the base class */
		/* Print the state configuration */
		void print();

		/* Number of displacements done so far */
		int calculate_g(State *parent);

		/* Approximate number of displacements */
		int calculate_h(State *parent);

		int calculate_f(State *parent);

		/* absolute value */
		int abs(int x);

		friend bool operator == (State &left, State &right);


		bool is_leaf() const;							/* leaf control for an admissible heuristic function; the test of h==0*/

		int get_f() const;								/* get functions for the variables */
		int get_g() const;
		int get_h() const;

		void set_f(int);								/* get functions for the variables */
		void set_g(int);
		void set_h(int);

		/* returns env */
		Environment* get_env();
};

#endif

