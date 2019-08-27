/*
Copyright 2019 Nicolas Emiliani

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


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

    WiFi.begin(
      HomieInternals::Interface::get().getConfig().get().wifi.ssid,
      HomieInternals::Interface::get().getConfig().get().wifi.password);
}

bool high = true;

void loop(){
    Homie.loop();
    ip = WiFi.localIP().toString().c_str();
    light_controller.loop();
    delay(10);
}