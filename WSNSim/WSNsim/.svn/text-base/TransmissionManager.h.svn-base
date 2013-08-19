#ifndef __TRANSMISSIONMANAGER_H__
#define __TRANSMISSIONMANAGER_H__

#include "sinkNode.h"
#include "sensorNode.h"

using namespace std;

class transmissionManager{
public:
	//constructor takes nodes that will be used.
	transmissionManager(sinkNode sinks[], sensorNode nodes[]){
		this->sinks = sinks;
		this->nodes = nodes;

	}

	//processes transmissions
	void processTransmissions(){
		processTransmissions(sinks, nodes);
	}

private:
	sinkNode * sinks;
	sensorNode * nodes;

	//loads all one hop neighbors based on distace from node n
	void loadOneHopNeighbors(sinkNode sinks[], sensorNode nodes[],int id){
		if (id >= NUMBER_OF_NODES){//sink node ids
			for (int i = 0; i<NUMBER_OF_NODES;i++){
			
				for (int j = 0; j < NUMBER_OF_SINKS; j++){
					//distance
					double r2 = sqrt(
							pow(nodes[i].x - sinks[j].x, 2.0) + 
							pow(nodes[i].y - sinks[j].y, 2.0));

					if (r2 <= RAIDO_RANGE && !nodes[i].isDead()){ //node has to be alive. by doing this here this is simulating a node not responding
						neighbor n;
						n.ID = nodes[i].ID;
						n.parent = sinks[j].ID;
					
						sinks[j].oneHopNeighbors.push_back(n);
						sinks[j].children.insert(nodes[i].ID);


					
							neighbor n2;
							n2.ID = sinks[j].ID;
							n2.parent = sinks[j].ID;
							nodes[i].oneHopNeighbors.push_back(n2);

						if(sinks[j].isSinkActive() == true){	
							nodes[i].children.insert(sinks[j].ID);

							nodes[i].setParent(sinks[j].ID);
						}
					}
				}

	
			}
			int ss = 0;
		}else{
			//id-=1;
			for (int i = 0; i<NUMBER_OF_NODES;i++){
			
				if (i != id){
					//distance
					double r = sqrt(
							pow(nodes[i].x - nodes[id].x , 2.0) + 
							pow(nodes[i].y - nodes[id].x , 2.0));

					if (r <= RAIDO_RANGE && !nodes[i].isDead() && !nodes[id].isDead()){//bothh nodes have to be alive. by doing this here this is simulating a node not responding
						neighbor n;
						n.ID = nodes[i].ID;
						nodes[id].oneHopNeighbors.push_back(n);
						nodes[id].children.insert(nodes[i].ID);

						neighbor n2;
						n2.ID = nodes[id].ID;
						nodes[i].oneHopNeighbors.push_back(n2);
						nodes[i].children.insert(nodes[id].ID);
					}
				}
	
			}
		}
	}


	//sends to neighbor nodes. 
	void sendToNeighbors(sinkNode sinks[], sensorNode nodes[],int id, message m){
		int sp=-3;
		if (id >= NUMBER_OF_NODES){
			vector<neighbor>::iterator it;
			int counter = 0;
			for(it = sinks[id - NUMBER_OF_NODES].oneHopNeighbors.begin(); it != sinks[id - NUMBER_OF_NODES].oneHopNeighbors.end(); it++){
				int neighborid = (*it).ID;
				if (neighborid != sinks[id - NUMBER_OF_NODES].ID){
					nodes[neighborid].childrenInQueue.push(m);
					counter++;
				}
			}
		}else{ 
			vector<neighbor>::iterator it;
			for(it = nodes[id].oneHopNeighbors.begin(); it != nodes[id].oneHopNeighbors.end(); it++){
				int neighborid = (*it).ID;
				sp=neighborid;
				if (neighborid != nodes[id].ID){
					if (neighborid >= NUMBER_OF_NODES)
						sinks[neighborid - NUMBER_OF_NODES].childrenInQueue.push(m);
					else
						nodes[neighborid].childrenInQueue.push(m);		
				}
			}
		}


	}

	//sends to all nodes
	void sendToAll(sinkNode sinks[], sensorNode nodes[],int id, message m){
		if (id >= NUMBER_OF_NODES){
		//sink
			if (sinks[id - NUMBER_OF_NODES].oneHopNeighbors.size() == 0){
				loadOneHopNeighbors(sinks, nodes, id);
				sendToNeighbors(sinks, nodes, id, m);
			}
			else{
				//sendToNeighbors
				sendToNeighbors(sinks, nodes, id, m);
			}
		}else{
			
			if (nodes[id].oneHopNeighbors.size() == 0){
				loadOneHopNeighbors(sinks, nodes, id);
				sendToNeighbors(sinks, nodes, id, m);
			}
			else{
				//sendToNeighbors
				sendToNeighbors(sinks, nodes, id, m);
			}
		}
	}


	//sends to child nodes queue.
	void sendMessageTochildrenInQueue(sinkNode sinks[], sensorNode nodes[], message m){
		if (m.receiver >= NUMBER_OF_NODES)
			sinks[m.receiver - NUMBER_OF_NODES].childrenInQueue.push(m);
		else
			nodes[m.receiver].childrenInQueue.push(m);
	}

	//send to parents nodes queue.
	void sendMessageToparentInQueue(sinkNode sinks[], sensorNode nodes[], message m){
		if (m.receiver >= NUMBER_OF_NODES)
			int i =0;//sink->childrenInQueue.push(m);
		else
			nodes[m.receiver].addMessageToQueue(sensorNode::queueID::PARENT_IN,m);//nodes[m.receiver].parentInQueue.push(m);
	}

	//sends to sink in queue; this is for sink message passing. since a sink can have a longer transmission range 
	void sendMessageToSinkInQueue(sinkNode sinks[], message m){
		sinks[m.receiver - NUMBER_OF_NODES].childrenInQueue.push(m);
	}

	//Main process of transmissions
	void processTransmissions(sinkNode sinks[], sensorNode nodes[]){

		int j = 0;
		while(j < NUMBER_OF_SINKS){
		
			while (!sinks[j].childrenOutQueue.empty()){
				message m = sinks[j].childrenOutQueue.front();
				sinks[j].childrenOutQueue.pop();

				if (m.receiver == -1){
					sendToAll(sinks, nodes, sinks[j].ID, m);
				}else if (m.receiver >= NUMBER_OF_NODES){
					sendMessageToSinkInQueue(sinks, m);
				}else {
					sendMessageToparentInQueue(&sinks[j], nodes, m);
				}
			}
			j++;
		}
		int i = 0;
		while( i < NUMBER_OF_NODES){
		

				while (!(nodes[i].childrenOutQueue.empty())){
					message m = nodes[i].childrenOutQueue.front();
					nodes[i].childrenOutQueue.pop();

					if (m.receiver == -1)
						sendToAll(sinks, nodes, i, m);
					else
						sendMessageToparentInQueue(sinks, nodes, m);
				}
				while (!(nodes[i].isMessageQueueEmpty(sensorNode::queueID::PARENT_OUT)) && (nodes[i].getParent() != -2)){//while (!(nodes[i].parentOutQueue.empty()) && (nodes[i].getParentID() != -2)){

					message m =  nodes[i].getMessageFromQueue(sensorNode::queueID::PARENT_OUT);
					if (m.receiver == -1)
						sendToAll(sinks, nodes, i, m);
					else
						sendMessageTochildrenInQueue(sinks, nodes, m);
				}

				while (!(nodes[i].isMessageQueueEmpty(sensorNode::queueID::OTHER_OUT))){//while (!(nodes[i].otherOutQueue.empty())){

					message m =  nodes[i].getMessageFromQueue(sensorNode::queueID::OTHER_OUT);
					if (m.receiver == -1)
						sendToAll(sinks, nodes, i, m);
					else
						sendMessageTochildrenInQueue(sinks, nodes, m);
				}
				i++;	
			}
			
	



	}
};

#endif