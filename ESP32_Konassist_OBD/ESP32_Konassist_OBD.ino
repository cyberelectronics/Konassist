/*  Konassist for Hyundai Kona EV + OBD Vgate iCar Pro BT4.0 
 *  Version: v1.1
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

//#include <esp8266-google-home-notifier.h>  // Google Home notifier
//GoogleHomeNotifier ghn;

#include "BluetoothSerial.h"
#include "ELMduino.h"

#include "SafeString.h"
createSafeString(dataFrame0, 14);   // will also add space for the terminating null =>15 sized array
createSafeString(dataFrame1, 14);
createSafeString(dataFrame2, 14);
createSafeString(dataFrame3, 14);
createSafeString(dataFrame4, 14);
createSafeString(dataFrame5, 14);
createSafeString(dataFrame6, 14);
createSafeString(dataFrame7, 14);
createSafeString(dataFrame8, 14);


/*
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
RunningAverage temp_avg(10);  // Average Temp sent to Blynk 
RunningAverage humi_avg(10);  // Average Humi sent to Blynk 
//----------------------------

DHT dht(DHTPIN, DHTTYPE);
*/

#include "Button2.h"

BluetoothSerial SerialBT;
#define ELM_PORT   SerialBT
#define DEBUG_PORT Serial

ELM327 myELM327;

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

WidgetLED ACcharge_led(V6);
WidgetLED DCcharge_led(V5);

// Bridge widget 
WidgetBridge bridge1(V90);  // Car Assistant 
WidgetBridge bridge2(V100);  // Home Device

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Config ==================================================

//#define DEBUG     // serial print for debug

#define car_off_delay 5             // Delay in minutes to send a notification (TTS) to Home Device, after the car was stoppped
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

//const char displayName[] = "CarAssistant";  // Google Home / Chromecast / Nvidia Shield / Smartphone using CastReceiver app - Device Name
//char language[] = "en"; // Select "text to speech" Response Language: hu, ro, en, etc...  

int ledBacklight = 80; // Initial TFT backlight intensity on a scale of 0 to 255. Initial value is 80.

//#define senduptime_interval 2    // Send 1 group of datas to Blynk every 2 seconds

// END Config ================================================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef DEBUG
 #define DEBUG_PRINT(...)   Serial.print(F(#__VA_ARGS__" = ")); Serial.print (__VA_ARGS__);Serial.print(F(" "))
 
#else
 #define DEBUG_PRINT(...)    
#endif
#ifdef DEBUG
 #define DEBUG_PRINTLN(...) DEBUG_PRINT(__VA_ARGS__); Serial.println()
#else
 #define DEBUG_PRINTLN(...)  
#endif

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
byte senduptime_counter;
boolean clicked;
boolean car_status_flag, car_battery_flag, cell_voltage_flag, home_temphumi_flag; // Start response
boolean online_flag, still_online;  // check if the Car is still Online 
boolean notify_startstop;
boolean notify_warning;
boolean charging_flag, charging;
boolean just_booted;
boolean notify_once;
boolean reset_trip;
boolean alldata_ready;
//boolean reset_distance_trip;
boolean sync_started;
byte sec_counter, check_delay;

float CECvalue_buffer; // Cummulative Energy Charged temp. buffer
float CEDvalue_buffer; // Cummulative Energy Discharged temp. buffer

float CECvalue; // Cummulative Energy Charged
float CEDvalue; // Cummulative Energy Discharged
float AUXBATTv; // Auxiliary Battery Voltage
float AUXBATTcur; // Auxiliary Battery Current
byte  AUXBATTsoc; // Auxiliary Battery State Of Charge
float BATTpow;  // MAin Battery Power
float BATTtemp; // Battery temperature
int BATTINtemp; // Battery Inlet Temperature
float MAXcellv; // Maximum Cell Voltage
float MINcellv; // Minimum Cell Voltage
byte  MAXcellvno; // Maximum Cell Voltage Number
byte  MINcellvno; // Minimum Cell Voltage Number
byte BMSrelay;      // BMS relay
byte CHARGE;       // ACDC charging signal
byte BATTFANspeed; // FAN Speed
int COOLtemp;    // Cooling water temperature
int HEATtemp1, HEATtemp2; // Heater temperatures
float TRIP1ch;    // Trip 1 charged kWh
float TRIP1dch;   // Trip 1 discharged kWh
float tempBATTpow; // temporary
float INtemp, OUTtemp; // Indoor / Outdoor temperatures
float RLTpress, RRTpress, FLTpress, FRTpress;  // Tyre Pressure - in bar
float ACCcur;     // AC charge current
float SPEED;      // Vehicle speed in km/h
int OUTDOORtemp, INDOORtemp; // Outdoor/Indoor temperature
float VOLTdiff;    // Voltage difference
float temp_average, humi_average;
float TRIP1used;  // TRIP1 energy used
float TRIP1avg;   // TRIP1 energy kWh/100km

int SOCpercent;   // State Of Charge in percent
int SOCreal;       // State Of Charge in percent
int SOHpercent;   // State Of Health in percent
float BATTv;        // Main Battery Voltage
float BATTcur;      // Main Battery Current
int BATTtemp1, BATTtemp2, BATTtemp3, BATTtemp4;  // Main Battery temperature per module
int BATTMAXtemp, BATTMINtemp;                    // Main Battery MAX and MIN temperature 
int RLTtemp, RRTtemp, FLTtemp, FRTtemp;          // Tyre temperature 
int TRIP1odo;    // Trip1 distance
byte CURbyte1, CURbyte2;   // main batt. current debug

int AUXSOCpercent;            // AUX battery State Of Charge   
uint32_t OPtimehours;         // Operating time in hours
uint32_t ODOkm;               // ODO -  distance already in km
uint32_t ODOkm_buffer;        // Saved ODO in Blynk cloud    
int cell_voltage_difference;  // Difference between MIN and MAX cell voltages

//String  txtnotify;

int ms_counter; // incremented every 100ms
int online_counter; // check every minute if Car is still Online
boolean alive;
boolean display_onoff; // button from Blynk to turn ON/OFF the display
boolean cast_error;    // error flag - Cast device was not found
boolean first_sync;    // first sync flag, stops TTS (text to speech) notifications
boolean ACcharging, DCcharging, send_once;

//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40];
char mqtt_port[6] = "8080";
char blynk_token[40] = "";             // Config from WebUI; Here Leave it empty - OBD device token 
char blynk_home_token[40] = "";        // Config from WebUI; Here leave it empty - Home device blynk token
char blynk_carassist_token[40] = "";   // Config from WebUI; Here leave it empty - Car Assistant blynk token

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
                    bridge1.setAuthToken(blynk_carassist_token);  // AuthToken of the Car TTS hardware
                    bridge2.setAuthToken(blynk_home_token);       // AuthToken of the Home hardware
                    
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

BLYNK_WRITE(V26){
                  reset_trip = param.asInt();
                }                

/*
BLYNK_WRITE(V101){  // "Set language" 
                   language_number = param.asInt();
                   if(language_number == 1) strcpy(language,"hu");
                   else if(language_number == 2) strcpy(language,"ro");
                   else if(language_number == 3) strcpy(language,"en");
                   ghn.device(displayName, language);
                   if (!first_sync) language_change();
                } 

BLYNK_WRITE(V102){  // "Car status" response
                   check_response = param.asInt();
                   if (!first_sync) {
                                      if(check_response == 1) car_status_flag = 1;
                                      else if(check_response == 2) car_battery_flag = 1; 
                                      else if(check_response == 3) cell_voltage_flag = 1;
                                      else if(check_response == 4) home_temphumi_flag = 1;
                                    }
                } 


BLYNK_WRITE(V103){  // "Online?" from car
                   online_flag = param.asInt();
                   //if (!first_sync) online_flag = 1;  // 
                   
                } 


BLYNK_WRITE(V104){  // Enable/disable "Start/Stop" notifications 
                   notify_startstop = param.asInt();
                                   
                }  

BLYNK_WRITE(V105){  // Enable/Disable "Warning" messages" 
                   notify_warning = param.asInt();
                                   
                } 

BLYNK_WRITE(V106){  // Charging/Not Charging flag 
                   charging_flag = param.asInt();
                   if(!first_sync) notify_once = 1;                
                } 
*/                 
 BLYNK_WRITE(V107){  // Home Average temperature sent from the Home device  
                   temp_average = param.asInt()*0.1;
                               
                }  

 BLYNK_WRITE(V108){  // Home Average temperature sent from the Home device  
                   humi_average = param.asInt()*0.1;
                               
                } 


BLYNK_WRITE(V80){  // Saved Cumulative energy charged  
                   CECvalue_buffer = param.asInt()*0.1;
                               
                }   

BLYNK_WRITE(V81){  // Saved Cumulative energy discharged  
                   CEDvalue_buffer = param.asInt()*0.1;
                               
                }

BLYNK_WRITE(V82){  // Saved Cumulative energy discharged  
                   ODOkm_buffer = param.asInt();
                               
                }

BLYNK_WRITE(V36){  // Saved Cumulative energy discharged  
                   TRIP1odo = param.asInt();
                               
                }                                                                                                            

// SEND to Blynk =============================================================================
void sendUptime()
                  { 
                     // send datas in "group" - otherwise Blynk can disconnect your HW (temporary) for flooding         
                                   
                     if(reset_trip) {  // Trip Reset Button or TTS command
                                       reset_trip = 0;
                                       ODOkm_buffer = ODOkm;
                                       TRIP1odo  = 0; // reset Trip 1 Distance (km)
                                       TRIP1ch = 0;   // reset Trip 1 Charged value
                                       TRIP1dch = 0;  // reset Trip 1 Discharged value
                                       CECvalue_buffer = CECvalue;
                                       CEDvalue_buffer = CEDvalue;
                                       Blynk.virtualWrite(V80, CECvalue_buffer*10);     // Trip 1 Saved CEC buffer*10, for Trip1ch calc. 
                                       Blynk.virtualWrite(V81, CEDvalue_buffer*10);     // Trip 1 Saved CED buffer*10, for Trip1dch calc 
                                       Blynk.virtualWrite(V26, 0);    // Reset Trip reset button in Blynk app
                                       Blynk.virtualWrite(V36, TRIP1odo);        // Trip 1 distance (km)
                                       bridge1.virtualWrite(V99, 1);  // Send Trip Reset to Car Assistant
                                       bridge2.virtualWrite(V99, 1);  // Send Trip Reset to Home Device 
                                       
                                       Blynk.virtualWrite(V82, ODOkm_buffer); // save actual ODOkm for distance calc.   
                                   } 
                     
                     else if((BATTv > 270) && alldata_ready){  // check if OBD was inited
                                                           group++;
                                                           
                                                           if(group == 1){
                                                                           //first group of datas to send
                                                                                                                                                        
                                                                           Blynk.virtualWrite(V0, SOCpercent);
                                                                           bridge1.virtualWrite(V107, SOCpercent);  // Send to CarAssistant
                                                                           bridge2.virtualWrite(V107, SOCpercent);  // Send to Home device
                                                                           bridge2.virtualWrite(V103, 1);  // Send to Home device - Online status

                                                                           Blynk.virtualWrite(V1, String(BATTtemp, 1) ); // Main Battery temperature
                                                                           bridge1.virtualWrite(V108, BATTtemp*10);      // Send to CarAssistant
                                                                           bridge2.virtualWrite(V108, BATTtemp*10);      // Send to Home device
                                                                           
                                                                           bridge1.virtualWrite(V126, temp_average*10);  // Home temperature to CarAssistant
                                                                           bridge1.virtualWrite(V127, humi_average*10);  // Home humidity to CarAssistant
                                                                           
                                                                         }
                                                           else if(group == 2){ 
                                                                                // group 2 of datas to send
                                                                                Blynk.virtualWrite(V2, String(BATTv, 1));      // Main battery Voltage
                                                                                bridge1.virtualWrite(V112, BATTv);  // Send to CarAssistant
                                                                                bridge2.virtualWrite(V112, BATTv);  // Send to Home device
                                                                                
                                                                                Blynk.virtualWrite(V38, String(BATTcur, 1));        // Main Battery current (A)

                                                                                Blynk.virtualWrite(V4, String(BATTpow, 2));  // Main battery Charging/Disch Power
                                                                                bridge1.virtualWrite(V111, BATTpow*10);      // Send to CarAssistant
                                                                                bridge2.virtualWrite(V111, BATTpow*10);      // Send to Home device
                                                                                
                                                                                Blynk.virtualWrite(V3, String(AUXBATTv, 1));     // Auxiliary Battery Voltage
                                                                                bridge1.virtualWrite(V114, AUXBATTv*10);  // Send to CarAssistant
                                                                                bridge2.virtualWrite(V114, AUXBATTv*10);  // Send to Home device  
                                                                                
                                                                                
                                                                                
                                                                                
                                                                               }                 
                                                          else if(group == 3){   
                                                                               // group 3 of datas to send
                                                                               Blynk.virtualWrite(V19, BATTINtemp);         // Main battery Inlet temperature
                                                                               Blynk.virtualWrite(V25, HEATtemp1);          // Main battery Heater1 temperature
                                                                               Blynk.virtualWrite(V32, String(ACCcur, 1));  // AC current charging
                                                                               Blynk.virtualWrite(V35, COOLtemp);           // Cooling liquid temperature
                                                                               Blynk.virtualWrite(V33, OUTDOORtemp);        // Car Outdoor temperature
                                                                               Blynk.virtualWrite(V34, INDOORtemp);         // Car Indoor temperature
                                                                               
                                                                               Blynk.virtualWrite(V27, AUXBATTsoc);     // Auxiliary Battery State Of Charge
                                                                               bridge1.virtualWrite(V113, AUXBATTsoc);  // Send to CarAssistant
                                                                               bridge2.virtualWrite(V113, AUXBATTsoc);  // Send to Home device

                                                                               
                                                                                                                                                              
                                                                            } 
                                                          else if(group == 4){
                                                                               Blynk.virtualWrite(V7, String(CECvalue, 1)); // Main battery Cumulative Energy Charged
                                                                               bridge1.virtualWrite(V117, CECvalue);        // Send to CarAssistant
                                                                               bridge2.virtualWrite(V117, CECvalue);        // Send to Home device
                                                                               Blynk.virtualWrite(V8, String(CEDvalue, 1)); // Main battery Cumulative Energy Discharged
                                                                               bridge1.virtualWrite(V118, CEDvalue);        // Send to CarAssistant
                                                                               bridge2.virtualWrite(V118, CEDvalue);        // Send to Home device
                                                                               
                                                                               Blynk.virtualWrite(V9, OPtimehours);      // Car Operating time in hours
                                                                               bridge1.virtualWrite(V79, OPtimehours);   // Send to CarAssistant
                                                                               
                                                                               

                                                                             }
                                                                             
                                                          else if(group == 5){
                                                                               
                                                                               
                                                                               
                                                                               Blynk.virtualWrite(V23, String(TRIP1ch, 1));     // Trip 1 charged energy (kWh)
                                                                               bridge1.virtualWrite(V119, TRIP1ch*10);  // Send to CarAssistant
                                                                               bridge2.virtualWrite(V119, TRIP1ch*10);  // Send to Home device
                                                                               
                                                                               Blynk.virtualWrite(V24, String(TRIP1dch, 1));     // Trip 1 discharged energy (kWh)
                                                                               Blynk.virtualWrite(V39, String(TRIP1used, 1));    // Trip 1 used energy (kWh)
                                                                               Blynk.virtualWrite(V40, String(TRIP1avg, 1));     // Trip 1 energy kWh/100km
                                                                               
                                                                               Blynk.virtualWrite(V36, TRIP1odo);        // Trip 1 distance (km)
                                                                               bridge1.virtualWrite(V83, TRIP1odo);      // Send to CarAssistant
                                                                               bridge2.virtualWrite(V83, TRIP1odo);      // Send to Home device

                                                                            }

                                                          else if(group == 6){
                                                                              // group 6 of datas to send
                                                                               bridge1.virtualWrite(V120, TRIP1dch*10);  // Send to CarAssistant
                                                                               bridge2.virtualWrite(V120, TRIP1dch*10);  // Send to Home device
                                                                               
                                                                               //Blynk.virtualWrite(V86, BMSrelay);     // 131dec when ON , Debug/Check BMS relay Signal (value) in Blynk app
                                                                               //Blynk.virtualWrite(V83, CHARGE);       // Debug/Check Charge Signal (value) in Blynk app

                                                                               //Blynk.virtualWrite(V84, CURbyte1);     // Debug/Check BMS relay Signal (value) in Blynk app
                                                                               //Blynk.virtualWrite(V85, CURbyte2);     // Debug/Check Charge Signal (value) in Blynk app

                                                                              if(ACcharging && !send_once){  // ACcharging
                                                                                                             send_once = 1;
                                                                                                             ACcharge_led.on();
                                                                                                             bridge1.virtualWrite(V106, 1);  // Send to CarAssistant
                                                                                                             bridge2.virtualWrite(V106, 1);  // Send to Home device
                                                                                                          }
                                                                               else if(DCcharging && !send_once){   // DCcharging
                                                                                                                    send_once = 1;
                                                                                                                    DCcharge_led.on();
                                                                                                                    bridge1.virtualWrite(V106, 2);  // Send to CarAssistant
                                                                                                                    bridge2.virtualWrite(V106, 2);  // Send to Home device
                                                                                                                } 
                                                                               
                                                                               else {
                                                                                      Blynk.virtualWrite(V10, SOHpercent);     // Auxiliary Battery State Of Charge
                                                                                      bridge1.virtualWrite(V115, SOHpercent);  // Send to CarAssistant
                                                                                      bridge2.virtualWrite(V115, SOHpercent);  // Send to Home device 
                                                                                }                                               
                                                                               
                                                                               Blynk.virtualWrite(V37, ODOkm);        // ODO (km)
                                                                               
                                                                                                                                                                
                                                                               Blynk.virtualWrite(V15, String(MINcellv, 2));     // Minimum Cell Voltage
                                                                               bridge1.virtualWrite(V124, MINcellv*100);  // Send to CarAssistant
                                                                               bridge2.virtualWrite(V124, MINcellv*100);  // Send to Home device

                                                                            }                  

                                                           else if(group == 7){
                                                                                // group 7 of datas to send

                                                                               Blynk.virtualWrite(V17, String(MAXcellv, 2));     // Maximum Cell Voltage
                                                                               bridge1.virtualWrite(V123, MAXcellv*100);  // Send to CarAssistant
                                                                               bridge2.virtualWrite(V123, MAXcellv*100);  // Send to Home device

                                                                               Blynk.virtualWrite(V16, MINcellvno);     // Minimum Cell Voltage - Cell Number
                                                                               Blynk.virtualWrite(V18, MAXcellvno);     // Maximum Cell Voltage - Cell Number

                                                                               cell_voltage_difference = (MAXcellv - MINcellv)*1000; // Voltage difference in millivolts
                                                                               bridge1.virtualWrite(V125, cell_voltage_difference);  // Send to CarAssistant
                                                                               bridge2.virtualWrite(V125, cell_voltage_difference);  // Send to Home device
                                                                               
                                                                              }

                                                           else if(group >= 8){
                                                                                // group 8 of datas to send
                                                                               Blynk.virtualWrite(V11, String(FLTpress, 2));  // Front Left Tyre Pressure (bar)
                                                                               Blynk.virtualWrite(V12, String(FRTpress, 2));  // Front Right Tyre Pressure (bar)
                                                                               Blynk.virtualWrite(V13, String(RLTpress, 2));  // Rear Left Tyre Pressure (bar)
                                                                               Blynk.virtualWrite(V14, String(RRTpress, 2));  // Rear Right Tyre Pressure (bar)

                                                                               Blynk.virtualWrite(V28, FLTtemp);  // Front Left Tyre Pressure (bar)
                                                                               Blynk.virtualWrite(V29, FRTtemp);  // Front Right Tyre Pressure (bar)
                                                                               Blynk.virtualWrite(V30, RLTtemp);  // Rear Left Tyre Pressure (bar)
                                                                               Blynk.virtualWrite(V31, RRTtemp);  // Rear Right Tyre Pressure (bar)
                                                                               
                                                                               group = 0; // reset the group counter 
                                                                               
                                                                              }
                                                                                            
                                               }
                     else { 
                            alldata_ready = 0;
                            group = 0; // reset the group counter 

                            bridge1.virtualWrite(V126, temp_average*10);
                            bridge1.virtualWrite(V127, humi_average*10);
                            
                          }
                                                    
                  }
   
// SETUP =======================================================================
void setup() {
               // put your setup code here, to run once:
                   //ESP_ERROR_CHECK( heap_trace_init_standalone(trace_record, NUM_RECORDS) );

                   //pinMode(A0, INPUT);           // Vinput and Batt voltage
                   //pinMode(DHTPIN, INPUT);
                   //dht.begin();   
                    
                   pinMode(BUTTON_A_PIN, INPUT_PULLUP);
                   pinMode(ADC_EN, OUTPUT);
                   digitalWrite(ADC_EN, HIGH);
                   pinMode(TFT_BL, OUTPUT); 

                    ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
                    ledcAttachPin(TFT_BL, pwmLedChannelTFT);
                    ledcWrite(pwmLedChannelTFT, ledBacklight);
                   
                  DEBUG_PORT.begin(115200);
                  DEBUG_PORT.println();
                  //delay(3000);

                  //clean FS, for testing
                  //SPIFFS.format();
                
                  //read configuration from FS json
                  DEBUG_PORT.println("mounting FS...");

                 if (SPIFFS.begin()) {
                                        DEBUG_PORT.println("mounted file system");
                                        if (SPIFFS.exists("/config.json")) {
                                                                              //file exists, reading and loading
                                                                              DEBUG_PORT.println("reading config file");
                                                                              File configFile = SPIFFS.open("/config.json", "r");
                                                                              if (configFile) {
                                                                                                    DEBUG_PORT.println("opened config file");
                                                                                                    size_t size = configFile.size();
                                                                                                    // Allocate a buffer to store contents of the file.
                                                                                                    std::unique_ptr<char[]> buf(new char[size]);
                                                                                                    configFile.readBytes(buf.get(), size);
                                                                                                    DynamicJsonDocument jsonBuffer(1024);
                                                                                                    
                                                                                                    deserializeJson(jsonBuffer, buf.get());
                                                                                                    auto error = serializeJson(jsonBuffer, Serial);
                                                                                                    
                                                                                                    if (error) {
                                                                                                                  DEBUG_PORT.println("\nparsed json");
                                                                                                        
                                                                                                               ///   strcpy(mqtt_server, jsonBuffer["mqtt_server"]);
                                                                                                               ///   strcpy(mqtt_port, jsonBuffer["mqtt_port"]);
                                                                                                                 strcpy(blynk_token, jsonBuffer["blynk_token"]);
                                                                                                                 strcpy(blynk_carassist_token, jsonBuffer["blynk_carassist_token"]);
                                                                                                                 strcpy(blynk_home_token, jsonBuffer["blynk_home_token"]);
                                                                                                                } 
                                                                                                    else {
                                                                                                           DEBUG_PORT.println("failed to load json config");
                                                                                                         }
                                                                                                    configFile.close();
                                                                                               }
                                                                            }
                                      }
                  else {
                          DEBUG_PORT.println("failed to mount FS");
                        }
                //end read

                listFiles(); // Lists the files so you can see what is in the SPIFFS

                ////////////////// Init and Draw JPEG Welcome Screen BEGIN
                
                  tft.begin();
                  tft.setRotation(0);  // 0 & 2 Portrait. 1 & 3 landscape
                  tft.fillScreen(TFT_BLACK);
                  drawJpeg("/tiger.jpg", 0 , 0);     // 240 x 320 image
                  
                ////////////////// Draw JPEG Welcome Screen END

                // The extra parameters to be configured (can be either global or just in the setup)
                // After connecting, parameter.getValue() will get you the configured value
                // id/name placeholder/prompt default length
                // WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
                // WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
                   WiFiManagerParameter custom_blynk_token("blynk", "Car Blynk Token", blynk_token, 33);
                   WiFiManagerParameter custom_blynk_carassist_token("blynk1", "Car TTS Blynk Token", blynk_carassist_token, 33); 
                   WiFiManagerParameter custom_blynk_home_token("blynk2", "Home Blynk Token", blynk_home_token, 33);
              
                WiFiManager wifiManager;
                //set config save notify callback
                wifiManager.setSaveConfigCallback(saveConfigCallback);
              
                //set static ip
                //wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
                
                //add all your parameters here
              /// wifiManager.addParameter(&custom_mqtt_server);
              /// wifiManager.addParameter(&custom_mqtt_port);
                  wifiManager.addParameter(&custom_blynk_token);
                  wifiManager.addParameter(&custom_blynk_carassist_token);
                  wifiManager.addParameter(&custom_blynk_home_token);


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
              
              if(digitalRead(BUTTON_A_PIN) == LOW) {    // Push the A button at powerup, if you want to enter in WiFi/Blynk token setup page. Access 192.168.4.1 after connected to this AP  
                                                        wifiManager.resetSettings();
                                                        wifiManager.setEnableConfigPortal(true);
                                                        wifiManager.autoConnect("Setup_Konassist_OBD","konapass"); // start config Portal / SSID: Setup_Konassist / Password: konapass
                                                    }
              
              else{
                      wifiManager.setEnableConfigPortal(false);
                      if(!wifiManager.autoConnect()) {
                        WiFi.disconnect();
                        WiFi.mode(WIFI_OFF);
                       // wifiManager.setEnableConfigPortal(true);
                        wifiManager.autoConnect();
                      }
              }

              //if you get here you have connected to the WiFi
              DEBUG_PORT.println("connected...yeey :)");
            
              //read updated parameters
             /// strcpy(mqtt_server, custom_mqtt_server.getValue());
             /// strcpy(mqtt_port, custom_mqtt_port.getValue());
              strcpy(blynk_token, custom_blynk_token.getValue());
              strcpy(blynk_carassist_token, custom_blynk_carassist_token.getValue());
              strcpy(blynk_home_token, custom_blynk_home_token.getValue());
              
              //save the custom parameters to FS
              if (shouldSaveConfig) {
                                      Serial.println("saving config");
                                      DynamicJsonDocument jsonBuffer(1024);
                                      
                                     /// jsonBuffer["mqtt_server"] = mqtt_server;
                                     /// jsonBuffer["mqtt_port"] = mqtt_port;
                                      jsonBuffer["blynk_token"] = blynk_token;
                                      jsonBuffer["blynk_carassist_token"] = blynk_carassist_token;
                                      jsonBuffer["blynk_home_token"] = blynk_home_token;
                                      
                                  
                                      File configFile = SPIFFS.open("/config.json", "w");
                                      if (!configFile) {
                                                          Serial.println("failed to open config file for writing");
                                                        }
                                  
                                      //serializeJson(jsonBuffer, Serial);
                                      serializeJson(jsonBuffer, configFile);
                                      
                                      configFile.close();
                                      //end save
                                    }
            
              DEBUG_PORT.println("local ip");
              DEBUG_PORT.println(WiFi.localIP());
              //Serial.print("Blynk Token : ");
              //Serial.println(blynk_token);
            
            Blynk.config(blynk_token);

            //---------------------------- Connect BT
             
              ELM_PORT.setPin("1234");         // PIN for iCar Vgate pro BT4.0 OBD adapter 
              ELM_PORT.begin("Boost", true);
              
              if (!ELM_PORT.connect("Android-Vlink"))   // Device name of iCar Vgate pro BT4.0 OBD adapter 
              {
                DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");
                while(1);
              }
            
              // if (!myELM327.begin(ELM_PORT))  // select protocol Auto
              if (!myELM327.begin(ELM_PORT,'6')){ // select protocol '6'
                                                  DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 2");
                                                  delay(1000);
                                                  //reset and try again, or maybe put it to deep sleep
                                                  ESP.restart();
                                                  //while (1);
                                                }
            
              DEBUG_PORT.println("Connected to ELM327");
           
            //----------------------- END Connect BT
            
            
            
            
            
            timer.setInterval(2000L, sendUptime); //  Here you set interval (1sec) and which function to call
            
            buttonA.setClickHandler(click);
            buttonB.setClickHandler(click);
            buttonA.setLongClickHandler(longpress);
            
            
            /*
            // connecting to a Cast device
            DEBUG_PORT.println("connecting to a Cast device...");
            
            if (ghn.device(displayName, "en") != true) 
                                                      {
                                                        DEBUG_PORT.println(ghn.getLastError());
                                                        cast_error = 1;    // cast device connection error flag
                                                        connectedscreen(); // display connection status at boot
                                                        tft.setTextDatum(MC_DATUM);
                                                        x = tft.width()/2;
                                                        return;
                                                      }
            DEBUG_PORT.print("Cast device found(");
            DEBUG_PORT.print(ghn.getIPAddress());
            DEBUG_PORT.print(":");
            DEBUG_PORT.print(ghn.getPort());
            DEBUG_PORT.println(")");
            if(!strcmp(language, "hu")) ghn.notify("Konassist csatlakozva.");
            else if(!strcmp(language, "ro")) ghn.notify("Konassist conectat.");
            else ghn.notify("Konassist connected."); 
            DEBUG_PORT.println("Konassist connected");
            */
           
            connectedscreen();
            tft.setTextDatum(MC_DATUM);
            x = tft.width()/2;

            // reset charging indicators after reboot
            ACcharge_led.off();
            DCcharge_led.off();
            bridge1.virtualWrite(V106, 0);  // Send to CarAssistant
            bridge2.virtualWrite(V106, 0);  // Home device           
            
}

//========================================== END Setup



// ===================================================================
// =============== BEGIN LOOP ========================================

void loop() {
              // put your main code here, to run repeatedly:
                            
              buttonA.loop();
              buttonB.loop();
/*            // TTS notifications
              if(car_status_flag) car_status();
              else if(car_battery_flag) car_battery();
              else if(cell_voltage_flag) cell_voltage(); 
              else if(home_temphumi_flag) home_temphumi();
*/              
              if (millis() < currentMillis) {
                                              currentMillis = 0;
                                            }
              
              currentMillis = millis();

              if (currentMillis - previousMillis >= interval) {   // 100ms timer
                                                                  // save the last time
                                                                  previousMillis = currentMillis;

                                                                  // Read Datas from OBD adapter
                                                                  ms_counter++;
                                                                  if(ms_counter >= sec_interval) {  // every 1 second
                                                                                                    
                                                                                                    ms_counter = 0;
                                                                                                    
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


void read_rawdata(){
                       // move data from OBD to Rawdata array
                       char rawData[myELM327.recBytes];
                       int n = 0;
                                                                            
                       //DEBUG_PORT.print("Payload received: ");
                       
                       for (int i=0; i<myELM327.recBytes; i++) {
                                                                  rawData[n++] = myELM327.payload[i];  
                                                                  //DEBUG_PORT.print(myELM327.payload[i]); // Serial print OBD Rawdata
                                                               }
                       //DEBUG_PORT.println(); 
                                                                        
                       parse(rawData);  // parse data received from OBD
                       
                       
  }

// -------------------------------- Read Datas from sensors
void read_data(){
      
          sec_counter++;        
   
          switch (sec_counter){       
                                     case 1: 
                                                     myELM327.sendCommand("AT SH 7E4");       // Set Header BMS 
                                                     
                                                     if (myELM327.queryPID("220101")) {      // BMS PID = hex 22 0101 => dec 34, 257
                                                  
                                                                                                           read_rawdata(); 

                                                                                                           CURbyte1 = convertToInt(dataFrame2, 0, 1);  // Batt Discharge from 0 > 255, Batt charge from 255 > 0 
                                                                                                           CURbyte2 = convertToInt(dataFrame2, 2, 3);  // when discharge current value from 0 > 255, when charge current value from 255 > 0 
                                                                                                           if(CURbyte1 > 200) BATTcur = ((255 - CURbyte1)*255 + (255 - CURbyte2))/10.0; // charge current
                                                                                                           else BATTcur = (CURbyte1*255 + CURbyte2)/10.0;       // discharge current
                                                                                                           
                                                                                                           BATTv = ((convertToInt(dataFrame2, 4, 5)<<8) + convertToInt(dataFrame2, 6, 7))/10.0;  
                                                                                                           BATTMAXtemp = convertToInt(dataFrame2, 8, 9);
                                                                                                           if(BATTMAXtemp > 200) BATTMAXtemp -= 255;
                                                                                                           BATTMINtemp = convertToInt(dataFrame2, 10, 11);
                                                                                                           if(BATTMINtemp > 200) BATTMINtemp -= 255;
                                                                                                           BATTtemp1 = convertToInt(dataFrame2, 12, 13);
                                                                                                           BATTtemp2 = convertToInt(dataFrame3, 0, 1);
                                                                                                           BATTtemp3 = convertToInt(dataFrame3, 2, 3);
                                                                                                           BATTtemp4 = convertToInt(dataFrame3, 4, 5);
                                                                                                           BATTINtemp = convertToInt(dataFrame3, 10, 11);
                                                                                                           if (BATTINtemp > 200) BATTINtemp -= 255;
                                                                                                           MAXcellv = convertToInt(dataFrame3, 12, 13)/50.0;
                                                                                                           MAXcellvno = convertToInt(dataFrame4, 0, 1);
                                                                                                           MINcellv = convertToInt(dataFrame4, 2, 3)/50.0;
                                                                                                           MINcellvno = convertToInt(dataFrame4, 4, 5);
                                                                                                           BATTFANspeed = convertToInt(dataFrame4, 8, 9);
                                                                                                           AUXBATTv = convertToInt(dataFrame4, 10, 11)*0.1;
                                                                                                           SOCreal = convertToInt(dataFrame1, 2, 3)/2;       // Real State Of Charge from BMS
                                                                                                           BMSrelay = convertToInt(dataFrame1, 12, 13);        // BMS Relay
                                                                                                           CECvalue = ((convertToInt(dataFrame6, 0, 1)<<24) + (convertToInt(dataFrame6, 2, 3)<<16) + (convertToInt(dataFrame6, 4, 5)<<8) + convertToInt(dataFrame6, 6, 7))/10.0;
                                                                                                           CEDvalue = ((convertToInt(dataFrame6, 8, 9)<<24) + (convertToInt(dataFrame6, 10, 11)<<16) + (convertToInt(dataFrame6, 12, 13)<<8) + convertToInt(dataFrame7, 0, 1))/10.0;
                                                                                                           OPtimehours = ((convertToInt(dataFrame7, 2, 3)<<24) + (convertToInt(dataFrame7, 4, 5)<<16) + (convertToInt(dataFrame7, 6, 7)<<8) + convertToInt(dataFrame7, 8, 9))/3600;
                                                                                                          
                                                                                                           BATTpow = (BATTv * BATTcur)/1000.0;                                 // Energy Draw/Charge (kW)
                                                                                                           
                                                                                                           if (BATTtemp1 > 200) BATTtemp1 -= 255; // negativ temperature
                                                                                                           if (BATTtemp2 > 200) BATTtemp2 -= 255;
                                                                                                           if (BATTtemp3 > 200) BATTtemp3 -= 255;
                                                                                                           if (BATTtemp4 > 200) BATTtemp4 -= 255;
                                                                                                           BATTtemp =  (BATTtemp1 + BATTtemp2 + BATTtemp3 + BATTtemp4)/4.0;  // Average BATT temp.
                                                                                                           
                                                                                                           if(!CECvalue){  // until IGNition/BMS relay is OFF the CEC and CED values are 0
                                                                                                                           TRIP1ch = 0;
                                                                                                                           TRIP1dch = 0;
                                                                                                                           TRIP1used = 0;
                                                                                                                           TRIP1avg = 0;   
                                                                                                                         }
                                                                                                           else {    
                                                                                                                     TRIP1ch  = CECvalue - CECvalue_buffer;   // Trip 1 Charged Energy (kWh)
                                                                                                                     TRIP1dch = CEDvalue - CEDvalue_buffer;   // Trip 1 Discharged Energy (kWh)
                                                                                                                     TRIP1used = TRIP1dch - TRIP1ch;          // Trip 1 Energy used 
                                                                                                                     TRIP1avg = TRIP1used*100.0 / TRIP1odo;  //  Trip 1 >>> kWh/100km  
                                                                                                                     
                                                                                                                }              

                                                                                                             DEBUG_PRINTLN(BATTcur);
                                                                                                             DEBUG_PRINTLN(BATTv);
                                                                                                             DEBUG_PRINTLN(BATTMAXtemp);
                                                                                                             DEBUG_PRINTLN(BATTMINtemp);
                                                                                                             DEBUG_PRINTLN(BATTtemp1);
                                                                                                             DEBUG_PRINTLN(BATTtemp2);
                                                                                                             DEBUG_PRINTLN(BATTtemp3);
                                                                                                             DEBUG_PRINTLN(BATTtemp4);
                                                                                                             DEBUG_PRINTLN(BATTINtemp);
                                                                                                             DEBUG_PRINTLN(MAXcellv);
                                                                                                             DEBUG_PRINTLN(MAXcellvno); 
                                                                                                             DEBUG_PRINTLN(MINcellv);
                                                                                                             DEBUG_PRINTLN(MINcellvno);
                                                                                                             DEBUG_PRINTLN(BATTFANspeed);
                                                                                                             DEBUG_PRINTLN(AUXBATTv);                                                                                           
                                                                                                             DEBUG_PRINTLN(SOCreal);
                                                                                                             DEBUG_PRINTLN(BMSrelay);
                                                                                                             DEBUG_PRINTLN(CECvalue);
                                                                                                             DEBUG_PRINTLN(CEDvalue);
                                                                                                             DEBUG_PRINTLN(OPtimehours);
                                                                                                             DEBUG_PRINTLN(BATTpow);
                                                                                                             DEBUG_PRINTLN(BATTtemp);
                                                                                                             DEBUG_PRINTLN(TRIP1ch);
                                                                                                             DEBUG_PRINTLN(TRIP1dch);
                                                                                                             DEBUG_PRINTLN("");

                                                                                                             DEBUG_PRINTLN(dataFrame0);
                                                                                                             DEBUG_PRINTLN(dataFrame1);
                                                                                                             DEBUG_PRINTLN(dataFrame2);
                                                                                                             DEBUG_PRINTLN(dataFrame3);
                                                                                                             DEBUG_PRINTLN(dataFrame4);
                                                                                                             DEBUG_PRINTLN(dataFrame5);
                                                                                                             DEBUG_PRINTLN(dataFrame6); 
                                                                                                             DEBUG_PRINTLN(dataFrame7);
                                                                                                             DEBUG_PRINTLN(dataFrame8);
                                                                                                             DEBUG_PRINTLN("");
                                                                                                             
                                                                                                           clearData();
                                                                                                        }
                                                  break;                                              
                                     case 2:
                                                      myELM327.sendCommand("AT SH 7E4");     // Set Header BMS 
                                                      
                                                      if (myELM327.queryPID("220105")) {      // BMS PID = hex 22 0105 => dec 34, 261
                                                                                            read_rawdata();

                                                                                            VOLTdiff = convertToInt(dataFrame3, 6, 7)/50;  // Voltage difference
                                                                                            HEATtemp1 = convertToInt(dataFrame3, 12, 13);  // Heater 1 temperature
                                                                                            if(HEATtemp1 > 200) HEATtemp1 -= 255;
                                                                                            //HEATtemp2 = convertToInt(dataFrame4, 0, 1);    // Heater 2 temperature
                                                                                            //if(HEATtemp2 > 200) HEATtemp2 -= 255;
                                                                                            SOHpercent = ((convertToInt(dataFrame4, 2, 3)<<8) + convertToInt(dataFrame4, 4, 5)) / 10; // State Of Health %
                                                                                            SOCpercent = convertToInt(dataFrame5, 0, 1)/2;  // State Of Charge Displayed
                                                                                            
                                                                                            DEBUG_PRINTLN(VOLTdiff);
                                                                                            DEBUG_PRINTLN(HEATtemp1);
                                                                                            //DEBUG_PRINTLN(HEATtemp2);
                                                                                            DEBUG_PRINTLN(SOHpercent);
                                                                                            DEBUG_PRINTLN(SOCpercent);
                                                                                            DEBUG_PRINTLN("");

                                                                                            DEBUG_PRINTLN(dataFrame0);
                                                                                            DEBUG_PRINTLN(dataFrame1);
                                                                                            DEBUG_PRINTLN(dataFrame2);
                                                                                            DEBUG_PRINTLN(dataFrame3);
                                                                                            DEBUG_PRINTLN(dataFrame4);
                                                                                            DEBUG_PRINTLN(dataFrame5);
                                                                                            DEBUG_PRINTLN(dataFrame6); 
                                                                                            DEBUG_PRINTLN(dataFrame7);
                                                                                            DEBUG_PRINTLN(dataFrame8);
                                                                                            DEBUG_PRINTLN("");
                                                                                             
                                                                                            clearData();
                                                                                       }
                                                  break;  

                                     case 3:
                                                      myELM327.sendCommand("AT SH 7E4");     // Set Header BMS 
                                                      
                                                      if (myELM327.queryPID("220106")) {      // BMS PID = hex 22 0106 => dec 34, 262
                                                                                            read_rawdata();
                                                                                           
                                                                                            COOLtemp = convertToInt(dataFrame1, 2, 3);  // Cooling water temperature
                                                                                            if(COOLtemp > 200) COOLtemp -= 255;
                                                                                            
                                                                                            DEBUG_PRINTLN(COOLtemp);
                                                                                            DEBUG_PRINTLN("");

                                                                                            DEBUG_PRINTLN(dataFrame0);
                                                                                            DEBUG_PRINTLN(dataFrame1);
                                                                                            DEBUG_PRINTLN(dataFrame2);
                                                                                            DEBUG_PRINTLN(dataFrame3);
                                                                                            DEBUG_PRINTLN(dataFrame4);
                                                                                            DEBUG_PRINTLN(dataFrame5);
                                                                                            DEBUG_PRINTLN(dataFrame6); 
                                                                                            DEBUG_PRINTLN(dataFrame7);
                                                                                            DEBUG_PRINTLN(dataFrame8);
                                                                                            DEBUG_PRINTLN("");
                                                                                                                                                              
                                                                                            clearData();
                                                                                       }                                    
                                                   break;

                                     case 4:          
                                                      myELM327.sendCommand("AT SH 7B3");      // Set Header Aircon
                                                          
                                                      if (myELM327.queryPID("220100")) {      // Aircon PID = hex 22 0100 => dec 34, 256
                                                                                            read_rawdata();
                                                                                            
                                                                                            INDOORtemp = (convertToInt(dataFrame1, 4, 5)/2)-40;  // Indoor temperature
                                                                                            OUTDOORtemp = (convertToInt(dataFrame1, 6, 7)/2)-40;  // Outdoor temperature
                                                                                            SPEED = convertToInt(dataFrame4, 10, 11)/1.609; // Vehicle speed
                                                                                            
                                                                                            DEBUG_PRINTLN(INDOORtemp);
                                                                                            DEBUG_PRINTLN(OUTDOORtemp);
                                                                                            DEBUG_PRINTLN(SPEED);
                                                                                            DEBUG_PRINTLN("");

                                                                                            DEBUG_PRINTLN(dataFrame0);
                                                                                            DEBUG_PRINTLN(dataFrame1);
                                                                                            DEBUG_PRINTLN(dataFrame2);
                                                                                            DEBUG_PRINTLN(dataFrame3);
                                                                                            DEBUG_PRINTLN(dataFrame4);
                                                                                            DEBUG_PRINTLN(dataFrame5);
                                                                                            DEBUG_PRINTLN(dataFrame6); 
                                                                                            DEBUG_PRINTLN(dataFrame7);
                                                                                            DEBUG_PRINTLN(dataFrame8);
                                                                                            DEBUG_PRINTLN("");
                                                                                            
                                                                                            clearData();
                                                                                       }  
                                                    break;

                                     case 5:               
                                                      
                                                      myELM327.sendCommand("AT SH 7E2");    // Set Header Vehicle Control Unit
                                                      
                                                      if (myELM327.queryPID("2102")) {      // Vehicle Control Unit PID = hex 21 02 => dec 33, 2

                                                                                            read_rawdata();
                          
                                                                                            //??? AUXBATTv = ((convertToInt(substr(dataFrame3, 4, 5))<<8) + convertToInt(substr(dataFrame3, 2, 3)))/1000.0;       // Auxiliary battery voltage
                                                                                           
                                                                                            AUXBATTcur = (32700 - ((convertToInt(dataFrame3, 6, 7)<<8) + convertToInt(dataFrame3, 8, 9)))/100.0;  // Auxiliary battery current
                                                                                            AUXBATTsoc = convertToInt(dataFrame3, 10, 11);  // Auxiliary battery state of charge
                                                                                            
                                                                                            DEBUG_PRINTLN(AUXBATTcur);
                                                                                            DEBUG_PRINTLN(AUXBATTsoc);
                                                                                            DEBUG_PRINTLN("");

                                                                                            DEBUG_PRINTLN(dataFrame0);
                                                                                            DEBUG_PRINTLN(dataFrame1);
                                                                                            DEBUG_PRINTLN(dataFrame2);
                                                                                            DEBUG_PRINTLN(dataFrame3);
                                                                                            DEBUG_PRINTLN(dataFrame4);
                                                                                            DEBUG_PRINTLN(dataFrame5);
                                                                                            DEBUG_PRINTLN(dataFrame6); 
                                                                                            DEBUG_PRINTLN(dataFrame7);
                                                                                            DEBUG_PRINTLN(dataFrame8);
                                                                                            DEBUG_PRINTLN("");
                                                                                                                                                                                                                               
                                                                                            clearData();
                                                                                       }  
                                                    break;

                                     case 6:               
                                                      
                                                      myELM327.sendCommand("AT SH 7C6");      // Set Header CLU Cluster Module
                                                      
                                                      if (myELM327.queryPID("22B002")) {      // CLU Cluster Module PID = hex 22 B002 => dec 34, 45058
                                                                                            read_rawdata();
                          
                                                                                            ODOkm = ((convertToInt(dataFrame1, 6, 7)<<16) + (convertToInt(dataFrame1, 8, 9)<<8) + (convertToInt(dataFrame1, 10, 11)));  // ODOmeter in km
                                                                                            
                                                                                            TRIP1odo  = ODOkm - ODOkm_buffer;   // Trip 1 Distance (km)
                                                                                             
                                                                                            DEBUG_PRINTLN(ODOkm);
                                                                                            DEBUG_PRINTLN(ODOkm_buffer);
                                                                                            DEBUG_PRINTLN(TRIP1odo);
                                                                                            DEBUG_PRINTLN("");

                                                                                            DEBUG_PRINTLN(dataFrame0);
                                                                                            DEBUG_PRINTLN(dataFrame1);
                                                                                            DEBUG_PRINTLN(dataFrame2);
                                                                                            DEBUG_PRINTLN(dataFrame3);
                                                                                            DEBUG_PRINTLN(dataFrame4);
                                                                                            DEBUG_PRINTLN(dataFrame5);
                                                                                            DEBUG_PRINTLN(dataFrame6); 
                                                                                            DEBUG_PRINTLN(dataFrame7);
                                                                                            DEBUG_PRINTLN(dataFrame8);  
                                                                                            DEBUG_PRINTLN("");
                                                                                                                                
                                                                                            clearData();
                                                                                       } 
                                                    break;

                                     case 7:                
                                                      
                                                      myELM327.sendCommand("AT SH 7A0");       // Set Header BCM
                                                      
                                                      if (myELM327.queryPID("22C00B")) {      // BCM Body Control Module PID = hex 22 C00B => dec 34, 49163
                                                                                            read_rawdata();
                                                                                            
                                                                                            FLTpress = (convertToInt(dataFrame1, 2, 3)/5.0)/14.504;    // FrontLeft tyre press PSI / 14.504 = bar
                                                                                            FLTtemp = convertToInt(dataFrame1, 4, 5)-50;               // FrontLeft tyre temperature
                                                                                            FRTpress = (convertToInt(dataFrame1, 10, 11)/5.0)/14.504;  // FrontRight tyre press PSI / 14.504 = bar
                                                                                            FRTtemp = convertToInt(dataFrame1, 12, 13)-50;             // FrontRight tyre temperature
                                                                                            
                                                                                            RRTpress = (convertToInt(dataFrame2, 4, 5)/5.0)/14.504;    // RearRight tyre press PSI / 14.504 = bar
                                                                                            RRTtemp = convertToInt(dataFrame2, 6, 7)-50;               // RearRight tyre temperature
                                                                                            RLTpress = (convertToInt(dataFrame2, 12, 13)/5.0)/14.504;  // RearLeft tyre press PSI / 14.504 = bar
                                                                                            RLTtemp = convertToInt(dataFrame3, 0, 1)-50;               // RearLeft tyre temperature
                                                                                            
                                                                                            DEBUG_PRINTLN(FLTpress);
                                                                                            DEBUG_PRINTLN(FLTtemp);
                                                                                            DEBUG_PRINTLN(FRTpress);
                                                                                            DEBUG_PRINTLN(FRTtemp);
                                                                                            DEBUG_PRINTLN("");
                                                                                            DEBUG_PRINTLN(RRTpress);
                                                                                            DEBUG_PRINTLN(RRTtemp);
                                                                                            DEBUG_PRINTLN(RLTpress);
                                                                                            DEBUG_PRINTLN(RLTtemp);
                                                                                            DEBUG_PRINTLN("");
                                                                                           
                                                                                           DEBUG_PRINTLN(dataFrame0);
                                                                                           DEBUG_PRINTLN(dataFrame1);
                                                                                           DEBUG_PRINTLN(dataFrame2);
                                                                                           DEBUG_PRINTLN(dataFrame3);
                                                                                           DEBUG_PRINTLN(dataFrame4);
                                                                                           DEBUG_PRINTLN(dataFrame5);
                                                                                           DEBUG_PRINTLN(dataFrame6); 
                                                                                           DEBUG_PRINTLN(dataFrame7);
                                                                                           DEBUG_PRINTLN(dataFrame8);
                                                                                           DEBUG_PRINTLN("");
                                                                                                                                                                                                                        
                                                                                           clearData();
                                                                                       } 
                                                    break;

                                     case 8:                 
                                                      
                                                      myELM327.sendCommand("AT SH 7E5");    // Set Header OBC
                                                      
                                                      if (myELM327.queryPID("2103")) {      // OBC Onboard Battery Chargers PID = hex 21 03 => dec 33, 3
                                                                                            read_rawdata();
                          
                                                                                            
                                                                                            ACCcur = ((convertToInt(dataFrame1, 0, 1)<<8) + convertToInt(dataFrame1, 2, 3))/100.0;  // AC charge current
                                                                                            CHARGE = convertToInt(dataFrame1, 4, 5);  // ACDC charge indicator
                                                                                            
                                                                                            if(CHARGE == 20) {
                                                                                                                if(check_delay < 4) check_delay++;
                                                                                                                else ACcharging = 1;
                                                                                                                
                                                                                                             }
                                                                                            else if (CHARGE == 13) {
                                                                                                                       if(check_delay < 4) check_delay++;
                                                                                                                       else DCcharging = 1;
                                                                                                                   }
                                                                                            else if (ACcharging || DCcharging){
                                                                                                                                               ACcharging = 0;
                                                                                                                                               DCcharging = 0;
                                                                                                                                               check_delay = 0;
                                                                                                                                               send_once = 0;
                                                                                                                                               ACcharge_led.off();
                                                                                                                                               DCcharge_led.off();
                                                                                                                                               bridge1.virtualWrite(V106, 0);  // Send to CarAssistant
                                                                                                                                               bridge2.virtualWrite(V106, 0);  // Home device
                                                                                                                               }  
                                                                                            
                                                                                            DEBUG_PRINTLN(ACCcur);
                                                                                            DEBUG_PRINTLN(CHARGE);
                                                                                            DEBUG_PRINTLN("");
                                                                                           
                                                                                            DEBUG_PRINTLN(dataFrame0);
                                                                                            DEBUG_PRINTLN(dataFrame1);
                                                                                            DEBUG_PRINTLN(dataFrame2);
                                                                                            DEBUG_PRINTLN(dataFrame3);
                                                                                            DEBUG_PRINTLN(dataFrame4);
                                                                                            DEBUG_PRINTLN(dataFrame5);
                                                                                            DEBUG_PRINTLN(dataFrame6); 
                                                                                            DEBUG_PRINTLN(dataFrame7);
                                                                                            DEBUG_PRINTLN(dataFrame8);
                                                                                            DEBUG_PRINTLN("");
                                                                                                                                                                                                                                                                                                                       
                                                                                            clearData();
                                                                                       } 
                                                      alldata_ready = 1;
                                                      sec_counter = 0;
                                                    break;                                                                    

                      
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

//--------------------------------- Check Button Longpress
void longpress(Button2& btn) {  // Reset Trip Counter
                                unsigned int time = btn.wasPressedFor();
                                if (time > 2000) {
                                                   reset_trip = 1;
                                                }
}
//--------------------------------- END Check Button Longpress

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
                                 tft.println("WiFi-BT");
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


//--------------------------------- Display Page 1
void page1(){   
                init_background(); // init background image
                
                SOC(1);              // display SOC on Top
                BATTtemperature(0);  // display BATTtemperature on bottom
                
                
}
//--------------------------------- END Display Page 1

//--------------------------------- Display Page 2
void page2(){   
                init_background(); // init background image
                
                INtemperature(1);    // display Indoor temp. on Top
                OUTtemperature(0);   // display Outdoor temp. on bottom
                
}
//--------------------------------- END Display Page 2

//--------------------------------- Display Page 3
void page3(){

                init_background(); // init background image
 
                BATTpower(1);      // Main Battery - Energy Draw
                BATTvolts(0);      // Main Battery Voltage
                
}
//--------------------------------- END Display Page 3

//--------------------------------- Display Page 4
void page4(){
                init_background(); // init background image
                
                AUXSOC(1);         // AUX BAttery SOC 
                AUXBATTvolts(0);   // AUX Battery Voltage
                
}
//--------------------------------- END Display Page 4

//--------------------------------- Display Page 5
void page5(){
                init_background(); // init background image
                
                HOMEtemperature(1);  // display Home Temperature
                HOMEhumidity(0);     // display Home Humidity
                
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
                                                                 tft.drawNumber( INDOORtemp, x, topdraw, 6); // x = center
                                                           }
                                                else       { // bottom = 0
                                                                 tft.drawString("    In TempC  ", 0, bottomtext, 4);
                                                                 draw_normalbox_bottom();
                                                                 tft.setTextPadding( tft.textWidth("888", 6) );
                                                                 tft.drawNumber( INDOORtemp, x, bottomdraw, 6); // x = center
                                                           }   
                                   
}

// Outdoor Temperature
void OUTtemperature(boolean topbottom){
                                             
                  
                                           tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                           if (topbottom) { // top = 1
                                                               tft.drawString(" Out TempC", 0, toptext, 4);
                                                               draw_normalbox_top();
                                                               tft.setTextPadding( tft.textWidth("888", 6) );
                                                               tft.drawNumber( OUTDOORtemp, x, topdraw, 6); // x = center
                                                          }
                                           else           { // bottom = 0
                                                               tft.drawString(" Out TempC", 0, bottomtext, 4);
                                                               draw_normalbox_bottom();
                                                               tft.setTextPadding( tft.textWidth("888", 6) );
                                                               tft.drawNumber( OUTDOORtemp, x, bottomdraw, 6); // x = center
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
                                                               tft.drawNumber( BATTv, x, topdraw, 6); // x = center
                                                          }
                                           else           { // bottom = 0
                                                               tft.drawString(" MainBatt  V", 0, bottomtext, 4);
                                                               if(BATTv < BATTv_LOW_Warning || BATTv > BATTv_HIGH_Warning) draw_warningbox_bottom();
                                                               else  draw_normalbox_bottom();
                                                               tft.setTextPadding( tft.textWidth("888", 6) );
                                                               tft.drawNumber( BATTv, x, bottomdraw, 6); // x = center
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
                                                                 tft.setTextPadding( tft.textWidth("888.8", 6) );
                                                                 tft.drawFloat( BATTcur, 1, x, topdraw, 6); // x = center
                                                           }
                                                else       { // bottom = 0
                                                                 tft.drawString("BattCur A", 0, bottomtext, 4);
                                                                 draw_normalbox_bottom();
                                                                 tft.setTextPadding( tft.textWidth("888.8", 6) );
                                                                 tft.drawFloat( BATTcur, 1, x, bottomdraw, 6); // x = center
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
                                                                 tft.drawNumber( SOHpercent, x, topdraw, 6); // x = center
                                                           }
                                                else       { // bottom = 0
                                                                 tft.drawString("         SOH   %     ", 0, bottomtext, 4);
                                                                 draw_normalbox_bottom();
                                                                 tft.setTextPadding( tft.textWidth("888", 6) );
                                                                 tft.drawNumber( SOHpercent, x, bottomdraw, 6); // x = center
                                                           }   
                                   
}


// Operating time
void OPtime(boolean topbottom){
                                            

                                            tft.setTextColor(TFT_WHITE,TFT_BLUE);
                                            if (topbottom) { // top = 1
                                                                 tft.drawString("      Op. Time H ", 0, toptext, 4);
                                                                 draw_normalbox_top();
                                                                 tft.setTextPadding( tft.textWidth("888888", 4) );
                                                                 tft.drawNumber( OPtimehours, x, topdraw, 4); // x = center
                                                           }
                                                else       { // bottom = 0
                                                                 tft.drawString("      Op. Time H ", 0, bottomtext, 4);
                                                                 draw_normalbox_bottom();
                                                                 tft.setTextPadding( tft.textWidth("888888", 4) );
                                                                 tft.drawNumber( OPtimehours, x, bottomdraw, 4); // x = center
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



              
