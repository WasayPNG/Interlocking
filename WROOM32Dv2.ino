#include <Stepper.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Ticker.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <string>
#include "TrainData.h"
#include "Station.h"
#include "Switch.h"

using namespace std;

#define NUMSLAVES 2
#define CHANNEL 1
#define PRINTSCANRESULTS 0

// Basestation relays
#define A_RELAY 0
#define B_RELAY 4
#define C_RELAY 13

// Signals
#define SIGNAL1 1
#define SIGNAL2 2

// Train Commands
#define TRAIN_START 1
#define TRAIN_STOP 0

vector<TrainData> trainDataArray = {
  TrainData({ 0x58, 0xBF, 0x25, 0x9D, 0x76, 0x35 }, 16), 
  TrainData({ 0x3C, 0x61, 0x05, 0x64, 0xFA, 0x61 }, 5)
};

vector<Station> stationArray {
  {StationAnton, 17},
  {StationBaker, 18},
  {StationCharlie, 15}
};

vector<Switch> switchArray {

  Switch(14, {
    {
      {StationAnton, StationBaker},
      PositionIrrelevant
    },
    {
      {StationAnton, StationCharlie},
      PositionIrrelevant
    },
    {
      {StationBaker, StationAnton},
      PositionMain
    },
    {
      {StationCharlie, StationAnton},
      PositionSecondary
    },
  }),
  

  Switch(19, {
    {
      {StationAnton, StationBaker},
      PositionMain
    },
    {
      {StationAnton, StationCharlie},
      PositionSecondary
    },
    {
      {StationBaker, StationAnton},
      PositionIrrelevant
    },
    {
      {StationCharlie, StationAnton},
      PositionIrrelevant
    },    
  })
};

esp_now_peer_info_t slaves[NUMSLAVES] = {};

bool inited = false;
int Slaves = 0;  // Slaves number

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
int LCDs = 0;

// set LCD address, number of columns and rows
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void setup() {
  Serial.begin(115200);

  Wire.begin();
  WiFi.mode(WIFI_STA);
  Serial.println("ESPNOW LOG: WifiMode: WIFI_STA");
  // This is the mac address of the Master in Station Mode
  Serial.print("ESPNOW LOG: BOARD MAC: ");
  Serial.println(WiFi.macAddress());
  InitESPNow();
  esp_now_register_send_cb(OnDataSent);

  // initialize LCD
  lcd.init();
  lcd.begin(20, 4);
  lcd.backlight();

  for (Station st : stationArray) {
    pinMode(st.mButtonPin, INPUT_PULLUP);
  }

  for (TrainData tData : trainDataArray) {
    pinMode(tData.getButtonPin(), INPUT_PULLUP);
  }

  pinMode(A_RELAY, INPUT_PULLUP);
  pinMode(B_RELAY, INPUT_PULLUP);  
  pinMode(C_RELAY, INPUT_PULLUP); 

  pinMode(26, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(12, OUTPUT);
}



// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// Scan for slaves in AP mode
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  //reset slaves
  memset(slaves, 0, sizeof(slaves));
  Slaves = 0;
  Serial.println("");
  if (scanResults == 0) {
    Serial.println("ESPNOW LOG: No WiFi devices in AP Mode found");
  } else {
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);
      delay(10);
      // Check if the current device starts with `WROOM32D`
      if (SSID.indexOf("WROOM32D") == 0) {
        // SSID of interest
        //Serial.print("ESPNOW LOG: "); Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
        // Get BSSID => Mac Address of the Slave
        int mac[6];
        if (6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5])) {
          for (int ii = 0; ii < 6; ++ii) {
            slaves[Slaves].peer_addr[ii] = (uint8_t)mac[ii];
          }
        }
        slaves[Slaves].channel = CHANNEL;  // pick a channel
        slaves[Slaves].encrypt = 0;        // no encryption
        Slaves++;
      }
    }
  }

  if (Slaves == 0) {
    Serial.println("ESPNOW LOG: No Slave Found, trying again.");
  }
  // clean up ram
  WiFi.scanDelete();
}



//This function check if the slave is already paired with the master and if not, pair the slave with the master
void ManageSlave() {
  if (Slaves > 0) {
    for (int i = 0; i < Slaves; i++) {
      bool exists = esp_now_is_peer_exist(slaves[i].peer_addr);
      if (!exists) {
        // Slave not paired, attempt pair
        esp_err_t addStatus = esp_now_add_peer(&slaves[i]);
        if (addStatus == ESP_OK) {
          Serial.println("ESPNOW LOG: Pair success");
        } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
          Serial.println("ESPNOW LOG: ESPNOW Not Init");
        } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
          Serial.println("ESPNOW LOG: Add Peer - Invalid Argument");
        } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
          Serial.println("ESPNOW LOG: Peer list full");
        } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
          Serial.println("ESPNOW LOG: Out of memory");
        } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
          Serial.println("ESPNOW LOG: Peer Exists");
        } else {
          Serial.println("ESPNOW LOG: Not sure what happened");
        }
        delay(100);
      }
    }
  } else {
    Serial.println("ESPNOW LOG: No Slave found to process");
  }
}

//send command
void sendCmdToTrain(const uint8_t commandNumber, uint8_t trainId) {
  const uint8_t *peer_addr = trainDataArray[trainId].getMac().data();

  esp_now_send(peer_addr, &commandNumber, sizeof(commandNumber));
  Serial.print("ESPNOW LOG: Data sent: ");
  Serial.println(commandNumber);
}

//Macaz


//ParcurgereTren1


// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("ESPNOW LOG: Last Packet Sent to: ");
  Serial.println(macStr);
}

void SignalState(int signal, bool state) {
  switch (signal) {
    case 1:
      digitalWrite(12, state);
      digitalWrite(27, !state);
      break;

    case 2:
      digitalWrite(26, state);
      digitalWrite(25, !state);
      break;
    default: break;
  }
}


/* Check if button for route has been pressed (and route needs to be set) */
void CheckForRoutes() {
  for (TrainData &tData : trainDataArray) {
    uint8_t buttonPin = tData.getButtonPin();
    if (digitalRead(buttonPin) == LOW) {
      for (Station st : stationArray) {
        if (digitalRead(st.mButtonPin) == LOW) {
          tData.pushDestination(st.mId);
        }
      }
    }
  }
}

void PrepareRoutes() {
  for (TrainData &tData : trainDataArray) {
    Route newRoute = {tData.getCurrentStation(), tData.getNextStation()};
    for (Switch &sw : switchArray) {
      sw.queueRoute(newRoute);
    }    
  }
}

/* Scan/Manage trains */
void CheckForTrains() {
  if (Slaves == 0) {
    ScanForSlave();
  } else if (Slaves > 0) {
    ManageSlave();
  }
}

void i2c() {
  byte error, address;
  Serial.println("Scanning...");
  LCDs = 0;
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      LCDs++;
    } else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (LCDs == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("done\n");
  }
  delay(5000);
}


void ClearLCDLine(int line) {
  lcd.setCursor(0, line);
  for (int n = 0; n < 20; n++)
    lcd.print(" ");
}

void SetLCDLine(int line, String newText) {
  ClearLCDLine(line);
  lcd.setCursor(0, line);
  lcd.print("Gara C: T2");
}

void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i = 0; i < lcdColumns; i++) {
    message = " " + message;
  }
  message = message + " ";
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}


void SystemInit() {
  inited = true;
  if (LCDs == 0) {
    i2c();
  }

  lcd.setCursor(0, 0);
  lcd.print("MERSUL TRENURILOR");
  lcd.setCursor(0, 1);
  lcd.print("Gara A: -");
  lcd.setCursor(0, 2);
  lcd.print("Gara B: -");
  lcd.setCursor(0, 3);
  lcd.print("Gara C: -");
}



void loop() {
  digitalWrite(12, HIGH);
  if (inited != true) {
    SystemInit();
    Serial.println("SYS LOG: SYSTEM INITED");
  }

  CheckForTrains();
  CheckForRoutes();
  PrepareRoutes();
  /* check if train reached switch and pop queue */
  /* check if train reached switch and it's not on the same line */
  /* send start/stop cmds to trains */
}