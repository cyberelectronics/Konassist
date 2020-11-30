/*  Konassist - Boiler Control  
 *  Version: v1.0
  *   
  * by CyberElectronics --> 
  * Thanks guys for all support and libs available for free on the net
  * VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
  * SafeString by Matthew Ford
  * Parser by Ovi and Matthew Ford
  * Google TTS and Notifier libs by Horihiro 
  * WiFi Manager by Tzapu
  * TFT - JPEG and SPIFFS by Bodmer
  * Elmduino by PowerBroker2
  * Button2 by Lennart Hennigs 
  * Blynk by Blynk
  * TFT - DHT Temp/Humi sensor by Adafruit
  * RunningAverage by Rob Tillaart
  * All Others... 
  * 
  * SOFTWARE LICENSE AGREEMENT: This code is released under the MIT License.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
* **************************************************************************
* 
* 
*/

/* Comment this out to disable prints and save space */
//#define BLYNK_PRINT Serial
#include <FS.h> 
#include <BlynkSimpleEsp8266.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

// -------- STA Mode
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ArduinoJson.h> 
#include "RunningAverage.h"
#include "Button2.h";
//#include <DHT.h>

/////////// Timer Interrupts
#define TIMER_INTERRUPT_DEBUG      0
#include "ESP8266TimerInterrupt.h"
volatile bool statusLed = false;
volatile uint32_t lastMillis = 0;
#define TIMER_INTERVAL_MS       1000
// Init ESP8266 timer 0
ESP8266Timer ITimer;
/////////////////////////////////////////////

#define  LED 2   // D4 
#define ONOFF 4  // output D2
//#define FUEL 5   // output D1
#define BUTTON_A_PIN  12  // D6
#define BUTTON_B_PIN  14  // D5

#define ONE_WIRE_PIN 13
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

WidgetLED led_alive(V3);
WidgetLED led_onoff(V4);

WidgetBridge bridge1(V98);

#define temperature_set_ADR 10

///////// end EEPROM addresses

// Config ============================================
#define offset 3          // offset temperature 
#define max_temp_value 70 // maximum temperature value 
#define push_delay 30     //  push_delay/10 = 3sec
//int bat_calib = 2472;   // voltage divider 15K/2.2K, max.input 24V /// 15K/1K, max.input 16V

// END Config =========================================

Button2 buttonA = Button2(BUTTON_A_PIN);
Button2 buttonB = Button2(BUTTON_B_PIN);

//---------------------------- How many samples
//RunningAverage bat_avg(20);  // Average Battery voltage
RunningAverage temp_avg(10);  // Average Temp sent to Blynk 
//RunningAverage humi_avg(4);  // Average Humi sent to Blynk 
//----------------------------

float bat_average; 
int sec_counter;
float temp_average;


byte button;
int temperature_set; // set STOP temperature
int saved_temperature_set;

boolean start_stop, once_flag, alive, onoffled;
boolean clicked, timetocheck, save_flag;

//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40];
char mqtt_port[6] = "8080";
char blynk_token[40] = "";
char blynk_home_token[40] = "";


//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


// constants won't change:
const long interval = 100;           // interval at which to blink (milliseconds)
unsigned long currentMillis;
unsigned long previousMillis = 0;        // will store last time update

//=======================================================================
void ICACHE_RAM_ATTR TimerHandler()
{

  timetocheck = 1;

  
#if (TIMER_INTERRUPT_DEBUG > 0)
  Serial.println("Delta ms = " + String(millis() - lastMillis));
  lastMillis = millis();
#endif
}
//=======================================================================

//DHT dht(DHTPIN, DHTTYPE);

BlynkTimer timer; // Create a Timer object called "timer"! 

// This function will run every time Blynk connection is established
BLYNK_CONNECTED() {
  //get data stored in virtual pin V0 from server
  bridge1.setAuthToken(blynk_home_token); // AuthToken of the second hardware
  Blynk.syncAll();
 // sync_started = 1;
}

// Button Pushed from Blynk APP ================================================

BLYNK_WRITE(V1){
                  start_stop = param.asInt();
               }

BLYNK_WRITE(V2){  // change pump ON timer
                   temperature_set = param.asInt();
                   if(temperature_set > max_temp_value) temperature_set = max_temp_value;
                   
                }

BLYNK_WRITE(V8){  // Save button from Blynk
                   save_flag = param.asInt();
                   
                }

                     

// SEND to Blynk =============================================================================
void sendUptime()
{
  // This function sends Arduino up time every 1 second to Virtual Pin (V5)
  // In the app, Widget's reading frequency should be set to PUSH
  // You can send anything with any interval using this construction
  // Don't send more that 10 values per second

                   if(save_flag){
                                  save_flag = 0;
                                                                    
                                  EEPROM.begin(512);
                                  EEPROM.put( temperature_set_ADR, temperature_set );
                                  EEPROM.commit();
                                  EEPROM.end();
                                  
                                  Serial.println("Saved to EEPROM !!!");
                                  Blynk.virtualWrite(V8, 0);
                                  
                    }
                   
                   if(onoffled) led_onoff.on();
                   else led_onoff.off();  
                   
                   Blynk.virtualWrite(V0, String(temp_average, 1));
                   bridge1.virtualWrite(V97, String(temp_average*10));
               
}
   
// SETUP =======================================================================
void setup() {
  // put your setup code here, to run once:

                   pinMode(A0, INPUT);           // Vinput and Batt voltage
                   pinMode(LED, OUTPUT);      // GPIO2 Status Led shared output
                   pinMode(ONOFF, OUTPUT);    // Siroco ON/OFF button control
                   pinMode(BUTTON_A_PIN, INPUT_PULLUP);
                   pinMode(BUTTON_B_PIN, INPUT_PULLUP);
               
      EEPROM.begin(512);
      EEPROM.get( temperature_set_ADR, temperature_set);

      if (isnan(saved_temperature_set)) {
                                                                               
                                            temperature_set = 60;
                                                                              
                                            EEPROM.put( temperature_set_ADR, temperature_set );
                                            EEPROM.commit();
                                        }
       EEPROM.end();
        
  // Debug console

Serial.begin(115200);

//Serial.println();
//delay(3000);

  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
                                        Serial.println("mounted file system");
                                        if (SPIFFS.exists("/config.json")) {
                                                                              //file exists, reading and loading
                                                                              Serial.println("reading config file");
                                                                              File configFile = SPIFFS.open("/config.json", "r");
                                                                              if (configFile) {
                                                                                                    Serial.println("opened config file");
                                                                                                    size_t size = configFile.size();
                                                                                                    // Allocate a buffer to store contents of the file.
                                                                                                    std::unique_ptr<char[]> buf(new char[size]);
                                                                                                    configFile.readBytes(buf.get(), size);
                                                                                                    DynamicJsonDocument jsonBuffer(1024);
                                                                                                    
                                                                                                    deserializeJson(jsonBuffer, buf.get());
                                                                                                    auto error = serializeJson(jsonBuffer, Serial);
                                                                                                    
                                                                                                    if (error) {
                                                                                                                  Serial.println("\nparsed json");
                                                                                                        
                                                                                                               ///   strcpy(mqtt_server, jsonBuffer["mqtt_server"]);
                                                                                                               ///   strcpy(mqtt_port, jsonBuffer["mqtt_port"]);
                                                                                                                 strcpy(blynk_token, jsonBuffer["blynk_token"]);
                                                                                                                 //strcpy(displayName, jsonBuffer["displayName"]);
                                                                                                                 strcpy(blynk_home_token, jsonBuffer["blynk_home_token"]);
                                                                                                                } 
                                                                                                    else {
                                                                                                           Serial.println("failed to load json config");
                                                                                                         }
                                                                                                    configFile.close();
                                                                                               }
                                                                            }
                                      }
                  else {
                          Serial.println("failed to mount FS");
                        }
                //end read

   //listFiles(); // Lists the files so you can see what is in the SPIFFS

  //end read

  // The extra parameters to be configured (can be either global or just in the setup)
                // After connecting, parameter.getValue() will get you the configured value
                // id/name placeholder/prompt default length
              ///  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
              ///  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
                   WiFiManagerParameter custom_blynk_token("blynk", "Boiler Blynk Token", blynk_token, 33);
                 //  WiFiManagerParameter custom_displayName("assistant", "Assistant Name", displayName, 25);
              WiFiManagerParameter custom_blynk_home_token("blynk1", "Home Blynk Token", blynk_home_token, 33);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  
  //add all your parameters here
///  wifiManager.addParameter(&custom_mqtt_server);
///  wifiManager.addParameter(&custom_mqtt_port);
   wifiManager.addParameter(&custom_blynk_token);
   wifiManager.addParameter(&custom_blynk_home_token);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);
/*
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
*/
 if(digitalRead(BUTTON_A_PIN) == LOW) {
                                                        wifiManager.resetSettings();
                                                        wifiManager.setEnableConfigPortal(true);
                                                        wifiManager.autoConnect("Setup_Boiler","boilerpass"); // start config Portal 
                                      }
              
              else{
                      wifiManager.setEnableConfigPortal(false);
                      if(!wifiManager.autoConnect()) {
                        WiFi.disconnect();
                        WiFi.mode(WIFI_OFF);
                       // wifiManager.setEnableConfigPortal(true);
                        wifiManager.autoConnect("Setup_Boiler","boilerpass");
                      }
              }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
 /// strcpy(mqtt_server, custom_mqtt_server.getValue());
 /// strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(blynk_token, custom_blynk_token.getValue());
  strcpy(blynk_home_token, custom_blynk_home_token.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
                                      Serial.println("saving config");
                                      DynamicJsonDocument jsonBuffer(1024);
                                      
                                     /// jsonBuffer["mqtt_server"] = mqtt_server;
                                     /// jsonBuffer["mqtt_port"] = mqtt_port;
                                      jsonBuffer["blynk_token"] = blynk_token;
                                      //jsonBuffer["displayName"] = displayName;
                                      jsonBuffer["blynk_home_token"] = blynk_home_token;
                                  
                                      File configFile = SPIFFS.open("/config.json", "w");
                                      if (!configFile) {
                                                          Serial.println("failed to open config file for writing");
                                                        }
                                  
                                      serializeJson(jsonBuffer, Serial);
                                      serializeJson(jsonBuffer, configFile);
                                      
                                      configFile.close();
                                      //end save
                                    }
            
    

    

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  /// Serial.print("Blynk Token : ");
  /// Serial.println(blynk_token);


Blynk.config(blynk_token);
// Blynk.begin(blynk_token, ssid, password);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

timer.setInterval(5000L, sendUptime); //  Here you set interval (1sec) and which function to call

buttonA.setClickHandler(click);
buttonB.setClickHandler(click);
buttonA.setLongClickHandler(longpress);

Serial.println("\nStarting, F_CPU = " + String(F_CPU));

  // Interval in microsecs
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    lastMillis = millis();
    Serial.println("Starting  ITimer OK, millis() = " + String(lastMillis));
  }
  else
    Serial.println("Can't set ITimer correctly. Select another freq. or interval");

}



// Begin Loop ===================================================================
void loop() {
              buttonA.loop();
              buttonB.loop();
              // put your main code here, to run repeatedly:

              if(timetocheck){
                                 timetocheck = 0;
                                 Read_Sensors();
                                 if(start_stop){
                                                 if(temp_average < (temperature_set - offset)){                                                                                     
                                                                                                 digitalWrite(ONOFF, HIGH);
                                                                                                 digitalWrite(LED, LOW);     // HW LED
                                                                                                 onoffled = 1;
                                                                                                } 
                                                 else if(temp_average > temperature_set){
                                                                                          digitalWrite(ONOFF, LOW);
                                                                                          digitalWrite(LED, HIGH);     // HW LED
                                                                                          onoffled = 0; 
                                                  }
                                                                                                               
                                                }
                                 else {
                                         digitalWrite(ONOFF, LOW);
                                         digitalWrite(LED, HIGH);     // HW LED
                                         onoffled = 0;
                                  }               
                }
              
                Blynk.run();
                timer.run(); // BlynkTimer is working...

}
// END Loop =====================================================


void Read_Sensors(){
  
                     sec_counter++;

                     if(sec_counter >= 2) {
                                             sec_counter = 0;
                                             
                                             sensors.requestTemperatures();
                                             //bat_avg.addValue(((analogRead(A0)*bat_calib)/1024.0)/100.0);  // Read Battery voltage, 16V at 1024 
                                             temp_avg.addValue(sensors.getTempCByIndex(0));
                                             
                                             //bat_average = bat_avg.getAverage();
                                             temp_average = temp_avg.getAverage();
                                              
                                             
                                             //Serial.print("Battery voltage: ");
                                             //Serial.println(bat_average);
                                             //Serial.println(analogRead(A0));
                                             Serial.print("Temperature: ");
                                             Serial.println(temp_average);
                                             Serial.print("Temperature set: ");
                                             Serial.println(temperature_set);
                                             Serial.print("Temperature offset: ");
                                             Serial.println(offset);
                                                                     
                                             }
                     //humi_avg.addValue(dht.readHumidity());
                     //temp_avg.addValue(dht.readTemperature());
                    
                   //  if (isnan(h) || isnan(t)) {
                   //                               Serial.println("Failed to read from DHT sensor!");
                   //                               return;
                   //                             }
   }

//--------------------------------- END Sensors

//--------------------------------- Buttons clicked
void click(Button2& btn) {
                            clicked = 1;
                            
                            if (btn == buttonA) {
                                                  Serial.print("A clicked");
                                                  Serial.println();
                                                } 
                            else if (btn == buttonB) {
                                                        
                                                        
                                                        Serial.println("B clicked");
                                                      }
                         }

//--------------------------------- END Buttons Clicked

void longpress(Button2& btn) {  // Start/Stop
                                unsigned int time = btn.wasPressedFor();
                                if (time > 2000) {
                                                   start_stop = 1;
                                                   
                                                 }
}
