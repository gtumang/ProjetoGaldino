#include <iostream>
#include <stdio.h>
#include <thread>
#include <string>

#include "argsparser.hpp"
#include "CNC_Fanuc.hpp"
#include "json.hpp"
#include "sqlite3.h"
using json = nlohmann::json;
using namespace std;

int main(int argc, char** argv){
	ARGS args = parseArgs(argc, argv);
	const auto sample_period = std::chrono::milliseconds(args.poll);
	try{
		CNC_Fanuc fanuc(args.ip);
		json j;	
		fanuc.getData(args.option, &j);
		int idx = 0;
		string str;
		int val;
		while(idx<j["Alarm"].size()){
			
			if ( j["Alarm"]["PMC_" + std::to_string(idx)]!=NULL){
				str = j["Alarm"]["PMC_" + std::to_string(idx)];
				j["Alarm"]["PMC_" + std::to_string(idx)] = stoi(str);
				}
			idx++;
		}
		cout << "Teste" << std::endl;
		cout << "Data Sent..." << std::endl;
		cout << j.dump() << std::endl;
	} catch (CNC_Exception& e) {
	    cerr << e.what() << endl;
	    return 1;
	}
	return 0;
}
