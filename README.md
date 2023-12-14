# Konassist
**Hyundai Kona EV - DIY OBD Assistant**

Here is a short video about this project: https://youtu.be/WdHU3YUXCjA
 - Hungarian version: https://youtu.be/mUl9IHsN-ao
 - Romanian version: https://youtu.be/QUVihgGiViE 

**Devices used in this project (no connection to sellers):**

**Full version:**

1) – 3x TTGO ESP32 TFT boards (Lilygo)
 - a) 2x boards needed in the car – 1 for BT OBD adapter and 1 for Google TTS (Optional, Text To Speech service)
 - b) 1x board at Home (Optional) + AM2301 temperature and humidity sensor, connected to GPIO27 / 5V / GND
2) – 1x ESP8266 – Wemos D1 mini (or any other ESP8266 board) for boiler control (Optional) + DS18B20 temperature sensor, connected to GPIO13 / 3V3 / GND
[Board pinout](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/)

**Standalone version (without Assistant):** Needs only 1x TTGO ESP32 TFT board, in the car.

3) – [1x Vgate iCar PRO BT4.0 – Bluetooth OBD adapter](https://www.amazon.com/Vgate-Bluetooth-Scanner-Android-Compatible/dp/B06XGB4873) (be careful with the cheap Clones, the OBD/BT communication isn’t stable and there are some missing components on the PCB. First sign, missing capacitors around the 40MHz oscillator on the BT module [on top]). The above link was shared just for the pictures, to indentify the OBD BT device.
- Default OBD BT Device Name: “Android-Vlink”
- Default PIN: “1234”

4) There is only one active USB port in the Kona (top) when the car is Closed and charging from AC/DC. You will need (build) a special cable (USB A male to USB C and USB A female, we need only 5V power supply) for powering the ESP32 device and 4G USB WiFi modem OR just use a small [USB Hub](https://www.tellur.com/product/usb-3-0-hub-with-4-usb-3-0-ports/) instead of this special cable.

5) Android Apps:
- a) Blynk – Scan/Use the provided QR code (Full or Standalone version) and this project will appear in your Blynk app (you need to buy “Energy”).
- b) CastReceiver app for GAssistant broadcast (paid version). Not needed for Standalone version.
- c) IFTTT account for voice commands (Google Assistant and Webhooks modules needed). IFTTT now needs monthly subscription. Not needed for Standalone version.


Before you use / download this software consider the following: 
you are interfering with your car and doing that with hardware and software beyond your control.
Any car is a possibly lethal piece of machinery and you might hurt or kill yourself or others using it, 
or even paying attention to the displays instead of watching the road. 
Be extremely prudent!

6) Create WiFi hotspot, using your smartphone or any 4G USB WiFi modem (Set SSID ex: Konassist, Password: konapass).

7) [Plastic case for TTGO boards](https://www.thingiverse.com/thing:4183337)

**How it works:**
- The ESP32 OBD device will connect to the OBD BT adapter over Bluetooth and WiFi hotspot.
- This will send Read commands periodically (only 1 command / sec) to the OBD BT adapter (we are not sending Write commands to the car CAN bus).
- The received datas, will be parsed and after doing some math, the results will be displayed locally on the ESP32 OBD device TFT screen and in the same time, all values will be sent to the Blynk Cloud Server (every 2 seconds, in small packets).
- The Konassist Blynk app on your phone and all ESP32 Car TTS / Home / ESP8266 Boiler devices, will receive those new values from Blynk Cloud Server in real time.

**Here is what’s happening when you are issue a voice command:**
Your Google Assistant / Google Home device will send to Google / analyze your voice command and will try to find the trigger words (applets) defined on IFTTT.
IFTTT will send a “web request” (URL) to Blynk Cloud Server, changing some values on different Virtual Pins.
Blynk Cloud Server will send this values to our ESP32 devices, which will call different functions on it.
The ESP32 device will send a predefined “Text”, to Google TTS (Text To Speech = Translate) online service.
Google TTS Service (translate) will respond with a link to an mp3 file.
This link will be sent back to your Google Assistant (CastReceiver app) / Google Home device (broadcast) by the ESP32 Home / TTS devices.
All this, in just a few seconds 🙂
- Note:
The GA / GHome devices must be connected, on the same WiFi network with the ESP32 device.
Unfortunately, right now, the Bluetooth and TTS libraries for ESP32, will not work at the same time on the same device. This is why we need a separate ESP32 device, only for TTS in our car.

**Steps:**
1) [Konassist Standalone version](https://github.com/cyberelectronics/Konassist/tree/main/Easy_Standalone_Uploader) (compiled .bin files), can be uploaded easily with ESP32 Flash Download Tool from Espressif.
Or you can Download the source code from [Github](https://github.com/cyberelectronics/Konassist) or from [here](https://drive.google.com/drive/folders/1UWpJNsDT1ZYAkPCkePzdeM-244Uaqx6i?usp=sharing) (with preconfigured Arduino 1.8.13 or sketches and libraries only)
Read the readme.txt file

2) Upload the sketches and the background picture for ESP32-TFT on each device, using Arduino (portable, preconfigured/preloaded version of Arduino provided, or use the sketches and libraries from Konassist_sketchbook.zip or Github, with your already installed Arduino version).
Path to sketches: Arduino-1.8.13\portable\sketchbook
The background image on the ESP32 boards, can be changed to any picture, just replace the “tiger.jpg” in the sketch “data” folder.
Do not rename the file and use pictures only with 135 x 240 resolution.
Upload the picture to the device from arduino:
Tools > ESP32 Sketch Data Upload

2) Keep pushed the Right Button (ESP32-TTGO-TFT Button A = GPIO35, for ESP8266 [Boiler] connect a pushbutton to GPIO12 and GND) while you restart / PowerON the device, until you see the background picture, now release the Right Button.

3) This will start the device in AP mode and you are able to access the device Setup page, just connect to the device with SSID: “Setup…” and Password: “konapass” for ESP32 boards and “boilerpass” for ESP8266 board (boiler control) and enter this IP in your web browser: 192.168.4.1

4) Select your home/car WiFi Network SSID, available from the list and enter the WiFi password in the Password field.

5) Copy paste the Device Tokens received from Blynk for each device (do not mix-up the device tokens)

6) Enter your Google Home / Assistant / CastReceiver app, Device Name: “Assistant” (default) for TTS (Text To Speech service).
Will work only if the GA device is connected to this Local Network/Hotspot and have the same Device Name.

7) “Save” and now the devices will try to connect to the selected WiFi hotspot and Blynk Cloud and should appear in your Blynk app.
Note: OBD BT adapter / WiFi hotspot and GA device (optional) must be turned ON/active, otherwise the OBD and TTS (optional) devices will keep restarting.

8) Define/Add voice commands to your IFTTT account (optional) using “Google Assistant” and “Webhooks” modules, to access and modify the “Virtual Pin” states on the Blynk server, using a link with Device Blynk Token (apikey) and “Virtual Pin” value.

**Obs:**
You can Reset the trip datas, from “Reset” button in the Konassist app, by voice command (“New Trip”) or longpress ButtonA (when Trip CH/DCH info are displayed).

Note:
To start using Konassist or Konassist Standalone app on your phone:
1) Download Blynk App: http://j.mp/blynk_Android or http://j.mp/blynk_iOS
2) Touch the QR-code icon (upper right corner) and point the camera to the code (provided in the package).
3) You need to buy Blynk “Energy” – for Konassist Full version (with GAssistant) we need around 19000 and for Konassist Standalone around 15000.
4) Go to Project Settings (Nut icon), click Email All – Auth Tokens
5) Click on the Play icon (upper right corner) to run the application.

You can Edit the Name of each module, add notifications and actions inside the app, just Stop the app from the Play button before.
Check the Eventor module on the EV Data 2 page:
Here you can add more notifications from Blynk app. You can create notifications and actions for anything (ex. Batt temp too high or turn ON/OFF a GPIO pin on devices).
On the “Home” tab, there are 3 buttons (bottom): ON/OFF Notif, Warning Notif and Boiler Temp (Save).
ON/OFF Notif – if it’s ON, will send notifications to your Google Home device at home, each time the car will be turned ON or OFF (after 5 minutes delay) and when it is connected to a DC/AC charger.
Warning Notif – if it’s ON, will send notifications to your Google Home device at home, each time the Main Battery Temperature is above the set limit (default 40°C) or the State Of Charge (SOC) percent is below the set limit (default 10%).
Boiler Temp – will Save the Set Boiler Temp(°C) value to the EEPROM of the ESP8266 (Boiler) device.

About IFTTT:
Create an account (only paid version allow you to create unlimited applets [voice commands]).
Create an applet:
1) Click on “Create”
2) Click on “If This”
3) Search and click on “Google Assistant” module
4) Select “Say a simple phrase” or “Say a phrase with a number” (depends by the use of your voice command)
5) Complete the fields and click “Create Trigger”
6) Click on “Then That”
7) Search and click on “Webhooks” > “Make a web request”
8) Enter the URL like in the example picture from this project (IFTTT_BatteryInfo_command_example.png) just complete with your Device Blynk Token (of the device where the command will be executed).
9) Select Method > “Get” and Content Type > “application/json”
10) Click on “Create action”

“Battery Info” URL example:
http://139.59.206.133/xxxxCarTTS_BlynkTokenxxxx/update/V102?value=2

Where “xxxxCarTTS_BlynkTokenxxxx” is the Blynk Token of your ESP32 TTS device in your car.
Replace with your Blynk Token sent to your email address by Blynk.
You received separate Blynk Token for each device.
V102 > Blynk Virtual Pin number, defined in the Blynk app and Arduino sketch file (do not change this).
value=2 > Value of the Virtual Pin for “Battery Info” command (do not change this).

**List of implemented Voice Commands** (the text can be changed to anything [quotes not needed], except the URL):
This can be extended, if you add more languages/commands/functions to “notifications” file, in Arduino.

**To ESP32 Car TTS (Text To Speech) device:**

- “Battery info” > http://139.59.206.133/xxxxCarTTS_BlynkTokenxxxx/update/V102?value=2
- “Cell voltage” > http://139.59.206.133/xxxxCarTTS_BlynkTokenxxxx/update/V102?value=3
- “My Home Temperature” > http://139.59.206.133/xxxxCarTTS_BlynkTokenxxxx/update/V102?value=4
- “Trip info” > http://139.59.206.133/xxxxCarTTS_BlynkTokenxxxx/update/V102?value=5
- “Car language Hungarian” > http://139.59.206.133/xxxxCarTTS_BlynkTokenxxxx/update/V101?value=1
- “Car language Romanian” > http://139.59.206.133/xxxxCarTTS_BlynkTokenxxxx/update/V101?value=2
- “Car language English” > http://139.59.206.133/xxxxCarTTS_BlynkTokenxxxx/update/V101?value=3

**To ESP32 Car OBD device:**

- “New Trip” > http://139.59.206.133/xxxxCar_BlynkTokenxxxx/update/V26?value=1
- “OBD Screen ON” > http://139.59.206.133/xxxxCar_BlynkTokenxxxx/update/V20?value=1
- “OBD Screen OFF” > http://139.59.206.133/xxxxCar_BlynkTokenxxxx/update/V20?value=0
- “OBD brightness #” > http://139.59.206.133/xxxxCar_BlynkTokenxxxx/update/V21?value= {{NumberField}}

**To ESP32 Home device:**

- “Car Status” > http://139.59.206.133/xxxxHome_BlynkTokenxxxx/update/V102?value=1
- “Car Battery” > http://139.59.206.133/xxxxHome_BlynkTokenxxxx/update/V102?value=2
- “Home Temperature” > http://139.59.206.133/xxxxHome_BlynkTokenxxxx/update/V102?value=4
- “Home trip info” > http://139.59.206.133/xxxxHome_BlynkTokenxxxx/update/V102?value=5
- “Boiler Temperature” > http://139.59.206.133/xxxxHome_BlynkTokenxxxx/update/V102?value=6
- “Home language Hungarian” > http://139.59.206.133/xxxxHome_BlynkTokenxxxx/update/V101?value=1
- “Home language Romanian” > http://139.59.206.133/xxxxHome_BlynkTokenxxxx/update/V101?value=2
- “Home language English” > http://139.59.206.133/xxxxHome_BlynkTokenxxxx/update/V101?value=3

**To ESP8266 Boiler control device:**

- “Boiler ON” > http://139.59.206.133/xxxxBoiler_BlynkTokenxxxx/update/V1?value=1
- “Boiler OFF” > http://139.59.206.133/xxxxBoiler_BlynkTokenxxxx/update/V1?value=0
- “Set Boiler temperature #” > http://139.59.206.133/xxxxBoiler_BlynkTokenxxxx/update/V2?value= {{NumberField}}
