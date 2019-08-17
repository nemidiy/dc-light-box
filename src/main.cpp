#include <Homie.h>
#include <FS.h>

#include <string>
#include <sstream>

#include <light_controller.h>
#include <homie_handler.h>

// WIFI IP
std::string ip;

//light controller
dc::utils::LightController light_controller;


void setup(){
    Serial.begin(115200);
    Serial << endl << endl;

    Homie_setFirmware("light-box", "1.0.0");

    Homie.onEvent(on_event);

    light_controller.add_channel(D1, D5, "whites");

    Homie.setup();
}

bool high = true;

void loop(){
    Homie.loop();
    ip = WiFi.localIP().toString().c_str();
    light_controller.loop();
    delay(10);
}