#ifndef __BASENODE_H__
#define __BASENODE_H__

#include <map>
#include <set>
#include <time.h>
#include <vector>
#include <sstream>
#include "message.h"
#include "customQueue.h"
#include "neighbor.h"



using namespace std;

class baseNode{
public :
	baseNode(){
		if (seedset == false){
			seedset = true;
			srand ( time(NULL) );
		}
		
		x = getRandomNumber() % FIELD_WIDTH;
		y = getRandomNumber() % FIELD_HEIGHT;
		seqint = 0;
	}

	string NodeXML(){

		 char buffer [33];

		string myxml = "<node>\n";
		//Type
		myxml += "	<Type>";
		myxml += getNodeType();
		myxml += "</Type>\n";

		//ID
		myxml += "	<ID>";
		itoa(ID, buffer, 10);
		myxml += buffer;
		myxml += "</ID>\n";

		//OneHopNeighbors
		vector<neighbor>::iterator it;
		for (it = oneHopNeighbors.begin();it != oneHopNeighbors.end();it++){
				myxml += "	<oneHopNeighbor>";
				itoa((*it).ID, buffer, 10);
				myxml += buffer;
				//myxml += (*it).ID;
				myxml += "</oneHopNeighbor>\n";
		}

		//Children
		set<NODE_ID>::iterator cit;
		for (cit = children.begin();cit != children.end();cit++){
				myxml += "	<children>";
				itoa(*cit, buffer, 10);
				myxml += buffer;
				//myxml += (*cit);
				myxml += "</children>\n";
		}

		//Position
		//myxml += "	<position>\n";
			myxml += "		<x>";
			//myxml += x;
			itoa(x, buffer, 10);
			myxml += buffer;
			myxml += "</x>\n";
			myxml += "		<y>";
			itoa(y, buffer, 10);
			myxml += buffer;
			//myxml += y;
			myxml += "</y>\n";
		//myxml += "	</position>\n";

		//energy
		string sentype = "Sensor";
		if (getNodeType().compare(sentype) == 0){
			myxml += "	<initial_energy>";
			itoa(getPower(), buffer, 10);
			myxml += buffer;
			//myxml += 100;//TODO:This will not be static when finished. 
			myxml += "</initial_energy>\n";
			myxml+= "	<parent>";
			itoa(getParent(), buffer, 10);
			myxml += buffer;
			myxml+= "</parent>\n";

		}

		myxml += "</node>\n";
		return myxml;

	}

	int getRandomNumber(){
		return rand();
	}

	void countdown(double miliseconds){
		clock_t endwait;
	    endwait = clock () + miliseconds * (CLOCKS_PER_SEC/1000) ;
	    while (clock() < endwait) {}
	}

	void sleep(){
	}

	int x;
	int y;

	/*bool fncomp (neighbor* lhs, neighbor* rhs) {return lhs->ID<rhs->ID;}
	bool(*fn_pt)(int,int) = fncomp;*/

	
	//Common things between the sink and sensors.
	vector<neighbor> oneHopNeighbors;
	//set<> test;
	set<NODE_ID> children;

    queue<message> childrenInQueue;
	queue<message> childrenOutQueue;
	set<NODE_ID> duplicated;
	int ID;//0-sink
	int maxLevel; //-1
	double networkResetTime;//-1
	clock_t universalClock;
	void virtual powerOn(){
	}
	void virtual run(){
	}

	void clearQueue(queue<message> &q){
		while(!q.empty()){
			q.pop();
		}
	}
	void clearQueueKeepSinkShiftAndData(queue<message> &q, int nextSink){
		vector<message> keeping;
		while(!q.empty()){
			message m = q.front();
			
			if (m.type == SINKSHIFT)
				keeping.push_back(m);

			if (m.type == DATA){
				m.finalReceiver = nextSink;
				keeping.push_back(m);
			}
			
			q.pop();
		}

		for (int i = 0; i<keeping.size();i++){
			q.push(keeping[i]);
		}

	}

	string virtual getNodeType(){
		return "";
	}
	NODE_ID virtual getParent(){
		return -2;
	}
	int virtual getPower(){
		return -2;
	}
	int seqint;

	int getNewSeq(){
		stringstream ss;
		ss << ID << seqint;
		long int n3 = strtol(ss.str().c_str(), NULL, 10);
		seqint++;
		return n3;
	}

	vector<int> processedSeq;

	//neighbor object used in both. it is in a2 for sink
	//neighbor s;
};

#endif
