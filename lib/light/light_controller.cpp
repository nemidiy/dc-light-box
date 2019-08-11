#include "light_controller.h"
#include <sstream>

using namespace dc::utils;

LightController::LightController(){

}

LightController::~LightController(){

}

void LightController::begin(){

}

bool LightController::add_channel(
        uint8_t relay_pin,
        uint8_t dimming_pin,
        const std::string& name){

	pinMode(relay_pin, OUTPUT);
	pinMode(dimming_pin, OUTPUT);
	digitalWrite(relay_pin, HIGH);
	analogWrite(dimming_pin, 0);

	auto conf = new channel_config;
	conf->relay_pin = relay_pin;
	conf->dimming_pin = dimming_pin;
	conf->dim_value = 0;
	conf->on = true;

	HomieNode* light_node = new HomieNode(name.c_str(), "switch");
    nodes.insert(make_pair(name, light_node));
    channels.insert(make_pair(name, conf));

    auto on_handler = [conf, this, light_node](
    		const HomieRange& range, const String& value) -> bool{
    	Serial.println("switch light handler");
    	if(value != "true" && value != "false")
    		return false;
    	if(value == "true"){
    		conf->on = true;
    		digitalWrite(conf->relay_pin, HIGH);
    	}else if(value == "false"){
    		conf->on = false;
    		digitalWrite(conf->relay_pin, LOW);
    	}
    	light_node->setProperty("on").send(value);
    	return true;
    };

    auto dim_handler = [conf, this, light_node](
    		const HomieRange& range, const String& value) -> bool{
    	Serial.println("dim light handler");

    	int v = value.toInt();
    	if(v > 1023){
    		v = 1023;
    	}else if(v < 0){
    		v = 0;
    	}
    	analogWrite(conf->dimming_pin, v);
    	conf->dim_value = v;
    	light_node->setProperty("dim").send(String(v));
    	return true;
    };

    light_node->advertise("on").settable(on_handler);
    light_node->advertise("dim").settable(dim_handler);

	return true;
}

bool LightController::add_channel(
        uint8_t relay_pin,
        const std::string& name){
	pinMode(relay_pin, OUTPUT);
	return true;
}

void LightController::loop(){
	if (millis() - this->lastSent >= this->INTERVAL * 1000UL
            || this->lastSent == 0) {
		for(auto& kv: nodes){
			String status = "false";
			if(channels[kv.first]->on)
				status = "true";
			kv.second->setProperty("on").send(status);
        	kv.second->setProperty("dim").send(
        		String(channels[kv.first]->dim_value));
    	}
        this->lastSent = millis();
    }
}