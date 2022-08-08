

#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key index number (needed only for WEP)

int greenLed =  2;
int redLed =  3;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

struct WIfiCreds {
  char pass[64];
  char ssid[64];
};


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Access Point Web Server");

  pinMode(greenLed, OUTPUT);      // set the LED pin mode
    pinMode(redLed, OUTPUT);      // set the LED pin mode


  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    Serial.println(status);
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
}




void loop() {
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      delayMicroseconds(10);                // This is required for the Arduino Nano RP2040 Connect - otherwise it will loop so fast that SPI will never be served.
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();    
        // read a byte, then
         currentLine.trim();
         
        if(c == '\n' && currentLine.length() == 0){
          String postParams = "";
         while(client.available()){
          char b = client.read();
          postParams += b;
              
         }

         Serial.println(postParams);

        }

          currentLine += c;
        


       

        if(c == '\n'){
         
          currentLine = "";
        }
        
        
         

          
        
      
   
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /green")) {
          digitalWrite(greenLed, HIGH);      
          digitalWrite(redLed, LOW);                // GET /L turns the LED off

          // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /red")) {
          digitalWrite(redLed, HIGH);  
          digitalWrite(greenLed, LOW);                // GET /L turns the LED off

          
          // GET /L turns the LED off
        }

      }else{
        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<h1>Welcome to the wifi homepage! I need wifi in order to work properly. Submit the details below</h1>");
            client.print("<form method=\"post\"><label for='SSID'>WIFI SSID:</label><input type='text' name='SSID'><br><br>");
            client.print("<label for='pass'>WIFI PASSWORD: </label><input type='password' name='pass'><br><br>");
            client.print("<button>SUBMIT WIFI CREDS</button>");
            client.print("</form>");

            // The HTTP response ends with another blank line:
            // break out of the while loop:
            break;
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}



void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}