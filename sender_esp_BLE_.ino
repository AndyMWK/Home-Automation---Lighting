//All libraries downloaded from the Arduino IDE
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

//---IR Receiver Initialization
const int IR = 14;
static IRrecv irReceiver(IR);
static decode_results results;

//---Adopted from example code to set up BLE Device---
static BLEServer* pServer = NULL;
static BLECharacteristic* pCharacteristic = NULL;

static bool deviceConnected = false;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

//---LED Pin Definition---
#define LED_PIN 2

void setup() {
  //---Serial Monitor Initialization---
  Serial.begin(115200);

  //----Adopted from example code-----
  //Creates the BLE Device
  BLEDevice::init("ESP32");

  //Creates the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks()); 
  
  //Creates the BLE Service
  BLEService* pService = pServer->createService(SERVICE_UUID);

  //Creates the BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                                CHARACTERISTIC_UUID, 
                                BLECharacteristic::PROPERTY_READ    | 
                                BLECharacteristic::PROPERTY_WRITE   |
                                BLECharacteristic::PROPERTY_NOTIFY  |
                                BLECharacteristic::PROPERTY_INDICATE
                              );

  //Create the BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  //start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  //Serial.println("Waiting a client connection to notify...");

  //---Infrared Receiver Initialization---
  irReceiver.enableIRIn();
  //---LED Pin Initialization
  pinMode(LED_PIN, OUTPUT);
}

//---Function Declaration---
//This function will send the received infrared signal through BLE
void sendSig(decode_results &results);


void loop() {
  //Check for any received infrared signals
  if(irReceiver.decode(&results)) {

    //Make sure that the BLE devices are connected
    while(!deviceConnected) {
      pServer->startAdvertising();
    } 

    //Send information once its been confirmed that the BLE devices are connected
    if(deviceConnected) {
      sendSig(results);
    }

    //Add a delay for the hardware to get ready to receive infrared signal again
    delay(1000);
    irReceiver.resume();
  }
}

//---Function Definition---
void sendSig(decode_results &results) {
    
  //convert the received signal information to hexadecimal and send!
  char hexString[17];  
  sprintf(hexString, "%016llX", results.value);
  pCharacteristic->setValue(hexString);
  pCharacteristic->notify();

  //---used for debugging---
  //serialPrintUint64(results.value, HEX);

  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
}
