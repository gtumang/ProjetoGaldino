#include <string>
#include "MQTTClient.hpp"
#include "mqtt/buffer_ref.h"
#include "mqtt/connect_options.h"

using namespace std;

const int  QOS = 1;

short MQTTClient::Connect(string username, string password){
	mqtt::connect_options options;
	mqtt::message willmsg("iot/events/disconnected", "Publisher disconnected", 1, true);
	mqtt::will_options will(willmsg);
	options.set_will(will);
	options.set_automatic_reconnect(1, 10);
	options.set_user_name(username);
	options.set_password(password);
	client->connect(options)->wait();
	return 0;
}

short MQTTClient::Publish(string topic,string message){
	try{
		mqtt::topic top(*client.get(), topic, QOS);
		mqtt::token_ptr tok;

		tok = top.publish(message);
		tok->wait();	// Just wait for the last one to complete.
	} catch (const mqtt::exception& exc) {
		cerr << exc.what() << endl;
		return 1;
	}
	return 0;
}

MQTTClient::MQTTClient(const string address, const string clientID){
	client.reset(new mqtt::async_client(address, clientID));
}

MQTTClient::~MQTTClient(){

	// cout << "\nDisconnecting..." << endl;
	client->disconnect()->wait();
	// cout << "  ...OK" << endl;
}
