/*
  Thermoino

  A simpler webserver that controls relays with simple commands via Webservice.

  Based on:
  WiFi Web Server
  created 13 July 2010
  by dlf (Metodo2 srl)
  modified 31 May 2012
  by Tom Igoe

  Also based on:
  https://create.arduino.cc/projecthub/nekhbet/hello-world-with-arduino-genuino-mkr1000-relay-board-and-ap-81fb63?f=1
  modified 4 March 2016
  by Sorin Trimbitas

*/

#include "config.h"

#include <SPI.h>
#include <WiFi101.h>
#include <Adafruit_SleepyDog.h>

int status = WL_IDLE_STATUS;

// Settings and vars for the relays
byte PIN_relay_1 = 3;
byte PIN_relay_2 = 4;
boolean relay_status_1 = HIGH;
boolean relay_status_2 = HIGH;

WiFiServer http_server(80);
WiFiClient http_client;

IPAddress ip = WiFi.localIP();

void setup() {

  Serial.begin(9600);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }

  pinMode(PIN_relay_1, OUTPUT);
  digitalWrite(PIN_relay_1, relay_status_2);
  pinMode(PIN_relay_2, OUTPUT);
  digitalWrite(PIN_relay_2, relay_status_2);

  http_server.begin();
  printWifiStatus();

  Watchdog.enable(4000);
}

byte _is_a_get_request(String txt) {
  String tmp = _get_request_path(txt);
  if (tmp == "N/A") {
    return 0;
  }
  return 1;
}

/**
   @returns "N/A" if txt is not the right header
*/
String _get_request_path(String txt) {
  if (txt.endsWith("HTTP/1.1")) {
    txt = txt.substring(0, txt.indexOf("HTTP/1.1"));
    if (txt.startsWith("GET")) {
      txt = txt.substring(3);
      txt.trim();
      Serial.println("Requesting " + txt);
      return txt;
    }
  }
  return "N/A";
}

void print_status(char relay_status)
{
  http_client.print("Status: ");
  http_client.print(!relay_status, BIN);
}

void process_http_request(String path) {
  if (path == "/") {
    http_client.println();
    write_java_script();
    http_client.println("Status Relay 1: <span id='statusR1' style='color:red; font-weight: bold;'>OFF</span>");
    http_client.println("Status Relay 2: <span id='statusR2' style='color:red; font-weight: bold;'>OFF</span>");
    http_client.println("<hr>Actions: <ul>");
    http_client.println("<li><button onclick=\"httpGetAsync('http://192.168.0.29/relay_1_on', retR1)\">START Pump Relay</button></li>");
    http_client.println("<li><button onclick=\"httpGetAsync('http://192.168.0.29/relay_1_off', retR1)\">STOP Pump Relay</button></li>");
    http_client.println("<li><button onclick=\"httpGetAsync('http://192.168.0.29/relay_1_toggle', retR1)\">TOGGLE Pump Relay</button></li>");
    http_client.println("<li><button onclick=\"httpGetAsync('http://192.168.0.29/relay_1_status', retR1)\">STATUS Pump Relay</button></li>");
    http_client.println("<li><button onclick=\"httpGetAsync('http://192.168.0.29/relay_2_on', retR2)\">START Pump Relay</button></li>");
    http_client.println("<li><button onclick=\"httpGetAsync('http://192.168.0.29/relay_2_off', retR2)\">STOP Pump Relay</button></li>");
    http_client.println("<li><button onclick=\"httpGetAsync('http://192.168.0.29/relay_2_toggle', retR2)\">TOGGLE Pump Relay</button></li>");
    http_client.println("<li><button onclick=\"httpGetAsync('http://192.168.0.29/relay_2_status', retR2)\">STATUS Pump Relay</button></li>");
    http_client.println("</ul>");
  } else if (path == "/relay_1_off") {
    relay_status_1 = HIGH;
    print_status(relay_status_1);
  } else if (path == "/relay_1_on") {
    relay_status_1 = LOW;
    print_status(relay_status_1);
  } else if (path == "/relay_1_toggle") {
    relay_status_1 = !relay_status_1;
    print_status(relay_status_1);
  } else if (path == "/relay_1_status") {
    print_status(relay_status_1);
  } else if (path == "/relay_2_off") {
    relay_status_2 = HIGH;
    print_status(relay_status_2);
  } else if (path == "/relay_2_on") {
    relay_status_2 = LOW;
    print_status(relay_status_2);
  } else if (path == "/relay_2_toggle") {
    relay_status_2 = !relay_status_2;
    print_status(relay_status_2);
  } else if (path == "/relay_2_status") {
    print_status(relay_status_2);
  } else {
    http_client.println("404");
  }

  digitalWrite(PIN_relay_1, relay_status_1);
  digitalWrite(PIN_relay_2, relay_status_2);
}

void write_java_script()
{
  http_client.println("<body>");
  http_client.println("<script>");
  http_client.println("function httpGetAsync(theUrl, callback)");
  http_client.println(" {");
  http_client.println(" var xmlHttp = new XMLHttpRequest(); ");
  http_client.println(" xmlHttp.onreadystatechange = function() {");
  http_client.println("   if (xmlHttp.readyState == 4 && xmlHttp.status == 200)");
  http_client.println("   callback(xmlHttp.responseText); ");
  http_client.println(" }");
  http_client.println(" xmlHttp.open(\"GET\", theUrl, true); // true for asynchronous ");
  http_client.println(" xmlHttp.send(null);");
  http_client.println("}");
  http_client.println("function retR1(response)");
  http_client.println("{");
  http_client.println(" if(response == 'Status: 0') {");
  http_client.println("   document.getElementById('statusR1').innerText = 'OFF';");
  http_client.println("   document.getElementById('statusR1').style = 'color:red; font-weight: bold;'");
  http_client.println(" } else {");
  http_client.println("   document.getElementById('statusR1').innerText = 'ON';");
  http_client.println("   document.getElementById('statusR1').style = 'color:green; font-weight: bold;'");
  http_client.println(" }");
  http_client.println("}");
  http_client.println("function retR2(response)");
  http_client.println("{");
  http_client.println(" if(response == 'Status: 0') {");
  http_client.println("   document.getElementById('statusR2').innerText = 'OFF';");
  http_client.println("   document.getElementById('statusR2').style = 'color:red; font-weight: bold;'");
  http_client.println(" } else {");
  http_client.println("   document.getElementById('statusR2').innerText = 'ON';");
  http_client.println("   document.getElementById('statusR2').style = 'color:green; font-weight: bold;'");
  http_client.println(" }");
  http_client.println("}");
  http_client.println("</script>");
  http_client.println("</body>");
}

void process_http_requests() {
  // Listen for incoming http_clients
  http_client = http_server.available();
  byte tmp_request_is_a_get_request = 0;
  String tmp_get_request_line = "";

  if (http_client) {                             // if you get a http_client,
    Serial.println("New client connected.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the http_client
    while (http_client.connected()) {            // loop while the http_client's connected
      if (http_client.available()) {             // if there's bytes to read from the http_client,
        char c = http_client.read();             // read a byte, then
        // Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {
          // If the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            http_client.println("HTTP/1.1 200 OK");
            http_client.println("Content-type:text/html");
            http_client.println("Access-Control-Allow-Origin: *");
            http_client.println();

            if (tmp_request_is_a_get_request == 1) {
              process_http_request(_get_request_path(tmp_get_request_line));
            }
            break;
          }

          // We don't really care about other headers .. just the /GET ones
          if (_is_a_get_request(currentLine)) {
            tmp_request_is_a_get_request = 1;
            tmp_get_request_line = currentLine;
          }
          // Clear the currentLine buffer
          currentLine = "";
        }
        else if (c != '\r') {
          // Add to the currentLine buffer
          currentLine += c;
        }
      }
    }
    // close the connection:
    http_client.stop();
    Serial.println("Client disconnected.");
  }
}

void loop() {
  process_http_requests();

  if (WiFi.status() == WL_CONNECTED)
  {
    Watchdog.reset();
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}

