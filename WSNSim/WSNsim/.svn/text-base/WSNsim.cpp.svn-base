// WSNsim.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "sinkNode.h"
#include "sensorNode.h"
#include "TransmissionManager.h"
#include "MessageGenerator.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include "xmlNodeLoader.h"

//PROTOTYPES
void printNodesToFile(sinkNode sinks[], sensorNode nodes[],char * filename);
void printResultsToFile(int cyclesran, int maxlevel, int totaldatamessages, string filename);
void printToScreen(int cyclesran, int maxlevel, int totaldatamessages);
void AddAllSinkLocations(sinkNode sinks[]);

//MAIN
int _tmain(int argc, _TCHAR* argv[])
{

	int run = 8;
	if (argc == 2){
		std::basic_istringstream<_TCHAR> ss(argv[1]);
		ss >> run;
	}

	

	//while (run <= 10){



		bool LoadLastRun = true; //If you want to load the last run again
		bool isSinkShifting = true; //If you want the sink to shift then use this flag
		bool disableCycling = true;

		switch (run){
		case 0:
			SHIFT_RATE = 0.45;
			break;
		case 1:
			SHIFT_RATE = 0.50;
			break;
		case 2:
			SHIFT_RATE = 0.55;
			break;
		case 3:
			SHIFT_RATE = 0.60;
			break;
		case 4:
			SHIFT_RATE = 0.65;
			break;
		case 5:
			SHIFT_RATE = 0.70;
			break;
		case 6:
			SHIFT_RATE = 0.75;
			break;
		case 7:
			SHIFT_RATE = 0.80;
			break;
		case 8:
			SHIFT_RATE = 0.85;
			break;
		case 9:
			SHIFT_RATE = 0.90;
			break;
		case 10://no shifting
			SHIFT_RATE = 0.00;
			isSinkShifting = false;
			break;
		}
	
		SHIFT_METHOD sm = SHIFT_METHOD::SHIFT_SINK_CIRCLE; //Circle is the default. If sink is not shifting because of the flag above it will change to the appropriate one. 

		bool done = false;
		cout<<"Starting Simulation."<<endl;cout<<endl;
		cout<<"Powering on nodes."<<endl;cout<<endl;

		int resets = 0;
		int totalMaxLevel = 0;

		 //means we are not shifting on current run.
		if (isSinkShifting){
			cout<<"Sinks will shift on this run."<<endl;cout<<endl;
		}else{
			cout<<"Sinks will not shift on this run."<<endl;cout<<endl;
			sm = SHIFT_METHOD::SHIFT_SINK_NO_SHIFT;
		}

		cout<<SHIFT_RATE<<endl;

		while (true) {

			//cout<<"Starting Simulation."<<endl;
			bool rountingSuccessful = false; //Since were using random points i would rather have all nodes and good data rather than scattered bad nodes.
			//cout<<endl;
			int currentSink = 0; //default
		
			//Do sink switching.
			sinkNode sinks[NUMBER_OF_SINKS] = {sinkNode()};//you need to specify what to use to do the shift. There are a few different algorithms for it.

			//Loads the shift methods for the sinks
			for (int i = 0; i < NUMBER_OF_SINKS; i++){
					sinks[i] = sinkNode();
					sinks[i].setShiftMethod(sm);
					sinks[i].setShiftRate(SHIFT_RATE);
			}

			//Im going to assume since all sinces have the ability to communicate by either web or sat. then they will automatically
			//know the location of all other sinks. 
			AddAllSinkLocations(sinks);
			
			sinkNode* sink = &sinks[currentSink];
			//sink->active = true;//set the first sink as the active sink.
			sink->setSinkActiveStatus(true);

			sensorNode nodes[NUMBER_OF_NODES] = {sensorNode()};

			if (disableCycling){
				for (int i = 0; i<NUMBER_OF_NODES; i++){
					nodes[i] = sensorNode();
					nodes[i].setCycling(false);
				}
			}
		
			transmissionManager tm(sinks, nodes);

			if (LoadLastRun){
				xmlNodeLoader xloader(sinks, nodes);
				xloader.loadXML("nodestructure.xml");
			}
		
			//Will generate random messages
			MessageGenerator mg(nodes);

			//if (LoadLastRun)
			//This will allow us to generate a list of ranodm numbers between 0 to 99 that will act as the random node message selecton. Keeping this constant will make comparing results more accurate. 
			mg = MessageGenerator(nodes, "RandomNodalOutput.txt");


			//In the intial power on the sinks will start to establish the tree and the nodes will send their hellos. 
			//cout<<"Powering on nodes."<<endl;
			//cout<<endl;
			//init all nodes
			sink->powerOn();
			for (int i = 0; i<NUMBER_OF_NODES; i++){
				nodes[i].powerOn();
			}

			//processTransmissions will process all pending outqueue transmissions
			//The processtransmission will simulate all raidio transmission. This simulated transmission will also 
			//do the lookups for neighbors since this is part of the transmission phase to send messages. 
			tm.processTransmissions();

			//For this sim I will try to use all nodes. Since the distribution of every node is completely random 
			//if i dont have a good distribution i will just start the process over again. I will give it 50 cycles. 
			int cyclesran = 0;

			//The way in which the cycles are processed are as follows
			//1) For the first 50 cycles all nodes are processed simultaniously. 
			//2) Once the routing tree is established then then sink will send down the LEVELANNOUNCE. 
			//3) Once this happens then nodes will set a timer to know when to begin
			//This timer is a countdown until the actual cycles for each level of the network are to begin.
			//This timer is the maxLevel - currentLevel. Once this reaches 0 then it starts processing based upon the wakeup timer. 
			//4) the cycles will only process nodes in which the wakeuptimer == 0 
			//This network has been mostly made to be completely self sufficent. It will figure out its on paths and such will not be relient
			// on any outside info. It will only need to know which nodes are in its range by using the radio wave max distance. 
			while (true){

				//on the 50th cycle if all nodes are not included then just start he process over with new random variables. 
				if (cyclesran == 50 && sink->getFoundSensorNodesCount() < NUMBER_OF_NODES){
					rountingSuccessful = false;
					//cout<<"Resetting Simulation"<<endl;
					//cout<<endl;
					resets++;
					break;//exit out because bad random values for node position. Then restart
				}else if (cyclesran > 50 && sink->getFoundSensorNodesCount() == NUMBER_OF_NODES && rountingSuccessful == false){
				
				
					cout<<"Resets: "<<resets<<endl;
					cout<<endl;
				
					if (!LoadLastRun){
						cout<<"Printing Node Info to nodestructure.xml"<<endl;
						cout<<endl;
						printNodesToFile(sinks, nodes, "");
					}else{
					
						cout<<"Running Last Run"<<endl;
						cout<<endl;
					}
				
					totalMaxLevel = sink->maxLevel;
					rountingSuccessful = true;

				}

				//TODO:Switch sinks after sink shift
				//Only Generate Messages if routing is complete cycle > 50 and also cycleran % maxLevel = 0
				if (cyclesran > 50 && (cyclesran % sink->getMaxLevel()) == 0){
					//This will generate the random messages at the beginning of each full network cycle.
					mg.generateRandomMessages();
					//mg.generateMessagesForAllNodes();
				}

				//This loop will allow all sink nodes to process messages. 
				for (int i = 0; i < NUMBER_OF_SINKS; i++){
					sinks[i].run();
				}

				//This loop will allow all nodes to process messages. 
				for (int i = 0; i<NUMBER_OF_NODES; i++){
					nodes[i].run();
				}

				///////////////////////////////////////////////////////////////TESTING PURPOSES
				int dead_count = 0;
				for (int i = 0; i<NUMBER_OF_NODES;i++){
					if (nodes[i].isDead())
						dead_count++;
				}
				if (dead_count >= 100){
						//SHOULD NEVER BE HERE.
					done = true;
					break;
				}

			
				
				///////////////////////////////////////////////////////////////

				tm.processTransmissions();
				 //cout<<cyclesran<<endl; //Just to print out cycle #s
				cyclesran++;

				if (cyclesran%10000 == 0)
					cout<<cyclesran<<endl;


				////////////////////Portion of no sink shifting///////////////////////////////////////////
				if (isSinkShifting == false){
					if (sink->getNumberOfChildren() <= 0){
						done = true;
						break;
					}
				}
				/////////////////////////////////////////////////////////////////////////////////////////

				int noreach = 0;
				for (int i = 0; i < NUMBER_OF_SINKS; i++){
					if (sinks[i].getNumberOfChildren() <= 0)
						noreach++;
				}
				if (noreach == NUMBER_OF_SINKS){
					done = true;
					break;
				}

			}



			if (done == true){

				//get total data messages
				int totaldatamessages = 0;
				for (int i = 0; i < NUMBER_OF_SINKS; i++){
					totaldatamessages += sinks[i].getDataMessageCount();
				} 

				string mains("output");
				std::ostringstream strs;
				strs << SHIFT_RATE;
				std::string srate = strs.str();
				string ext(".txt");

				string fullname = mains + srate + ext;
			
				printResultsToFile(cyclesran, totalMaxLevel, totaldatamessages, fullname);// sink->maxLevel);
				printToScreen(cyclesran, totalMaxLevel, totaldatamessages);//sink->maxLevel);
		
				break;
			}

	
		

		}

		//run++;

	//}

	return 0;
}

void printNodesToFile(sinkNode sinks[], sensorNode nodes[],char * filename){
	

	string xmloutput = "<?xml version=\"1.0\"?>";

	for (int i = 0; i<NUMBER_OF_SINKS;i++){
		xmloutput += sinks[i].NodeXML();
	}

	for (int i = 0; i<NUMBER_OF_NODES;i++){
		xmloutput += nodes[i].NodeXML();
	}

	ofstream myfile;
	myfile.open ("nodestructure.xml");
	myfile << xmloutput;
	myfile.close();


}

void printResultsToFile(int cyclesran, int maxlevel, int totaldatamessages, string filename){

			ofstream myfile;
			myfile.open (filename);		

			double lifetime_in_seconds = (cyclesran/(maxlevel * 1.0)) * NETWORK_CYCLE;
			double lifetime_in_hours = lifetime_in_seconds/3600;
			double lifetime_in_days = lifetime_in_hours/24;
			myfile<<"Sensor nodes started with 100 joules of energy"<<endl;
			myfile << "Sensor network died at cycle: "<< cyclesran <<  endl;
			myfile << printf("That is: \t Time in Seconds: %12.2f\n",lifetime_in_seconds);
			myfile << "\t\t Time in Hours: " << lifetime_in_hours <<  endl;
			myfile << "\t\t Time in Days: " << lifetime_in_days <<  endl;
		
			myfile << "Suppose that all nodes use two AA NiMH battery, each with 9072 joule. The total supply is 18144 joule,";
			myfile << " according to http://www.allaboutbatteries.com/Energy-tables.html." << endl;
			myfile << "Below is the lifelifetime when using AAA NiMH batteries." << endl;
			double m = BATTERY_RATIO;

			myfile << printf("Sensor network died at cycle: %12.2f\n", cyclesran * m);
			myfile << printf("That is: \t Lifetime in Seconds: %12.2f\n",lifetime_in_seconds * m);
			myfile << "\t\t Lifetime in Hours: " << lifetime_in_hours * m <<  endl;
			myfile << "\t\t Lifetime in Days: " << lifetime_in_days * m <<  endl;

			myfile<<endl;
			myfile<<"Total # of messages Received: "<<totaldatamessages<<endl;

			
			myfile.close();
}

void printToScreen(int cyclesran, int maxlevel, int totaldatamessages){
		double lifetime_in_seconds = (cyclesran/maxlevel * 1.0) * NETWORK_CYCLE;
		double lifetime_in_hours = lifetime_in_seconds/3600;
		double lifetime_in_days = lifetime_in_hours/24;
		cout<<"Sensor nodes started with 100 joules of energy"<<endl;
		cout << "Sensor network died at cycle: "<< cyclesran <<  endl;
		printf("That is: \t Time in Seconds: %12.2f\n",lifetime_in_seconds);
		cout << "\t\t Time in Hours: " << lifetime_in_hours <<  endl;
		cout << "\t\t Time in Days: " << lifetime_in_days <<  endl;
		
		cout << "Suppose that all nodes use two AA NiMH battery, each with 9072 joule. The total supply is 18144 joule,";
		cout << " according to http://www.allaboutbatteries.com/Energy-tables.html." << endl;
		cout << "Below is the lifelifetime when using AAA NiMH batteries." << endl;
		double m = BATTERY_RATIO;

		printf("Sensor network died at cycle: %12.2f\n", cyclesran * m);
		printf("That is: \t Lifetime in Seconds: %12.2f\n",lifetime_in_seconds * m);
		cout << "\t\t Lifetime in Hours: " << lifetime_in_hours * m <<  endl;
		cout << "\t\t Lifetime in Days: " << lifetime_in_days * m <<  endl;

		cout<<endl;
		cout<<"Total # of messages Received: "<<totaldatamessages<<endl;
		cout<<endl;
		/*cout << "Press Enter to continue" << endl;
		cin.ignore(100, '\n');*/
}

//This will add all sink location info. I could move it to regular communication if nessasary. I assume that 
//since the sinks have long range transmission or through the internet that this doesnt nessasarily have to be
//sent through a radio transmission. This can be changed. 
void AddAllSinkLocations(sinkNode sinks[]){

	customTreeNode tn[NUMBER_OF_SINKS];

	for (int i = 0; i<NUMBER_OF_SINKS; i++){
		tn[i].ID = sinks[i].ID;
		tn[i].x = sinks[i].x;
		tn[i].y = sinks[i].y;
	}

	for (int i = 0; i<NUMBER_OF_SINKS; i++){
		for (int j = 0; j<NUMBER_OF_SINKS;j++){
			//sinks[i].allSinks.push_back(tn[j]);
			//sinks[i].addSink(tn[j]);
			sinks[i].addSink(&sinks[j]);
		}
	}

}