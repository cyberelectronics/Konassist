/*  Konassist - Home device 
 *  Version: v1.0
  *   
  * by CyberElectronics --> 
  * Thanks guys for all the libs available for free on the net
  * VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
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

#include <SPIFFS.h>
#include <BlynkSimpleEsp32.h>
//#include <Wire.h>
#include <EEPROM.h>

// -------- STA Mode
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ArduinoJson.h> 

// JPEG decoder library
#include <JPEGDecoder.h>

#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

#include <esp8266-google-home-notifier.h>  // Google Home notifier
GoogleHomeNotifier ghn;

//#include "BluetoothSerial.h"
//#include "ELMduino.h"

// Temperature and Humidity sensor
#include "RunningAverage.h"
#include <DHT.h>

#define DHTPIN 27          // What digital pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
#define DHTTYPE DHT21   // DHT 21, AM2301

//---------------------------- How many samples take for average calc.
//RunningAverage bat_avg(4);  // Average Battery voltage
RunningAverage temp_avg(30);  // Average Temp sent to Blynk 
RunningAverage humi_avg(30);  // Average Humi sent to Blynk 
//----------------------------

DHT dht(DHTPIN, DHTTYPE);

#include "Button2.h";

//BluetoothSerial SerialBT;
//#define ELM_PORT   SerialBT
#define DEBUG_PORT Serial
//ELM327 myELM327;

#define TFT_DISPOFF 0x28
#define TFT_BL              4   // Display backlight control pin
#define ADC_EN              14  //ADC_EN is the ADC detection enable port
#define ADC_PIN             34
#define BUTTON_A_PIN  35
#define BUTTON_B_PIN  0

//TFT y positions for texts and numbers
#define toptext 20            // y coordinates for text 
#define bottomtext 145        //
#define topdraw 75            // and numbers
#define bottomdraw 200        // TTGO 135x250 TFT display

Button2 buttonA = Button2(BUTTON_A_PIN);
Button2 buttonB = Button2(BUTTON_B_PIN);

//WidgetLED led_alive(V6);
// Bridge widget on virtual pin 1
WidgetBridge bridge1(V100);

//#define APSTA_ADR 170
///////// end EEPROM addresses

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Config ==================================================

#define car_off_delay 5             // Delay in minutes to send a notification (TTS) after the car was stoppped
#define boiler_LOW_Warning 35.0     // HotWater Low temperature warning (RED color below this value)
#define boiler_HIGH_Warning 70.0    // HotWater High temperature warning (RED color above this value)
#define HOMEmintemp_Warning 20.0    // RED color below this temperature value (Celsius)
#define HOMEmaxtemp_Warning 30.0    // RED color above this temperature value (Celsius)
#define HOMEminhumi_Warning 30.0    // RED color below this humidity value (%)
#define HOMEmaxhumi_Warning 60.0    // RED color above this humidity value (%)
#define BATTtemp_Warning 40.0       // RED color above this value (Celsius)
#define SOCpercent_Warning 10       // RED color below this value (Percent)
#define AUXSOCpercent_Warning 30    // RED color below this value (Percent)
#define BATTv_LOW_Warning 320       // Main Battery Low Voltage warning (Volts)
#define BATTv_HIGH_Warning 420      // Main Battery High Voltage warning (Volts)
#define AUXBATTv_LOW_Warning 12.2   // Main Battery Low Voltage warning (Volts)
#define AUXBATTv_HIGH_Warning 15.0  // Main Battery High Voltage warning (Volts)
#define pagenumbers 7               // number of pages -1

char displayName[25] = "Bedroom speaker";  // Google Home / Chromecast / Nvidia Shield / Smartphone using CastReceiver app - Device Name
char language[] = "en"; // Select "text to speech" Response Language: hu, ro, en, etc...  

int ledBacklight = 80; // Initial TFT backlight intensity on a scale of 0 to 255. Initial value is 80.
bool debugmode = true;


// END Config ================================================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// Setting PWM properties, do not change this!
const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

int sec_interval = 10;  // sec_interval/10 = sec.
int sensor_read_period;
int sensor_calc_period;

uint16_t x,y;
byte button;
byte ledBacklight_percent;
byte language_number;
byte notify_on_once, notify_off_once, off_period, check_response, group;
boolean clicked;
boolean car_status_flag, car_battery_flag, cell_voltage_flag, home_temphumi_flag, home_tripinfo_flag, boiler_info_flag; // Start response
boolean online_flag, still_online;  // check if the Car is still Online 
boolean notify_startstop;
boolean notify_warning;
boolean just_booted;
boolean notify_once, AUXSOCpercent_Warning_notify_once, BATTtemp_Warning_notify_once, BATTtemp_notify_one_more_time, SOCpercent_Warning_notify_once, SOCpercent_notify_one_more_time;
byte charging_flag;


float CECvalue; // Cummulative Energy Charged
float CEDvalue; // Cummulative Energy Discharged
float AUXBATTv; // Auxiliary Battery Voltage
float AUXBATTcur; // Auxiliary Battery Current
byte  AUXBATTsoc; // Auxiliary Battery State Of Charge
float BATTpow;  // MAin Battery Power
float BATTtemp; // Battery temperature
float BATTINtemp; // Battery Inlet Temperature
float MAXcellv; // Maximum Cell Voltage
float MINcellv; // Minimum Cell Voltage
//byte  MAXcellvno; // Maximum Cell Voltage Number
//byte  MINcellvno; // Minimum Cell Voltage Number
byte CHARGE;      // Charging signal
//byte BATTFANspeed; // FAN Speed
byte COOLtemp;    // Cooling water temperature
//byte HEATtemp1, HEATtemp2; // Heater temperatures
float TRIP1ch;    // Trip 1 charged kWh
float TRIP1dch;   // Trip 1 discharged kWh
float tempBATTpow; // temporary
float INtemp, OUTtemp; // Indoor / Outdoor temperatures
//float RLTpress, RRTpress, FLTpress, FRTpress;  // Tyre Pressure - in bar
float ACCcur;     // AC charge current
float SPEED;      // Vehicle speed in km/h
//float OUTDOORtemp, INDOORtemp; // Outdoor/Indoor temperature
float VOLTdiff;    // Voltage difference
float boiler_temp;  // boiler temperature from Boiler Device
float TRIP1used;   // TRIP1 total energy used kWh
float TRIP1avg;     // TRIP 1 energy kWh/100km
float temp_humi, temp_temp;


int SOCpercent;   // State Of Charge in percent
int SOCreal;       // State Of Charge in percent
int SOHpercent;   // State Of Health in percent
int BATTv;        // Main Battery Voltage
int BATTcur;      // Main Battery Current
byte BATTtemp1, BATTtemp2, BATTtemp3, BATTtemp4;  // Main Battery temperature per module
byte BATTMAXtemp, BATTMINtemp;                    // Main Battery MAX and MIN temperature 
int RLTtemp, RRTtemp, FLTtemp, FRTtemp;          // Tyre temperature 
int TRIP1odo;    // Trip1 distance

int AUXSOCpercent;
uint32_t OPtimehours;         // Operating time in hours
uint32_t ODOkm;               // ODO -  distance already in km  
//int SOHpercent;               // State Of Health in percent
int cell_voltage_difference;  // Difference between MIN and MAX cell voltages

float temp_average, humi_average;

String  txtnotify;

int ms_counter; // incremented every 100ms
int online_counter; // check every minute if Car is still Online
int online_counter1; // first boot timer/delay

boolean alive;
boolean display_onoff; // button from Blynk to turn ON/OFF the display
boolean cast_error;    // error flag - Cast device was not found
boolean first_sync;    // first sync flag, stops TTS (text to speech) notifications
boolean sync_started;  // sync with Blynk at boot was started
boolean language_flag; // language change was requested
boolean welcome_flag;  // welcome message at boot


//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40];
char mqtt_port[6] = "8080";
char blynk_token[40] = "";
char blynk_car_token[40] = "";  // Config from WebUI; Here leave it empty - Car device blynk token

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
                              Serial.println("Should save config");
                              shouldSaveConfig = true;
                            }


// constants won't change:
const long interval = 100;           // interval to check datas (milliseconds)
unsigned long currentMillis;
unsigned long previousMillis = 0;        // will store last time update

BlynkTimer timer; // Create a Timer object called "timer"! 

// This function will run every time Blynk connection is established
BLYNK_CONNECTED() {
                    bridge1.setAuthToken(blynk_car_token); // AuthToken of the second hardware
                    //get data stored from server
                    
                    Blynk.syncAll();
                    sync_started = 1;
                  }

// Button Pushed from Blynk APP ================================================

BLYNK_WRITE(V20){  // turn ON/OFF the display
                   display_onoff = param.asInt();
                   if(display_onoff) ledcWrite(pwmLedChannelTFT, ledBacklight);
                   else ledcWrite(pwmLedChannelTFT, 0);
                }

BLYNK_WRITE(V21){  // change backlight luminosity
                   ledBacklight_percent = param.asInt();
                   ledBacklight = map(ledBacklight_percent,0,100,0,255); // input value from Blynk 0>100, output value to PWM 0>255
                   ledcWrite(pwmLedChannelTFT, ledBacklight);
                }

BLYNK_WRITE(V22){  // Select Screen number from Blynk
                   button = param.asInt()-1; 
                   clicked = 1;
                } 

BLYNK_WRITE(V101){  // "Set language" 
                   language_number = param.asInt();
                   if(language_number == 1) strcpy(language,"hu");
                   else if(language_number == 2) strcpy(language,"ro");
                   else if(language_number == 3) strcpy(language,"en");
                   ghn.device(displayName, language);
                   if (first_sync) language_flag = 1;
                } 

BLYNK_WRITE(V102){  // "Car status" response
                   check_response = param.asInt();
                   if (first_sync) {
                                      if(check_response == 1) car_status_flag = 1;
                                      else if(check_response == 2) car_battery_flag = 1; 
                                      else if(check_response == 3) cell_voltage_flag = 1;
                                      else if(check_response == 4) home_temphumi_flag = 1;
                                      else if(check_response == 5) home_tripinfo_flag = 1;
                                      else if(check_response == 6) boiler_info_flag = 1;
                                      
                                    }
                } 

BLYNK_WRITE(V103){  // "Online?" from car
                   online_flag = param.asInt();
                                     
                } 

BLYNK_WRITE(V104){  // Enable/disable "Start/Stop" notifications 
                   notify_startstop = param.asInt();
                                   
                }  

BLYNK_WRITE(V105){  // Enable/Disable "Warning" messages" 
                   notify_warning = param.asInt();
                                   
                } 

BLYNK_WRITE(V106){  // Charging/Not Charging flag 
                   charging_flag = param.asInt();
                   if(first_sync) notify_once = 1;                
                } 

 BLYNK_WRITE(V107){  // State Of Charge data from Car 
                   SOCpercent = param.asInt();
                                   
                }                
                 
BLYNK_WRITE(V108){  // Battery temperature, data from Car 
                   BATTtemp = param.asInt()*0.1;
                                   
                }                                                        

BLYNK_WRITE(V109){  // Indoor temperature, data from Car 
                   INtemp = param.asInt()*0.1;
                                   
                } 

BLYNK_WRITE(V110){  // Outdoor temperature, data from Car 
                   OUTtemp = param.asInt()*0.1;
                                   
                }       
                   
BLYNK_WRITE(V111){  // Battery use/reg. power (kW), data from Car 
                   BATTpow = param.asInt()*0.1;
                                   
                } 

BLYNK_WRITE(V112){  // Main Battery voltage data from Car 
                   BATTv = param.asInt();
                                   
                } 

BLYNK_WRITE(V113){  // AUX Battery State Of Charge data from Car 
                   AUXBATTsoc = param.asInt();
                                   
                } 

BLYNK_WRITE(V114){  // AUX Battery Voltage data from Car 
                   AUXBATTv = param.asInt()*0.1;
                                   
                } 

BLYNK_WRITE(V115){  // Main Battery State of Health data from Car 
                   SOHpercent = param.asInt();
                                   
                } 

BLYNK_WRITE(V116){  // AC charge current data from Car 
                   ACCcur = param.asInt()*0.1;
                                   
                } 

BLYNK_WRITE(V117){  // Cummulative Energy Charged data from Car 
                   CECvalue = param.asInt();
                                   
                } 

BLYNK_WRITE(V118){  // State Of Charge data from Car 
                   CEDvalue = param.asInt();
                                   
                } 

BLYNK_WRITE(V119){  // State Of Charge data from Car 
                   TRIP1ch = param.asInt()*0.1;
                                   
                } 

BLYNK_WRITE(V120){  // State Of Charge data from Car 
                   TRIP1dch = param.asInt()*0.1;
                                   
                } 

BLYNK_WRITE(V121){  // State Of Charge data from Car 
                   BATTMINtemp = param.asInt();
                                   
                } 

BLYNK_WRITE(V122){  // State Of Charge data from Car 
                   BATTMAXtemp = param.asInt();
                                   
                } 

BLYNK_WRITE(V123){  // State Of Charge data from Car 
                   MAXcellv = param.asInt()*0.1;
                                   
                } 

BLYNK_WRITE(V124){  // State Of Charge data from Car 
                   MINcellv = param.asInt()*0.1;
                                   
                }                                                                                       

BLYNK_WRITE(V125){  // State Of Charge data from Car 
                   cell_voltage_difference = param.asInt();
                                   
                } 

BLYNK_WRITE(V83){  // Trip1 distance from Car  
                   TRIP1odo = param.asInt();
                               
                }
/*
BLYNK_WRITE(V95){  // State Of Charge data from Car 
                   TRIP1used = param.asInt()*0.1;
                                   
                } 

BLYNK_WRITE(V96){  // State Of Charge data from Car 
                   TRIP1avg = param.asInt()*0.1;
                                   
                } 
*/
BLYNK_WRITE(V97){  // Trip1 distance from Car  
                   boiler_temp = param.asInt()*0.1;
                               
                }               
                                 

// SEND to Blynk =============================================================================
void sendUptime()
                  { 
                     // send datas in "group" - otherwise Blynk can disconnect your HW (temporary) for flooding         
                                   
                     group++;

                     //if(first_sync && !welcome_flag) welcome_message();
                     
                     //if(debugmode) send_debug_serial();  // send datas over seial also
                     
                     if(group >= 1){
                                     //first group of datas to send
                                                                                                                  
                                     Blynk.virtualWrite(V0, String(temp_average, 1));
                                     Blynk.virtualWrite(V1, String(humi_average, 1));
                                     bridge1.virtualWrite(V107, String(temp_average*10));
                                     bridge1.virtualWrite(V108, String(humi_average*10));

                                     if(check_response) Blynk.virtualWrite(V102, 0);  // clear the "check_response" flag in the cloud
                                        
                                     //alive = !alive;               // OK flag
                                     //digitalWrite(LED, alive);     // HW LED
                  
                                     //if(alive) led_alive.on();     // LED in Blynk app. Uncomment WidgetLED to use this function
                                     //else led_alive.off(); 
                                     
                                     group=0; // reset the group counter 
                                 }
                     
                     /*else if(group >= 2){ 
                                          // second group of datas to send
                                          group=0; // reset the group counter 
                                         }                 
                       */                      
                    
                  }
   
// SETUP =======================================================================
void setup() {
               // put your setup code here, to run once:

                   //pinMode(A0, INPUT);           // Vinput and Batt voltage
                   pinMode(DHTPIN, INPUT);
                   dht.begin();    
                   pinMode(BUTTON_A_PIN, INPUT_PULLUP);
                   pinMode(ADC_EN, OUTPUT);
                   digitalWrite(ADC_EN, HIGH);
                   pinMode(TFT_BL, OUTPUT); 

                    ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
                    ledcAttachPin(TFT_BL, pwmLedChannelTFT);
                    ledcWrite(pwmLedChannelTFT, ledBacklight);
                   
                  Serial.begin(115200);
                  Serial.println();
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
                                                                                                                 strcpy(blynk_car_token, jsonBuffer["blynk_car_token"]);
                                                                                                                 strcpy(displayName, jsonBuffer["displayName"]);
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

                listFiles(); // Lists the files so you can see what is in the SPIFFS

                ////////////////// Init and Draw JPEG Welcome Screen BEGIN
                
                  tft.begin();
                  tft.setRotation(0);  // 0 & 2 Portrait. 1 & 3 landscape
                  tft.fillScreen(TFT_BLACK);
                  //tft.setRotation(0);  // portrait
                  drawJpeg("/tiger.jpg", 0 , 0);     // 240 x 320 image
                  
                  
                ////////////////// Draw JPEG Welcome Screen END

                // The extra parameters to be configured (can be either global or just in the setup)
                // After connecting, parameter.getValue() will get you the configured value
                // id/name placeholder/prompt default length
              ///  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
              ///  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
                   WiFiManagerParameter custom_blynk_token("blynk", "Home Blynk Token", blynk_token, 33);
                   WiFiManagerParameter custom_blynk_car_token("blynk1", "Car Blynk Token", blynk_car_token, 33);
                   WiFiManagerParameter custom_displayName("assistant", "Assistant Name", displayName, 25);
                   
                WiFiManager wifiManager;
                //set config save notify callback
                wifiManager.setSaveConfigCallback(saveConfigCallback);
              
                //set static ip
                //wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
                
                //add all your parameters here
              /// wifiManager.addParameter(&custom_mqtt_server);
              /// wifiManager.addParameter(&custom_mqtt_port);
                  wifiManager.addParameter(&custom_blynk_token);
                  wifiManager.addParameter(&custom_blynk_car_token); 
                  wifiManager.addParameter(&custom_displayName);

              //reset settings - for testing
              //wifiManager.resetSettings();
            
              //set minimu quality of signal so it ignores AP's under that quality
              //defaults to 8%
              //wifiManager.setMinimumSignalQuality();
              
              //
              //sets timeout until configuration portal gets turned off
              //useful to make it all retry or go to sleep
              //in seconds
              //wifiManager.setConfigPortalTimeout(120);
              //wifiManager.setConnectTimeout(20); // wait for wifi signal
              //wifiManager.setTimeout(240);       // wait to edit/save config portal
  

              // Workarround for connection issue (WL_STATION_WRONG_PASSWORD)
              // see https://github.com/tzapu/WiFiManager/issues/979
              
              if(digitalRead(BUTTON_A_PIN) == LOW) {
                                                        wifiManager.resetSettings();
                                                        wifiManager.setEnableConfigPortal(true);
                                                        wifiManager.autoConnect("Setup_Konassist_Home","konapass"); // start config Portal / SSID: Setup_Konassist / Password: konapass
                                                    }
              
              else{
                      wifiManager.setEnableConfigPortal(false);
                      if(!wifiManager.autoConnect()) {
                        WiFi.disconnect();
                        WiFi.mode(WIFI_OFF);
                       // wifiManager.setEnableConfigPortal(true);
                        wifiManager.autoConnect("Setup_Konassist_Home","konapass");
                      }
              }

              //if you get here you have connected to the WiFi
              Serial.println("connected...yeey :)");
            
              //read updated parameters
             /// strcpy(mqtt_server, custom_mqtt_server.getValue());
             /// strcpy(mqtt_port, custom_mqtt_port.getValue());
              strcpy(blynk_token, custom_blynk_token.getValue());
              strcpy(blynk_car_token, custom_blynk_car_token.getValue());
              strcpy(displayName, custom_displayName.getValue());
              
              //save the custom parameters to FS
              if (shouldSaveConfig) {
                                      Serial.println("saving config");
                                      DynamicJsonDocument jsonBuffer(1024);
                                      
                                     /// jsonBuffer["mqtt_server"] = mqtt_server;
                                     /// jsonBuffer["mqtt_port"] = mqtt_port;
                                      jsonBuffer["blynk_token"] = blynk_token;
                                      jsonBuffer["blynk_car_token"] = blynk_car_token;
                                      jsonBuffer["displayName"] = displayName;
                                  
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
              //Serial.print("Blynk Token : ");
              //Serial.println(blynk_token);
            
            
            Blynk.config(blynk_token);
      
            
            timer.setInterval(10000L, sendUptime); //  Here you set interval (1sec) and which function to call
            
            buttonA.setClickHandler(click);
            buttonB.setClickHandler(click);
//            buttonA.setLongClickHandler(longpress);
            
            

            // connecting to a Cast device
            Serial.println("connecting to a Cast device...");
            
            if (ghn.device(displayName, language) != true) 
                                                      {
                                                        Serial.println(ghn.getLastError());
                                                        cast_error = 1;    // cast device connection error flag
                                                        connectedscreen(); // display connection status at boot
                                                        tft.setTextDatum(MC_DATUM);
                                                        x = tft.width()/2;
                                                        return;
                                                      }
            Serial.print("Cast device found(");
            Serial.print(ghn.getIPAddress());
            Serial.print(":");
            Serial.print(ghn.getPort());
            Serial.println(")");
                        
            Serial.println("Konassist connected");

            connectedscreen();
            tft.setTextDatum(MC_DATUM);
            x = tft.width()/2;

            temp_humi = dht.readHumidity();
            temp_temp = dht.readTemperature();
            humi_avg.addValue(temp_humi);     // read humidity
            temp_avg.addValue(temp_temp);     // read temperature
            temp_average = temp_avg.getAverage(); 
            humi_average = humi_avg.getAverage();
            
}

//========================================== END Setup



// ===================================================================
// =============== BEGIN LOOP ========================================

void loop() {
              // put your main code here, to run repeatedly:
              
              buttonA.loop();
              buttonB.loop();

              if(car_status_flag) car_status();
              else if(car_battery_flag) car_battery();
              else if(cell_voltage_flag) cell_voltage(); 
              else if(home_temphumi_flag) home_temphumi();
              else if(home_tripinfo_flag) trip_info();
              else if(language_flag) language_change();
              else if(boiler_info_flag) boiler_info();
              
              if (millis() < currentMillis) {
                                              currentMillis = 0;
                                            }
              
              currentMillis = millis();

              if (currentMillis - previousMillis >= interval) {   // 100ms timer
                                                                  // save the last time
                                                                  previousMillis = currentMillis;

                                                                  // Read Datas from OBD adapter
                                                                  ms_counter++;
                                                                  if(ms_counter >= sec_interval) {  // 1 second
                                                                                                    
                                                                                                    ms_counter = 0;
                                                                                                    online_counter++;
                                                                                                    sensor_read_period++;

                                                                                                    if(sync_started && (online_counter1 > 3)){ 
                                                                                                                                              //online_counter = 0;
                                                                                                                                              first_sync = 1; 
                                                                                                                                              
                                                                                                    }
                                                                                                    else if (sync_started) online_counter1++;  

                                                                                                    if(first_sync){
                                                                                                                    TRIP1used = TRIP1dch - TRIP1ch;          // Trip 1 Energy used 
                                                                                                                    TRIP1avg = TRIP1used*100.0 / TRIP1odo;  //  Trip 1 >>> kWh/100km     
                                                                                                                  }  
                                                                                                                  
                                                                                                    check_car_onoff_state();
                                                                                                    notify_charging();
                                                                                                    if(notify_warning) check_warnings();
                                                                                                    read_data();
                                                                                                
                                                                                                    // check buttons and select page
                                                                                                    if(clicked){  // clear the display, because of different textPadding on pages
                                                                                                                   clicked = 0;
                                                                                                                   tft.fillScreen(TFT_BLACK); 
                                                                                                                    
                                                                                                               }
                                                                                                    
                                                                                                    switch (button){  // select page to display
                                                                                      
                                                                                                                       case 0: page1(); break;
                                                                                                                       case 1: page2(); break;
                                                                                                                       case 2: page3(); break;
                                                                                                                       case 3: page4(); break;
                                                                                                                       case 4: page5(); break;
                                                                                                                       case 5: page6(); break;
                                                                                                                       case 6: page7(); break;
                                                                                                                       case 7: page8(); break;
                                                                                                                     }
                                                                                                 }
                                                                  
                                                               }
  
             Blynk.run();
             timer.run(); // BlynkTimer is working...

}

// =====================================================================
// =================== END LOOP ========================================

void send_debug_serial(){
                                     Serial.print("Temperature (C): ");        // temp
                                     Serial.print(temp_average); 
                                     Serial.print("  -----  Humidity (%): ");  // Humi
                                     Serial.println(humi_average); 
  }

void notify_charging(){
                         if (notify_once && notify_startstop) {
                                                                 notify_once = 0; // reset flag for next change
                                                                 car_charging();   // TTS notify for charging
                                                              }
                         else notify_once = 0;
  }

void check_warnings(){
                        if(first_sync){    // check if first sync with Blynk was completed
                                            
                                            AUXBATTsoc_warning();
                                            BATTtemperature_warning();                                         
                                            SOCpercent_warning(); 
                                      }
  }

void AUXBATTsoc_warning(){  // Auxiliary battery charge level, under set limit
                              if(AUXBATTsoc < AUXSOCpercent_Warning) {
                                                                        if(!AUXSOCpercent_Warning_notify_once) AUXSOCpercent_Warning_notify();
                                                                        AUXSOCpercent_Warning_notify_once = 1; // disable AUXSOCpercent warning notification  
                                                                     }  
                              else if (AUXBATTsoc > (AUXSOCpercent_Warning + 5)) AUXSOCpercent_Warning_notify_once = 0;   // re-enable AUXSOCpercent warning notification   
    
   }

void BATTtemperature_warning(){ // Main Battery temperature above set limit
                                if(BATTtemp > BATTtemp_Warning) {
                                                                        if(!BATTtemp_Warning_notify_once) BATTtemp_Warning_notify();
                                                                        BATTtemp_Warning_notify_once = 1; // disable BATTtemp warning notification  
                                                                 }  
                                else if (BATTtemp < (BATTtemp_Warning - 3)){
                                                                                BATTtemp_notify_one_more_time = 0; // reenable HIGH BATTtemp notifications
                                                                                BATTtemp_Warning_notify_once = 0;   // re-enable BATTtemp warning notifications after cooling below Warning level -3 degree Celsius   
                                                                              }
                                if ((BATTtemp > (BATTtemp_Warning + 10)) && !BATTtemp_notify_one_more_time) {
                                                                                                                    BATTtemp_notify_one_more_time = 1; // disable continous HIGH BATTtemp notifications
                                                                                                                    BATTtemp_Warning_notify_once = 0;
                                                                                                                  }                                     
                                 
  }

void SOCpercent_warning(){  // Main battery charge level, under set limit
                              if(SOCpercent < SOCpercent_Warning) {
                                                                        if(!SOCpercent_Warning_notify_once) SOCpercent_Warning_notify();
                                                                        SOCpercent_Warning_notify_once = 1; // disable AUXSOCpercent warning notification  
                                                                     }  
                              else if (SOCpercent > (SOCpercent_Warning + 5)){
                                                                                SOCpercent_notify_one_more_time = 0; // reenable LOW SOCpercent notifications
                                                                                SOCpercent_Warning_notify_once = 0;   // re-enable SOCpercent warning notifications after charging above Warning level +5 percent   
                                                                              }
                              if ((SOCpercent < (SOCpercent_Warning - 5)) && !SOCpercent_notify_one_more_time) {
                                                                                                                        SOCpercent_notify_one_more_time = 1; // disable continous LOW SOCpercent notifications
                                                                                                                        SOCpercent_Warning_notify_once = 0;
                                                                                                                      }
   }

void check_car_onoff_state(){
                               if(online_counter >= 60) { // after 1 minute check if the car is online
                                                           online_counter = 0;
                                                           if (online_flag) {
                                                                               still_online = 1; // yes Car is online
                                                                               if(notify_on_once == 1) {
                                                                                                          // notify that the car just started a trip 
                                                                                                          notify_on_once = 2; // TTS only once every ON/OFF cycle
                                                                                                          if (notify_startstop) car_status(); // check if notifications are enabled, then TTS Car status
                                                                                                       }
                                                                               
                                                                               notify_off_once = 1;  // enable notify, when Car will be offline again
                                                                               online_flag = 0;
                                                                               off_period = 0;
                                                                               Blynk.virtualWrite(V103, 0); // reset online_flag, to check later if it is still online, this will be updated by the Car under 1 minute
                                                                             }
                                                           else {
                                                                  still_online = 0; // Car is not online 
                                                                  off_period++;
                                                                  notify_on_once = 1;  // enable notify, when Car will be online again
                                                                  if(off_period >= car_off_delay && notify_off_once == 1) {  
                                                                                                                             // check if the car was stoppped for more than specified by "car_off_delay" in minutes 
                                                                                                                             // notify that the car just ended a trip
                                                                                                                             notify_off_once = 2;
                                                                                                                             if (notify_startstop) car_status(); 
                                                                                                                             
                                                                                                                            }
                                                                 
                                                                }
                                           }
}

// -------------------------------- Read Datas from sensors
void read_data(){

                  if(sensor_read_period >= 5){  // read temp/humi sensor every 2 seconds
                                                sensor_read_period = 0;
                                                
                                                       temp_humi = dht.readHumidity();
                                                       temp_temp = dht.readTemperature();
                                                       if(isnan(temp_humi) || isnan(temp_temp)){
                                                                                                  Serial.println("Failed to read from DHT sensor!");
                                                                                                  return;
                                                                                               }
                                                       humi_avg.addValue(temp_humi);     // read humidity
                                                       temp_avg.addValue(temp_temp);     // read temperature
                                                       temp_average = temp_avg.getAverage(); 
                                                       humi_average = humi_avg.getAverage();
                                                 
                                              }
                  
}

//--------------------------------- Buttons clicked
void click(Button2& btn) {
                            clicked = 1;
                            
                            if (btn == buttonB) {
                                                  if(button) button--;
                                                  else button = pagenumbers;  // number of pages -1
                                                  //DEBUG_PORT.println("A clicked");
                                                } 
                            else if (btn == buttonA) {
                                                        if(button < pagenumbers) button++;
                                                        else button = 0;
                                                        
                                                        //DEBUG_PORT.println("B clicked");
                                                      }
                         }

//--------------------------------- END Buttons Clicked
/*
//--------------------------------- Check Button Longpress
void longpress(Button2& btn) {  // Reset Trip Counter
                                unsigned int time = btn.wasPressedFor();
                                if (time > 2000) {
                                                   TRIP1ch = TRIP1dch = 0;
                                                 }
}
//--------------------------------- END Check Button Longpress
*/
//--------------------------------- Screen Connected
void connectedscreen() {
                          
                          tft.setTextColor(TFT_BLUE);
                          tft.setCursor(4, 35);
                          tft.setFreeFont(&Orbitron_Light_24);
                          
                          if(WiFi.status() != WL_CONNECTED) tft.println("  No WiFi");
                          else if (cast_error){
                                                 tft.println("  No Cast");
                                                 tft.setCursor(0, 70);
                                                 tft.println(" WiFi OK!"); 
                                              }
                          else {
                                 tft.println("WiFiCast");
                                 tft.setCursor(0, 70);
                                 tft.println("      OK!"); 
                                }
                          
                          
                          
                          delay(2000);
                          
                          tft.fillScreen(TFT_BLACK);
}
//-------------------------------- END Screen Connected

//-------------------------------- Draw objects Begin
void init_background(){
                         tft.setTextPadding(0);
                         tft.drawRect(0, 6, 135, 104, TFT_BLUE); // Blue rectangle
                         tft.drawRect(0, 131, 135, 104, TFT_BLUE); // Blue rectangle
                         //tft.drawLine(0, 120, 135, 120, TFT_BLUE);
                         
}

void draw_warningbox_top(){
                         tft.setTextColor(TFT_YELLOW,TFT_RED); 
                         tft.fillRect(7, 39, 122, 65, TFT_RED);
}

void draw_normalbox_top(){
                         tft.setTextColor(TFT_YELLOW,TFT_BLACK);
                         tft.fillRect(7, 39, 122, 65, TFT_BLACK); 
}

void draw_greenbox_top(){
                         tft.setTextColor(TFT_BLACK,TFT_GREEN); 
                         tft.fillRect(7, 39, 122, 65, TFT_GREEN);
}

void draw_warningbox_bottom(){
                         tft.setTextColor(TFT_YELLOW,TFT_RED); 
                         tft.fillRect(7, 163, 122, 65, TFT_RED);
}

void draw_normalbox_bottom(){
                         tft.setTextColor(TFT_YELLOW,TFT_BLACK);
                         tft.fillRect(7, 163, 122, 65, TFT_BLACK); 
}

void draw_greenbox_bottom(){
                         tft.setTextColor(TFT_BLACK,TFT_GREEN); 
                         tft.fillRect(7, 163, 122, 65, TFT_GREEN);
}
//--------------------------------- Draw objects END

// here you can config your Display as you wish
//--------------------------------- Display Page 1
void page1(){   
                init_background(); // init background image
 
                HOMEtemperature(1);    // display Home Temperature on TOP
                boiler_temperature(0); // display HotWater temperature on BOTTOM
                //HOMEhumidity(0);     // display Home Humidity
                
}
//--------------------------------- END Display Page 1

//--------------------------------- Display Page 2
void page2(){   
                init_background(); // init background image
 
                SOC(1);              // display SOC on Top
                BATTtemperature(0);  // display BATTtemperature on bottom
                
}
//--------------------------------- END Display Page 2

//--------------------------------- Display Page 3
void page3(){

                init_background(); // init background image
 
                INtemperature(1);    // display Indoor temp. on Top
                OUTtemperature(0);   // display Outdoor temp. on bottom
                
}
//--------------------------------- END Display Page 3

//--------------------------------- Display Page 4
void page4(){
                init_background(); // init background image
                
                BATTpower(1);      // Main Battery - Energy Draw
                BATTvolts(0);      // Main Battery Voltage
                
}
//--------------------------------- END Display Page 4

//--------------------------------- Display Page 5
void page5(){
                init_background(); // init background image
                
                AUXSOC(1);         // AUX BAttery SOC 
                AUXBATTvolts(0);   // AUX Battery Voltage
                
}
//--------------------------------- END Display Page 5

//--------------------------------- Display Page 6
void page6(){
                init_background(); // init background image
                
                SOH(1);         // AUX BAttery SOC 
                OPtime(0);        // operating time
                
}
//--------------------------------- END Display Page 6

//--------------------------------- Display Page 7
void page7(){
                init_background(); // init background image
                
                CEC(1);         // Cumulative Energy Charged 
                CED(0);        //  Cumulative Energy Discharged
                
}
//--------------------------------- END Display Page 7

//--------------------------------- Display Page 8
void page8(){
                init_background(); // init background image
                
                TRIP1charged(1);           //  Trip 1 counter Energy Charged  
                TRIP1discharged(0);        //  Trip 1 counter Energy Discharged
                
}
//--------------------------------- END Display Page 8


//Home Temperature 
void HOMEtemperature(boolean topbottom){ 
                                                           
                              
                              tft.setTextColor(TFT_WHITE,TFT_BLUE);
                              if (topbottom) { // top = 1
                                               tft.drawString("    H Temp C   ", 0, toptext, 4); 
                                               //check warning levels            
                                               if((temp_average < HOMEmintemp_Warning) || (temp_average > HOMEmaxtemp_Warning)) draw_warningbox_top();
                                               else  draw_normalbox_top();
                                               tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                               tft.drawFloat( temp_average,1, x, topdraw, 6);
                                             }
                              else           { // bottom = 0
                                               tft.drawString("    H Temp C   ", 0, bottomtext, 4); 
                                               //check warning levels            
                                               if((temp_average < HOMEmintemp_Warning) || (temp_average > HOMEmaxtemp_Warning)) draw_warningbox_bottom();
                                               else  draw_normalbox_bottom();
                                               tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                               tft.drawFloat( temp_average,1, x, bottomdraw, 6);
                                             }   
            
            
           
            /////////////////////////////////////////////////////// HOME Temperature 
            
            
}


//Home Humidity
void HOMEhumidity(boolean topbottom){ 
                                                           
                              
                              tft.setTextColor(TFT_WHITE,TFT_BLUE);
                              if (topbottom) { // top = 1
                                               tft.drawString("    H Humi %   ", 0, toptext, 4); 
                                               //check warning levels            
                                               if((humi_average < HOMEminhumi_Warning) || (humi_average > HOMEmaxhumi_Warning)) draw_warningbox_top();
                                               else  draw_normalbox_top();
                                               tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                               tft.drawFloat( humi_average,1, x, topdraw, 6);
                                             }
                              else           { // bottom = 0
                                               tft.drawString("    H Humi %   ", 0, bottomtext, 4); 
                                               //check warning levels            
                                               if((humi_average < HOMEminhumi_Warning) || (humi_average > HOMEmaxhumi_Warning)) draw_warningbox_bottom();
                                               else  draw_normalbox_bottom();
                                               tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                               tft.drawFloat( humi_average,1, x, bottomdraw, 6);
                                             }   
            
            
           
            /////////////////////////////////////////////////////// HOME Temperature and Humidity
            
            
}


//SOC - State Of Charge
void SOC(boolean topbottom){ 
                              //SOCpercent = random(100);
                              
                              
                              tft.setTextColor(TFT_WHITE,TFT_BLUE);
                              if (topbottom) { // top = 1
                                               tft.drawString("    SOC     %   ", 0, toptext, 4); 
                                               //check warning levels            
                                               if(SOCpercent < SOCpercent_Warning) draw_warningbox_top();
                                               else  draw_normalbox_top();
                                               tft.setTextPadding( tft.textWidth("888", 6) );
                                               tft.drawNumber( SOCpercent, x, topdraw, 6);
                                             }
                              else           { // bottom = 0
                                               tft.drawString("    SOC     %   ", 0, bottomtext, 4); 
                                               //check warning levels            
                                               if(SOCpercent < SOCpercent_Warning) draw_warningbox_bottom();
                                               else  draw_normalbox_bottom();
                                               tft.setTextPadding( tft.textWidth("888", 6) );
                                               tft.drawNumber( SOCpercent, x, bottomdraw, 6);
                                             }   
            
            
           
            /////////////////////////////////////////////////////// SOC
            
            
}

//AUXSOC - AUXiliary battery State Of Charge
void AUXSOC(boolean topbottom){ 
                              
                              //AUXSOCpercent = blynk_rpm/10;
                              
                              tft.setTextColor(TFT_WHITE,TFT_BLUE);
                              if (topbottom) { // top = 1
                                               tft.drawString(" Aux SOC %", 0, toptext, 4); 
                                               //check warning levels            
                                               if(AUXBATTsoc < AUXSOCpercent_Warning) draw_warningbox_top();
                                               else  draw_greenbox_top();
                                               tft.setTextPadding( tft.textWidth("888", 6) );
                                               tft.drawNumber( AUXBATTsoc, x, topdraw, 6);
                                             }
                              else           { // bottom = 0
                                               tft.drawString(" Aux SOC %", 0, bottomtext, 4); 
                                               //check warning levels            
                                               if(AUXBATTsoc < AUXSOCpercent_Warning) draw_warningbox_bottom();
                                               else  draw_greenbox_bottom();
                                               tft.setTextPadding( tft.textWidth("888", 6) );
                                               tft.drawNumber( AUXBATTsoc, x, bottomdraw, 6);
                                             }   
}
// Main Batt Temp
void BATTtemperature(boolean topbottom){                                       
                                           
                                           
                                           tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                           if (topbottom) { // top = 1
                                                                 tft.drawString(" Batt TempC", 0, toptext, 4); 
                                                                 //check warning levels            
                                                                 if(BATTtemp > BATTtemp_Warning) draw_warningbox_top();
                                                                 else  draw_normalbox_top();
                                                                 tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                                                 tft.drawFloat( BATTtemp, 1, x, topdraw, 6); //1 decimal places
                                                               }
                                                else           { // bottom = 0
                                                                 tft.drawString(" Batt TempC", 0, bottomtext, 4);
                                                                 //check warning levels            
                                                                 if(BATTtemp > BATTtemp_Warning) draw_warningbox_bottom();
                                                                 else  draw_normalbox_bottom();
                                                                 tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                                                 tft.drawFloat( BATTtemp, 1, x, bottomdraw, 6); //1 decimal places
                                                               }   
                                           
}

// Indoor Temperature
void INtemperature(boolean topbottom){
                                            

                                            tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                            if (topbottom) { // top = 1
                                                                 tft.drawString("    In TempC  ", 0, toptext, 4);
                                                                 draw_normalbox_top();
                                                                 tft.setTextPadding( tft.textWidth("888", 6) );
                                                                 tft.drawNumber( INtemp, x, topdraw, 6); // x = center
                                                           }
                                                else       { // bottom = 0
                                                                 tft.drawString("    In TempC  ", 0, bottomtext, 4);
                                                                 draw_normalbox_bottom();
                                                                 tft.setTextPadding( tft.textWidth("888", 6) );
                                                                 tft.drawNumber( INtemp, x, bottomdraw, 6); // x = center
                                                           }   
                                   
}

// Outdoor Temperature
void OUTtemperature(boolean topbottom){
                                             
                  
                                           tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                           if (topbottom) { // top = 1
                                                               tft.drawString(" Out TempC", 0, toptext, 4);
                                                               draw_normalbox_top();
                                                               tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                                               tft.drawNumber( OUTtemp, x, topdraw, 6); // x = center
                                                          }
                                           else           { // bottom = 0
                                                               tft.drawString(" Out TempC", 0, bottomtext, 4);
                                                               draw_normalbox_bottom();
                                                               tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                                               tft.drawNumber( OUTtemp, x, bottomdraw, 6); // x = center
                                                          }   
                         
}

// Main BATT Volts
void BATTvolts(boolean topbottom){
                                           
                  
                                           tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                           if (topbottom) { // top = 1
                                                               tft.drawString(" MainBatt  V", 0, toptext, 4);
                                                               if(BATTv < BATTv_LOW_Warning || BATTv > BATTv_HIGH_Warning) draw_warningbox_top();
                                                               else  draw_normalbox_top();
                                                               tft.setTextPadding( tft.textWidth("888", 6) );
                                                               tft.drawNumber( BATTv, x, topdraw, 6); //1 decimal places , x = center
                                                          }
                                           else           { // bottom = 0
                                                               tft.drawString(" MainBatt  V", 0, bottomtext, 4);
                                                               if(BATTv < BATTv_LOW_Warning || BATTv > BATTv_HIGH_Warning) draw_warningbox_bottom();
                                                               else  draw_normalbox_bottom();
                                                               tft.setTextPadding( tft.textWidth("888", 6) );
                                                               tft.drawNumber( BATTv, x, bottomdraw, 6); //1 decimal places , x = center
                                                          }   
}

// Battery Power Draw/Charge
void BATTpower(boolean topbottom){
                                            

                                            tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                            if (topbottom) { // top = 1
                                                                 tft.drawString("BattPow kW", 0, toptext, 4);
                                                                 if(BATTpow < 0) { // negative power REGEN
                                                                                    tempBATTpow = abs(BATTpow);
                                                                                    draw_greenbox_top(); 
                                                                                  }
                                                                 else {
                                                                         draw_warningbox_top();
                                                                         tempBATTpow = BATTpow; 
                                                                      }
                                                                 tft.setTextPadding( tft.textWidth("888.8", 6) );
                                                                 tft.drawFloat( tempBATTpow, 1, x, topdraw, 6); //1 decimal places , x = center
                                                           }
                                                else       { // bottom = 0
                                                                 tft.drawString("BattPower kW", 0, bottomtext, 4);
                                                                 if(BATTpow < 0) { // negative power REGEN
                                                                                    tempBATTpow = abs(BATTpow);
                                                                                    draw_greenbox_bottom(); 
                                                                                  }
                                                                 else {
                                                                         draw_warningbox_bottom();
                                                                         tempBATTpow = BATTpow; 
                                                                      }
                                                                 tft.setTextPadding( tft.textWidth("888.8", 6) );
                                                                 tft.drawFloat( BATTpow, 1, x, bottomdraw, 6); //1 decimal places , x = center
                                                           }   
                                   
}

// Main Battery current
void BATTcurrent(boolean topbottom){
                                            

                                            tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                            if (topbottom) { // top = 1
                                                                 tft.drawString("BattCur A", 0, toptext, 4);
                                                                 draw_normalbox_top();
                                                                 tft.setTextPadding( tft.textWidth("888", 6) );
                                                                 tft.drawNumber( BATTcur, x, topdraw, 6); //1 decimal places , x = center
                                                           }
                                                else       { // bottom = 0
                                                                 tft.drawString("BattCur A", 0, bottomtext, 4);
                                                                 draw_normalbox_bottom();
                                                                 tft.setTextPadding( tft.textWidth("888", 6) );
                                                                 tft.drawNumber( BATTcur, x, bottomdraw, 6); //1 decimal places , x = center
                                                           }   
                                   
}

//AUX BATT Volts
void AUXBATTvolts(boolean topbottom){
                                            
                  
                                           tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                           if (topbottom) { // top = 1
                                                               tft.drawString("  Aux Batt V  ", 0, toptext, 4);
                                                               if(AUXBATTv < AUXBATTv_LOW_Warning || AUXBATTv > AUXBATTv_HIGH_Warning) draw_warningbox_top();
                                                               else  draw_normalbox_top();
                                                               tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                                               tft.drawFloat( AUXBATTv, 1, x, topdraw, 6); //1 decimal places , x = center
                                                          }
                                           else           { // bottom = 0
                                                               tft.drawString("  Aux Batt V  ", 0, bottomtext, 4);
                                                               if(AUXBATTv < AUXBATTv_LOW_Warning || AUXBATTv > AUXBATTv_HIGH_Warning) draw_warningbox_bottom();
                                                               else  draw_normalbox_bottom();
                                                               tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                                               tft.drawFloat( AUXBATTv, 1, x, bottomdraw, 6); //1 decimal places , x = center
                                                          }   
}


// SOH - State Of Health
void SOH(boolean topbottom){
                                            

                                            tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                            if (topbottom) { // top = 1
                                                                 tft.drawString("         SOH   %     ", 0, toptext, 4);
                                                                 draw_normalbox_top();
                                                                 tft.setTextPadding( tft.textWidth("888", 6) );
                                                                 tft.drawNumber( SOHpercent, x, topdraw, 6); //1 decimal places , x = center
                                                           }
                                                else       { // bottom = 0
                                                                 tft.drawString("         SOH   %     ", 0, bottomtext, 4);
                                                                 draw_normalbox_bottom();
                                                                 tft.setTextPadding( tft.textWidth("888", 6) );
                                                                 tft.drawNumber( SOHpercent, x, bottomdraw, 6); //1 decimal places , x = center
                                                           }   
                                   
}


// Operating time
void OPtime(boolean topbottom){
                                            

                                            tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                            if (topbottom) { // top = 1
                                                                 tft.drawString("      Op. Time H ", 0, toptext, 4);
                                                                 draw_normalbox_top();
                                                                 tft.setTextPadding( tft.textWidth("888888", 4) );
                                                                 tft.drawNumber( OPtimehours, x, topdraw, 4); //1 decimal places , x = center
                                                           }
                                                else       { // bottom = 0
                                                                 tft.drawString("      Op. Time H ", 0, bottomtext, 4);
                                                                 draw_normalbox_bottom();
                                                                 tft.setTextPadding( tft.textWidth("888888", 4) );
                                                                 tft.drawNumber( OPtimehours, x, bottomdraw, 4); //1 decimal places , x = center
                                                           }   
                                   
}

// CEC Cumulative Energy Charged
void CEC(boolean topbottom){
                                            
                  
                                           tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                           if (topbottom) { // top = 1
                                                               tft.drawString("   CEC   kWh  ", 0, toptext, 4);
                                                               draw_greenbox_top();
                                                               tft.setTextPadding( tft.textWidth("888888.8", 4) );
                                                               tft.drawFloat( CECvalue, 1, x, topdraw, 4); //1 decimal places , x = center
                                                          }
                                           else           { // bottom = 0
                                                               tft.drawString("   CEC   kWh  ", 0, bottomtext, 4);
                                                               draw_greenbox_bottom();
                                                               tft.setTextPadding( tft.textWidth("888888.8", 4) );
                                                               tft.drawFloat( CECvalue, 1, x, bottomdraw, 4); //1 decimal places , x = center
                                                          }   
}


// CEC Cumulative Energy Discharged
void CED(boolean topbottom){
                                            
                  
                                           tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                           if (topbottom) { // top = 1
                                                               tft.drawString("   CED   kWh  ", 0, toptext, 4);
                                                               draw_warningbox_top();
                                                               tft.setTextPadding( tft.textWidth("888888.8", 4) );
                                                               tft.drawFloat( CEDvalue, 1, x, topdraw, 4); //1 decimal places , x = center
                                                          }
                                           else           { // bottom = 0
                                                               tft.drawString("   CED   kWh  ", 0, bottomtext, 4);
                                                               draw_warningbox_bottom();
                                                               tft.setTextPadding( tft.textWidth("888888.8", 4) );
                                                               tft.drawFloat( CEDvalue, 1, x, bottomdraw, 4); //1 decimal places , x = center
                                                          }   
}

// Trip1 Charged Counter, Power IN charged / OUT discharged
void TRIP1charged(boolean topbottom){
                                             
                  
                                           tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                           if (topbottom) { // top = 1
                                                               tft.drawString("  T1 Ch kWh  ", 0, toptext, 4);
                                                               draw_greenbox_top();
                                                               tft.setTextPadding( tft.textWidth("888.8", 6) );
                                                               tft.drawFloat( TRIP1ch, 1, x, topdraw, 6); //1 decimal places , x = center
                                                          }
                                           else           { // bottom = 0
                                                               tft.drawString(" T1 Ch kWh", 0, bottomtext, 4);
                                                               draw_greenbox_bottom();
                                                               tft.setTextPadding( tft.textWidth("888.8", 6) );
                                                               tft.drawFloat( TRIP1ch, 1, x, bottomdraw, 6); //1 decimal places , x = center
                                                          }   
                         
}

// Trip1 Discharged Counter, Power IN charged / OUT discharged
void TRIP1discharged(boolean topbottom){
                                             
                  
                                           tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                           if (topbottom) { // top = 1
                                                               tft.drawString("  T1 Dch kWh  ", 0, toptext, 4);
                                                               draw_warningbox_top();
                                                               tft.setTextPadding( tft.textWidth("888.8", 6) );
                                                               tft.drawFloat( TRIP1dch, 1, x, topdraw, 6); //1 decimal places , x = center
                                                          }
                                           else           { // bottom = 0
                                                               tft.drawString(" T1 Dch kWh", 0, bottomtext, 4);
                                                               draw_warningbox_bottom();
                                                               tft.setTextPadding( tft.textWidth("888.8", 6) );
                                                               tft.drawFloat( TRIP1dch, 1, x, bottomdraw, 6); //1 decimal places , x = center
                                                          }   
                         
}

void boiler_temperature(boolean topbottom){
                                            
                                           tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                           if (topbottom) { // top = 1
                                                               tft.drawString("BoilerTemp", 0, toptext, 4);
                                                               if(boiler_temp < boiler_LOW_Warning || boiler_temp > boiler_HIGH_Warning) draw_warningbox_top();
                                                               else  draw_normalbox_top();
                                                               tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                                               tft.drawFloat( boiler_temp, 1, x, topdraw, 6); //1 decimal places , x = center
                                                          }
                                           else           { // bottom = 0
                                                               tft.drawString("BoilerTemp", 0, bottomtext, 4);
                                                               if(boiler_temp < boiler_LOW_Warning || boiler_temp > boiler_HIGH_Warning) draw_warningbox_bottom();
                                                               else  draw_normalbox_bottom();
                                                               tft.setTextPadding( tft.textWidth("-88.8", 6) );
                                                               tft.drawFloat( boiler_temp, 1, x, bottomdraw, 6); //1 decimal places , x = center
                                                          }    
  }

              
