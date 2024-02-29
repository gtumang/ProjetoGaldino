#pragma once
#include <string>
#include "mqtt/async_client.h"

class MQTTClient
{
	private:
		std::unique_ptr<mqtt::async_client> client;

	public:
		MQTTClient(std::string address, std::string clientID);
		~MQTTClient();
		short Connect(std::string username, std::string password);
		short Publish(std::string topic, std::string message);

};
