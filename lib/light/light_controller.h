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

    void loop();

protected :

    typedef struct {
        uint8_t relay_pin;
        uint8_t dimming_pin;
        uint8_t dim_value;
        bool on;
    } channel_config;

    std::map<std::string, HomieNode*> nodes;
    std::map<std::string, channel_config*> channels;

    unsigned long int lastSent = 0;
    unsigned long int  INTERVAL = 10;
};

} //namespace utils

}//namespace dc

#endif