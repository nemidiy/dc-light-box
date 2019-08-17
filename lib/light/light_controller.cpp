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
    conf->sunsetting = false;
    conf->sunrising = false;
    conf->millis = 0;
    conf->start = 0;
    conf->millis_per_dimming_unit = 1;

    HomieNode* light_node = new HomieNode(name.c_str(), "switch");
    nodes.insert(make_pair(name, light_node));
    channels.insert(make_pair(name, conf));

    auto on_handler = [conf, this, light_node](
            const HomieRange& range, const String& value) -> bool{
        Serial.println("switch light handler");
        //in case we were sunsetting or sunrising
        conf->sunsetting = false;
        conf->sunrising = false;

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

        //in case we were sunsetting or sunrising
        conf->sunsetting = false;
        conf->sunrising = false;

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

    auto sunset_handler = [name, conf, this, light_node](
            const HomieRange& range, const String& value) -> bool{
        Serial.print("sunset handler -> ");
        Serial.print(name.c_str());
        Serial.print(" - ");
        Serial.println(value.toInt());
        this->sunset_channel(value.toInt(), name);
        return true;
    };

    auto sunrise_handler = [name, conf, this, light_node](
            const HomieRange& range, const String& value) -> bool{
        Serial.println("sunrise handler");
        Serial.print(name.c_str());
        Serial.print(" - ");
        Serial.println(value.toInt());
        this->sunrise_channel(value.toInt(), name);
        return true;
    };

    light_node->advertise("on").settable(on_handler);
    light_node->advertise("dim").settable(dim_handler);
    light_node->advertise("sunset").settable(sunset_handler);
    light_node->advertise("sunrise").settable(sunrise_handler);
    light_node->advertise("sunsetting");
    light_node->advertise("sunrising");

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

            status = "false";
            if(channels[kv.first]->sunrising)
                status = "true";
            kv.second->setProperty("sunrising").send(status);

            status = "false";
            if(channels[kv.first]->sunsetting)
                status = "true";
            kv.second->setProperty("sunsetting").send(status);
        }
        this->lastSent = millis();
    }
    //apply sunset or sunrise
    for(auto& kv: channels){
        if(kv.second->sunsetting){
            sunset(kv.second);
        }else if(kv.second->sunrising){
            sunrise(kv.second);
        }
    }
}

void LightController::sunset_channel(
            int total_seconds,
            const std::string& name){
    if(!channels[name]->on)
        return;
    channels[name]->sunsetting = true;
    channels[name]->sunrising = false;
    channels[name]->millis = total_seconds * 1000;
    channels[name]->start = millis();
    channels[name]->dimming_offset = channels[name]->dim_value;
    int dimming_units = 1024 - channels[name]->dim_value;
    channels[name]->millis_per_dimming_unit = channels[name]->millis / dimming_units;
}

void LightController::sunset(channel_config* conf){

    if(conf->dim_value >= 1023){
        conf->dim_value = 1023;
        conf->sunsetting = false;
        conf->on = false;
        digitalWrite(conf->relay_pin, LOW);
        return;
    }

    // get the second we are standing on in the sunset
    unsigned long int  millis_in_sunset =  millis() - conf->start;
    unsigned long int dimming_units = millis_in_sunset / conf->millis_per_dimming_unit;

    conf->dim_value = conf->dimming_offset + dimming_units;
    analogWrite(conf->dimming_pin, conf->dim_value);
}

void LightController::sunrise_channel(
            int total_seconds,
            const std::string& name){
    if(channels[name]->on)
        false;
    channels[name]->sunsetting = false;
    channels[name]->sunrising = true;
    channels[name]->millis = total_seconds * 1000;
    channels[name]->start = millis();
    channels[name]->dimming_offset = channels[name]->dim_value;
    int dimming_units = 1024 - channels[name]->dim_value;
    channels[name]->millis_per_dimming_unit = channels[name]->millis / dimming_units;
    channels[name]->on = true;
    channels[name]->dim_value = 1023;
    analogWrite(channels[name]->dimming_pin, channels[name]->dim_value);
    digitalWrite(channels[name]->relay_pin, HIGH);
}

void LightController::sunrise(channel_config* conf){

    if(conf->dimming_offset == conf->dim_value){
        conf->sunrising = false;
        return;
    }

    // get the second we are standing on in the sunset
    unsigned long int  millis_in_sunset =  millis() - conf->start;
    unsigned long int dimming_units = millis_in_sunset / conf->millis_per_dimming_unit;

    conf->dim_value = 1024 - dimming_units;
    analogWrite(conf->dimming_pin, conf->dim_value);
}