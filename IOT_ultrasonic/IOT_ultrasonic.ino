#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <QuickStats.h>
QuickStats stats; //initialize an instance of this class

// creating webserver object
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80
// defining the wifi username and pass
const char* ssid = "*****";
const char* password = "******";



// Variable to store the HTTP request String header;

IPAddress local_IP(192, 168, 1, 142);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

void handleRoot();              // function prototypes for HTTP handlers
void handleNotFound();



// ultrasonic var
int trigPin = D2;    //Trig - green Jumper
int echoPin = D1;    //Echo - yellow Jumper
long duration;
float cm;


void setup(void) {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  // Start wifi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());// this will display the Ip address of the Pi which should be entered into your browser

  // setting the webserver handlers


  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  server.on("/WaterLevel", HTTP_GET, handleanalogue);
  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");


  //ultrasonic setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


}


float ultrasonicRead() {

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  float RR;
  float pp[10];
  for (int ii = 0; ii < 10; ii++) {
    delay(25);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read the signal from the sensor: a HIGH pulse whose
    // duration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.
    pinMode(echoPin, INPUT);
    pp[ii] = pulseIn(echoPin, HIGH);
    RR += pp[ii];
    Serial.println("sensor iterations =" + String(ii) + " with value = " + String((pp[ii] / 2) / 29.1));
  };

  duration = stats.median(pp, 10);
  // convert the time into a distance
  cm = (duration / 2) / 29.1;
  Serial.println("sensor =" + String(cm));

  return cm;
};

void handleanalogue() {

  float reading = 16.74 - ultrasonicRead();

  String rrr = "The ultasonic reading is = " + String(reading);
  Serial.println(rrr);
  server.send(200, "text/plan", rrr);


};
void handleRoot() {
  Serial.println("handleRoot");
  server.send(404, "text/plain", "Hello IOT"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request


}
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void loop(void) {
  server.handleClient(); // Listen for HTTP requests from clients
  //analogWrite(D0, out);
  //Serial.println("control value");
  //    Serial.println(out);
  delay(20);
}
