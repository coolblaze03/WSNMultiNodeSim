#ifndef __SENSORNODE_H__
#define __SENSORNODE_H__
#include "baseNode.h"
#include "neighbor.h"
using namespace std;

class sensorNode: public baseNode{
public:
	sensorNode();
	void powerOn();
	void run();
	void setParent(int parentID);
	NODE_ID getParent();
	static enum queueID{
		CHILD_IN = 0,
		CHILD_OUT = 1,
		PARENT_IN = 2,
		PARENT_OUT = 3,
		OTHER_IN = 4,
		OTHER_OUT = 5
	};
	void addMessageToQueue(queueID qid, message m);
	bool isMessageQueueEmpty(queueID qid);
	message getMessageFromQueue(queueID qid);
	bool isDead(){
		return this->dead;
	}
	void GenerateDataMessageToSink();
	void setPower(int value){
		power = value;
	}
	int getPower();
	void setCycling(bool val){
		runcycles = val;
	}
	bool isCycleRunning(){
		return runcycles;
	}
	void putToSleep(float num_full_cycles){
		sleep = true;
		power -= ENERGY_SLEEP_PROCESSOR_PER_SECOND * num_full_cycles;
	}
	void wakeUp(){
		sleep = false;
		power-= WAKEUP_ENERY_CONSUMED;
	}
	bool isSleep(){
		return sleep;
	}


private:
	bool sleep;
	bool disconnected; //disconnected from tree. in transition to another parent.
	int level;
	int cnum;
	int MaxLevelAnnounceCountDown;
	queue<message> parentInQueue;
	queue<message> otherInQueue;
	queue<message> parentOutQueue;
	queue<message> otherOutQueue;
	int parent;
	vector<NODE_ID> pathFromSink;
	set<NODE_ID> returnedChildren;
	double nodeSetupTimer;
	double wakeUpTimer;
	double childrenTransimissionTimer;
	double parentTransmissionTimer;
	double parentReceptionTimer;
	double childernReceptionTimer;
	double sleepTimer;
	bool runcycles;

	neighbor h;
	neighbor s;
	set<int> d;//data
	vector<neighbor> backupParents;
	double power; //1000
	bool dead;//false
	void broadcastHello();
	void sendToChildern(message m);
	void sendToParent(message m);
	void SendToOther(message m);

	void systemCheck();
	void processMessage(message m);
	void processQueues();
	void processChildrenRx();
	void processChildrenTx();
	void processParentTx();
	void processParentRx();


	void a1();
	void a2(message m);
	void a3(message m);
	void a4(message m);
	void a5(message m);
	void a6();
	void a7(message m);
	void a8();
	void a9(message m);
	void a10(message m);
	void a11(message m);
	void a12();
	void a13(message m);
	string getNodeType();

};
#endif