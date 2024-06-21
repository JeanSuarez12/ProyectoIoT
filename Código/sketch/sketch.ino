#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <AccelStepper.h>

#define DHTPIN 19
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4); // Dirección I2C del LCD y tamaño (20x4)

// Driver de motor paso a paso A4988
#define STEP_PIN 16
#define DIR_PIN 17

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// Pin analógico del LDR
#define LDR_AO_PIN 34

// Pines del sensor ultrasónico HC-SR04
#define TRIG_PIN 5
#define ECHO_PIN 18

// Umbrales para temperatura, humedad y luz
#define TEMP_THRESHOLD 25.0 // Ajustar según necesidad
#define HUMIDITY_THRESHOLD 70.0 // Ajustar según necesidad
#define LIGHT_THRESHOLD 2000 // Ajustar según necesidad (1024 es el máximo valor en una lectura analógica)

bool isCurtainOpen = false; // Bandera para rastrear el estado de la cortina

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

  // Inicializar motor paso a paso
  stepper.setMaxSpeed(2000); // Aumentar la velocidad máxima del motor
  stepper.setAcceleration(1000); // Aumentar la aceleración del motor

  // Configurar pin analógico del LDR como entrada
  pinMode(LDR_AO_PIN, INPUT);

  // Configurar pines del HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Mensaje de configuración completa
  Serial.println("Configuración completa.");
}

void loop() {
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

  // Leer valor analógico del LDR
  int ldrAnalogValue = analogRead(LDR_AO_PIN);

  // Mostrar luminosidad en el LCD
  lcd.setCursor(0, 2);
  lcd.print("Luz: ");
  if (ldrAnalogValue > LIGHT_THRESHOLD) {
    lcd.print("Baja ");
  } else {
    lcd.print("Alta ");
  }

  // Controlar cortinas basado en el valor del LDR y las condiciones ambientales
  if (ldrAnalogValue > LIGHT_THRESHOLD || temperature > TEMP_THRESHOLD || humidity > HUMIDITY_THRESHOLD) {
    abrirCortina();
  } else {
    cerrarCortina();
  }

  // Leer distancia del HC-SR04 para detectar cortina completamente cerrada/abierta
  long duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1; // Convertir duración a distancia (cm)

  // Verificar si la cortina está completamente cerrada o abierta
  if (distance > 200 && !isCurtainOpen) { // se colocaron 200 cm a modo de prueba
    // La cortina está completamente abierta
    isCurtainOpen = false;
    detenerCortina();
  } else if (distance < 40 && isCurtainOpen) { // se colocaron 40 cm a modo de prueba
    // La cortina está completamente cerrada
    isCurtainOpen = true;
    detenerCortina();
  }

  // Retardo antes de la próxima iteración del bucle
  delay(500); // Ajustar retardo según necesidad
}

void abrirCortina() {
  if (!isCurtainOpen) {
    // Mostrar estado en el LCD
    lcd.setCursor(0, 3);
    lcd.print("Abriendo cortina    ");
    // Girar motor en sentido horario para abrir la cortina
    digitalWrite(DIR_PIN, HIGH);
    stepper.move(10000); // Mover motor paso a paso
    stepper.runSpeedToPosition(); // Ejecutar el movimiento
  }
}

void cerrarCortina() {
  if (isCurtainOpen) {
    // Mostrar estado en el LCD
    lcd.setCursor(0, 3);
    lcd.print("Cerrando cortina    ");
    // Girar motor en sentido antihorario para cerrar la cortina
    digitalWrite(DIR_PIN, LOW);
    stepper.move(-10000); // Mover motor paso a paso
    stepper.runSpeedToPosition(); // Ejecutar el movimiento
  }
}

void detenerCortina() {
  // Detener el motor
  stepper.stop();
  // Mostrar estado en el LCD
  lcd.setCursor(0, 3);
  lcd.print("Cortina detenida    ");
}
