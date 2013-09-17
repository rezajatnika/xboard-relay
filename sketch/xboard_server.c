/*
 * Simple web server for controlling relay using Arduino and Xbee Wireless
 * modules
 *
 */

#include <Client.h>
#include <Server.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Udp.h>

// Variable setup
String readString = String(30);
boolean LEDON = false;

int state;
int val = 0;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,123);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

// End variable setup

void setup() {
  pinMode(4, OUTPUT);

 // Open serial communications and wait for port to open:
  Serial.begin(9600); // Xbee baud rate
   while (!Serial) {
    ; // wait for serial port to connect.
  }

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New Client");

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (readString.length() < 100) {
          readString += c;
        }

        // Button action status
        if (readString.indexOf("IO=1") > 0) {
          if (LEDON == false) {
            LEDON = true;
            Serial.println("ON");
          }
        }
        if (readString.indexOf("IO=0") > 0) {
          if (LEDON == true) {
            LEDON = false;
            Serial.println("OFF");
          }
        }

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {

          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 2");  // refresh the page automatically every 2 sec
          client.println();

          //meta html
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          //header
          client.println("<head>");
          client.println("<title>Xboard Server Test</title>");
          client.println("</head>");

          //body
          client.println("<body>");
          client.println("<h1>Xboard Server Test</h1>");

          // output the value of each analog input pin
          client.println("<h2>Analog Input</h2>");
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("Analog input A");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }

          // output the value of each digital input pin
          client.println("<h2>Digital Input</h2>");
          for (int digitalChannel = 0; digitalChannel < 6; digitalChannel++) {
            int sensorDigitalReading = digitalRead(digitalChannel);
            client.print("Digital Input D");
            client.print(digitalChannel);
            client.print(" is ");
            client.print(sensorDigitalReading);
            client.println("<br />");
          }

          client.println("<br />");
          client.println("<hr />");
          client.print("<h2>Relay Control</h2>");
          client.print("<form>");
          client.print("<input type=radio name=IO value=1 />On<br />");
          client.print("<input type=radio name=IO value=0 />Off<br />");
          client.print("<br />");
          client.print("<input type=submit value=Submit />");
          client.print("</form>");
          client.print("<br />");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }

    // Print door status on web page
    if (LEDON) {
      client.print("<font size='3'>Relay status: ");
      client.println("<font color='green' size='3'>ON");
    }
    else {
      client.print("<font size='3'>Relay status: ");
      client.println("<font color='grey' size='3'>OFF");
    }

    client.println("<hr />");
    client.println("</body>");
    client.println("</html>");

    // clearing string for the next read
    readString = "";

    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("Client Disconnected");
  }

  // Routine to read response from remote Arduino
  // and local LED at PIN4
  if (Serial.available() > 0) {
    val = Serial.read();
    if (val == 'H') {
      digitalWrite(4, HIGH);
      delay(20);
    }
    if (val == 'L') {
      digitalWrite(4, LOW);
      delay(20);
    }
  }
}
