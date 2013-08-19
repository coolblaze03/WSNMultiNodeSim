#ifndef __MESSAGEGENERATOR_H__
#define __MESSAGEGENERATOR_H__
#include "sensorNode.h"
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

class MessageGenerator{
public:
	MessageGenerator(sensorNode nodes[]){
		this->nodes = nodes;
		srand ( time(NULL) );
		useFile = false;
	}

	MessageGenerator(sensorNode nodes[], char * filename){//loads set of numbers from a file instead of randomly. For testing
		this->nodes = nodes;
		srand ( time(NULL) );
		useFile = true;

		fn = filename;
		myfile.open(filename);
		//myfile = ifstream (filename);
		
	}


	void generateRandomMessages(){

		generatemessages(nodes);

	}

	void generateMessagesForAllNodes(){

		generateallmessages(nodes);

	}

static ifstream  myfile;

private:
	sensorNode * nodes;
	void generatemessages(sensorNode nodes[]){
		int num_mess_gen = int((double(MESSAGE_GENERATION_PERCENTAGE)/(double)100) * double(NUMBER_OF_NODES));

		for (int i = 0; i < num_mess_gen; i++){
			int rand_node = rand() % NUMBER_OF_NODES;
			
			if (useFile)
				rand_node = getNextint();
			
			if (nodes[rand_node].isDead())
				continue;
			nodes[rand_node].GenerateDataMessageToSink();
		}
	}
	void generateallmessages(sensorNode nodes[]){
		

		for (int i = 0; i < NUMBER_OF_NODES; i++){
			
			if (nodes[i].isDead())
				continue;
			nodes[i].GenerateDataMessageToSink();
		}
	}
	bool useFile;
	
	char * fn;

	int getNextint(){
		string line;
		int data = -1;
		if (myfile.is_open())
		{
			if ( myfile.good() )
			{
				getline (myfile,line);
				data = atoi(line.c_str());//istringstream ( line ) >> data;// = (int)line;
			}else{
				myfile.close();
				myfile.open(fn);// = ifstream (fn);
				data = getNextint();
			}
			return data;
			
		}
		return -1;
	}

};


ifstream MessageGenerator::myfile;

#endif