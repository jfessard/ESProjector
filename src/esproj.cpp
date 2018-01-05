#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <fauxmoESP.h>
#include <SoftwareSerial.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Wifi parameters

char passphrase[] = "***";
char ssid[]       = "***";

char rbuf[64] = {0};


#define PROJECTOR_PWR   0
#define CHROMECAST_IN   1
#define FIRETV_IN       2
#define CONSOLE_IN      3
#define PC_IN           4

/* Monoprice HDX-401TA remote codes (NEC, 32 bits) */
#define SWITCH_IR_1     0xFF10EF //chromecast
#define SWITCH_IR_2     0xFF50AF //firetv
#define SWITCH_IR_3     0xFF30CF //console
#define SWITCH_IR_4     0xFF708F //PC
#define SWITCH_IR_MUTE  0xFF40BF
#define SWITCH_IR_LEFT  0xFF08F7
#define SWITCH_IR_RIGHT 0xFF48B7
#define SWITCH_IR_2CH   0xFF18E7
#define SWITCH_IR_51CH  0xFF58A7
#define SWITCH_IR_POWER 0xFF00FF

// Pin assignments
IRsend  irsend(D3);  // IR LED control
#define PROJ_TX D2 //To projector
#define PROJ_RX D1

#define SERIAL_BAUDRATE                 115200
#define PROJ_BAUDRATE                   9600
#define LED                             2

fauxmoESP fauxmo;

SoftwareSerial SerialP(PROJ_RX, PROJ_TX); // RX, TX

char pon[]  = {0x2, 'P', 'O', 'N', 0x3};
char poff[] = {0x2, 'P', 'O', 'F', 0x3};
char menu[] = {0x2, 'O', 'M', 'N', 0x3};
char back[] = {0x2, 'O', 'B', 'K', 0x3};
char qpw[]  = {0x2, 'Q', 'P', 'W', 0x3}; // query power state

uint32_t codes[] = {SWITCH_IR_1, SWITCH_IR_2, SWITCH_IR_3, SWITCH_IR_4};

void switch_control(unsigned char device) {
  device--;
  Serial.printf("%s: Setting Input %d: IR[0x%x]\n", __func__, device, codes[device]);
  irsend.sendNEC(codes[device], 32);
  delay(500);
  Serial.print("Sending AUDIO 2CH keypress\n");
  irsend.sendNEC(SWITCH_IR_2CH, 32); //Force stereo
}


void wifiSetup() {
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", ssid);
  WiFi.begin(ssid, passphrase);

  // Wait
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println();

  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}

void setup() {

  // Init serial port and clean garbage
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();
  Serial.println();

  SerialP.begin(PROJ_BAUDRATE);
  SerialP.println();
  SerialP.println();

  // Wifi
  wifiSetup();

  // LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // IR
  irsend.begin();

  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);

  // Add virtual devices (order matters)
  fauxmo.addDevice("projector"); //Panasonic PT-AE3000U
  fauxmo.addDevice("chromecast");
  fauxmo.addDevice("firetv");  // TRIGGERS input 1
  fauxmo.addDevice("console");
  fauxmo.addDevice("pc input"); // TRIGGERS input 3

  // fauxmoESP 2.0.0 has changed the callback signature to add the device_id, this WARRANTY
  // it's easier to match devices to action without having to compare strings.
  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state) {

    Serial.printf("\n[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");

    switch(device_id) {
      case PROJECTOR_PWR:
        if (state) {
          for (int i = 0; i < 5; i++) { // clunky but works.
            SerialP.write(pon[i]);
            Serial.write(pon[i]);
            delay(300);
          }
        } else {
          for (int i = 0; i < 5; i++) {
            SerialP.write(poff[i]);
            Serial.write(poff[i]);
            delay(300);
          }
        }
        digitalWrite(LED, !state);
        break;
      case FIRETV_IN:
      case CHROMECAST_IN:
      case CONSOLE_IN:
      case PC_IN:
        if(state)
          switch_control(device_id);
        else
          irsend.sendNEC(SWITCH_IR_2, 32); /* Back to firetv whenever they're turned off*/
        break;
    }
  });

}

void loop() {

  // Since fauxmoESP 2.0 the library uses the "compatibility" mode by
  // default, this means that it uses WiFiUdp class instead of AsyncUDP.
  // The later requires the Arduino Core for ESP8266 staging version
  // whilst the former works fine with current stable 2.3.0 version.
  // But, since it's not "async" anymore we have to manually poll for UDP
  // packets
  fauxmo.handle();

  if (SerialP.available()) {     // If anything comes in SerialP (pins D1 & D2)
    Serial.write(SerialP.read());   // read it and send it out Serial (USB)
  }

  static unsigned long last = millis();
  if (millis() - last > 5000) {
    last = millis();
    //Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf(".");
  }

}