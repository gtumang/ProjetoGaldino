#include <iostream>
#include <stdio.h>
#include <thread>
#include <string>
#include <map>
#include <fstream>

#include "argsparser.hpp"
#include "MQTTClient.hpp"
#include "CNC_Fanuc.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

int main(int argc, char** argv){
	ARGS args = parseArgs(argc, argv);
	const auto sample_period = std::chrono::milliseconds(args.poll);
	const auto err_poll = std::chrono::milliseconds(5000);
	while(true){
		try{
			CNC_Fanuc fanuc(args.ip);
			try {
				MQTTClient mqtt(args.mqttHost, args.clientId);
   				mqtt.Connect(args.username,args.password);
				std::string str;
				while (true) {
					json j;
					fanuc.getData(args.option, &j);
					short mqtterror = mqtt.Publish(args.topic, j.dump());
					cout << "Data Sent..." << std::endl;
					cout << j.dump(0) << std::endl;
					this_thread::sleep_for(sample_period);
				}
			} catch (const mqtt::exception& exc) {
				cerr << exc.what() << endl;
				this_thread::sleep_for(err_poll);
			}
		} catch (CNC_Exception& e) {
	    		cerr << e.what() << endl;
	    		this_thread::sleep_for(err_poll);
		}
	}
	return 0;
}
