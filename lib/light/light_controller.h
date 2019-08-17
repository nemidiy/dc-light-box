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


#ifndef __DIFFCUMULUS_LIGHT_CONTROLLER_H__
#define __DIFFCUMULUS_LIGHT_CONTROLLER_H__

#include <Homie.h>
#include <map>
#include <string>

namespace dc {

namespace utils {

struct LightController {

    LightController();

    virtual ~LightController();

    void begin();

    bool add_channel(
        uint8_t relay_pin,
        uint8_t dimming_pin,
        const std::string& name);

    bool add_channel(
        uint8_t relay_pin,
        const std::string& name);

    void sunset_channel(
        int total_seconds,
        const std::string& name);

    void sunrise_channel(
        int total_seconds,
        const std::string& name);

    void loop();

protected :

    typedef struct {
        uint8_t relay_pin;
        uint8_t dimming_pin;
        unsigned long int dim_value;
        bool on;
        bool sunsetting;
        bool sunrising;
        unsigned long int millis;
        unsigned long int start;
        unsigned long int millis_per_dimming_unit;
        unsigned long int dimming_offset;
    } channel_config;

    std::map<std::string, HomieNode*> nodes;
    std::map<std::string, channel_config*> channels;

    unsigned long int lastSent = 0;
    unsigned long int  INTERVAL = 10;

    void sunset(channel_config* conf);

    void sunrise(channel_config* conf);
};

} //namespace utils

}//namespace dc

#endif