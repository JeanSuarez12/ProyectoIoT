#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define DHTPIN 19
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C del LCD y tamaño (16x2)

// Pines del controlador de motor L298N
#define IN1 16
#define IN2 17
#define ENA 18

// Información de la red WiFi
const char* ssid = "CASA CABALLERO_PLUS";
const char* password = "Amazonas";

// Información del servidor MQTT
const char* mqttServer = "192.168.1.36";
const int mqttPort = 1883;
const char* mqttUser = "user";
const char* mqttPassword = "contraseña";

WiFiClient espClient;
PubSubClient client(espClient);

// Variables globales
bool isCurtainOpen = false; // Bandera para rastrear el estado de la cortina
bool manualMode = false; // Bandera para rastrear el modo manual/automático

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
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
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

  // Inicializar pines del L298N
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Conectar a WiFi
  setup_wifi();

  // Verificar conexión WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Error: No se pudo conectar al WiFi.");
  } else {
    // Configurar cliente MQTT
    client.setServer(mqttServer, mqttPort);
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

  if (!manualMode) {
    // Leer temperatura y humedad
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Mostrar temperatura y humedad en el LCD
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(humidity);
    lcd.print(" %");

    // Controlar cortinas basado en la temperatura y la humedad
    if (temperature > TEMP_THRESHOLD || humidity > HUMIDITY_THRESHOLD) {
      abrirCortina();
    } else {
      cerrarCortina();
    }
  }

  // Retardo antes de la próxima iteración del bucle
  delay(500); // Ajustar retardo según necesidad
}

void abrirCortina() {
  if (!isCurtainOpen) {
    // Mostrar estado en el LCD
    lcd.setCursor(0, 1);
    lcd.print("Abriendo cortina    ");
    // Girar motor en sentido horario para abrir la cortina
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 255); // Ajustar PWM según necesidad
  }
}

void cerrarCortina() {
  if (isCurtainOpen) {
    // Mostrar estado en el LCD
    lcd.setCursor(0, 1);
    lcd.print("Cerrando cortina    ");
    // Girar motor en sentido antihorario para cerrar la cortina
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, 255); // Ajustar PWM según necesidad
  }
}

void detenerCortina() {
  // Detener el motor
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0); // Detener el motor
  // Mostrar estado en el LCD
  lcd.setCursor(0, 1);
  lcd.print("Cortina detenida    ");
}
