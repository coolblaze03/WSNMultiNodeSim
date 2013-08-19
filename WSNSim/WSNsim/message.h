#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "constants.h"

using namespace std;

static enum MESSAGE_TYPES{
	TREESETUP = 0,
	MYPATH = 1,
	HELLO = 2,
	DATA = 3,
	MAXLEVELANNOUNCE = 4,
	MAXLEVELRESET = 5,
	LOST = 6,
	LIFEPROBE = 7,
	LIFERESPONSE = 8,
	SINKSHIFT = 9
};

class message{
public:
	message(){
		networkResetTime = -1;

		this->alive = true;
		this->flooding =false;
		this->maxLevel = -1;
		this->networkResetTime = -1;
		this->nextSink = -1;
		this->oldLevel = -1;
		this->originator = -2;
		this->originatorLevel = -2;
		this->path.clear();
		this->receiver =  -2;
		this->sender = -2;
		this->sequence = -2;
		this->suspect = -2;
		this->finalReceiver = -2;
		this->type = HELLO;
		this->data = -1;
		update=false;
		x = -1;
		y = -1;
		skip = false;
		newParent = -2;
	}

	bool alive;
	bool flooding;
	int maxLevel;
	double networkResetTime;
	int nextSink;
	int oldLevel;
	int originator;
	int originatorLevel;
	//int path[30];
	vector<NODE_ID> path;
	int receiver; //ALL == -1
	int sender; 
	int sequence; //Random num for message id
	int suspect;
	int data;
	int finalReceiver;
	bool update;
	int x;
	int y;
	int newParent;
	bool skip;
	MESSAGE_TYPES type;
	set<NODE_ID> children;
	set<NODE_ID> neighbors;

	message& operator=(const message &rhs){
		this->alive = rhs.alive;
		this->flooding = rhs.flooding;
		this->maxLevel = rhs.maxLevel;
		this->networkResetTime = rhs.networkResetTime;
		this->nextSink = rhs.nextSink;
		this->oldLevel = rhs.oldLevel;
		this->originator = rhs.originator;
		this->originatorLevel = rhs.originatorLevel;
		this->path = rhs.path;
		this->receiver = rhs.receiver;
		this->sender = rhs.sender;
		this->sequence = rhs.sequence;
		this->suspect = rhs.suspect;
		this->type = rhs.type;
		this->data = rhs.data;
		this->finalReceiver = rhs.finalReceiver;
		update = rhs.update;
		x = rhs.x;
		y = rhs.y;
		children = rhs.children;
		skip = rhs.skip;
		newParent = rhs.newParent;
		neighbors = rhs.neighbors;
		return *this;
	}

};

#endif