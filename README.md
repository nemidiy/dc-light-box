# Light Box
Light box is an IOT controller for ESP8266 based on the [Homie](https://homieiot.github.io/) standard for MQTT and the [Homie Lib for ESP8266](https://github.com/homieiot/homie-esp8266) firmware.

# Disclaimer
* Basic c++ coding knowledge is assumed
* The instructions assume you are using some Linux flavor, if using Windows or CrApple some things may not apply.
* Basic knowledge of wiring
* Electrocution hazzard, be smart and stay off if this is too much for you :)

# Features
* ON/OFF
* Dimming
* Support for multiple channels (ie : whites, reds, etc)
* Sunrise and sunset functions

# Pre-requisites
* python-virtualenv : used to create a virtual env that will contain platform.io
* cURL

# Downloading and building the code

First clone the the repo somewhere in your computer:

```
$ git clone https://github.com/nemiliani/dc-light-box.git
$ cd dc-light-box
$ git checkout develop
```
Create and activate the python virtual environment so that we can install platformio
```
$ virtualenv ven
$ source venv/bin/activate
```
Install the requirements file (has the dep for platformio)
```
(venv)$ pip install -r requirements.txt
```
Once it finishes you should be able to build the code:
```
(venv)$ platformio run
....
Compiling .pioenvs/nodemcu/FrameworkArduino/uart.cpp.o
Compiling .pioenvs/nodemcu/FrameworkArduino/umm_malloc/umm_malloc.cpp.o
Archiving .pioenvs/nodemcu/libFrameworkArduino.a
Indexing .pioenvs/nodemcu/libFrameworkArduino.a
Linking .pioenvs/nodemcu/firmware.elf
Retrieving maximum program size .pioenvs/nodemcu/firmware.elf
Checking size .pioenvs/nodemcu/firmware.elf
Building .pioenvs/nodemcu/firmware.bin
Memory Usage -> http://bit.ly/pio-memory-usage
DATA:    [====      ]  40.6% (used 33300 bytes from 81920 bytes)
PROGRAM: [====      ]  42.1% (used 440044 bytes from 1044464 bytes)
Creating BIN file ".pioenvs/nodemcu/firmware.bin" using ".pioenvs/nodemcu/firmware.elf"
========================================================================================== [SUCCESS] Took 33.80 seconds ==========================================================================================
```

This means you are good to go, you should just upload it to your nodemcu. The following step depends on the fact that you are using nodemcu V2. If you are using some other flavour for the ESP8266 then you are probably going to have to make changes on :
* The PIN declarations in main.cpp
* platformio.ini file to specify the correct parameters for your board

Back to uploading :
```
(venv)$ platformio run -t upload
```

# Setting it up
You are now ready to start setting it up. If you have never read about Homie I highly recommend that you do here :
* https://homieiot.github.io/ : for the generic spec
* https://homieiot.github.io/homie-esp8266/docs/develop/ : for the esp8266 spific stuff

Otherwise you will be missing on cool features like :
* OTA
* Auto-discovery

Now to the steps you need to follow in order to set it up:

### AP mode
The board will go into AP mode, this means that it will present a Wifi network you will need to connect to in order to set up your node. This is pretty much the same mechanism you would use with any other WiFi appliance you buy (a WiFi extender comes to mind) 
* Go to your OS Wifi settings
* Look for one called homie-xxXXxxxxXXxxx
* Connect to it

You are now in the same network as your nodemcu board. The board will take the IP address **192.168.123.1**. We will use that address to end the configuration parameters 

If you connect to the Serial port of your nodemcu board you will see the follwing :
```
(venv)$ pio device monitor --port /dev/ttyUSB0 --baud 115200
HomieEventType::CONFIGURATION_MODE
💡 Firmware light-box (1.0.0)
🔌 Booting into config mode 🔌
Device ID is cc50e33cf888
AP started as Homie-cc50e33cf888 with IP 192.168.123.1
Triggering Wi-Fi scan...
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
✔ Wi-Fi scan completed
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
Triggering Wi-Fi scan...
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
✔ Wi-Fi scan completed
```
This means that the board is effectively in AP mode and that it's in Config mode.

### Sending the configuration
In the etc folder for the project there is a sample configuration file device_config.example.json that looks like this :
```
(venv)$ cat etc/device_config.example.json
{
  "name": "Light Box",
  "device_id": "lightbox_01",
  "device_stats_interval": 10,
  "wifi": {
    "ssid": "wifi",
    "password": "pass"
  },
  "mqtt": {
    "host": "192.168.0.100",
    "port": 1883,
    "base_topic": "devices/",
    "auth": false
  },
  "ota": {
    "enabled": true
  }
}
```
Change :
* wifi.ssid with the name of your Wifi network
* wifi.password with the pass for your network
* mqtt.host with the IP of your MQTT broker
* mqtt.port with the port of your MQTT broker
* mqtt.base_topic: change this if you want LightBox in a different topic
* mqtt.auth : change this to true if your broker has auth enabled and if so also set mqtt.username and mqtt.password

For more details on configs go [here](https://homieiot.github.io/homie-esp8266/docs/develop/configuration/json-configuration-file/)

Now we have the config file in place we can send it to to board. For that we will use cURL but you can use whatever tool that allows to send a POST HTTP request with a json body (ie: postman, although a bit overkill)
```
(venv)$ curl -XPUT --header "Content-Type: application/json" http://192.168.123.1/config -d @device_config.sample.txt
{"success":true}
```
If you get the **{"success":true}** means you are good to go. As a consequence to board will restart and connect to the Wifi and MQTT broker you specified.
If at that moment you were connected to the Serial port you will see :

```
Received config request
✔ Configured
↻ Rebooting into normal mode...

 ets Jan  8 2013,rst cause:2, boot mode:(3,6)

load 0x4010f000, len 1384, room 16 
tail 8
chksum 0x2d
csum 0x2d
v951aeffa
~ld


HomieEventType::NORMAL_MODE
💡 Firmware light-box (1.0.0)
🔌 Booting into normal mode 🔌
{} Stored configuration
  • Hardware device ID: cc50e33cf888
  • Device ID: lightbox_01
  • Name: Light Box
  • Device Stats Interval: 10 sec
  • Wi-Fi: 
    ◦ SSID: XYZ
    ◦ Password not shown
  • MQTT: 
    ◦ Host: 192.168.X.X
    ◦ Port: 1883
    ◦ Base topic: devices/
    ◦ Auth? no
  • OTA: 
    ◦ Enabled? yes
↕ Attempting to connect to Wi-Fi...
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
✖ setNodeProperty(): impossible now
✔ Wi-Fi connected, IP: 192.168.86.64
Triggering WIFI_CONNECTED event...
HomieEventType::WIFI_CONNECTED
↕ Attempting to connect to MQTT...
Sending initial information...
✔ MQTT ready
Triggering MQTT_READY event...
HomieEventType::MQTT_READY
Calling setup function...
〽 Sending statistics...
  • Wi-Fi signal quality: 92%
  • Uptime: 8s
〽 Sending statistics...
  • Wi-Fi signal quality: 100%
  • Uptime: 18s
```
This shows that the board :
* got the HTTP request
* that it was properly formated
* that it saved the config on it's internal storage
* that it reseted itself
* After resetting it correctly connected to Wifi and to the MQTT broker

### Resetting to configuration mode again
If u wanted to change any configuration, ie the Wifi network or password you will need to put the board again in configuration mode. For that, hold the flash button down for 5 secs, the board will delete the current configuration from it's internal storage and go back to configuration mode.


# MQTT Topics
### Turning on and off the lights
Assuming when configuring you kept **devices** as base topic and the device_id to **lightbox_01**

**devices/lightbox_01/whites/on/set**

Example using mosquitto as the publisher:
```
# Turn on/off
mosquitto_pub -t devices/lightbox_01/whites/on/set -m true
mosquitto_pub -t devices/lightbox_01/whites/on/set -m false
```

**devices/lightbox_01/whites/dim/set**

* Dimming goes from 0 (the brightest) to 1023 (the dimmest)
Example using mosquitto as the publisher:
```
# Dimming
mosquitto_pub -t devices/lightbox_01/whites/dim/set -m 0
mosquitto_pub -t devices/lightbox_01/whites/dim/set -m 500
mosquitto_pub -t devices/lightbox_01/whites/dim/set -m 1023
```

**devices/lightbox_01/whites/sunrise/set**

* Sunrise takes the amount of seconds that the sunrise should last
* For sunrise to work the channel needs to have the "on" attribute set to false
* At any point sunrise may be aborted by changing the states of the "on" or "dim" attributes
Example using mosquitto as the publisher:
```
# Sunrise
mosquitto_pub -t devices/lightbox_01/whites/sunrise/set -m 3600
```

**devices/lightbox_01/whites/sunset/set**

* Sunset takes the amount of seconds that the sunset should last
* For sunset to work the channel needs to have the "on" attribute set to false
* At any point sunset may be aborted by changing the states of the "on" or "dim" attributes
Example using mosquitto as the publisher:
```
# Sunset
mosquitto_pub -t devices/lightbox_01/whites/sunrise/set -m 3600
```

# The code

```c++
#include <Homie.h>
#include <FS.h>

#include <string>
#include <sstream>

/*
  This are the two includes from the lightbox library
*/
#include <light_controller.h>
#include <homie_handler.h>

// WIFI IP
std::string ip;

/*
  Light controller.
  This is the class that will control the channels, each channel will control
  a led driver. The main.cpp included expects only one channel that we will call
  and advertise over MQTT as "whites".
*/
dc::utils::LightController light_controller;


void setup(){
    Serial.begin(115200);
    Serial << endl << endl;

    Homie_setFirmware("light-box", "1.0.0");

    Homie.onEvent(on_event);

  /*
      Add the channel
      "D1" is the pin that will control the relay
      "D5" is the pin that will control the dimming using the opto-coupler
      "whites" is the name that will be advertised for the channel over MQTT
    */
    light_controller.add_channel(D1, D5, "whites");
    
  /*
      In case you wanted a second channel because you have a second driver
      to control the far red end of the spectrum you could add a second channel
    light_controller.add_channel(D2, D6, "farred");
    */
    
    Homie.setup();
}

bool high = true;

void loop(){
    Homie.loop();
    ip = WiFi.localIP().toString().c_str();
    
    // Call the light controller loop
    light_controller.loop();
    
    delay(10);
}
```

# Controler board

### Diagram
![diagram](https://github.com/nemiliani/dc-light-box/blob/develop/doc/dc-light-box_bb.png "Diagram")


# TODO
* Unit test
* Hooking up some CI tool to run unit tests when commiting
* Captive site to avoid the curl stuff
* OTA documentation
* MQTT auto discovery documentation
* BOM