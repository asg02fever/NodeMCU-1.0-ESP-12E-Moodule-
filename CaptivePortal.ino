#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display TWI address
#define OLED_ADDR   0x3C
// Reset pin not used but required for library
#define OLED_RESET -1
#define SCREEN_HEIGHT 64 // Hauteur de l'écran OLED en pixels

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String responseHTML = ""
                      "<!DOCTYPE html><html lang='en'><head>"
                      "<meta name='viewport' content='width=device-width'>"
                      "<title>CaptivePortal</title></head><body>"
                      "<h1>Hello World!</h1>" //big title of the webpage
                      "<p>https://linktr.ee/keebasg" //link in the page
                      "<p>Ce lien redirige vers une page web" //this link rederect to a web page
                      "<p>avec tout mes reseau" //the web page contain all my social links
                      "<p>Copiez le lien et utilisez le dans google. </p></body></html>" //instruction for people to copy past the link on google
                      "<html><body><img src='./logo.jpg' alt='Image'></body></html>";

const int buttonPin = D1;
bool servingPortal = true; // Start serving portal by default

void setup() {
  
  Serial.begin(115200);
  
  // Initialize the OLED display
  Wire.begin(14, 12); // SDA on GPIO14 (D6), SCL on GPIO12 (D5)
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // WiFi setup
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("YOUTUBE KEEBASG"); //replace "YOUTUBE KEEBASG" for "ssid you want"

  // DNS and web server setup
  dnsServer.start(DNS_PORT, "*", apIP);
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.begin();
  
  // Button setup
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  
  // Check if the button is pressed to toggle the portal and DNS server
  if (digitalRead(buttonPin) == LOW) {
    delay(100); // Debounce
    if (digitalRead(buttonPin) == LOW) {
      servingPortal = !servingPortal;
      if (servingPortal) {
        dnsServer.start(DNS_PORT, "*", apIP);
      } else {
        dnsServer.stop();
      }
      display.clearDisplay();
      display.setCursor(15, 0);
      display.setTextSize(2);
      display.println(F("Server")); // Yellow text
      display.setCursor(15, 20);
      display.setTextSize(2);
      display.println(servingPortal ? F("ONLINE") : F("OFFLINE")); // Online status in French
      display.setCursor(15, 55);
      display.setTextSize(1);
      display.println(F("Made by keebasg")); // Credit
      display.display();
      delay(1000); // Delay to avoid multiple toggles
    }
  }
  
  // Update OLED display with the current status
  display.clearDisplay();
  display.setCursor(15, 0);
  display.setTextSize(2);
  display.println(F("Server")); // Yellow text
  display.setCursor(15, 20);
  display.setTextSize(2);
  display.println(servingPortal ? F("ONLINE") : F("OFFLINE")); // Online status in French
  display.setCursor(15, 55);
  display.setTextSize(1);
  display.println(F("Made by keebasg")); // Credit
  display.display();
  delay(100);
}


