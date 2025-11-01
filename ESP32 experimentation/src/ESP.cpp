#include <Arduino.h>
#include <WiFi.h>

// put function declarations here:
int myFunction(int, int);
void clearBuffer();

const char* ssid ="e^x";
const char* password = "PASSWORD";

WiFiServer server(80);
void setup() {
Serial.begin(9600);
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
Serial.println("WIFI SSID:");
Serial.println(ssid);
Serial.println(WiFi.localIP());
server.begin();
}

void loop() {
  // 3. Check for a new client connection
  // The WiFiServer checks for connection requests and returns a WiFiClient object if available.
  WiFiClient client = server.available(); 

  // If a client object is valid (i.e., someone connected)
  if (client) { 
    Serial.println("New client connected.");

    // This loop WAITS for the client to send data.
    // The server is momentarily blocked here while handling the request.
    while (client.connected()) {
      if (client.available()) {
        
        // 4. Read the incoming request (e.g., "GET / HTTP/1.1")
        String line = client.readStringUntil('\r'); // Read the first line of the HTTP request
        Serial.println(line);

        // 5. Send the HTTP Response Header
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close"); // Tell the browser to close the connection after sending the page
        client.println(); // Mandatory blank line separates headers from content

        // 6. Send the HTML Webpage Content
        client.println("<!DOCTYPE html><html>");
        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
        client.println("<body>");
        
        client.print("<h1>ESP32 Server Status</h1>");
        client.print("<h1> vedant is a neek <h1>");
        client.print("<h2>IP Address: ");
        client.print(WiFi.localIP());
        client.println("</h2>");
        
        client.println("<p>Communication via WiFiClient successful!</p>");
        client.println("</body></html>");

        // 7. End the connection and break out of the loop
        break; 
      }
    }
    
    // Clean up the client connection
    client.stop();
    Serial.println("Client disconnected.");
  }
}