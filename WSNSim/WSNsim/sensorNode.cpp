#include "stdafx.h"
#include "sensorNode.h"
#include <iostream>

using namespace std;

//Not all functions in here will be commented since they will be self explanitory. I will comment all that is needed. 
//If anything is not clear just let me know. 

//The constructor will just load the IDs. It also randomizes the location but that is done in the base class by default. 
sensorNode::sensorNode(){
	if (nodeID >= NUMBER_OF_NODES){
		nodeID = 0;
	}
	this->ID = nodeID;
	nodeID++;
	power = -1;
	seqint = 0;
	disconnected = true;
	runcycles = true;
	sleep = false;
}


void sensorNode::a1(){

	this->oneHopNeighbors.clear();
	this->duplicated.clear();
	this->children.clear();
	clearQueue(this->childrenInQueue);
	clearQueue(this->parentInQueue);
	clearQueue(this->otherInQueue);
	clearQueue(this->childrenOutQueue);
	clearQueue(this->parentOutQueue);
	clearQueue(this->otherOutQueue);
	this->level = -1;
	this->maxLevel = -1;
	this->MaxLevelAnnounceCountDown = -1;
	this->pathFromSink.clear();
	
	if (power == -1)
		this->power = NODE_INITIAL_ENERGY;

	this->backupParents.clear();
	this->dead = false;
	broadcastHello();
	this->power -= .00000816;
	this->nodeSetupTimer = NODE_SETUP_TIME;
	countdown(this->nodeSetupTimer);
	this->wakeUpTimer = -1;
	this->childernReceptionTimer = -1;
	this->childrenTransimissionTimer = -1;
	this->parentTransmissionTimer = -1;
	this->parentReceptionTimer = -1;
	this->sleepTimer = -1;
	this->parent= -2;
	this->returnedChildren.clear();
	cnum = 0;


}

//Powers on the node.
void sensorNode::powerOn(){
	a1();
}

//Broadcast the hello to all nodes in range. 
void sensorNode::broadcastHello(){
	message m;
	m.type = HELLO;
	m.originator = this->ID;
	m.sender= this->ID;
	m.receiver = ALL;

	this->childrenOutQueue.push(m);
	this->parentOutQueue.push(m);
	this->otherOutQueue.push(m);

	power -= ENERGY_SENDING_A_BROADCAST_MESSAGE;

} 


//Sends messages to just the child nodes of this node. 
void sensorNode::sendToChildern(message msg){
	msg.sender= ID;
		//Send to kids
	set<NODE_ID>::iterator it;
	for ( it = children.begin(); it != children.end(); it++){
		message m  = msg;

		m.receiver = (*it);

		if (m.receiver >= 0)
			this->childrenOutQueue.push(m);
	}
}

//Sends a message to the parent node of this node. 
void sensorNode::sendToParent(message msg){
		message m = msg;

		m.receiver = this->parent;
		m.sender= ID;

		if (m.receiver >= 0)
			this->parentOutQueue.push(m);

}

//sends to other nodes or any one hop neighbor node. 
void sensorNode::SendToOther(message msg){
	msg.sender= ID;
	if (msg.receiver >= ALL)
		this->otherOutQueue.push(msg);
}


//hello processor
void sensorNode::a2(message m){
	vector<neighbor>::iterator it;

	bool found = false;

	for (it = this->oneHopNeighbors.begin(); it != this->oneHopNeighbors.end();it++){
		if (((neighbor)*it).ID == m.sender ){
			found = true;
			break;
		}
	}

	if (found == false){
		neighbor t;
		s = t;
		s.ID = m.sender;
		this->oneHopNeighbors.push_back(s);

	}else{
		this->duplicated.insert(m.sender);
	}
}

//Treesetup processor
void sensorNode::a3(message m){
	int p = m.path.size();


	//See if path contains redundant paths wich make a loop;
	set<int> dups; 
	pair<set<int>::iterator,bool> ret;
	for (int i = 0; i < p ; i++){
		ret = dups.insert(m.path[i]);
		if (ret.second == false){
			//cout<<"Duplicates in Path"<<endl;
			return;
		}
	}
	ret = dups.insert(this->ID);
	if (ret.second == false){
		//cout<<"Duplicates in Path"<<endl;
		return;
	}
	///////////////////////////////
	if (this->level != -1 && p >= level){
		neighbor n;
		n.ID = m.sender;
		n.pathFromSink = m.path;
		n.level = m.path.size() - 1;

		vector<NODE_ID>::iterator it = m.path.end();
		if (m.path.size() > 0){
				it--;

			n.parent = *it;
			this->backupParents.push_back(n);
		}


		return;
	}

	//i added the small protion below to weed out any reception of duplicate messages. It might not be needed but its here for safty. 
	vector<int>::iterator it;
	vector<int>::iterator it2;
	it = m.path.begin();
	it2 = pathFromSink.begin();
	while (true){

		if (it == m.path.end()){
			return;
		}
		if (it2 == pathFromSink.end()){
			break;
		}

		if ((*it) != (*it2)){
			break;
		}

		it++;
		it2++;
	}


	//Testing
	if (m.path.size() > 0 && this->pathFromSink.size() > 0 && m.originator == this->pathFromSink[0] && m.path.size() >= this->pathFromSink.size()){
		return;
	}


	//cout<<"TreeSetup: "<<ID<<" SINK:"<<m.originator<<endl;

	//sets level and then removes it if it is a child. 
	this->level = p;

	this->pathFromSink = m.path;
	this->pathFromSink.push_back(this->ID);
	this->parent = m.sender;
	this->children.erase(m.sender);
	disconnected = false;
	//Send to kids

	message m1;

	m1.type = TREESETUP;
	m1.originator  = m.originator;
	m1.sender = this->ID;
	m1.path = this->pathFromSink;
	m1.sequence = m.sequence;
	m1.flooding = true;
	this->sendToChildern(m1);


	power -= .000066912;

	//sending to parent;
		message m2;

		m2.type = MYPATH;
		m2.originator  = this->ID;
		m2.sender = this->ID;
		m2.path = this->pathFromSink;
		m2.sequence = this->getNewSeq();//m.sequence;
		m2.flooding = true;
		m2.x = this->x;
		m2.y = this->y;
		m2.children = children;
		for (int i = 0; i < oneHopNeighbors.size();i++){
			if (oneHopNeighbors.at(i).ID < NUMBER_OF_NODES)
				m2.neighbors.insert(oneHopNeighbors.at(i).ID);
		}
		this->sendToParent(m2);

		power -= .000066912;

}

//Mypath processor
void sensorNode::a4(message m){

	vector<int>::iterator seqiterator = find(processedSeq.begin(),processedSeq.end(),m.sequence);
	if (seqiterator != processedSeq.end()){
		return;//it should not find it or it has been processed
	}

	/////////////////////////////////////////////////////////////////////////////
	if (m.sender == parent){
		

	}
	/////////////////////////////////////////////////////////////////////////////
	//since children uses a set datastructure there will be no duplicates entered. 
	this->children.insert(m.sender);
	
	message m1;
	m1.type = MYPATH;
	m1.originator = m.originator;
	m1.sender = this->ID;
	m1.path = m.path;
	m1.update = m.update;
	m1.flooding = false;
	m1.x = m.x;
	m1.y = m.y;
	m1.children = m.children;
	m1.newParent = m.newParent;
	m1.sequence = m.sequence;
	m1.neighbors = m.neighbors;

	this->sendToParent(m1);

	processedSeq.push_back(m.sequence);
	return;

}

//MAXLEVELANNOUNCE processor
void sensorNode::a5(message m){
	if (maxLevel == -1){
		this->maxLevel = m.maxLevel;
	
		message m1;
		m1.type = MAXLEVELANNOUNCE;
		m1.originator = m.originator;
		m1.sender = this->ID;
		m1.maxLevel = m.maxLevel;
		m1.flooding = true;
		m1.sequence = m.sequence;
		this->sendToChildern(m1);

		//This will set the wakeup timer
		this->wakeUpTimer = maxLevel - pathFromSink.size();
		//when MaxLevelAnnounceCountDown gets to zero it signals to start processing in cycles by using wakeupTimer. This gives all
		//levels a chance to get the levelannouce so their wakeuptimers can be set. 
		MaxLevelAnnounceCountDown = maxLevel - level;
	}

}

//Nodesetuptimer. I found no purpose for this since i have done this in maxlevelannouce.
void sensorNode::a6(){
	
	this->power -= NODE_SETUP_TIME * NODE_RECEIVING_POWER;
	this->wakeUpTimer = (NETWORK_CYCLE/ this->maxLevel) * (level - 0.5);
	

}

//Data message processor. It will forward the message to the correct target.  
void sensorNode::a7(message m){
	power -= ENERGY_RECEIVING_A_DATA_MESSAGE;
	set<NODE_ID>::iterator it;
	it = this->children.find(m.sender);

	if (it == this->children.end()){

		return;
	}

	

	if (m.finalReceiver == ID){
		//cout<<"Got data message. from: "<<m.originator<<endl; 
		d.insert(m.data);
	}else{

		it = this->children.find(m.finalReceiver);

		if (it != this->children.end()){
			m.receiver = (*it);
			m.sender= ID;
			this->childrenOutQueue.push(m);
			//cout<<"forwarding to receiver: "<<m.receiver<<endl; 
		}else{
			m.receiver = this->parent;
			this->sendToParent(m);
			//cout<<"forwarding from: "<<ID<<" forwarding to: "<<m.receiver<<" final destination: "<<m.finalReceiver<<endl; 
		}

		
	}
	
}

//If the node is dying it will send out a lost message to all nodes. 
void sensorNode::a8(){
	message m;
	m.type = LOST;
	m.originator = this->ID;
	m.sender = this->ID;
	m.originatorLevel  = this->level;
	m.receiver = ALL;
	this->sendToChildern(m);
	this->sendToParent(m);
	this->SendToOther(m);
	this->power -= .000014688;
	this->dead = true;
	char buffer[33];
	char buffer2[33];
	itoa(ID, buffer, 10);
	itoa(this->parent, buffer2, 10);
	//cout<<"A8 Node: "<<buffer<<" passing death message to:"<<buffer2<<endl;
}

//This will process a lost message and send it to the sink and also make any changes to its own parent if nessasary. 
void sensorNode::a9(message m){
	power -= ENERGY_RECEIVING_A_DEAD_BROADCAST_MESSAGE;
	if (m.originator == this->parent){
		if (this->backupParents.size() == 0){

			if (this->dead == false){
				char buffer[33];
				itoa(ID, buffer, 10);
				cout<<"Node: "<<buffer<<"Is Dead No backup Parents a09:"<<endl;
				//tempdeadcounter++;
			}

			this->parent = -2;
			message m1;
			m1.type = LOST;
			m1.originator = this->ID;
			m1.sender = this->ID;
			m1.originatorLevel  = this->level;
			m1.receiver = ALL;
			this->sendToChildern(m1);
			this->sendToParent(m1);
			this->SendToOther(m1);
			this->power -= .000014688;
			/*char buffer[33];
			itoa(ID, buffer, 10);*/
			//cout<<"Node: "<<buffer<<"Is Dead No backup Parents a09:"<<tempdeadcounter<<endl;
			return;
		}
		disconnected = true;
		vector<neighbor>::iterator it;
		it = this->backupParents.begin();
		neighbor p = *it;
		message m2;
		m2.type = LIFEPROBE;
		m2.sender = this->ID;
		m2.receiver = p.ID;
		this->SendToOther(m2);
		this->power -= .00000816;
		return;
	}
	vector<neighbor>::iterator it2;
	for(it2 = this->backupParents.begin(); it2 != this->backupParents.end(); it2++){
		if ((*it2).ID == m.originator){
			this->backupParents.erase(it2);
			break;
		}

	}

	set<NODE_ID> temp_list;
	set<NODE_ID>::iterator it3;
	for(it3 = this->children.begin(); it3 != this->children.end(); it3++){
		if ((*it3) == m.originator){

			temp_list.insert(*it3);
			message m3;
			m3.type = LOST;
			m3.originator = m.originator ;
			m3.originatorLevel = m.originatorLevel;
			m3.receiver = this->parent;
			this->sendToParent(m3);
			this->power -= .000014688;
			char buffer[33];
			char buffer2[33];
			itoa(ID, buffer, 10);
			itoa(this->parent, buffer2, 10);
			//cout<<"A9 Node: "<<buffer<<" passing child death message to:"<<buffer2<<endl;
			this->children.erase(it3);
			break;
		}

	}

	/////////////////////////////
	vector<neighbor>::iterator it4;
	for(it4 = this->oneHopNeighbors.begin(); it4 != this->oneHopNeighbors.end(); it4++){
		if ((*it4).ID == m.originator){
			this->oneHopNeighbors.erase(it4);
			break;
		}
	}
	/////////////////////////////

	if (m.sender == this->parent){
		//cout<<"RECURSIVE LOOP"<<endl;
		return;
	}

	message m3;
	m3.type = LOST;
	m3.originator = m.originator ;
	m3.originatorLevel = m.originatorLevel;
	m3.receiver = this->parent;
	this->sendToParent(m3);
	this->power -= .000014688;
	char buffer[33];
	char buffer2[33];
	itoa(ID, buffer, 10);
	itoa(this->parent, buffer2, 10);
	//cout<<"A9 e Node: "<<buffer<<" passing death message to:"<<buffer2<<endl;


}

//Lifeprobe processor
void sensorNode::a10(message m){
	if (this->ID != m.receiver){
		return;
	}

	if (this->dead == true){
		message m1;
		m1.type = LIFERESPONSE;
		m1.sender = this->ID;
		m1.receiver = m.sender;
		m1.alive = false;
		this->SendToOther(m1);
	}else{
		message m1;
		m1.type = LIFERESPONSE;
		m1.sender = this->ID;
		m1.receiver = m.sender;
		m1.alive = true;
		m1.path = this->pathFromSink;
		m1.oldLevel  = this->level;
		m1.skip = disconnected;
		this->SendToOther(m1);
	}
	this->power -= .000011424;

}

//Life Response Processor
void sensorNode::a11(message m){
	
	if (this->ID != m.receiver){
		return;
	}
	
	if (m.alive == true){
		
		if (this->backupParents.size() > 0){

			if (this->parent == m.sender){
				return; //already processed. no need to repeat.
			}

			/////SKIP///////////////////////////////////////////////////////////////////

			vector<int>::iterator fit = find(m.path.begin(), m.path.end(), this->parent);
			if (fit != m.path.end()){//contains invalid parent
				m.skip = true;
			}

			

			if (m.skip){ //Goto the next because the one requested is in an inconsitant state. It is not connected to a sink.
			
				neighbor p;

				vector<neighbor>::iterator it;
				
				it = this->backupParents.begin();
				p = *it;
				message m2;
				m2.type = LIFEPROBE;
				m2.sender = this->ID;
				m2.receiver = p.ID;
				this->SendToOther(m2);
				this->power -= .00000816;
				return;
				
			
			}

			vector<int>::iterator fit2 = find(m.path.begin(), m.path.end(), this->ID);
			if (fit2 != m.path.end()){//contains invalid parent
				return;
			}


			

			/////END SKIP//////////////////////////////////////////////////////////////'










			neighbor p;
			p.ID = m.sender;
			vector<neighbor>::iterator it = find(backupParents.begin(), backupParents.end(), p);

			if (it != backupParents.end()){
				this->backupParents.erase(it);
			}

			//it = this->backupParents.begin();
			//p = *it;
			int oldparent = this->parent;
			//this->backupParents.erase(it);
			this->parent = m.sender;

			disconnected = false;

			this->children.erase(m.sender);

			this->pathFromSink = m.path;
			this->pathFromSink.push_back(this->ID);

			message m2;

			m2.type = MYPATH;
			m2.originator  = this->ID;
			m2.sender = this->ID;
			m2.path = this->pathFromSink;
			m2.sequence = getNewSeq();//ID;//m.sequence;
			m2.flooding = true;
			m2.update=true;
			m2.x = x;
			m2.y = y;
			m2.newParent = this->parent;
			m2.oldLevel = this->level ;
			m2.children = children;
			
			for (int i = 0; i < oneHopNeighbors.size();i++){
				if (oneHopNeighbors.at(i).ID < NUMBER_OF_NODES)
					m2.neighbors.insert(oneHopNeighbors.at(i).ID);
			}
			
			this->sendToParent(m2);

			power -= .000066912;

			this->power -= .00007344;
			this->level = m.oldLevel + 1 ; //Old level holds level from node the life probe came from.
		}
		
	}else{
		
		if (backupParents.size() > 0 ){
			neighbor p;
			p.ID = m.sender;
			vector<neighbor>::iterator it = find(backupParents.begin(), backupParents.end(), p);

			if (it != backupParents.end()){
				this->backupParents.erase(it);
			}


			
			if (this->backupParents.size() == 0){
				if (this->dead == false){
					
				}
				this->dead = true;
				message m;
				m.type = LOST;
				m.originator = this->ID;
				m.sender = this->ID;
				m.originatorLevel  = this->level;
				m.receiver = ALL;
				this->sendToChildern(m);
				this->sendToParent(m);
				this->SendToOther(m);
				this->power -= .000014688;
				char buffer[33];
				itoa(ID, buffer, 10);
				cout<<"Node: "<<buffer<<"Is Dead No backup Parents a11:"<<tempdeadcounter<<endl;
				tempdeadcounter++;
				/*char buffer[33];
				itoa(ID, buffer, 10);
				cout<<"Node: "<<buffer<<"Is Dead No backup Parents a11:"<<tempdeadcounter<<endl;*/
			}else{
				it = this->backupParents.begin();
				p = *it;
				message m2;
				m2.type = LIFEPROBE;
				m2.sender = this->ID;
				m2.receiver = p.ID;
				this->SendToOther(m2);
				this->power -= .00000816;
			}
		}

	}

}

//Not sure if needed since moved wakeup timer setup to LEVELANNOUNCE
void sensorNode::a12(){
	this->wakeUpTimer = (NETWORK_CYCLE / this->maxLevel) * (this->level - 0.5);


}


//SINKSHIFT Processor. Forwards message to children, clears all data, then sends hellos. 
void sensorNode::a13(message m){
		if (parent >= 0){
			message m1;
			m1.type = SINKSHIFT;
			m1.originator  = this->ID;
			m1.receiver = ALL;
			m1.nextSink = m.nextSink;
			m1.networkResetTime = this->networkResetTime;
			m1.flooding = true;
			this->childrenOutQueue.push(m1);

			this->parent = -1;

			this->children.clear();

			vector<neighbor>::iterator it4;
			for(it4 = this->oneHopNeighbors.begin(); it4 != this->oneHopNeighbors.end(); it4++){
				this->children.insert((*it4).ID);
			}
			
			clearQueueKeepSinkShiftAndData(this->childrenInQueue, m.nextSink);
			clearQueueKeepSinkShiftAndData(this->parentInQueue, m.nextSink);
			clearQueueKeepSinkShiftAndData(this->otherInQueue, m.nextSink);
			clearQueueKeepSinkShiftAndData(this->childrenOutQueue, m.nextSink);
			clearQueueKeepSinkShiftAndData(this->parentOutQueue, m.nextSink);
			clearQueueKeepSinkShiftAndData(this->otherOutQueue, m.nextSink);
			this->level = -1;
			this->maxLevel = -1;
			this->MaxLevelAnnounceCountDown = -1;
			this->pathFromSink.clear();
			//this->power = NODE_INITIAL_ENERGY;
			this->backupParents.clear();
			this->dead = false;
			this->power -= .00000816;
			this->nodeSetupTimer = NODE_SETUP_TIME;
			countdown(this->nodeSetupTimer);
			this->wakeUpTimer = -1;
			this->childernReceptionTimer = -1;
			this->childrenTransimissionTimer = -1;
			this->parentTransmissionTimer = -1;
			this->parentReceptionTimer = -1;
			this->sleepTimer = -1;
			this->sleepTimer = -1;
			this->parent= -2;
			this->returnedChildren.clear();
			cnum = 0;


			broadcastHello();
		}

		
}


//This is the main loop of the sensor node.
void sensorNode::run(){

	if (dead)
		return;

	if (isCycleRunning()) {// run cycles
		//If the MaxLevelAnnounceCountDown == -1 which means they are all running in sync since it has not recieved notice from the sink on what the levels
		//are and once the wakeup timer is set and MaxLevelAnnounceCountDown has ended it starts its cycle. 
		if ( MaxLevelAnnounceCountDown == -1 || (this->wakeUpTimer == 0 && MaxLevelAnnounceCountDown == 0)){
		
		

					//Random Power Consumption to simulate death randomly over time. 
					int myrandom = this->getRandomNumber() % 1000 + 1;

					if (myrandom == 7 && this->pathFromSink.size() != 0 && cnum>50 && maxLevel != -1){

					

					}
					
			
		
			
				//As long as this node is not dead then process. 
					systemCheck();
					processQueues();
					power -= WAKEUP_ENERY_CONSUMED*4;
					power -= ENERGY_ACTIVE_LISTENING_AT_RECEIVING*2;
					cnum++;


				//If you have gotten into here becasue MaxLevelAnnounceCountDown == -1 then this if will do nothing
				//If MaxLevelAnnounceCountDown == 0 then it is because wakeuptimer is also 0 then reset wakeuptimer. 
				if (MaxLevelAnnounceCountDown == 0){
					this->wakeUpTimer = this->maxLevel;
				}

			}else{
			
				//decrement counter. MaxLevelAnnounceCountDown > 0 then do countdown to start wakeuptimer else if it is zero then decrement wakeuptimer.
				if (MaxLevelAnnounceCountDown > 0 ){
					MaxLevelAnnounceCountDown--;
				}else if (MaxLevelAnnounceCountDown == 0){
					this->wakeUpTimer--;
					power -= ENERGY_SLEEP_PROCESSOR_PER_SECOND * (NETWORK_CYCLE/maxLevel);
				}	
		
			}


	}else{//No cycleling 

		systemCheck();
		processQueues();
		//power -= WAKEUP_ENERY_CONSUMED*4;
		
		power -= ENERGY_ACTIVE_LISTENING_AT_RECEIVING*2;
		cnum++;

	}

}

//Process message based on type.
void sensorNode::processMessage(message m){
	
	switch (m.type){
		
		case TREESETUP:
			a3(m);
			break;
		case MYPATH:
			a4(m);
			break;
		case HELLO:
			a2(m);
			break;
		case DATA:
			a7(m);
			break;
		case MAXLEVELANNOUNCE:
			a5(m);
			break;
		case MAXLEVELRESET:
			break;
		case LOST:
			a9(m);
			break;
		case LIFEPROBE:
			a10(m);
			break;
		case LIFERESPONSE:
			a11(m);
			break;
		case SINKSHIFT:
			a13(m);
			break;

	
	}

}

//Various things to check on the node for each run. 
void sensorNode::systemCheck(){

	

	if (this->power <= WAKEUP_ENERY_CONSUMED && this->dead == false){
		char buffer[33];
		itoa(ID, buffer, 10);

		cout<<"Node: "<<buffer<<"Is Dead :"<<tempdeadcounter<<endl;
		tempdeadcounter++;
		a8();
	}





}


//Queues to be processed
void sensorNode::processQueues(){
	//TODO::This needs to be put on a timer so that the loops process them one at a time and pull the first thing off of the queue
	processChildrenRx();
	processChildrenTx();
	processParentTx();
	processParentRx();

}

//Rx of the childnodes
void sensorNode::processChildrenRx(){

	while (!this->childrenInQueue.empty()){
		message m = this->childrenInQueue.front();
		this->childrenInQueue.pop();
		processMessage(m);
	}

}

//tranfer. This will be handled by the transmission manager outside of this calss. 
void sensorNode::processChildrenTx(){

	for(set<NODE_ID>::iterator it = this->children.begin(); it != this->children.end(); it++){
		//fill childs in queue;
		//simulating movement of messages outside of this class. will be handled by sim.
	}

}
//tranfer. This will be handled by the transmission manager outside of this calss. 
void sensorNode::processParentTx(){


		//simulating movement of messages outside of this class. will be handled by sim.


}
	
//Rx from parent. 
void sensorNode::processParentRx(){
		while (!this->parentInQueue.empty()){

		message m = this->parentInQueue.front();
		this->parentInQueue.pop();
		processMessage(m);
	}
}

void sensorNode::setParent(int parentID){
	this->parent = parentID;
}

NODE_ID sensorNode::getParent(){
	return this->parent;
}

int sensorNode::getPower(){
	return this->power;
}

void sensorNode::addMessageToQueue(queueID qid, message m){
	switch (qid){
		case CHILD_IN:
			this->childrenInQueue.push(m);
			break;
		case CHILD_OUT:
			this->childrenOutQueue.push(m);
			break;
		case PARENT_IN:
			this->parentInQueue.push(m);
			break;
		case PARENT_OUT:
			this->parentOutQueue.push(m);
			break;
		case OTHER_IN:
			this->otherInQueue.push(m);
			break;
		case OTHER_OUT:
			this->otherOutQueue.push(m);
			break;
	}
}
bool sensorNode::isMessageQueueEmpty(queueID qid){
	switch (qid){
		case CHILD_IN:
			return this->childrenInQueue.empty();
			break;
		case CHILD_OUT:
			return this->childrenOutQueue.empty();
			break;
		case PARENT_IN:
			return this->parentInQueue.empty();
			break;
		case PARENT_OUT:
			return this->parentOutQueue.empty();
			break;
		case OTHER_IN:
			return this->otherInQueue.empty();
			break;
		case OTHER_OUT:
			return this->otherOutQueue.empty();
			break;
	    default:
			return true;
	}

}
message sensorNode::getMessageFromQueue(queueID qid){
	message m;
	switch (qid){
		case CHILD_IN:
			m = this->childrenInQueue.front();
			this->childrenInQueue.pop();
			break;
		case CHILD_OUT:
			m = this->childrenOutQueue.front();
			this->childrenOutQueue.pop();
			break;
		case PARENT_IN:
			m = this->parentInQueue.front();
			this->parentInQueue.pop();
			break;
		case PARENT_OUT:
			m = this->parentOutQueue.front();
			this->parentOutQueue.pop();
			break;
		case OTHER_IN:
			m = this->otherInQueue.front();
			this->otherInQueue.pop();
			break;
		case OTHER_OUT:
			m = this->otherOutQueue.front();
			this->otherOutQueue.pop();
			break;
	}
	return m;
}

string sensorNode::getNodeType(){
	return "Sensor";
}

void sensorNode::GenerateDataMessageToSink(){
	if (this->pathFromSink.size() > 0){
		message m;
		m.receiver = parent;
		m.sender = ID;
		m.originator = ID;
		m.finalReceiver = this->pathFromSink[0];
		m.data = 22;
		m.path = this->pathFromSink;
		m.path.pop_back();
		m.type = DATA;
		m.sequence = getNewSeq();
		this->sendToParent(m);

		power -= ENERGY_SENDING_A_DATA_MESSAGE;
	}
}