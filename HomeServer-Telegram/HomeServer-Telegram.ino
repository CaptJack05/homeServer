#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define WIFI_SSID     "ssid"
#define WIFI_PASSWORD "password"

#define BOT_TOKEN "your bot token" // telgram bot token
#define CHAT_ID    "chat id" // for multi user

#define BUZZER_PIN 4  // Change to your actual buzzer pin
#define CHECK_INTERVAL 2000  // 2 seconds

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long lastCheckTime = 0;

void beepOnce() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
}

void beepPattern(int duration_ms) {
  unsigned long start = millis();
  while (millis() - start < duration_ms) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(300);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("Booting... Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n❌ Wi-Fi failed. Restarting...");
    delay(2000);
    ESP.restart();
  }

  Serial.println("\n✅ Wi-Fi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  secured_client.setInsecure();  // Skip certificate verification

  Serial.println("🤖 Telegram bot initialized.");
  Serial.println("✅ Group ID: " + String(CHAT_ID));
}

void loop() {
  if (millis() - lastCheckTime > CHECK_INTERVAL) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.printf("📩 %d new message(s) received.\n", numNewMessages);

      for (int i = 0; i < numNewMessages; i++) {
        String text = bot.messages[i].text;
        String sender = bot.messages[i].from_name;
        String chat_id = bot.messages[i].chat_id;

        if (chat_id != CHAT_ID) {
          Serial.println("⚠️ Ignoring message from another chat.");
          continue;
        }

        Serial.print("👤 From: " + sender);
        Serial.println("💬 Msg: " + text);
        beepOnce();

        text.toLowerCase();
        if (text == "/buzz" || text == "openthedoor") {
          Serial.println("🔔 Triggering buzzer alert for 20s...");
          beepPattern(20000);
        }
      }

      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastCheckTime = millis();
  }
}
