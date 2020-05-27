#include <M5StickC.h>
#include <PubSubClient.h>
#include "WiFi.h"
#include "settings.h" // copy settingsexample.h to settings.h and fill in 

long currentchannelidx = 0; 
long timerQR = 0; // global: no messages are shown while this timer > 0 

uint16_t backgroundcolor = TFT_BLACK;
uint16_t textcolor = TFT_WHITE;
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
  M5.begin();
  M5.update();

  delay(1000); // Wake up time
  
  M5.Lcd.setTextColor(textcolor,backgroundcolor);  
  M5.Lcd.fillScreen(backgroundcolor);
  M5.Lcd.setCursor(0,0);
  M5.Lcd.setTextSize(2);  
  M5.Lcd.setRotation(1);  
 
  // Serial.begin(115200);
  startWifi();
  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  M5.Lcd.fillScreen(backgroundcolor);
  while (!client.connected()) {
    M5.Lcd.println("Connecting to MQTT...");
    if (client.connect(sensorId)) { // , mqttUser, mqttPassword
      M5.Lcd.println("Connected");   
    } else {
 
      M5.Lcd.print("failed with state ");
      M5.Lcd.println(client.state());
    }
    
    delay(2000);
  }

  char topic[256];
  sprintf(topic, listenToTopic, sensorId);

  char message[200];
  sprintf(message, "MQTT subscribe to %s", topic);
  M5.Lcd.println(message);   
  
  client.subscribe(topic);
  delay(1000);

  currentchannelidx=0;
  setChannel(); // init screen / text color
}

void startWifi() {
  
  char message[200];
  bool connecting = false;
  long waitidx = 0;
  long attemptsidx = 0;
  long ssididx = 0;

  do {
    if(!connecting||waitidx==nWifiConnectWaitSecs*10) { // waited long enough, disconnect and next attempt - 1 waitidx = 100ms delay
      if(connecting) { 
        showMessage(message, true, true);

        WiFi.disconnect();
        connecting=false;
        delay(500); 
        
        if(++attemptsidx==nWifiAttemptsPerSSID) {  // next attempt, if all attempts done, next SSID
          attemptsidx=0;
  
          if(++ssididx==nSSIDs) { // all SSIDs done, start at 0 again
            ssididx=0;
          }
        }
      }
      
      WiFi.begin(ssids[ssididx], passwords[ssididx]);
      connecting=true;
      waitidx=0; 

      sprintf(message, "Wifi connect to %s - attempt %d / %d", ssids[ssididx], attemptsidx+1, nWifiAttemptsPerSSID);
      showMessage(message, true, true);
    }

    delay(100);
    waitidx++;
  } while (WiFi.status() != WL_CONNECTED);
  
  sprintf(message, "Wifi connected to %s", ssids[ssididx]);
  showMessage(message, true, true);
}

void showMessage(char * message, bool cleanscreen, bool setcursor) {
  if(timerQR>0) { return; } // mute while showing QR
  
  if(cleanscreen) { M5.Lcd.fillScreen(backgroundcolor); };
  if(setcursor) { M5.Lcd.setCursor(0,0); };
  M5.Lcd.println(message);
}

void printDot() {
  if(timerQR>0) {return; } // mute while showing QR
  
  M5.Lcd.print(".");
}
 
void callback(char* topic, byte* payload, unsigned int length) {
   char message[512];
   strcpy(message, "");
  
  if(length==4&&payload[0]=='c'&&payload[1]=='h') {
    int channel = (payload[2]-(byte) '0')*10 + (payload[3]- (byte) '0');
    if(channel>=0 && channel<nchannels) {
      sprintf(message,"change channel to %d", channel);
      currentchannelidx=channel;
      setChannel();
      return;
    } else {
      sprintf(message,"change channel - invalid channel %d", channel);
    }
  } else if(length>5&&payload[0]=='q'&&payload[1]=='r') {
    if(length<261) { // cap QR max length
      int duration = (payload[2]-(byte) '0')*100 + (payload[3]-(byte) '0')*10 + (payload[4]- (byte) '0');
      timerQR = duration * 100; 
      char * qrdata = (char *) malloc(length-1);
      for(int i=0;i<length-2;i++) { qrdata[i] = (char) payload[i+2]; }
      M5.Lcd.fillScreen(backgroundcolor);
      M5.Lcd.qrcode(qrdata,0,0,80, 4);
      free(qrdata);
      // no message
    } else {
      sprintf(message,"show QR- data too long (%d)", length);
    }
  } else {
    if(length<512) { // cap max length
      sprintf(message,"unknown command received (%s)", payload);
    } else {
      sprintf(message,"unknown command received");
    }
  }

  if(strlen(message)>0) { showMessage(message, true, true); };
}

void sendTemperature() {
  float temperature = random(200, 300) / 10;
  
  char topic[200];
  // sprintf(topic, "bumos/temp/%s", channels[currentchannelidx]);
  sprintf(topic, "smartys-%s/temp", channels[currentchannelidx]);

  char value[100];
  sprintf(value, "%2.2f", temperature);
  
  client.publish(topic, value);  

  char message[100];
  sprintf(message,"%s -> %s", channels[currentchannelidx], value);
  showMessage(message, true, true);
}

void setChannel() {
  if(currentchannelidx>=0 && currentchannelidx<nchannels) {
    textcolor = textcolors[currentchannelidx];
    backgroundcolor = backgroundcolors[currentchannelidx];
  } else {
    textcolor = TFT_WHITE;
    backgroundcolor = TFT_BLACK;
  }

  M5.Lcd.setTextColor(textcolor,backgroundcolor);  
  showMessage("", true, true);
  delay(500);
}

int nextPublish = 500; // every 5 seconds
void loop() {
  if(nextPublish % 100 == 0) {
    printDot();
  }

  if(nextPublish--<=0) {
    sendTemperature();
    nextPublish = 500;
  }
  
  M5.update();

  if (M5.BtnA.wasReleased()){
    sendTemperature();
    nextPublish = 500;
    timerQR = 0; // hide QR
  }

  if (M5.BtnB.wasReleased()){
    if(currentchannelidx<nchannels-1) {
      currentchannelidx++;
    } else {
      currentchannelidx=0;
    }
    
    setChannel();
    timerQR = 0; // hide QR
  }

  if(timerQR>0) { 
    timerQR--; 
    if(timerQR==0) {
      showMessage("", true, true);
    }
   };

  M5.update();

  client.loop();
  delay(10);
}
