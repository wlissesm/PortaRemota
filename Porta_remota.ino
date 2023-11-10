
#include "AsyncUDP.h"
#include <Arduino.h>
#ifdef ESP32
#include "WiFi.h"
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
#define ACCESS_POINT 1
const char* ssid = "#TRILOBIT";
const char* password = "coorp*2023";

IPAddress ip(192, 168, 17, 100); //IP FIXO
IPAddress gateway(192, 168, 17, 1); //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
IPAddress subnet(255, 255, 0, 0); //MASCARA DE REDE
AsyncUDP udp;

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";
const char* PARAM_INPUT_3 = "input3";
int LED_BUILTIN = 2;
/*<form action="/get">
    input1: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    input2: <input type="text" name="input2">
    <input type="submit" value="Submit">
  </form><br>*/


// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>TRILOBIT</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body bgcolor="#1b3f71">
  <form action="/get">
  <body text ="white"><h1>TRILOBIT 2024 </h1>
   <p> ... </p>
    
    DADO: <input type="text" name="input3">
    <input type="submit" value="Enviar">
  </form>


<p> Em desenvolvimento </p> 

</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);//flex serial
  Serial.println("inicio!");
  Serial2.println("123456789");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(23, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);  //isso para fixar o ip
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    Serial2.println(inputMessage);
    if (inputMessage == "1233") {
      digitalWrite(23, HIGH);
      delay(1000);
      digitalWrite(23, LOW);
      request->send(200, "text/html", "<h1>ENVIO 2 </h1>  <p> a porta abrira </p>  ("
                    + inputParam + ") with value: " + inputMessage +
                    "<br><a href=\"/\">Return to Home Page</a>");
    }

    if (inputMessage == "ABREPORTA") {
      udp.connect(IPAddress(192, 168, 17, 178), 17000) ;
      //delay(1000);
      //Send broadcast on port 1234
      //udp.broadcastTo("Anyone here?", 1234);
      uint8_t buffer[50] = {0xEA, 0x13, 0X7C, 0x63, 0x01, 0x0C, 0XE5}; // EMERGENCIA   eSTA FALTANDO DOIS BITS PARA FUNCIONAR 
      udp.write(buffer, 7);
      udp.close();

      request->send(200, "text/html", "<h1>ENVIO 2 </h1>  <p> a porta abrira </p>  ("
                    + inputParam + ") with value: " + inputMessage +
                    "<br><a href=\"/\">Return to Home Page</a>");
    }

    request->send(200, "text/html", "<h1>ENVIO CONCLUIDO </h1>  ("
                  + inputParam + ") with value: " + inputMessage +
                  "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);

  server.begin();
}

void loop() {
  //digitalWrite(LED_BUILTIN, HIGH);
  //   delay(100);
  //   digitalWrite(LED_BUILTIN, LOW);
  //   delay(100);


}
