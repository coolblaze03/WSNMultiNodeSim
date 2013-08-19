#ifndef __SINKNODE_H__
#define __SINKNODE_H__
#include "baseNode.h"
#include "tree.hh"
#include "neighbor.h"
#include "circle.h"

using namespace std;

class sinkNode: public baseNode{
public:
	sinkNode();
	void powerOn();
	void run();
	void addSink(sinkNode* sink);
	bool isSinkActive();
	void setSinkActiveStatus(bool isactive);
	int getFoundSensorNodesCount();
	int getMaxLevel();
	int getNumberOfChildren();
	void removeNodeID(int ID);
	int getDataMessageCount();
	int getNextSinkID();
	void setShiftMethod(SHIFT_METHOD sm);
	void setShiftRate(double SR);
	SHIFT_METHOD getShiftMethod();


private:
	int nextSink;
	neighbor s;
	//int SID;
	bool active;
	bool markAsNotActive;
	tree<customTreeNode> pathSet;//
	set<int> deletedPaths;
	set<int> curdeletedPaths;
	vector<sinkNode*> allSinks;
	int setupSequence; //random nu
	double sinkSetupTimer; //-1
	int survivedRate;
	int tempCounter;
	bool minCircleCalculated;
	double minCircleX;
	double minCircleY;
	double minRadius;
	bool atest[NUMBER_OF_NODES + NUMBER_OF_SINKS];
	double treeAdjustTimer;
	void broadcastTreeSetup();
	void calculateNextSink();
	void markNodesDeleted(NODE_ID id);
	int runCount;
	vector<customTreeNode> nodesintree;
	int sinkShiftTimer;
	int levelUpdateTimer;
	bool runTreeSetup;
	int shiftCycleCountdown;
	SHIFT_METHOD sm;

	//set<int> testpath;
	vector<vector<int>> testpath;

	//////
	vector<cluster> clusters;
	vector<tree<customTreeNode>> currenttrees;
	//////

	void systemCheck();
	void processMessage(message m);
	void processQueues();
	void processChildrenRx();
	void processChildrenTx();
	void dataProcessor(message m);
	Circle findSec(int n, Point p[], int m, Point b[]);
	void clearNotActiveClusters();
	void calcSinkClusterMethod();
	void calcSinkCircleMethod();
	void calcSinkRevisedClusterMethod();
	void calcNoSinkShift();
	void updateLostSensorNeighbor(int lost_id);
	//void test(int n, Point fdf);
	//Point test[10];

	//Power Up
	void a1(bool sinkShifted);
	void a2(message m);
	void a3(message m);
	void a4();
	void a5(message m);
	void a6();
	void a7();
	void a8(message m);
	string getNodeType();

	map<int, set<int>> sensorChildren;
	static map<int, set<int>> sensorNeighbors;
	int initalTreeSize;

	//Testing
	int dataMessagesRecieved;
};
#endif