
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>


const int IR = 14;
static IRrecv irReceiver(IR);
static decode_results results;


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


void setup() {
  // connect to the wi-fi network
  Serial.begin(115200);
  
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

  //infrared receiver
  irReceiver.enableIRIn();

  pinMode(2, OUTPUT);
}


void sendSig(decode_results &results);


void loop() {

  // if (deviceConnected) {
  //       pCharacteristic->setValue((uint8_t*)&value, 4);
  //       pCharacteristic->notify();
  //       value++;
  //       delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
  //       Serial.println("Value Sent");
  //   }
  //   // disconnecting
  //   if (!deviceConnected && oldDeviceConnected) {
  //       delay(500); // give the bluetooth stack the chance to get things ready
  //       pServer->startAdvertising(); // restart advertising
  //       Serial.println("start advertising");
  //       oldDeviceConnected = deviceConnected;
  //   }
  //   // connecting
  //   if (deviceConnected && !oldDeviceConnected) {
  //       // do stuff here on connecting
  //       oldDeviceConnected = deviceConnected;
  //   }
  if(irReceiver.decode(&results)) {

    
    while(!deviceConnected) {
      pServer->startAdvertising();
      
      
    } 
    
    if(deviceConnected) {

      sendSig(results);
      
    }

    delay(1000);
    irReceiver.resume();
  }
}

void sendSig(decode_results &results) {

  char hexString[17];  
  sprintf(hexString, "%016llX", results.value);
  pCharacteristic->setValue(hexString);
  pCharacteristic->notify();
  //serialPrintUint64(results.value, HEX);

  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
}