#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

#define TRAIN_START 1
#define TRAIN_STOP 0

#define DEBOUNCE_THRESHOLD 200

#define IR 13
#define LIGHT_SENSOR 34
#define ENGINE 17

bool LEDUP = 0;

int ir_read;

int last_ir = LOW;
int last_ls = LOW;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("ESPNOW LOG: AP MAC: ");
  Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(ENGINE, OUTPUT);
  pinMode(IR, INPUT);
  pinMode(LIGHT_SENSOR, INPUT);
}


// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNOW LOG: Init Success");
  } else {
    Serial.println("ESPNOW LOG: Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}


// config AP SSID
void configDeviceAP() {
  String Prefix = "WROOM32D";
  String Mac = WiFi.macAddress();
  //String SSID = Prefix + Mac;
  String SSID = Prefix;
  String Password = "123456789";
  bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), CHANNEL, 0);
  if (!result) {
    Serial.println("ESPNOW LOG: AP Config failed.");
  } else {
    Serial.println("");
    Serial.println("ESPNOW LOG: AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}


void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {  
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
   Serial.print("ESPNOW LOG: Recieved data from "); Serial.print(macStr) ;
   Serial.println(*data);
   digitalWrite(ENGINE, *data);
   }


void ReadInputs(){
  static unsigned long debounceTimer = 0;
  int ls_read = analogRead(LIGHT_SENSOR);
  int ls_mv = getLightmV(ls_read);
  ir_read = digitalRead(IR);
  
  if (ir_read == HIGH){
    debounceTimer = 0;
    if(last_ir != ir_read){
    Serial.println("IR LOG: HIGH");
    last_ir = HIGH;
    }
  }
  else{
    if (!debounceTimer)
      debounceTimer = millis();
    if(last_ir != ir_read && millis() - debounceTimer > DEBOUNCE_THRESHOLD){
      debounceTimer = 0;
      Serial.println("IR LOG: LOW");
      digitalWrite(ENGINE,LOW);
      last_ir = LOW;
    }
  }
}

int getLightmV(int ls_read){
  int ls_voltage = ls_read *(3.7/4095) * 1000;
  return ls_voltage;
}

void loop() {
  ReadInputs();
}
