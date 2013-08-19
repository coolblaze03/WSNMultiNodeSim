#ifndef __NEIGHBOR_H__
#define __NEIGHBOR_H__
#include <set>
#include <vector>
#include "constants.h"

using namespace std;

class neighbor{
public:
	neighbor();
	int ID;
	int level;
	int parent;
	vector <NODE_ID> pathFromSink;
	
	neighbor& operator=(const neighbor &rhs){
		this->ID = rhs.ID;
		this->level = rhs.level;
		this->parent = rhs.parent;
		this->pathFromSink = rhs.pathFromSink;
		return *this;
	}

	inline bool operator == (const neighbor &rhs){
		return (this->ID == rhs.ID);
	}




};
#endif