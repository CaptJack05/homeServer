#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Amit";       // Replace with your Wi-Fi SSID
const char* password = "Jaimatadi"; // Replace with your Wi-Fi password

WebServer server(80);

// HTML content for the web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Message Input</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
    input[type="text"] { width: 300px; padding: 10px; font-size: 16px; }
    input[type="submit"] { padding: 10px 20px; font-size: 16px; }
  </style>
</head>
<body>
  <h2>Enter Your Message</h2>
  <form action="/send" method="post">
    <input type="text" name="message" placeholder="Type your message here" required>
    <br><br>
    <input type="submit" value="Send">
  </form>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  delay(1000); // Allow time for Serial Monitor to initialize

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 10000; // 10 seconds timeout

  // Attempt to connect until timeout
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to Wi-Fi. Restarting...");
    delay(2000);
    ESP.restart();
  }

  Serial.println("\nConnected to Wi-Fi.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Serve the HTML page
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", index_html);
  });

  // Handle form submission
  server.on("/send", HTTP_POST, []() {
    if (server.hasArg("message")) {
      String message = server.arg("message");
      Serial.println("Received message: " + message);
      server.send(200, "text/html", "<h3>Message received</h3><a href='/'>Go Back</a>");
    } else {
      server.send(400, "text/html", "<h3>No message received</h3><a href='/'>Go Back</a>");
    }
  });

  // Start server
  server.begin();
}

void loop() {
  server.handleClient();
}
