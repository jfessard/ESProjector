# ESProjector
Voice control your Panasonic projector with an esp8266 (Alexa compatible) and control sources on an 4x1 HDMI switch

## The problem at hand
My projector is on a high closet, which also houses the media sources. Since they're behind a door and I don't want to setup an expensive universal remote, I wanted a reliable way of turning the projector on and off, and also switching video sources.
Since I have several amazon echos, I like the concept of voice controlling everything so that's what I went for. 

Thankfully I found out about [fauxmoesp] which emulates a wemo device with on/off control, requiring little work and no extra servers/accounts to get it to work. (unlike other solutions I have seen)

## Requirements
* Panasonic PT-AE3000U projector (might work on other Panasonic models)
* Monoprice [HDX-401TA][hdmiswitch] 4x1 HDMI switch
* ESP8266 (I used a [D1 mini][d1mini])
* MAX232-based level shifter [board][rs232]
* [IR led module][irmodule]. 5V powered, 3.3V data
* [TSOP38238][tsop] (38kHz IR receiver, for learning codes)
* wire, soldering iron, box etc.

## Software Used
* platformio
* Arduino esp8266 framework (latest version)
* [fauxmoesp]
* [IRremoteESP8266][irremote]

[irremote]: https://github.com/markszabo/IRremoteESP8266
[fauxmoesp]: https://bitbucket.org/xoseperez/fauxmoesp
[hdmiswitch]: https://www.monoprice.com/product?p_id=5557
[rs232]: https://www.ebay.com/itm/201579248241
[d1mini]: https://www.ebay.com/itm/172646774462
[irmodule]: https://www.ebay.com/itm/Infrared-Transmitter-Module-IR-Infrared-Sensor-LED-Indicate-For-Arduino-SLM/132243573356
[tsop]: https://www.ebay.com/itm/2x-TSOP38238-Remote-control-receiver-infrared-receiver-head-Photoelectric-switch/292101297804
