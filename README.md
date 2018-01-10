# ESProjector
Voice control your Panasonic projector with an esp8266 (Alexa compatible) and control sources on an 4x1 HDMI switch

## The problem at hand
My projector is on a high closet, which also houses the media sources. Since they're behind a door and I don't want to setup an expensive universal remote, I wanted a reliable way of turning the projector on and off, and also switching video sources.
Since I have several amazon echos, I like the concept of voice controlling everything so that was the main requirement. 

Thankfully I found out about [fauxmoesp] which emulates a wemo device with on/off control, requiring little work and no extra servers/accounts to get it to work. (unlike other solutions I have seen)

## Requirements
* Panasonic PT-AE3000U projector (might work on other Panasonic models if they use the same protocol) [Projector manual][controlspec]
* Monoprice [HDX-401TA][hdmiswitch] 4x1 HDMI switch
* ESP8266: [D1 mini][d1mini]
* MAX232-based level shifter [board][rs232]
* [IR led module][irmodule]. 5V powered, 3.3V data, built in transistor for high-power driving of the LED and a visible blue LED to see transmit activity. Nicely made.
* [TSOP38238][tsop] : 38kHz IR receiver, for learning codes. Not used in final project.
* wire, soldering iron, box, the usual

## Software
* platformio core [piocore]
* Arduino esp8266 framework (latest version)
* [fauxmoesp]
* [IRremoteESP8266][irremote]

### Projector commands
STX and ETX, Ctrl+B and Ctrl+C respectively, are used to show message start and end, and the rest is ASCII uppercase chars.
* ON : `char pon[]  = {0x2, 'P', 'O', 'N', 0x3};`
* OFF: `char poff[] = {0x2, 'P', 'O', 'F', 0x3};`

### Monoprice Switcher commands:
They are all NEC 32 bytes codes so pretty straightforward. All the values are in teh code as `#defines`. I originally tried to use my NUC's IR receiver to get the raw codes but that didn't go anywhere so I ordered the TSOP devices.

## Hardware

- MAX232 board pictured in a little project board, next to the upside-down esp8266. The IR led module is at the end of a long 3 conductor wire, to put it where it's needed.

![mvimg_20180105_011141](https://user-images.githubusercontent.com/11471500/34757449-188e5d96-f586-11e7-9d6b-f21c1c85773b.jpg)

- IR emitter aimed at the HDMI switch. Blue tape is for labelling better, also the blue leds are brighter than a thousand suns, so it dims them a fair amount.

![mvimg_20180105_101719](https://user-images.githubusercontent.com/11471500/34757745-2ae277c8-f588-11e7-8af2-44cf7f94f5cf.jpg)


### Extra ground pin
I used two devices so I was a bit short on ground pins and didn't want to make a perf board. I just added an extra ground pin to the D1 mini with a 90 degree bend on it. Works like a charm.

![img_20180105_010757](https://user-images.githubusercontent.com/11471500/34757563-ed5fc636-f586-11e7-976f-eda03e1e835f.jpg)

[piocore]: http://platformio.org/get-started/cli
[controlspec]: http://pdfstream.manualsonline.com/9/9176fcb0-11f1-412d-8ffe-7b7810664a2b.pdf
[irremote]: https://github.com/markszabo/IRremoteESP8266
[fauxmoesp]: https://bitbucket.org/xoseperez/fauxmoesp
[hdmiswitch]: https://www.monoprice.com/product?p_id=5557
[rs232]: https://www.ebay.com/itm/201579248241
[d1mini]: https://www.ebay.com/itm/172646774462
[irmodule]: https://www.ebay.com/itm/Infrared-Transmitter-Module-IR-Infrared-Sensor-LED-Indicate-For-Arduino-SLM/132243573356
[tsop]: https://www.ebay.com/itm/2x-TSOP38238-Remote-control-receiver-infrared-receiver-head-Photoelectric-switch/292101297804
