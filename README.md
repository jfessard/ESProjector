# ESProjector
Voice control your Panasonic projector with an esp8266 (Alexa compatible) and control sources on an 4x1 HDMI switch

## The problem at hand
My projector is on a high closet, which also houses the media stuff and HDMI cables. Since they're behind a door and I don't want to setup an expensive universal remote or leave the door open for IR to work, I wanted a reliable way of turning the projector on and off, and also switching video sources. Most crucially, searching for remotes is annoying in the dark.
I decided I would find a way to voice control those functions for more convenience.

Thankfully I found out about [fauxmoesp] which emulates a wemo device with on/off control, requiring little setup/piping and no extra servers/accounts to get it to work. (unlike other solutions I have seen through IFTTT and such)

## Requirements
(Links are just examples for what I have ordered)
* Panasonic PT-AE3000U projector (might work on other Panasonic models if they use the same protocol) [Projector manual][controlspec]
* Monoprice [HDX-401TA][hdmiswitch] 4x1 HDMI switch
* ESP8266: [D1 mini][d1mini] [wiki]
* MAX232-based level shifter [board][rs232]
* [IR led module][irmodule]: 5V powered, 3.3V data, built in transistor for high-power driving of the LED and a visible blue LED to see transmit activity. Handy.
* [TSOP38238][tsop]: 38kHz IR receiver, for learning codes. Not used in final project.
* wire, soldering iron, box, the usual

## Software
* [platformIO core][piocore]
* Arduino esp8266 framework (latest version)
* [fauxmoesp]
* [IRremoteESP8266][irremote] (I used `IRrecvDumpV2` to get the IR codes)

Please ignore the pretty ugly `for` loops for writing the bytes, it worked. I may revisit the source to clean it up a bit sometime.
You can probably use the Arduino IDE for building, assuming you git clone the libraries manually to your machine. platformio takes care of all of that for you.

### Projector commands
`STX` and `ETX`, Ctrl+B and Ctrl+C respectively, are used to show message start and end, and the rest is ASCII uppercase chars. Pretty straightforward from the manual. I actually used "menu" keypresses to test stuff repeatedly without having to turn it on and off.
* ON : `char pon[]    = {0x2, 'P', 'O', 'N', 0x3};`
* OFF: `char poff[] = {0x2, 'P', 'O', 'F', 0x3};`

I don't really bother checking the return bytes from the projector since I can hear it powering up or down.

### Monoprice Switcher commands
They are all NEC 32 bytes codes, easy once you know the actual value to use. All the values are in the code as `#define` if you need them. I originally tried to use my Intel NUC PC's IR receiver to get the raw codes but that didn't go anywhere so I ordered the TSOP chip.

## Hardware
### MAX323
- MAX232 board pictured in a little project board, next to the upside-down esp8266. The annoying part is that it that the MAX232 breakout was wired opposite to what I thought it should be which caused a lot of head scratching (projector control was working sending bytes straight from a PC but not from the ESP board) Once I swapped RX/TX wires it all worked. Always double check mystery breakfout boards!

### IR
- Setup I used to learn codes and make sure the LED module worked (the remote is the one I learned codes from)

![mvimg_20171222_112703](https://user-images.githubusercontent.com/11471500/34812212-94f7a148-f659-11e7-8f24-75ba53918ac6.jpg)

- The IR led module is at the end of a ~60 cm long wire, to route it where it's needed. I was worried of signal integrity, but it's low-ish frequency (38kHz) and works very reliably inside the closed cabinet.

![mvimg_20180105_011141](https://user-images.githubusercontent.com/11471500/34757449-188e5d96-f586-11e7-9d6b-f21c1c85773b.jpg)

- IR emitter aimed at the HDMI switch. Blue tape is to dim the brighter-than-a-thousand-suns blue leds, and makes for clearer labelling. 

![mvimg_20180105_101719](https://user-images.githubusercontent.com/11471500/34757745-2ae277c8-f588-11e7-8af2-44cf7f94f5cf.jpg)

### Integration

- Full setup. Hardware is next to the NUC (powered by it and serial can be monitored/firware reflashed), switch is above it. All wiring goes behind the shelf, and through a hole in the top to the projector.

![img_20180110_223709](https://user-images.githubusercontent.com/11471500/34811732-288416a6-f657-11e7-8e77-c1b21e1a309d.jpg)

### Extra ground pin
I used two "gadget boards" so I was short one ground pin and didn't want to make a perf board just for that. I just added an extra ground pin to the D1 mini with a 90 degree bend on it. Works like a charm.

![img_20180105_010757](https://user-images.githubusercontent.com/11471500/34757563-ed5fc636-f586-11e7-976f-eda03e1e835f.jpg)

[wiki]: https://wiki.wemos.cc/products:d1:d1_mini
[piocore]: http://platformio.org/get-started/cli
[controlspec]: http://pdfstream.manualsonline.com/9/9176fcb0-11f1-412d-8ffe-7b7810664a2b.pdf
[irremote]: https://github.com/markszabo/IRremoteESP8266
[fauxmoesp]: https://bitbucket.org/xoseperez/fauxmoesp
[hdmiswitch]: https://www.monoprice.com/product?p_id=5557
[rs232]: https://www.ebay.com/itm/253052504776
[d1mini]: https://www.ebay.com/itm/172646774462
[irmodule]: https://www.ebay.com/itm/132243573356
[tsop]: https://www.ebay.com/itm/292101297804
