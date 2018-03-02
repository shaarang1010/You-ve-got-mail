/* Author: Shaarang Tanpure
 * Email: shaarang.1@hotmail.com
 * License: MIT
*/


#include <WiFi.h>

// WiFi network name and password:
#define WIFISSID "HTC Portable Hotspot B3BA" // Put your WifiSSID here
#define PASSWORD "Jackass8=D" // Put your wifi password here

char * MakerIFTTT_Event = "received_mail";
char * MakerIFTTT_Key = "g6TZjCkdIPahlFndLRN2IBJtym9fepN4LG6RIuoQkzV"; // Webhooks settings key
char * hostDomain = "maker.ifttt.com";
const int hostPort = 80;

const int LED_PIN = 5;

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  300        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;


/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

void setup()
{
// Initilize hardware:
Serial.begin(115200);
 pinMode(5, INPUT);// set pin as input

// Connect to the WiFi network (see function below loop)
connectToWiFi(WIFISSID, PASSWORD);
Serial.println("Press button 0 to send a text ");

//Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

}

void loop()
{
int detect = digitalRead(5);// read obstacle status and store it into "detect"
  if(detect == LOW){
    
   Serial.println("You've got Mail!");
   sendSms();
   esp_deep_sleep_start(); //go to deep sleep after sending notification
  }else{ 
   Serial.println("All clear");
   delay(1000);  
  }
// send text message
}

void connectToWiFi(const char * ssid, const char * pwd)
{
delay(1000);

printLine();
Serial.println("Connecting to WiFi network: " + String(ssid));

WiFi.begin(ssid, pwd);

while (WiFi.status() != WL_CONNECTED)
{
// Blink LED while we’re connecting:
delay(500);
Serial.print(".");
}

Serial.println();
Serial.println("WiFi connected!");
Serial.print("IP address: ");
Serial.println(WiFi.localIP());
}

void printLine()
{
Serial.println();
for (int i = 0; i < 30; i++)
Serial.print("-");
Serial.println();
}

 

void sendSms() {

WiFiClient client;
client.stop();

if (client.connect(hostDomain, hostPort)) {

String PostData = "{\"value1\" : \"Sparkfun ESP32 test!\" }";

Serial.println("connected to server… Getting name…");
// send the HTTP PUT request:
String request = "POST /trigger/";
request += MakerIFTTT_Event;
request += "/with/key/";
request += MakerIFTTT_Key;
request += " HTTP/1.1";

Serial.println(request);
client.println(request);

client.println("Host: " + String(hostDomain) );
client.println("User-Agent: Energia/1.1");
client.println("Connection: close");
client.println("Content-Type: application/json");
client.print("Content-Length: ");
client.println(PostData.length());
client.println();
client.println(PostData);
client.println();
}
else {
// if you couldn’t make a connection:
Serial.println("Connection failed");
return ;
}

// Capture response from the server. (10 second timeout)
long timeOut = 4000;
long lastTime = millis();

while ((millis() - lastTime) < timeOut) { // Wait for incoming response from server
while (client.available()) { // Characters incoming from the server
char c = client.read(); // Read characters
Serial.write(c);
}

}
Serial.println();
Serial.println("Request Complete!");

return ;

}
