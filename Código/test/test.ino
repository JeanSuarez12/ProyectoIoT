#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define DHTPIN 19
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C del LCD y tamaño (16x2)

// Pines del controlador L298N
#define IN1_PIN 16
#define IN2_PIN 17
#define ENA_PIN 4

// Pin analógico del LDR
#define LDR_AO_PIN 34

// Umbrales para temperatura, humedad y luz
#define TEMP_THRESHOLD 22.0 // Ajustar según necesidad
#define LIGHT_THRESHOLD 2600 // Ajustar según necesidad (4095 es el máximo valor en una lectura analógica)

// Información de la red WiFi
const char* ssid = "Alexander";
const char* password = "12345678";

// Información del servidor MQTT (Node-RED)
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

// Variables globales
bool isCurtainOpen = false; // Bandera para rastrear el estado de la cortina
bool manualMode = false; // Bandera para rastrear el modo manual/automático
int motorSpeed = 50; // Velocidad actual del motor (255 es el 100%)

float receivedHumidityIn = 0;
float receivedTemperatureIn = 0;
int receivedLightIn = 0;
bool receivedIsAutomaticMode = false;
bool receivedIsCurtainClosed = false;
bool dataPrinted = false; // Variable para controlar la impresión única

unsigned long curtainMoveStartTime = 0; // Tiempo de inicio del movimiento de la cortina
const unsigned long curtainMoveDuration = 8000;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int retries = 0; // Contador de reintentos
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("No se pudo conectar al WiFi.");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Si se recibe un mensaje de los outputs, procesarlo aquí
  if (String(topic) == "/ThinkIOT/humedadOut") {
    Serial.print("Humedad recibida: ");
    Serial.println(message);
  } else if (String(topic) == "/ThinkIOT/luzOut") {
    Serial.print("Luz recibida: ");
    Serial.println(message);
  } else if (String(topic) == "/ThinkIOT/tempOut") {
    Serial.print("Temperatura recibida: ");
    Serial.println(message);
  } else if (String(topic) == "/ThinkIOT/humedadIn") {
    receivedHumidityIn = message.toFloat();
  } else if (String(topic) == "/ThinkIOT/luzIn") {
    receivedLightIn = message.toInt();
  } else if (String(topic) == "/ThinkIOT/tempIn") {
    receivedTemperatureIn = message.toFloat();
  } else if (String(topic) == "/ThinkIOT/isAutomaticMode") {
    receivedIsAutomaticMode = (message == "true");
  } else if (String(topic) == "/ThinkIOT/isCurtainClose") {
    receivedIsCurtainClosed = (message == "true");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");

      // Suscribirse a los tópicos de salida
      client.subscribe("/ThinkIOT/humedadOut");
      client.subscribe("/ThinkIOT/luzOut");
      client.subscribe("/ThinkIOT/tempOut");
      client.subscribe("/ThinkIOT/humedadIn");
      client.subscribe("/ThinkIOT/luzIn");
      client.subscribe("/ThinkIOT/tempIn");
      client.subscribe("/ThinkIOT/isAutomaticMode");
      client.subscribe("/ThinkIOT/isCurtainClose");
    } else {
      Serial.print("fallo, rc=");
      Serial.print(client.state());
      Serial.println(" intentando en 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  // Iniciar comunicación serial
  Serial.begin(115200);

  // Inicializar comunicación I2C para el LCD
  Wire.begin();
  lcd.init(); // Inicializar el LCD
  lcd.backlight(); // Encender la retroiluminación del LCD

  // Inicializar sensor DHT
  dht.begin();

  // Configurar pines del motor
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);

  // Configurar pin analógico del LDR como entrada
  pinMode(LDR_AO_PIN, INPUT);

  // Conectar a WiFi
  setup_wifi();

  // Verificar conexión WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Error: No se pudo conectar al WiFi.");
  } else {
    // Configurar cliente MQTT
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
  }

  // Mensaje de configuración completa
  Serial.println("Configuración completa.");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leer temperatura y humedad
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Leer valor analógico del LDR
  int ldrAnalogValue = analogRead(LDR_AO_PIN);

  // Publicar los valores en los tópicos MQTT
  client.publish("/ThinkIOT/tempOut", String(temperature).c_str());
  client.publish("/ThinkIOT/humedadOut", String(humidity).c_str());
  client.publish("/ThinkIOT/luzOut", String(ldrAnalogValue).c_str());

  // Mostrar valores en el LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C ");
  lcd.print("H:");
  lcd.print(humidity, 1);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Luz:");
  lcd.print(ldrAnalogValue);
  lcd.print(" lux");

  // Mostrar valores en el monitor serial
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" C, Hum: ");
  Serial.print(humidity);
  Serial.print(" %, Luz: ");
  Serial.println(ldrAnalogValue);

  // Controlar cortinas basado en el valor del LDR y las condiciones ambientales
  if (ldrAnalogValue > LIGHT_THRESHOLD && temperature > TEMP_THRESHOLD && !isCurtainOpen) {
    abrirCortina();
    curtainMoveStartTime = millis();
  } else if((ldrAnalogValue < LIGHT_THRESHOLD || temperature < TEMP_THRESHOLD) && isCurtainOpen) {
    cerrarCortina();
    curtainMoveStartTime = millis();
  }

  // Verificar si ha pasado el tiempo de movimiento de la cortina
  if (millis() - curtainMoveStartTime >= curtainMoveDuration) {
    detenerCortina(); 
  }

  if (!dataPrinted) {
    Serial.print("Humedad recibida: ");
    Serial.println(receivedHumidityIn);
    Serial.print("Luz recibida: ");
    Serial.println(receivedLightIn);
    Serial.print("Temperatura recibida: ");
    Serial.println(receivedTemperatureIn);
    Serial.print("Modo automático recibido: ");
    Serial.println(receivedIsAutomaticMode ? "true" : "false");
    Serial.print("Cortina cerrada recibida: ");
    Serial.println(receivedIsCurtainClosed ? "true" : "false");

    dataPrinted = true;
  }

  // Retardo antes de la próxima iteración del bucle
  delay(500); // Ajustar retardo según necesidad
}

void abrirCortina() {
  if (!isCurtainOpen) {
    // Mostrar estado en el LCD
    lcd.setCursor(0, 1);
    lcd.print("Abriendo cortina");
    // Girar motor en sentido horario para abrir la cortina
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    analogWrite(ENA_PIN, motorSpeed);
    isCurtainOpen = true;
  }
}

void cerrarCortina() {
  if (isCurtainOpen) {
    // Mostrar estado en el LCD
    lcd.setCursor(0, 1);
    lcd.print("Cerrando cortina");
    // Girar motor en sentido antihorario para cerrar la cortina
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    analogWrite(ENA_PIN, motorSpeed);
    isCurtainOpen = false;
  }
}

void detenerCortina() {
  // Detener el motor
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  analogWrite(ENA_PIN, 0); 
  // Mostrar estado en el LCD
  lcd.setCursor(0, 1);
  lcd.print("Cortina detenida");
}
