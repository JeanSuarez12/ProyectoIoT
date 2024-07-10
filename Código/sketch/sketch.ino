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

// Pines del sensor ultrasónico HC-SR04
#define TRIG_PIN 5
#define ECHO_PIN 18

// Umbrales para temperatura, humedad y luz
#define TEMP_THRESHOLD 25.0 // Ajustar según necesidad
#define HUMIDITY_THRESHOLD 80.0 // Ajustar según necesidad
#define LIGHT_THRESHOLD 2000 // Ajustar según necesidad (4095 es el máximo valor en una lectura analógica)

// Información de la red WiFi
const char* ssid = "Alexander";
const char* password = "12345678";

// Información del servidor MQTT (Node-RED)
const char* mqtt_server = "DIRECCION_DEL_SERVIDOR_MQTT";

WiFiClient espClient;
PubSubClient client(espClient);

// Variables globales
bool isCurtainOpen = false; // Bandera para rastrear el estado de la cortina
bool manualMode = false; // Bandera para rastrear el modo manual/automático
int motorSpeed = 2; // Velocidad actual del motor (255 es el 100%)

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
  String incoming = "";
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  
  Serial.print("Mensaje recibido de ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(incoming);
  
  // Cambiar entre modo manual y automático
  if (String(topic) == "cortina/mode") {
    if (incoming == "manual") {
      manualMode = true;
    } else if (incoming == "automatico") {
      manualMode = false;
    }
  }
  
  // Control manual de la cortina
  if (String(topic) == "cortina/control") {
    if (manualMode) {
      if (incoming == "abrir") {
        abrirCortina();
      } else if (incoming == "cerrar") {
        cerrarCortina();
      } else if (incoming == "detener") {
        detenerCortina();
      }
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar al servidor MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("conectado");
      client.subscribe("cortina/mode");
      client.subscribe("cortina/control");
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}




void setup() {
  // Iniciar comunicación serial
  Serial.begin(115200);

  // Inicializar comunicación I2C para el LCD
  Wire.begin();

  // Inicializar LCD
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

  // Configurar pines del HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Conectar a WiFi
  setup_wifi();

  // Verificar conexión WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Error: No se pudo conectar al WiFi.");
    // ESP.restart(); // Descomenta esta línea para reiniciar el ESP32 en caso de fallo
  } else {
    // Configurar cliente MQTT
    //client.setServer(mqtt_server, 1883);
    //client.setCallback(callback);
  }

  // Mensaje de configuración completa
  Serial.println("Configuración completa.");
}

void loop() {
  //if (!client.connected()) {
  //  reconnect();
  //}
  //client.loop();

  if (!manualMode) {
    // Leer temperatura y humedad
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Leer valor analógico del LDR
    int ldrAnalogValue = analogRead(LDR_AO_PIN);

    // Leer distancia del HC-SR04 para detectar cortina completamente cerrada/abierta
    long duration, distance;
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH);
    distance = (duration / 2) / 29.1; // Convertir duración a distancia (cm)



    // Mostrar valores en el LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temperature, 1);
    lcd.print(" C ");
    lcd.print("H:");
    lcd.print(humidity, 1);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("Luz:");
    lcd.print(ldrAnalogValue);
    lcd.print(" lux");


    /*
    lcd.setCursor(0, 1);
    lcd.print("Luz: ");
    if (ldrAnalogValue < LIGHT_THRESHOLD) {
      lcd.print("Baja ");
    } else {
      lcd.print("Alta ");
    }
    */


    // Mostrar valores en el monitor serial
    Serial.print("Distancia: ");
    Serial.print(distance);
    Serial.print(" cm, Temp: ");
    Serial.print(temperature);
    Serial.print(" C, Hum: ");
    Serial.print(humidity);
    Serial.print(" %, Luz: ");
    Serial.println(ldrAnalogValue);



    // Controlar cortinas basado en el valor del LDR y las condiciones ambientales
    if (ldrAnalogValue > LIGHT_THRESHOLD && temperature > TEMP_THRESHOLD && humidity > HUMIDITY_THRESHOLD) {
      abrirCortina();
    } else {
      cerrarCortina();
    }


    // Verificar si la cortina está completamente cerrada o abierta
    if (distance > 32 && !isCurtainOpen) { // Se colocaron 32 cm a modo de prueba
      // La cortina está completamente abierta
      isCurtainOpen = true;
      detenerCortina();
    } else if (distance < 6 && isCurtainOpen) { // Se colocaron 6 cm a modo de prueba
      // La cortina está completamente cerrada
      isCurtainOpen = false;
      detenerCortina();
    }
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
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    analogWrite(ENA_PIN, motorSpeed); 
  }
}

void cerrarCortina() {
  if (isCurtainOpen) {
    // Mostrar estado en el LCD
    lcd.setCursor(0, 1);
    // Girar motor en sentido antihorario para cerrar la cortina
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    analogWrite(ENA_PIN, motorSpeed); 
  }
}

void detenerCortina() {
  // Detener el motor
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  analogWrite(ENA_PIN, 0); 
  // Mostrar estado en el LCD
  lcd.setCursor(0, 1);
  lcd.print("Cortina detenida    ");
}