#include "cxxopts.hpp"
#include <string>

struct ARGS {
	std::string ip;
	std::string clientId;
	std::string mqttHost;
	std::string username;
	std::string password;
	std::string option;
	std::string format;
	std::string topic;
	int poll;
};

ARGS parseArgs(int argc, char** argv){

	ARGS args;

	cxxopts::Options options("CNC Fanuc Monitor", "Get data from cnc machines and send to mqtt");

	options.add_options()
	  ("ip", "IP da maquina Fanuc", cxxopts::value<std::string>()->default_value("10.103.0.59")) 
	  ("clientid", "ID do Client", cxxopts::value<std::string>()->default_value("fanuc"))
	  ("mqtt", "MQTT Host - tcp://hostip:port", cxxopts::value<std::string>()->default_value("tcp://localhost:1883"))
	  ("u,username", "MQTT Username", cxxopts::value<std::string>()->default_value("rabbitmq"))
	  ("p,password", "MQTT Password", cxxopts::value<std::string>()->default_value("rabbitmq"))
	  ("o,option","Specifc monitor -status, axis, prog, alarm or all- specific informartion only", cxxopts::value<std::string>()->default_value("all"))
	  ("poll", "CNC Data Polling Interval (ms)",cxxopts::value<int>()->default_value("10000"))
	  ("f,format","Output format: json, string, influx")
	  ("t,topic", "MQTT Topic", cxxopts::value<std::string>()->default_value("iot/test"))
	  ("h,help", "Print Usage");

	auto result = options.parse(argc, argv);
    if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }

    args.ip = result["ip"].as<std::string>();
    args.mqttHost = result["mqtt"].as<std::string>();
  	args.clientId = result["clientid"].as<std::string>();
  	args.username = result["username"].as<std::string>();
  	args.password = result["password"].as<std::string>();
  	args.option = result["option"].as<std::string>();
  	args.topic = result["topic"].as<std::string>();
  	args.poll = result["poll"].as<int>();
  	return args;
}
