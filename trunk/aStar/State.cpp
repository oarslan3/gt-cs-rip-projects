/*
 *
 * State.cpp
 *
 *
 * */

#include <iostream>
#include "State.h"

using namespace std;

/* Initialization of the environment */
Environment::Environment(int **array, Parameter &param){
	map = array;
	size_x  = param.size_x;
	size_y  = param.size_y;
	numTargets = param.numTargets;
	type = param.type;
	startSokoban = param.startSokoban;
	startBoxes = param.startBoxes;
	targetBoxes = param.targetBoxes;

	//map[s.x][s.y] = START;
	//map[t.x][t.y] = TARGET;
}

void Environment::print(){

	for(int i=-1; i<=size_x; i++,cout << endl){
		if(-1==i || size_x==i){
			for(int j=-1; j<=size_y; j++)
				cout << '#';
		}
		else{
			cout << '#';
			for(int j=0; j<size_y; j++){
				switch(map[i][j]){
					case EMPTY: cout << '-'; break;
					case TOUCHED: cout << 'x'; break;
					case OBSTACLE: cout << 'O'; break;
					case START: cout << 's'; break;
					case TARGET: cout << 't'; break;
				}
			}
			cout << '#';
		}
	}
}

void Environment::touch(Location ll){
	int tmp = map[ll.x][ll.y];
	if(EMPTY==tmp)
		map[ll.x][ll.y] = TOUCHED;
}

int** Environment::get_map(){ return map; }

int
Environment::get_numTargets() const
{
	return numTargets;
}

int
Environment::get_type() const
{
	return type;
}

Location
Environment::get_startSokoban() const
{
	return startSokoban;
}

std::vector <Location>
Environment::getStartBoxes() const
{
	return startBoxes;
}

std::vector <Location>
Environment::getTargetBoxes() const
{
	return targetBoxes;
}

int Environment::get_size_x() const {return size_x;}
int Environment::get_size_y() const {return size_y;}

Environment::~Environment() {}

Environment* State::env = NULL;

State::State(){
	calculate_f(NULL);
	last_movement = START;
}

State::~State(){}

State::State(int **initial_map, Parameter &param){

	env = new Environment(initial_map, param);
	coorSokoban = param.startSokoban;
	calculate_f(NULL);
	last_movement = START;
}

State::State(State& parent,Direction operation){
	*this = parent;
	switch(operation){
		case UP: --coorSokoban.x; break;
		case LEFT: --coorSokoban.y; break;
		case DOWN: ++coorSokoban.x; break;
		case RIGHT: ++coorSokoban.y; break;
		default: break;
	}

	env->touch(coorSokoban);
	last_movement = operation;

	calculate_f(&parent);
	last_movement = operation;
}

/* For each branch branched_nodes are initialized with a four element array. The
 * members of the array with NULL are inappropriate nodes. These are not
 * taken into account in A_Star::solve()  */
State** State::Branch(int &nodes_n)
{
	nodes_n = 4;

	State** branched_nodes = new State* [nodes_n];

	int dx[]={0,-1,0,0,1},
		dy[]={0,-1,1,0},
		gain[]= {1,2,4,8};

	Direction inverse[] ={START,DOWN,RIGHT,UP,LEFT};


	for(int i=0;i<nodes_n;i++)
		branched_nodes[i] = NULL;

	int tmp;
	Location newPlace;
	int value = 0;
	int **map = env->get_map();
	int numTargets = env->get_numTargets();
	int boxNumber = -1;

	for (int dir = 1; dir <= 4; dir = dir + 1) // UP:1 , LEFT:2 , DOWN:3 , RIGHT:4
	{
		// ilk kosulu denetle

		if(/*coorSokoban.x > 0 &&*/ dir !=inverse[last_movement] &&
			(EMPTY==(tmp=map[coorSokoban.x + dx[dir]][coorSokoban.y + dy[dir]])))
		{
			// is there any pushing action
			boxNumber = -1;
			for (int i = 0;  i < numTargets; ++i)
			{
				if (coorBoxes[i].x == coorSokoban.x + dx[dir] && coorBoxes[i].y == coorSokoban.y + dy[dir])
				{
					boxNumber = i;
					break;
				}
			}

			if (boxNumber > -1 )
			{
				newPlace.x = coorBoxes[boxNumber].x + dx[dir];
				newPlace.y = coorBoxes[boxNumber].y + dy[dir];

				int value = -gain[inverse[dir]];

				// Wall detection on the edges of the new place of the box
				for (int edge = 1; edge <= 4; ++edge ) // UP:1 , LEFT:2 , DOWN:3 , RIGHT:4
					value += (EMPTY != map[newPlace.x + dx[edge]][newPlace.y + dy[edge]])*gain[edge];

				// Box detection on the edges of the new place of the box
				for (int k = 0; k < numTargets; ++k)
					for (int edge = 1; edge <= 4; ++edge ) // UP:1 , LEFT:2 , DOWN:3 , RIGHT:4
						value += (coorBoxes[k].x == (newPlace.x + dx[edge]) && coorBoxes[k].y == (newPlace.y + dy[edge]))*gain[edge];


				if ( value == 0 || value == 1 || value == 2 || value == 4 || value == 5 || value == 8 || value == 10)
					//branched_nodes[dir-1] = new State(*this,dir, boxNumber);	// kutu durumunu da guncelle
					;

			}
			else
				branched_nodes[dir-1] = new State(*this, static_cast<Direction>(dir));

		}
	}

	/*
	if(coorSokoban.x<env->get_size_x()-1 &&
		(EMPTY==(tmp=env->get_map()[coorSokoban.x+1][coorSokoban.y]) || TARGET==tmp))
			branched_nodes[1] = new State(*this,DOWN);
	if(coorSokoban.y>0 &&
		(EMPTY==(tmp=env->get_map()[coorSokoban.x][coorSokoban.y-1]) || TARGET==tmp))
			branched_nodes[2] = new State(*this,LEFT);
	if(coorSokoban.y<env->get_size_y()-1 &&
		(EMPTY==(tmp=env->get_map()[coorSokoban.x][coorSokoban.y+1]) || TARGET==tmp))
			branched_nodes[3] = new State(*this,RIGHT);
	 */

	return branched_nodes;
}

/* Prints the state information and the last operation is
 * shown as the move of the appropriate neighboring block
 * of the empty space */
void State::print(){
	cout << '(' << coorSokoban.x << ',' << coorSokoban.y << ')' << endl;
}

/* Number of steps gone so far */
int State::calculate_g(State *parent){
	if(!parent)
		return g=0;
	return (g = parent->get_g() + 1);
}

int
State::calculate_f(State *parent)
{
	return f = (calculate_g(parent) + calculate_h(parent));
}


bool
operator == (State &left, State &right)
{
	bool check = false;


	check = (left.coorSokoban.x == right.coorSokoban.x) && (left.coorSokoban.y == right.coorSokoban.y);

	if (check == true){
		std::vector<Location>::iterator itLeft, itRight;

		for ( itLeft = left.coorBoxes.begin(), itRight = right.coorBoxes.begin();
			  itLeft != left.coorBoxes.end() && itRight != right.coorBoxes.end();
			  ++itLeft, ++itRight  )
		{
			check = check && (itLeft->x == itRight->x) && (itLeft->y == itRight->y);
			if (check == false)
				return false;
		}

		return true;
	}

	return false;

}

bool
State::is_leaf() const
{
	return (0==h);
}

int State::get_f() const {return f;}
int State::get_g() const {return g;}
int State::get_h() const {return h;}

void State::set_f(int x){f=x;}
void State::set_g(int x){g=x;}
void State::set_h(int x){h=x;}

int State::abs(int x){ return x<0?-x:x; }

/* Approximate number of displacements that will be taken to reach the goal.
 * This is the total of Mahalonobis distance of each block to its exact place
 * in the final state */
int State::calculate_h(State *parent){
	if(!parent)
		return h = 1;

	return h;
	//return h = abs(coorSokoban.x - env->get_target().x) + abs(coorSokoban.y - env->get_target().y);
}

Environment* State::get_env() { return env; }

