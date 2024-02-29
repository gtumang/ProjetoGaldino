
#include <iostream>
#include <stdio.h>
#include <thread>
#include <string>
#include <fstream>
#include "argsparser.hpp"
#include "CNC_Fanuc.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

int main(int argc, char** argv){

	ARGS args = parseArgs(argc, argv);

	const auto sample_period = std::chrono::milliseconds(args.poll);
	while(1){
		try{
		
			CNC_Fanuc fanuc(args.ip);
			json j;	
			//string j;
			fanuc.getData(args.option, &j);
		
						
			cout << "Data Sent..." << std::endl;
			std::ofstream joutput("DatabaseTeste.json");
			joutput << j << std::endl;
			
		} catch (CNC_Exception& e) {
		    cerr << "error" << endl;
		    cerr << e.what() << endl;
		    return 1;
		}

		this_thread::sleep_for(sample_period);
	}
	return 0;
}
