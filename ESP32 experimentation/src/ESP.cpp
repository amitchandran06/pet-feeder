#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>           // Dependency for ESPAsyncWebServer
#include <ESPAsyncWebServer.h>  // The main server library
#include <stdlib.h>

// --- WiFi Credentials ---
const char* ssid = "e^x";
const char* password = "Ferrariisthebestteam16";

// --- Server & WebSocket Setup ---
AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // Create a WebSocket endpoint at /ws

// Time variables for non-blocking data sending
unsigned long previousMillis = 0;
const long interval = 1000; // Send data every 2 seconds
float mockSensorValue = 0.0; 

// --- HTML Content (Client-Side) ---
const char* index_html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Live Data</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, sans-serif; text-align: center; }
    .data-box { background-color: #f0f0f0; padding: 20px; border-radius: 8px; display: inline-block; margin-top: 20px; }
    h1 { color: #333; }
    .big-button {
  padding: 15px 32px; 
  font-size: 40px;    
  cursor: pointer;  
  margin-bottom: 20px;  
}
    #sensorValue { font-size: 3em; color: #007bff; }
  </style>
</head>
<body>
  <h1>Live WebSocket Stream</h1>
  <div class="data-box">
    <p>Time Since Reset (S):</p>
    <span id="sensorValue">--</span> s
  </div>

  <div class="controls">
    <h2>Controls</h2>
    <button class ="big-button"onclick="sendMessage('LED_ON')">Turn LED ON</button>

  
    <button class ="big-button"onclick="sendMessage('LED_OFF')">Turn LED OFF</button>
  </div>

<script>
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.onload = function() {
  connectWebSocket();
}

function connectWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  
  // Event handler for successful connection
  websocket.onopen    = onOpen;
  
  // Event handler for receiving data
  websocket.onmessage = onMessage; 
  
  // Event handler for closing connection (or error)
  websocket.onclose   = onClose;
  websocket.onerror   = onError;
}

// Fires when the connection is opened
function onOpen(event) {
  console.log('Connection opened');
}

// Fires when data is received from the server (ESP32)
function onMessage(event) {
  console.log('Received: ' + event.data);
  // Update the displayed value directly
  document.getElementById('sensorValue').innerHTML = event.data; 
}

// Fires when the connection is closed
function onClose(event) {
  console.log('Connection closed. Retrying...');
  setTimeout(connectWebSocket, 2000); // Try to reconnect every 2 seconds
}

// Fires on error
function onError(event) {
  console.log('WebSocket error detected: ' + event.data);
}
  function sendMessage(message) {
  console.log("Sending: " + message);
  websocket.send(message);
}
</script>
</body>
</html>
)rawliteral";

// --- ESP32 Server-Side Functions ---

// Handles WebSocket events (connect, disconnect, receive)
// Handles WebSocket events
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch(type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
      
    // NEW: Handle incoming data from the client
    case WS_EVT_DATA: { // The {} create a new scope for the variable
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        // Convert the received byte array to a String
        String message = "";
        for (size_t i = 0; i < len; i++) {
          message += (char)data[i];
        }
        
        Serial.printf("Received message from client #%u: %s\n", client->id(), message.c_str());

        // Process the command
        if(message == "LED_ON"){
          digitalWrite(5,HIGH);
        }
        else if (message == "LED_OFF"){
          digitalWrite(5,LOW);
        }
      }
      break;
    }
      
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

// Sends mock sensor data to all connected clients
void notifyClients() {
  // Convert the float value to a string
  String dataToSend = String(mockSensorValue, 2); 
  // Send the string data via WebSocket
  ws.textAll(dataToSend); 
}

void setup() {
  pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);
  Serial.begin(115200);

  // 1. Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP Address: ");
  Serial.println(WiFi.localIP());

  // 2. Attach WebSocket handler
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // 3. Serve the HTML page (client code) at the root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // 4. Start the Asynchronous Web Server
  server.begin();
  Serial.println("Async WebServer started.");
}

void loop() {
  // Check if it's time to send new data
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Simulate reading a sensor (incrementing a mock value)
    mockSensorValue += 1; 
    
    
    // Send the new data to all connected clients
    notifyClients();
  }
  
  // NOTE: No need for server.handleClient() or ws.cleanupClients() in the loop
  // because the Async library handles these tasks automatically in the background.
}