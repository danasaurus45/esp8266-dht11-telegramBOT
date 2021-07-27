#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "DHT.h"

#define DHTPIN D1 // Diisi sesuai pin yang dihubungkan dari ESP8266 ke DHT11
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

// Initialize Wifi connection to the router
char ssid[] = "XXXXXXXXXXX";     // diisi nama wifi
char password[] = "YYYYYYYYYYYY"; // diisi password wifi

// Initialize Telegram BOT
#define BOTtoken "ZZZZZZZZZZZZZZZZ" // diisi Token Bot (Dapat dari Telegram Botfather)
#define id_chat "XXXXXXXXXXXXXXXX" // diisi Chat ID (Dapat dari Telegram IDBot)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    //Cek Pembacaan Sensor DHT11
    if (text == "/status") {
      String statusDHT;
      int t = dht.readTemperature() - 2;
      int h = dht.readHumidity();
      if ( t <= 18.00 || h <= 40 ) {
        statusDHT = "Minimum";
      }
      else if (t <= 21.00 || h <= 48) {
        statusDHT = "Normal";
      }
      else if (t <= 27.00 || h <= 55) {
        statusDHT = "Maximal";
      }
      else {
        statusDHT = "Melebihi batas maximum";
      }
      String readDHT = "Suhu saat ini : ";
      readDHT += int(t);
      readDHT += " *C\n";
      readDHT += "Kelembaban: ";
      readDHT += int(h);
      readDHT += " %\n";
      readDHT += "Status: " + statusDHT + "\n";

      bot.sendMessage(chat_id, readDHT, "");
    }

    //Cek Command untuk setiap aksi
    if (text == "/start") {
      String welcome = "Welcome  " + from_name + ".\n";
      welcome += "/status : Status Suhu dan Kelembapan saat ini\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup() {

  Serial.begin(115200);
  dht.begin();
  // This is the simplest way of getting this working
  // if you are passing sensitive information, or controlling
  // something important, please either use certStore or at
  // least client.setFingerPrint
  client.setInsecure();

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int t = dht.readTemperature() - 2;
  int h = dht.readHumidity();
  if (t <= 18.00 || h <= 40 ) {
    String readDHT = "Suhu saat ini : ";
    readDHT += int(t);
    readDHT += " *C\n";
    readDHT += "Kelembaban: ";
    readDHT += int(h);
    readDHT += " %\n";
    readDHT += "Status: Minimum \n";
    
    bot.sendMessage(id_chat, readDHT, "");
  }

  else if (t <= 21.00 || h <= 48) {
    String readDHT = "Suhu saat ini : ";
    readDHT += int(t);
    readDHT += " *C\n";
    readDHT += "Kelembaban: ";
    readDHT += int(h);
    readDHT += " %\n";
    readDHT += "Status: Normal \n";

    bot.sendMessage(id_chat, readDHT, "");
  }
  else if (t <= 27.00 || h <= 55) {
    String readDHT = "Suhu saat ini : ";
    readDHT += int(t);
    readDHT += " *C\n";
    readDHT += "Kelembaban: ";
    readDHT += int(h);
    readDHT += " %\n";
    readDHT += "Status: Maximum \n";

    bot.sendMessage(id_chat, readDHT, "");
  }
  else {
    String readDHT = "Suhu saat ini : ";
    readDHT += int(t);
    readDHT += " *C\n";
    readDHT += "Kelembaban: ";
    readDHT += int(h);
    readDHT += " %\n";
    readDHT += "Status: Melebihi batas maximum \n";

    bot.sendMessage(id_chat, readDHT, "");
  }
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    lastTimeBotRan = millis();
  }
}
