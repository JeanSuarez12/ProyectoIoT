#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <DHTesp.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include <ESP32Servo.h>

#define PIR_PIN 14
#define DHT_INT_PIN 27
#define DHT_EXT_PIN 26
#define LDR_EXT_PIN 33
#define SERVO_PIN 19
#define LED_PIN 18

#define DHTTYPE DHTesp::DHT22
#define TEMP1 22
#define TEMP2 30

DHTesp dht_int;
DHTesp dht_ext;
Servo servo;

WiFiClientSecure net;
PubSubClient client(net);

bool cortinaAbierta = false;
bool focoEncendido = false;

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  dht_int.setup(DHT_INT_PIN, DHTTYPE);
  dht_ext.setup(DHT_EXT_PIN, DHTTYPE);
  servo.attach(SERVO_PIN);
  CerrarCortina(); // Inicialmente aseguramos que la cortina esté cerrada

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Verificar conexión a internet
  IPAddress ip;
  if (WiFi.hostByName("amazon.com", ip)) {
    Serial.print("amazon.com IP: ");
    Serial.println(ip);
  } else {
    Serial.println("DNS lookup failed for amazon.com");
  }

  net.setInsecure(); // Solo para desarrollo, no usar en producción
  client.setServer(AWS_IOT_ENDPOINT, 8883);
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
  client.setCallback(callback);
  client.setBufferSize(512);
  client.setKeepAlive(60);

  connectAWS();
}

void connectAWS() {
  int retries = 0;
  while (!client.connected() && retries < 5) {
    Serial.println("Connecting to AWS IoT...");
    if (client.connect(THINGNAME)) {
      Serial.println("Connected to AWS IoT");
      client.subscribe(AWS_IOT_TOPIC_SUB);
    } else {
      Serial.print("Failed to connect, status code = ");
      Serial.println(client.state());
      retries++;
      Serial.print("Retrying in 5 seconds... Attempt ");
      Serial.println(retries);
      delay(5000);
    }
  }

  if (!client.connected()) {
    Serial.println("Failed to connect to AWS IoT after 5 attempts. Continuing without connection.");
  }
}

void PrenderFoco() {
  digitalWrite(LED_PIN, HIGH);
  focoEncendido = true;
}

void ApagarFoco() {
  digitalWrite(LED_PIN, LOW);
  focoEncendido = false;
}

void AbrirCortina() {
  if (!cortinaAbierta) {
    for (int pos = 0; pos <= 90; pos += 1) {
      servo.write(pos);
      delay(15);
    }
    cortinaAbierta = true;
  }
}

void CerrarCortina() {
  if (cortinaAbierta) {
    for (int pos = 90; pos >= 0; pos -= 1) {
      servo.write(pos);
      delay(15);
    }
    cortinaAbierta = false;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en el tema: ");
  Serial.println(topic);

  if (strcmp(topic, AWS_IOT_TOPIC_SUB) == 0) {
    if (payload[0] == '1') {
      AbrirCortina();
    } else if (payload[0] == '0') {
      CerrarCortina();
    }
  }
}

void loop() {
  if (!client.connected()) {
    Serial.println("Connection lost. Reconnecting...");
    connectAWS();
  }
  client.loop();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("WiFi reconnected");
  }

  float temp_int = dht_int.getTemperature();
  float temp_ext = dht_ext.getTemperature();
  int ldr_ext = analogRead(LDR_EXT_PIN);
  bool pir = digitalRead(PIR_PIN);

  StaticJsonDocument<512> doc;
  doc["temperature_internal"] = temp_int;
  doc["temperature_external"] = temp_ext;
  doc["light_external"] = ldr_ext;
  doc["pir"] = pir;
  doc["foco"] = focoEncendido ? "true" : "false";
  doc["cortina"] = cortinaAbierta ? "true" : "false";

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  
  Serial.print("Publishing message: ");
  Serial.println(jsonBuffer);
  if (client.publish(AWS_IOT_TOPIC_PUB, jsonBuffer)) {
    Serial.println("Publish successful");
  } else {
    Serial.println("Publish failed");
  }

  // Lógica local basada en los sensores y PIR
  if (pir) {
    if (ldr_ext < 2000 || temp_int < TEMP1) {
      PrenderFoco();
    }
    if (ldr_ext > 2000) {
      if ((temp_int < TEMP1 && temp_ext > TEMP1) || (temp_int > TEMP2 && temp_ext < TEMP2)) {
        AbrirCortina();
      } else {
      CerrarCortina();  // Cierra la cortina si no se cumplen las condiciones para mantenerla abierta
      }
    }
  } else {
    ApagarFoco();
    if (ldr_ext > 2000) {
      if ((temp_int < TEMP1 && temp_ext > TEMP1) || (temp_int > TEMP2 && temp_ext < TEMP2)) {
        AbrirCortina(); // Abre la cortina si cumple alguna de las condiciones para abrir
      } else {
        CerrarCortina(); // Cierra la cortina si no se cumplen las condiciones para mantenerla abierta
      }
    } else {
      CerrarCortina(); // Cierra la cortina si no hay suficiente luz exterior
    }
  }

  delay(2000);
}
