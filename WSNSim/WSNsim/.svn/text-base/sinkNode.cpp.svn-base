#include "stdafx.h"
#include "sinkNode.h"
#include "constants.h"
#include "message.h"
#include <iostream>
using namespace std;


//constructor
sinkNode::sinkNode(){
	if (sinkID >= NUMBER_OF_SINKS + NUMBER_OF_NODES){
		sinkID = SINK_START_ID;
	}
	this->ID = sinkID++;
	this->survivedRate=1;
	this->minCircleCalculated = false;
	this->minCircleX = -1;
	this->minCircleY = -1;
	this->minRadius = 0;
	this->runCount = 0;
	allSinks.clear();
	active = false;
	markAsNotActive = false;
	sinkShiftTimer = -1;
	levelUpdateTimer = -1;
	this->runTreeSetup	= false;
	shiftCycleCountdown = -1;
	dataMessagesRecieved = 0;
	this->sensorNeighbors.clear();


}

//broadcast of treesetup to neighbors 
void sinkNode::broadcastTreeSetup(){

	pathSet.clear();
	tree<customTreeNode>::iterator top;
	top=pathSet.begin();
	customTreeNode tn(ID, x, y);
	nodesintree.push_back(tn);
	//pathSet.insert(top, 0);
	pathSet.insert(top, tn);
	int issdf = 0;

	message m;

	m.type = TREESETUP;
	m.originator = ID;
	m.sender  = ID;
	m.receiver = ALL;
	m.path.clear();
	m.path.push_back(ID);
	m.flooding = true;
	m.sequence = this->setupSequence;
	this->childrenOutQueue.push(m);

}

void sinkNode::calcSinkCircleMethod(){
	 //calculate min circle. using current layout.
    Point p[NUMBER_OF_NODES + NUMBER_OF_SINKS];
    Point b[100];
    int count=0;


		


	try {

		int nitsize = nodesintree.size();
			vector<customTreeNode>::iterator top = nodesintree.begin();
		while (top != nodesintree.end()){
		  p[count] = Point((*top).x,(*top).y);
		  count++;
		  ++top;
		  if (count >= NUMBER_OF_NODES + NUMBER_OF_SINKS){
			break;
		  }
		}


    
 
    //get min circle and loads values
    Circle cir = findSec(count, p, 0, b);
    this->minCircleX = cir.p.getX();
    this->minCircleY = cir.p.getY();
    this->minRadius = cir.getRadius();
    this->minCircleCalculated = true;
    double lowestSinkDist=-1;
    int lowestSinkID = -1;
 
    //loads sink based on min circle. It uses the closest. but can be another.
    for (int i =0; i < allSinks.size(); i++){
      if (allSinks[i]->ID != ID){
          //get dist from center of circle to to node.

		  

          double r2 = sqrt(
              pow(allSinks[i]->x - minCircleX, 2.0) +
              pow(allSinks[i]->y - minCircleY, 2.0));
 
          if ((r2 < lowestSinkDist || lowestSinkDist == -1) && allSinks[i]->oneHopNeighbors.size() > 0){
              lowestSinkDist = r2;
              lowestSinkID = allSinks[i]->ID;
          }
          //keep track of min.
 
      }
    }
 
    if (lowestSinkID <= -1){
      //ERROR!!!!!!!!!!!!!!
      int hill=0;
      throw "No Next Sink Found Could be a Min Circle ERROR";
      exit(1);
    }else{
      nextSink = lowestSinkID;
    }

		}catch (...){
		cout << "Catch-all hit" << endl;
	}
}

void sinkNode::calcSinkClusterMethod(){
	//step 1 Calculate nodes below number and inital clusters
	tree<customTreeNode>::iterator top;
	top=pathSet.begin();

	tree<customTreeNode>::iterator sib=pathSet.begin();
		   
	while(sib!=pathSet.end(top)) {
		
		//calc nodes below.
		(*sib).num_nodes_below = pathSet.size(sib);
		int testcount =  (*sib).num_nodes_below;
		int childcount = sib.number_of_children();
		if (childcount > 1 && sib != top){
			//Add Cluster
			cluster newcluster;
			newcluster.values.push_back((*sib).ID);

			tree<customTreeNode>::iterator cpos = sib.node->first_child;

			while (cpos != sib.node->last_child){
				newcluster.values.push_back((*cpos).ID);
				cpos=cpos.node->next_sibling;
			}

			clusters.push_back(newcluster);
		}
		++sib;

	}

	//Adding currenttrees to cluster also. these are trees in which the parent node has been deleted. 
	for (int i = 0; i < currenttrees.size(); i++){
		tree<customTreeNode> curtree = currenttrees[i];
		tree<customTreeNode>::iterator sib=curtree.begin();
		while(sib!=curtree.end()) {
			//calc nodes below.
			(*sib).num_nodes_below = pathSet.size(sib);
			int testcount =  (*sib).num_nodes_below;
			int childcount = sib.number_of_children();
			if (childcount > 1 && (*sib).ID != (*top).ID){
				//Add Cluster
				cluster newcluster;
				newcluster.values.push_back((*sib).ID);

				tree<customTreeNode>::iterator cpos = sib.node->first_child;

				while (cpos != sib.node->last_child){
					newcluster.values.push_back((*cpos).ID);
					cpos=cpos.node->next_sibling;
				}

				clusters.push_back(newcluster);
			}
			++sib;

		}
	}

	//Combine clusters 
	//step two combine parents and kids
	if (clusters.size() > 1){
		vector<cluster>::iterator cit = clusters.begin();
		while(cit != clusters.end()){
			vector<cluster>::iterator cit2 = clusters.begin();

			while(cit2 != clusters.end()){

				if ((*cit).values[0] != ((*cit2).values[0]) && (*cit).isActive() && (*cit2).isActive()){
				
					vector<int>::iterator valit = (*cit).values.begin();
					while(valit != (*cit).values.end()){
						if ((*cit2).contains(*valit)){
							(*cit2).combine(*cit);
							(*cit).setActive(false);
							break;
						}
						valit++;
					}

				}
				cit2++;
			}
			cit++;
		}
	}

	clearNotActiveClusters();

	//step three combine clusters who has a node referenced twice in another clusters children. 
	if (clusters.size() > 1){
		vector<cluster>::iterator cit = clusters.begin();
		while(cit != clusters.end()){
			vector<cluster>::iterator cit2 = clusters.begin();

			while(cit2 != clusters.end()){

				if ((*cit).values[0] != ((*cit2).values[0]) && (*cit).isActive() && (*cit2).isActive()){
				
					vector<int>::iterator valit = (*cit).values.begin();
					while(valit != (*cit).values.end()){
						vector<int>::iterator valit2 = (*cit2).values.begin();
						int links = 0;
						while(valit2 != (*cit2).values.end()){
							if (sensorChildren[*valit2].find(*valit) != sensorChildren[*valit2].end()){
								links++;
							}
							if (links >= 2){
								//combine
								(*cit2).combine(*cit);
								(*cit).setActive(false);
								break;
							}
							valit2++;
						}
						valit++;
					}

				}
				cit2++;
			}
			cit++;
		}
	}

	clearNotActiveClusters();


	//
	int sinkid = -1;
	int totalclusterreach = 0;
	for (int i =0; i < allSinks.size(); i++){
		int foundcount = 0;

		if (allSinks[i]->ID != ID){
			

			vector<cluster>::iterator clusterit;
			int clusterreach = 0;
			
			for (clusterit = clusters.begin(); clusterit != clusters.end(); clusterit++){
				set<NODE_ID>::iterator childit;
				for ( childit = allSinks[i]->children.begin(); childit != allSinks[i]->children.end(); childit++){
				
					if ((*clusterit).contains(*childit)){
						clusterreach += (*clusterit).values.size();
						break;
					}
			
				}
			}

			if (clusterreach > totalclusterreach){
				totalclusterreach = clusterreach;
				sinkid = allSinks[i]->ID;
			}
			
			
		}
	}

	int cnum = 0;
	//i need to go down an exta level just to determine if it has sufficent node access. using clusters.
	if (sinkid == -1){
		for (int i =0; i < allSinks.size(); i++){
			int foundcount = 0;

			if (allSinks[i]->ID != ID){
			

				
				//cnum = allSinks[i]->children.size();
				if (cnum > allSinks[i]->children.size()){
					cnum = allSinks[i]->children.size();
					sinkid = allSinks[i]->ID;
				}
			
			
			}
		}
	}


	if (sinkid == -1){
		//throw "error";
		//dont shift just die
	}

	//sink with the most reach;
	nextSink = sinkid;

	//TODO:Take out and replace with sinkCalculated
	minCircleCalculated = true;
}

void sinkNode::calcSinkRevisedClusterMethod(){
//This will use my new method that has just not been implemented. Its a take on the old method but uses the data about a node
//and its neighbors instead of the data just from using the generated tree. This will be tree independent and passed to each sink. 

	//step 1 Calculate nodes below number and inital clusters
	clusters.clear();
		   
	//clusters


	map<int, set<int>>::iterator sensorneighborit;

	for (sensorneighborit = sensorNeighbors.begin(); sensorneighborit != sensorNeighbors.end(); sensorneighborit++){
		int nsize = (*sensorneighborit).second.size();
		if (nsize >= 3){
			cluster newCluster;
			newCluster.values.push_back((*sensorneighborit).first);

			set<NODE_ID>::iterator neit;
			for (neit = (*sensorneighborit).second.begin(); neit != (*sensorneighborit).second.end(); neit++){
				newCluster.values.push_back((*neit));
			}
			clusters.push_back(newCluster);
		}

	}

	
	clearNotActiveClusters();


	//Combine clusters 
	//step two combine parents and kids
	if (clusters.size() > 1){
		vector<cluster>::iterator cit = clusters.begin();
		while(cit != clusters.end()){
			vector<cluster>::iterator cit2 = clusters.begin();

			while(cit2 != clusters.end()){

				if ((*cit).values[0] != ((*cit2).values[0]) && (*cit).isActive() && (*cit2).isActive()){
				
					vector<int>::iterator valit = (*cit).values.begin();
					while(valit != (*cit).values.end()){
						if ((*cit2).contains(*valit)){
							(*cit2).combine(*cit);
							(*cit).setActive(false);
							break;
						}
						valit++;
					}

				}
				cit2++;
			}
			cit++;
		}
	}

	clearNotActiveClusters();

		//step three combine clusters who has a node referenced twice in another clusters children. 
	if (clusters.size() > 1){
		vector<cluster>::iterator cit = clusters.begin();
		while(cit != clusters.end()){
			vector<cluster>::iterator cit2 = clusters.begin();

			while(cit2 != clusters.end()){

				if ((*cit).values[0] != ((*cit2).values[0]) && (*cit).isActive() && (*cit2).isActive()){
				
					vector<int>::iterator valit = (*cit).values.begin();
					while(valit != (*cit).values.end()){
						vector<int>::iterator valit2 = (*cit2).values.begin();
						int links = 0;
						while(valit2 != (*cit2).values.end()){
							if (sensorNeighbors[*valit2].find(*valit) != sensorNeighbors[*valit2].end()){
								links++;
							}
							if (links >= 2){
								//combine
								(*cit2).combine(*cit);
								(*cit).setActive(false);
								break;
							}
							valit2++;
						}
						valit++;
					}

				}
				cit2++;
			}
			cit++;
		}
	}

	clearNotActiveClusters();
	
	//Find next sink
	int sinkid = -1;
	int totalclusterreach = 0;
	for (int i =0; i < allSinks.size(); i++){
		int foundcount = 0;

		if (allSinks[i]->ID != ID){
			

			vector<cluster>::iterator clusterit;
			int clusterreach = 0;
			
			
			
			for (clusterit = clusters.begin(); clusterit != clusters.end(); clusterit++){
				//rethink. many child can be in one cluster. 
				set<NODE_ID>::iterator childit;
				for ( childit = allSinks[i]->children.begin(); childit != allSinks[i]->children.end(); childit++){
				
					if ((*clusterit).contains(*childit)){
						clusterreach += (*clusterit).values.size();
						break;
					}
			
				}
			}

			clusterreach += int((double)allSinks[i]->children.size() * (double)1.5);

			if (clusterreach > totalclusterreach){
				totalclusterreach = clusterreach;
				sinkid = allSinks[i]->ID;
			}
			
			
		}
	}

	int cnum = 0;
	//i need to go down an exta level just to determine if it has sufficent node access. using clusters.
	if (sinkid == -1){
		for (int i =0; i < allSinks.size(); i++){
			int foundcount = 0;

			if (allSinks[i]->ID != ID){
			

				
				//cnum = allSinks[i]->children.size();
				if (cnum > allSinks[i]->children.size()){
					cnum = allSinks[i]->children.size();
					sinkid = allSinks[i]->ID;
				}
			
			
			}
		}
	}
	if (sinkid == -1){
	}

	//sink with the most reach;
	nextSink = sinkid;

	//TODO:Take out and replace with sinkCalculated
	minCircleCalculated = true;
}

void sinkNode::calcNoSinkShift(){
	nextSink = -1;
}

//calculates next sink. 
void sinkNode::calculateNextSink(){

	if (this->active){


		switch (sm){
			case SHIFT_METHOD::SHIFT_SINK_NO_SHIFT:
				calcNoSinkShift();
				break;
			case SHIFT_METHOD::SHIFT_SINK_CIRCLE:
				calcSinkCircleMethod();
				break;
			case SHIFT_METHOD::SHIFT_SINK_CLUSTER:
				calcSinkRevisedClusterMethod();
				break;
			default:
				calcSinkCircleMethod();
				break;

		}
		

	}
	

}

void sinkNode::clearNotActiveClusters(){
	//need to clear clusters marked not active
	vector<cluster>::iterator it = clusters.begin();
	while (it != clusters.end()){
		if (!it->isActive()){
			
			it = clusters.erase(it);
		
		}else{
			it++;		
		}
	}
}

//marks nodes in the tree as deleted. 
void sinkNode::markNodesDeleted(NODE_ID id){

		customTreeNode tn(id,-1,-1);
		tree<customTreeNode>::breadth_first_iterator top;
		top=pathSet.begin();

		
 		bool found = false;
		
		if (tn == *top){
			found = true;
			(*top).active = false;
			
		}else{
			
		
			tree<customTreeNode>::breadth_first_iterator sib=pathSet.begin_breadth_first();
		   
			while(sib!=pathSet.end_breadth_first()) {
				
				if (*sib == tn){
					found = true;
					(*sib).active = false;
					
					top = sib;

					int belowtest = (*sib).num_nodes_below;

					/////DELETE FROM CHILDREN OF SENSORS AND SINKS///////////////////
					for (int i = 0; i<sensorChildren.size();i++){

						set<int>::iterator it = sensorChildren[i].find(tn.ID);
						if (it != sensorChildren[i].end()){
							sensorChildren[i].erase(it);
						}

					}
					////////////////////////////////////////////////////
					updateLostSensorNeighbor(tn.ID);
					/////////////////////////////////////////////////////

					for (int i = 0; i < allSinks.size(); i++){
						set<int>::iterator it = allSinks[i]->children.find(tn.ID);
						if (it != allSinks[i]->children.end()){
							allSinks[i]->children.erase(it);
						}
					}

					vector<tree<customTreeNode>>::iterator vtit;
					for (vtit = currenttrees.begin(); vtit != currenttrees.end(); vtit++){
						tree<customTreeNode> curtree = *vtit;
						tree<customTreeNode>::iterator tsib=curtree.begin();
						
						if ((*tsib).ID != tn.ID){
							currenttrees.erase(vtit);
							break;
						}
						
					}
					/////////////////////////////////////////////////////////////////


					//Im about to test an actual delete
					tree<customTreeNode> delsubtree = pathSet.subtree(sib, sib.end()); 

					int pitsize = pathSet.size(sib);
					
					tree<customTreeNode>::iterator treeit = delsubtree.begin(sib);

					belowtest = treeit.number_of_children();

					int counttest = delsubtree.size(sib);

					///////////////////////////////////////////////
					tree<customTreeNode>::iterator cpos = sib.node->first_child;
					int tsizetotal = 0;
					int savesizetotal = 0;
					if (belowtest > 0 && counttest > 1){
						while (true){
						
							if (cpos == sib.node->last_child){
								tree<customTreeNode> sub= delsubtree.subtree(cpos,cpos.end());
								int subsize = sub.size();
								int subsizei = sub.size(cpos);
								int id = (*cpos).ID;
								int nb = (*cpos).num_nodes_below ;
								tsizetotal += subsize;
								savesizetotal += nb;
								currenttrees.push_back(sub);
								break;
							}else{
								tree<customTreeNode> sub = delsubtree.subtree(cpos,cpos.end());
								int subsize = sub.size();
								int subsizei = sub.size(cpos);
								int id = (*cpos).ID;
								int nb = (*cpos).num_nodes_below ;
								tsizetotal += subsize;
								savesizetotal += nb;
								currenttrees.push_back(sub);

								cpos=cpos.node->next_sibling;
							}

						}
					}

					
					counttest = delsubtree.size(sib);
					
					tree<customTreeNode>::breadth_first_iterator delit ;
					for (delit = delsubtree.begin_breadth_first(); delit != delsubtree.end_breadth_first(); delit++){
						deletedPaths.insert((*delit).ID);
						curdeletedPaths.insert((*delit).ID);
						
					
					}


					pathSet.erase_children(sib); 
					pathSet.erase(sib);

					
					belowtest = (*treeit).num_nodes_below;

					counttest = delsubtree.size(sib);


				
					
					this->calculateNextSink(); //calculates new clusters;
					
					if (belowtest >= this->initalTreeSize * 0.7)
						levelUpdateTimer = this->maxLevel;
					
					break;
				}

				++sib;

			}
		}
		
		//}


		for (int i = 0; i < allSinks.size(); i++){
			allSinks[i]->removeNodeID(id);
		}
		///////////////////////////////////////////////////////////////

		deletedPaths.insert(id);
		curdeletedPaths.insert(id);
		return;

}

void sinkNode::removeNodeID(int dID){
	set<int>::iterator it = this->children.find(dID);
	if (it != this->children.end()){
		this->children.erase(it);
	}
	deletedPaths.insert(dID);
	curdeletedPaths.insert(dID);//remove later
}

//inital clearing of nodes
void sinkNode::a1(bool sinkShifted = false){


	if (sinkShifted == false){
	    this->oneHopNeighbors.clear();
		this->children.clear();
	}
	
	clearQueue(this->childrenInQueue);
	clearQueue(this->childrenOutQueue);
	this->pathSet.clear();
	this->maxLevel = -1;
	this->networkResetTime = -1;
	this->setupSequence = getRandomNumber();
	this->sinkSetupTimer = SINK_SETUP_TIME;
	countdown(this->sinkSetupTimer);
	this->treeAdjustTimer = -1;
	testpath.clear();

	currenttrees.clear();


	curdeletedPaths.clear();

	this->runCount = 0;
	this->initalTreeSize = -1;
	
	for (int i = 0; i<NUMBER_OF_NODES + NUMBER_OF_SINKS;i++)
		atest[i] = false;

	if (sinkShifted == false)
		broadcastTreeSetup();
	else{
		this->runTreeSetup = true;
		shiftCycleCountdown = 5;
		set<NODE_ID>::iterator it;
		for ( it = children.begin(); it != children.end(); it++){
			message m;

			//Send to Children
			m.type = SINKSHIFT;
			m.originator  = this->ID;
			m.receiver = (*it);
			m.nextSink = this->nextSink;
			m.networkResetTime = this->networkResetTime;
			m.flooding = true;
			this->childrenOutQueue.push(m);


		}
	}

}

//hello processor
void sinkNode::a2(message m){

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

//Mypath processor
void sinkNode::a3(message m){

	if (m.path[0] != this->ID)
		return;

	int ml=this->maxLevel ;
	int ps=m.path.size();
	if (ml< ps){
		this->maxLevel = m.path.size();
	}


	vector<vector<int>>::iterator findit = find (testpath.begin(), testpath.end(), m.path);

	if (findit != testpath.end())
		return;//exits if whats coming in is already in testpath

	testpath.push_back(m.path);

	///need to check messages update status to see if it is an updated tree part
	if (m.update && m.path.size() > 0){
		
		int SensorNodeParent = m.newParent;
		int SensorNode = m.originator;

		customTreeNode cn;
		cn.ID = SensorNodeParent;

		tree<customTreeNode>::iterator treeit = find(pathSet.begin(), pathSet.end(), cn);

		if (treeit != pathSet.end()){

			//find in currenttrees
			vector<tree<customTreeNode>>::iterator vtit;
			for (vtit = currenttrees.begin(); vtit != currenttrees.end(); vtit++){
				tree<customTreeNode> curtree = *vtit;
				tree<customTreeNode>::iterator tsib=curtree.begin();
				int cfnid = (*tsib).ID;
				if ((*tsib).ID == SensorNode){
					//then append currenttrees entry to treeit

					customTreeNode dummy;
					dummy.ID = -999;
					tree<customTreeNode>::iterator dummyit = pathSet.append_child(treeit,dummy);

				
					pathSet.replace(dummyit, curtree.begin());//replaces dummy with tree

					int testsize = curtree.size();
					
					//undelete subtree from deletednodes;
					tree<customTreeNode>::breadth_first_iterator delit ;
					for (delit = curtree.begin_breadth_first(); delit != curtree.end_breadth_first(); delit++){
						int tid = (*delit).ID;
						deletedPaths.erase((*delit).ID);
						curdeletedPaths.erase((*delit).ID);

				
					}

					currenttrees.erase(vtit);

					sensorChildren[m.originator] = m.children;
					sensorNeighbors[m.originator] = m.neighbors;
					
					break;
				}
						
			}
			

		}else{
			
			this->childrenInQueue.push(m);
		
		}
					
						

			

        this->calculateNextSink();//Recalculate clusters.
		return;
		
	}
		

	

	tree<customTreeNode>::iterator top;
	top=pathSet.begin();

	vector<int>::iterator vit;
	vit = m.path.begin();
	for (vit = m.path.begin(); vit != m.path.end(); vit++){
		customTreeNode tn(*vit,-1 , -1);
		atest[*vit] = true;
		bool found = false;
		
		if (tn == *top){
			found = true;
			if (tn.ID == m.originator && tn.x == -1 && tn.y == -1){
				(*top).x = m.x;
				(*top).y = m.y;
			}
			continue;
		}else{
			
			tree<customTreeNode>::sibling_iterator sib=pathSet.begin(top);
		   
			while(sib!=pathSet.end(top)) {
				
				if (*sib == tn){
					found = true;
					top = sib;
					if (tn.ID == m.originator && tn.x == -1 && tn.y== -1){
						(*top).x = m.x;
						(*top).y = m.y;
					}
					break;
				}else{
					found = false;
				}
				++sib;

			}

			if (found == false)
			{
				if (tn.ID == m.originator && tn.x == -1 && tn.y == -1){
						tn.x = m.x;
						tn.y = m.y;
				}
				nodesintree.push_back(tn);
				pathSet.append_child(top,tn);
			}
		}
		
	}

	sensorChildren[m.originator] = m.children;
	sensorNeighbors[m.originator] = m.neighbors;

	
}

//Sends max level announce.
void sinkNode::a4(){


		message m;

		m.type = MAXLEVELANNOUNCE;
		m.originator = ID;
		m.sender  = ID;
		m.receiver = ALL;
		m.maxLevel = this->maxLevel;
		m.flooding = true;
		m.sequence = this->setupSequence;

		this->childrenOutQueue.push(m);
}

//lost processor
void sinkNode::a5(message m){

	//find 
	markNodesDeleted(m.originator);
	if (this->treeAdjustTimer == -1){
		this->treeAdjustTimer = TREE_ADJUST_TIME;
		countdown(this->treeAdjustTimer);
	}


}


////tree adjust timer == 0
void sinkNode::a6(){

	this->networkResetTime = this->universalClock + TREE_ADJUST_TIME;
	
	set<NODE_ID>::iterator it;

	//broadcasting to all childern.

	for ( it = children.begin(); it != children.end(); it++){
		message m;

		m.type = MAXLEVELRESET;
		m.sender  = this->ID;
		m.receiver = (*it);
		m.maxLevel = this->maxLevel;
		m.networkResetTime = this->networkResetTime;
		m.flooding = true;
		m.sequence = this->setupSequence;

		this->childrenOutQueue.push(m);
	}


	
}

//Initiates sink shift.
void sinkNode::a7(){

	this->calculateNextSink();

	if (nextSink == -1)
		return;


	this->networkResetTime = this->universalClock + TREE_ADJUST_TIME;
	
	set<NODE_ID>::iterator it;

	//broadcasting to all childern.

	for ( it = children.begin(); it != children.end(); it++){
		message m;

		//Send to Children
		m.type = SINKSHIFT;
		m.originator  = this->ID;
		m.receiver = (*it);
		m.nextSink = this->nextSink;
		m.networkResetTime = this->networkResetTime;
		m.flooding = true;
		this->childrenOutQueue.push(m);


	}
	//This sink shift timer gives children and childrens children ect time for the message to propagate through the tree. 
	this->sinkShiftTimer = this->maxLevel;
}

void sinkNode::a8(message m){

	if (m.nextSink != this->ID){
		return;
	}


	if (active == true)
		return; //Duplicate Message from network

	active = true;


	
	char buffer[33];
	itoa(ID, buffer, 10);
	cout<<"New Sink: "<<ID<<endl;
	a1(true);

}

//processes data messages received from nodes.
void sinkNode::dataProcessor(message m){

	vector<int>::iterator seqiterator = find(processedSeq.begin(),processedSeq.end(),m.sequence);
	if (seqiterator != processedSeq.end()){
		return;//it should not find it or it has been processed
	}

	processedSeq.push_back(m.sequence);
	this->dataMessagesRecieved++;

	
	//cout<<"Got data message. originator: "<<m.originator<<" sender:"<<m.sender<<endl; 
}


//power on
void sinkNode::powerOn(){
	a1();
}

int sinkNode::getDataMessageCount(){
	return this->dataMessagesRecieved;
}


//main loop for sink
void sinkNode::run(){

	//decrements the sink shift timer if shink shift has been initiated. once it gets to 0 then all nodes have sinkshift and
	//the next sink will be informed that it should proceed. This will be moved to its own function. 
	if (this->sinkShiftTimer > 0){
		this->sinkShiftTimer--;
	}else if(this->sinkShiftTimer == 0){
		message m;
		m.type = SINKSHIFT;
		m.originator  = this->ID;
		m.receiver = this->nextSink;
		m.nextSink = this->nextSink;
		m.networkResetTime = this->networkResetTime;
		m.flooding = true;
		this->childrenOutQueue.push(m);
		markAsNotActive = true;
		sinkShiftTimer = -1;
		processQueues();
		return;//get out early because we still want to go one more loop. 
		
	}

	//once 50 iterations has passed the sink will send out the maxlevelannounce. There is no reason the tree is not setup by then. Maybe. 
	if (active && runCount == 51){
		a4();
		this->initalTreeSize = pathSet.size();
	}

	//if nodes not active then skip normal checks
	if (active == true)
		systemCheck();
	
	//the node will still process its own queues for messages.
	processQueues();

	//dont change count if not active. 
	if (active == true)
		runCount++;
	

}

//processess messages. 
void sinkNode::processMessage(message m){
	
	switch (m.type){
		
		case TREESETUP:
			break;
		case MYPATH:
			a3(m);
			break;
		case HELLO:
			a2(m);
			break;
		case DATA:
			dataProcessor(m);
			break;
		case MAXLEVELANNOUNCE:
			
			break;
		case MAXLEVELRESET:
			break;
		case LOST:
			a5(m);
			break;
		case LIFEPROBE:
			
			break;
		case LIFERESPONSE:
			
			break;
		case SINKSHIFT:
			a8(m);
			break;

	
	}

}

//checks various items on the sink. 
void sinkNode::systemCheck(){

	//if it has been marked to not active then active will be set to false. 
	if (markAsNotActive == true){
		active = false;
		markAsNotActive = false;
		return;
	}

	if (this->runTreeSetup == true && this->shiftCycleCountdown == -1){
		runTreeSetup = false;
		this->broadcastTreeSetup();
	}else if (this->runTreeSetup == true){
		this->shiftCycleCountdown--;
	}


	if (this->levelUpdateTimer > -1)
		this->levelUpdateTimer -= 1;

	//once 50 iterations have passed we know whe have the tree. so next we calculate the sink based off of all nodes in tree. 
	if (runCount > 50 && this->minCircleCalculated == false){
		this->calculateNextSink();
	}

	//shifts sink based off of shift rate. 
	double val = ((double)curdeletedPaths.size()/ (double)initalTreeSize);// ((double)deletedPaths.size()/ (double)initalTreeSize);//double val = ((double)deletedPaths.size()/ (double)NUMBER_OF_NODES);//initalTreeSize
	if (val > SHIFT_RATE && this->sinkShiftTimer == -1 && this->levelUpdateTimer == -1){
        a7();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      		a7();
	}

}

void sinkNode::processQueues(){
	//TODO::This needs to be put on a timer so that the loops process them one at a time and pull the first thing off of the queue
	processChildrenRx();
	processChildrenTx();

}
void sinkNode::processChildrenRx(){

	while (!this->childrenInQueue.empty()){
		message m = this->childrenInQueue.front();
		this->childrenInQueue.pop();
		processMessage(m);
	}

}
void sinkNode::processChildrenTx(){

	for(set<NODE_ID>::iterator it = this->children.begin(); it != this->children.end(); it++){
		//fill childs in queue;
		//simulating movement of messages outside of this class. will be handled by sim.
	}

}

//finds smallest circle for all nodes. uses welzi's algorithm which is O(n)
Circle sinkNode::findSec(int n, Point p[], int m, Point b[])
{
	Circle sec =  Circle();
	
	// Compute the Smallest Enclosing Circle defined by B
	if(m == 1)
	{
		sec = Circle(b[0]);
	}
	else if(m == 2)
	{
		sec = Circle(b[0], b[1]);
	}
	else if(m == 3)
	{
		return Circle( b[0], b[1], b[2]);
	}
	
	// Check if all the points in p are enclosed
	for(int i=0; i<n; i++)
	{
		if(sec.contain(p[i]) == 1)
		{
			// Compute B <--- B union P[i].
			b[m] =  Point(p[i]);	
			// Recurse
			sec = findSec(i, p, m+1, b);
		}
	}
	
	return sec;
}

void sinkNode::addSink(sinkNode* sink){
	this->allSinks.push_back(sink);
}

bool sinkNode::isSinkActive(){
	return active;
}

void sinkNode::setSinkActiveStatus(bool isactive){
	this->active = isactive;
}

int sinkNode::getFoundSensorNodesCount(){
	return testpath.size();
}

string sinkNode::getNodeType(){
	return "Sink";
}

int sinkNode::getMaxLevel(){
	return this->maxLevel;
}

int sinkNode::getNumberOfChildren(){
	return children.size();
}

int sinkNode::getNextSinkID(){
	return this->nextSink;
}

void sinkNode::updateLostSensorNeighbor(int lost_id){
	//delete node itself
		map<int,set<int>>::iterator snnit = sensorNeighbors.find(lost_id);//find(sensorNeighbors.begin(), sensorNeighbors.end(), lost_id);

		if (snnit != sensorNeighbors.end()){
			sensorNeighbors.erase(snnit);
		}

		//delete node contained in other parts.
		for (int i = 0; i<sensorNeighbors.size();i++){

			set<int>::iterator it = sensorNeighbors[i].find(lost_id);
			if (it != sensorNeighbors[i].end()){
				sensorNeighbors[i].erase(it);
			}

		}
}

void sinkNode::setShiftMethod(SHIFT_METHOD sm){
	this->sm = sm;
}

void sinkNode::setShiftRate(double SR){
	SHIFT_RATE = SR;
}

SHIFT_METHOD sinkNode::getShiftMethod(){
	return this->sm;
}

map<int, set<int>> sinkNode::sensorNeighbors;