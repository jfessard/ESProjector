#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <fauxmoESP.h>
#include <SoftwareSerial.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Wifi parameters

char passphrase[] = "***";
char ssid[]       = "***";

typedef enum
{
    DEV_PROJ = 0,
    DEV_CHROMECAST,
    DEV_FIRETV,
    DEV_CONSOLE,
    DEV_PC,
    DEV_MAX,
} device_type_t;


/* Monoprice HDX-401TA remote codes (NEC, 32 bits) */
#define SWITCH_IR_1     (0xFF10EF) //chromecast
#define SWITCH_IR_2     (0xFF50AF) //firetv
#define SWITCH_IR_3     (0xFF30CF) //console
#define SWITCH_IR_4     (0xFF708F) //PC
#define SWITCH_IR_MUTE  (0xFF40BF)
#define SWITCH_IR_LEFT  (0xFF08F7)
#define SWITCH_IR_RIGHT (0xFF48B7)
#define SWITCH_IR_2CH   (0xFF18E7)
#define SWITCH_IR_51CH  (0xFF58A7)
#define SWITCH_IR_POWER (0xFF00FF)

// Pin assignments
IRsend  irsend(D3);  // IR LED control
#define PROJ_TX D2 //To projector
#define PROJ_RX D1 //From projector. Unused.

#define SERIAL_BAUDRATE                 115200
#define PROJ_BAUDRATE                   9600
#define LED_PIN                         2

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
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    // IR
    irsend.begin();

    // Add virtual devices (order matters)
    fauxmo.addDevice("projector"); //Panasonic PT-AE3000U
    fauxmo.addDevice("chromecast");
    fauxmo.addDevice("firetv");  // TRIGGERS input 1
    fauxmo.addDevice("console");
    fauxmo.addDevice("pc"); // TRIGGERS input 3

    fauxmo.setPort(80); //gen3 devices
    fauxmo.enable(true);

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {

        Serial.printf("\n[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");

        switch (device_id) {
        case DEV_PROJ:
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
            digitalWrite(LED_PIN, !state);
            break;
        case DEV_FIRETV:
        case DEV_CHROMECAST:
        case DEV_CONSOLE:
        case DEV_PC:
            if (state)
                switch_control(device_id);
            else
                irsend.sendNEC(SWITCH_IR_2, 32); /* Back to firetv whenever they're turned off*/
            break;
        default:
            Serial.printf("Invalid/Unknown device.");
        }
    });

}

void loop() {

    // fauxmoESP uses an async TCP server but a sync UDP server
    // Therefore, we have to manually poll for UDP packets
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
