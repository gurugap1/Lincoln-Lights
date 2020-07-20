#include "Arduino.h"
#include <esp_now.h>
#include <WiFi.h>



// REPLACE WITH THE MAC Address of your receiver 
// uint8_t broadcastAddress[] = {0x4C, 0x11, 0xAE, 0xEA, 0xA8, 0x54};
uint8_t broadcastAddress[] = {0xC4, 0x4F, 0x33, 0x3E, 0xED, 0x1D};


// Define variables to store incoming readings
  int inc_array;
  bool inc_ctOn;
  int inc_ctP;
  bool inc_gradOn;
  int inc_gradP;
  bool inc_efxOn;
  int inc_efxP;
  uint8_t inc_hue;
  uint8_t inc_sat;
  uint8_t inc_lvl;
  uint8_t inc_ledGlobalLvl;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  int array;//16
  bool ctOn;//1
  int ctP;//16
  bool gradOn;//1
  int gradP;//16
  bool efxOn;//1
  int efxP;//16
  uint8_t hue;//8
  uint8_t sat;//8
  uint8_t lvl;//8
  uint8_t ledGlobalLvl;//8
} struct_message;

// Create a struct_message called BME280Readings to hold sensor readings
struct_message ledData;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

void generateData(){
  ledData.array=random(0,5);
  Serial.print("ledData.array= ");
  Serial.println(ledData.array);
  ledData.ctOn=random(0,1);
  Serial.print("ledData.ctOn= ");
  Serial.println(ledData.ctOn);
  ledData.ctP=random(0,255);
  Serial.print("ledData.ctP= ");
  Serial.println(ledData.ctP);
  ledData.gradOn=random(0,1);
  Serial.print("ledData.gradOn= ");
  Serial.println(ledData.gradOn);
  ledData.gradP=random(0,255);
  Serial.print("ledData.gradP= ");
  Serial.println(ledData.gradP);
  ledData.efxOn=random(0,1);
  Serial.print("ledData.efxOn= ");
  Serial.println(ledData.efxOn);
  ledData.efxP=random(0,255);
  Serial.print("ledData.efxP= ");
  Serial.println(ledData.efxP);
  ledData.hue=random(0,255);
  Serial.print("ledData.hue= ");
  Serial.println(ledData.hue);
  ledData.sat=random(0,255);
  Serial.print("ledData.sat= ");
  Serial.println(ledData.sat);
  ledData.lvl=random(0,255);
  Serial.print("ledData.lvl= ");
  Serial.println(ledData.lvl);
  ledData.ledGlobalLvl=random(0,255);
  Serial.print("ledData.ledGlobalLvl= ");
  Serial.println(ledData.ledGlobalLvl);
  Serial.println("-------------------------------------------------");
  Serial.println("");
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  inc_array=incomingReadings.array;
  ledData.array=inc_array;
  Serial.print("inc_array= ");
  Serial.println(inc_array);
  inc_ctOn=incomingReadings.ctOn;
  ledData.ctOn=inc_ctOn;
  Serial.print("inc_ctOn= ");
  Serial.println(inc_ctOn);  
  inc_ctP=incomingReadings.ctP;
  ledData.ctP=inc_ctP;  
  Serial.print("inc_ctP= ");
  Serial.println(inc_ctP);    
  inc_gradOn=incomingReadings.gradOn;
  ledData.ctOn=inc_ctOn;
  Serial.print("inc_gradOn= ");
  Serial.println(inc_gradOn);    
  inc_gradP=incomingReadings.gradP;
  ledData.gradP=inc_gradP;
  Serial.print("inc_gradP= ");
  Serial.println(inc_gradP);    
  inc_efxOn=incomingReadings.efxOn;
  ledData.efxOn=inc_efxOn;
  Serial.print("inc_efxOn= ");
  Serial.println(inc_efxOn);    
  inc_efxP=incomingReadings.efxP;
  ledData.efxP=inc_efxP;
  Serial.print("inc_efxP= ");
  Serial.println(inc_efxP);    
  inc_hue=incomingReadings.hue;
  ledData.hue=inc_hue;
  Serial.print("inc_hue= ");
  Serial.println(inc_hue);    
  inc_sat=incomingReadings.sat;
  ledData.sat=inc_sat;
  Serial.print("inc_sat= ");
  Serial.println(inc_sat);    
  inc_lvl=incomingReadings.lvl;
  ledData.lvl=inc_lvl;
  Serial.print("inc_lvl= ");
  Serial.println(inc_lvl);    
  inc_ledGlobalLvl=incomingReadings.ledGlobalLvl;
  ledData.ledGlobalLvl=inc_ledGlobalLvl;
  Serial.print("inc_ledGlobalLvl= ");
  Serial.println(inc_ledGlobalLvl); 
  Serial.println("-------------------------------------------------");
  Serial.println("");     
  delay(250);
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &ledData, sizeof(ledData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }   
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
//   generateData();

//   // Send message via ESP-NOW
//   esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &ledData, sizeof(ledData));
   
//   if (result == ESP_OK) {
//     Serial.println("Sent with success");
//   }
//   else {
//     Serial.println("Error sending the data");
//   }

//   delay(10000);
}




