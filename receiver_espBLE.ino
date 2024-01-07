//BLEDevice.h comes from a library that was downloaded through the Arduino IDE
#include "BLEDevice.h"
#include <Servo.h>

//---Pin Definitions---
const int servo1_pin = 4;
const int servo2_pin = 5;

//---Servo Object Creations---
Servo servo1;
Servo servo2;

//Variables used for BLE. Adopted Code. 
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID    charUUID("99d918d6-2f15-11ee-be56-0242ac120002");

static bool tryConnect = false;
static bool tryScan = false;
static bool connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);

    String receivedData = String(*pData);

    //I wrote this segment to control the servo motor to turn the light switch on and off. 
    /*
    
    --------------------------------------------------------------------------------------------------
    Explanation:
    When I press 1 on my TV Remote, the received signal in hexadecimal always contains a '1' at the end. 
    When I press 2 on my TV Remote, the received signal in hexadecimal always contsins a '2' at the end. 
    Therefore, I check for 1 or 2 from the end of the received data to control the servo motor
    --------------------------------------------------------------------------------------------------
    
    */

    //Lights ON
    if (receivedData.charAt(receivedData.length()-2) == '1') {
      servo1.write(0);
      delay(1000);
      servo1.write(60);

      Serial.println("on");

    }
      
    //Lights OFF
    else if (receivedData.charAt(receivedData.length()-2) == '2') {
        servo2.write(0);
        delay(1000);
        servo2.write(60);
        Serial.println("off");
    }
  }

//Adopted Code
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial.println("onConnect");
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

//Adopted Code
bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}

//Adopted Code
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {

  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // Serial.print("BLE Advertised Device Found");
    // Serial.println(advertisedDevice.toString().c_str());


    //when the server that we want is found: 
    if(advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      tryConnect = true;
      tryScan = true;
    } 
  }
};



void setup() {

  //---Serial Monitor Initalization
  Serial.begin(115200);
  
  //The code associated with BLE below is provided by the example library I was using
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  //---Servo Motor Initialization--- I wrote this part
  servo1.attach(servo1_pin);
  servo2.attach(servo2_pin);
  servo1.write(60);
  servo2.write(60);
}

//Adopted Code
void loop() {
  if (tryConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    tryConnect = false;
  }
    
  if(tryScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
}

