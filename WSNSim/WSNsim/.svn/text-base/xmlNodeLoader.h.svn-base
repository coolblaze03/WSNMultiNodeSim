#ifndef __XMLNODELOADER_H__
#define __XMLNODELOADER_H__

#include "sinkNode.h"
#include "sensorNode.h"
#include "neighbor.h"
#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>


#include <locale>
#include <sstream>

using namespace std;
using namespace pugi;

class xmlNodeLoader{
public:

	xmlNodeLoader(sinkNode* sinks, sensorNode* nodes){
		this->sinks = sinks;
		this->nodes = nodes;
	}

	void loadXML(char* filename){

		string line;
		string data = "";
		ifstream myfile (filename);
		if (myfile.is_open())
		{
			while ( myfile.good() )
			{
				getline (myfile,line);
				data = data + line;
			}
			myfile.close();
		}

		else cout << "Unable to open file"; 

		wstring temp = widen(data.c_str());
		int s1 = data.length();
		int s2 = temp.length();

		xml_document doc;    // character type defaults to char
		xml_parse_result result = doc.load_file("nodestructure.xml");

		if (result)
			std::cout << "XML [" << "nodestructure.xml" << "] parsed without errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n\n";
		else
		{
			std::cout << "XML [" << "nodestructure.xml" << "] parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
			std::cout << "Error description: " << result.description() << "\n";
			std::cout << "Error offset: " << result.offset << " (error at [..." << ("nodestructure.xml" + result.offset) << "]\n\n";
		}


		string name = doc.first_child().name();

		int cursink = -1;
		int cursensor = -1; 


		

		for (pugi::xml_node_iterator it = doc.begin(); it != doc.end(); ++it)
		{

			baseNode *bn;
			
			for (pugi::xml_node ait = it->first_child(); ait <= it->last_child(); ait = ait.next_sibling())
			{
				
				if (ait.empty()){
					///cout<<endl<<endl;
					break;
				}

			
				if ((string)ait.name() == "Type"){

					string ty = (string)ait.first_child().value();

					if (ty == "Sink"){
					
						cursink++;
						bn = &sinks[cursink];
					}else if (ty == "Sensor"){
					
						cursensor++;
						bn = &nodes[cursensor];
					}else{
						throw "Error No Type";
					}

				}else if ((string)ait.name() == "ID"){
					int temp;
					sscanf(ait.first_child().value(), "%d", &temp); 
					bn->ID = temp;
				}else if ((string)ait.name() == "oneHopNeighbor"){
					int temp;
					sscanf(ait.first_child().value(), "%d", &temp); 
					neighbor n;
					n.ID = temp;
					bn->oneHopNeighbors.push_back(n);
				}else if ((string)ait.name() == "children"){
					int temp;
					sscanf(ait.first_child().value(), "%d", &temp); 
					int id = temp;
					bn->children.insert(id);
				}else if ((string)ait.name() == "x"){
					int temp;
					sscanf(ait.first_child().value(), "%d", &temp); 
					int loc = temp;
					bn->x = loc;
				}else if ((string)ait.name() == "y"){
					int temp;
					sscanf(ait.first_child().value(), "%d", &temp); 
					int loc = temp;
					bn->y = loc;
				}else if ((string)ait.name() == "initial_energy"){
					int temp;
					sscanf(ait.first_child().value(), "%d", &temp); 
					int energy = temp;
					nodes[cursensor].setPower(energy);
					//throw "need to do change to ndoe";
				}else if ((string)ait.name() == "parent"){
					///not currently used for anything. Will let tree set itself up. 
				}else{
					throw (string)ait.name() + "Not Implemented";
				}

				

				//std::cout << " " << ait.name() << "=" << ait.first_child().value();
			}

			
			
		}
		cout<<filename<<" Loaded"<<endl;
		cout<<endl;
		int jsklj = 0;
	}

private:
	sinkNode * sinks;
	sensorNode * nodes;

	wstring widen( const string& str )
	{
		wostringstream wstm ;
		const ctype<wchar_t>& ctfacet = use_facet< ctype<wchar_t> >( wstm.getloc() ) ;
		for( size_t i=0 ; i<str.size() ; ++i )
			wstm << ctfacet.widen( str[i] ) ;
			return wstm.str() ;
		}
	};

#endif