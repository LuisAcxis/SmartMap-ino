/*---------------------------------------
  Librerias
-----------------------------------------*/

// Envia || Recibe datos en formato json
#include <ArduinoJson.h>

/*---------------------------------------
  Variables estaticas
-----------------------------------------*/

int sensores[][3] = { // Array con los puertos del/los sensores
//{triger, echo, led}
  {23, 22, 2}, // Sensor 1
  {25, 24, 3}, // Sensor 2
  {27, 26, 4}, // Sensor 3
  {29, 28, 5}, // Sensor 4
  {31, 30, 6}, // Sensor 5
  {33, 32, 7}, // Sensor 6
  {35, 34, 8}, // Sensor 7
  {37, 36, 9}, // Sensor 8
  {39, 38, 10}, // Sensor 9
  {41, 40, 11}, // Sensor 10
  {43, 42, 12}, // Sensor 11
  {45, 44, 13} // Sensor 12
};
int triger = 0; // Establece la posicion triger como posición en el array
int echo = 1; // Establece la posicion echo como posición en el array
int led = 2; // Establece la posicion del/los leds en el array
int sensores_length = (sizeof(sensores) / sizeof(int)) / 3;

int niveles[] = { // Array con los sensores activos segun el nivel
 0, // Nivel 0  (Sensores activados: 0)
 6, // Nivel 1 (Sensores activados: 1 - 6)
 9, // Nivel 2 (Sensores activados: 1 - 9)
 12 // Nivel 3 (Sensores activados: 1 - 12)
};

int distanciaMin = 0; // Distancia minima en cm
int distanciaMax = 10; // Distancia maxima en cm

StaticJsonDocument<200> jsonSend;
StaticJsonDocument<200> jsonReceived;

// Variables de posición dentro de un array

int horas = 0;
int minutos = 1;
int segundos = 2;

/*---------------------------------------
  Variables dinamicas
-----------------------------------------*/

bool is_started;

int nivel;
int sensor_activo;
int puntos;

unsigned long tiempo_lastMillis;
byte tiempo_limite[3];
byte tiempo_restante[3];

/*---------------------------------------
    Setup Events Functions
-----------------------------------------*/

/**
 * @deprecated Inicia las configuraciones iniciales
 */
void initialize(bool setConfig) {
  if(setConfig) {
    Serial.begin(9600);
    randomSeed(analogRead(A0));
    for (int i = 0; i < sensores_length; i++) {
      pinMode(sensores[i][triger], OUTPUT); // Define triger como salida
      pinMode(sensores[i][echo], INPUT); // Define echo como entrada
      pinMode(sensores[i][led], OUTPUT); // Define led como salida
    }
  }
  
  is_started = false;  
  sensor_activo = -1;
  puntos = 0;
}

/**
 * @deprecated Inicia las variables con las configuraciones dinamicas
 */
void start() {
  if(nivel == 0) {
    sendData(5, "Selecciona un nivel antes de iniciar.");
    return false;
  }
  
  if(tiempo_limite[horas] == 0 && tiempo_limite[minutos] == 0 && tiempo_limite[segundos] == 0) {
    sendData(5, "Selecciona el tiempo limite para iniciar.");
    return false;
  } else {
    tiempo_restante[horas] = tiempo_limite[horas];
    tiempo_restante[minutos] = tiempo_limite[minutos];
    tiempo_restante[segundos] = tiempo_limite[segundos];
  }
  
  is_started = true;
  sensor_activo = getSensor();
  tiempo_lastMillis = millis();
}

/**
 * @deprecated Ejecuta el proceso de finalziación
 */
void finish() {
  is_started = false;
}

/**
 * @deprecated Reinicia la aplicación con las configuraciones iniciales
 */
 void reset() {
   initialize(false);
   start();
}

/*---------------------------------------
  setup
-----------------------------------------*/
   
void setup() {
   initialize(true);
}

/*---------------------------------------
  loop
-----------------------------------------*/

void loop() {
  if(Serial.available()) {
    String json = Serial.readString();
    DeserializationError error = deserializeJson(jsonReceived, json);
    if(error) {
      sendData(4, error.c_str());
      return;
    }
    else if(jsonReceived["nivel"]) {
      setNivel(jsonReceived["nivel"]);
    }
    else if(jsonReceived["tiempo"]) {
      if(jsonReceived["tiempo"]["horas"])
        tiempo_limite[horas] = jsonReceived["tiempo"]["horas"];
      if(jsonReceived["tiempo"]["minutos"])
        tiempo_limite[minutos] = jsonReceived["tiempo"]["minutos"];
      if(jsonReceived["tiempo"]["segundos"])
        tiempo_limite[segundos] = jsonReceived["tiempo"]["segundos"];
    }
    else if(jsonReceived["start"]) {
      start();
    }
    else if(jsonReceived["finish"]) {
      finish();
    }
    else if(jsonReceived["reset"]) {
      reset();
    }
  }

  if(is_started) {
    if(!enableSensor(sensor_activo)) {
      disableSensor(sensor_activo);
      sensor_activo = getSensor();
      puntos++;
    }

    if(tiempo_restante[horas] == 0 && tiempo_restante[minutos] == 0 && tiempo_restante[segundos] == 0) {
      sendData(2, "Tiempo agotado.");
      finish();
    } else {
      calculateRemainingTime();
    }
    
    setData();
  }
}

/*---------------------------------------
    Functions
-----------------------------------------*/

/**
 * @deprecated Obtiene un numero aleatorio entre 0 y el total de sensores -1
 * @return int Numero aleatorio
 */
int getSensor() {
  int min = 0;
  int max = niveles[nivel];
  while(true) {
    int random_sensor = random(min, max);
    if(random_sensor != sensor_activo || sensor_activo == -1) {
      return random_sensor;
    }
  }
}

/**
 * @deprecated Establece el proceso que ejecutara un sensor
 * @param  int Especifica el sensor
 * @return false si la proximdiad del objeto se encuentra dentro de la distancia minima y maxima establecida
 */
 boolean enableSensor(int sensor) {
  long getDistancia = generatePulse(sensor);
  if (getDistancia >= distanciaMin && getDistancia <= distanciaMax) {
    return false;
  }
  return true;
 }

/**
 * @deprecated Establece el proceso que ejecutara un sensor al deshabilitado
 * @param  int Especifica el sensor
 */
 void disableSensor(int sensor) {
  digitalWrite(sensores[sensor][led], LOW);
 }

/**
 * @deprecated Genera un pulso hacia un sensor
 * @param  int Especifica el sensor
 * @return distancia en centimetros generada por el sensor
 */
 long generatePulse(int sensor) {
  digitalWrite(sensores[sensor][led], HIGH);
  digitalWrite(sensores[sensor][triger], LOW);
  delayMicroseconds(4);
  digitalWrite(sensores[sensor][triger], HIGH); // Genera el pulso de triger
  delayMicroseconds(8);
  digitalWrite(sensores[sensor][triger], LOW);
  long tiempo = pulseIn(sensores[sensor][echo], HIGH);
  return tiempo * 0.0343 / 2; // Calcula la distancia obtenida en centimetros
 }

/**
 * @deprecated Setea el nivel actual
 * @param  int Especifica el nivel
 */
 void setNivel(int level) {
  nivel = level;
  disableSensor(sensor_activo);
  sensor_activo = getSensor();
 }

/**
 * @deprecated Calcula el tiempo restante
 */
void calculateRemainingTime() {
  int time_elapsed = 1000 - (millis() - tiempo_lastMillis);
  if(time_elapsed > 0) {
    delay(time_elapsed);
  }
  tiempo_lastMillis = millis();
  if(tiempo_restante[segundos] > 0 ) {
    tiempo_restante[segundos]--;
  }
  if(tiempo_restante[segundos] == 0 && tiempo_restante[minutos] > 0) {
    tiempo_restante[minutos]--;
    tiempo_restante[segundos] = 59;
  }
  if(tiempo_restante[minutos] == 0 && tiempo_restante[horas] > 0) {
    tiempo_restante[horas]--;
    tiempo_restante[minutos] = 59;
  }
}

/*---------------------------------------
    json Functions
-----------------------------------------*/

/**
 * @deprecated Establece las funciones a llamar para crear el objeto json
 */
 void setData() {
  jsonNivel();
  jsonPuntos();
  jsonTiempoRestante();
  sendData(2, "");
}

/**
 * @deprecated Envia la informacion al puerto serial
 * @param  int Especifica el codigo de estado
 * @param  String Mensaje a mostrar
 */
void sendData(int code, String message) {
  jsonSend["status"]["code"] = code;
  jsonSend["status"]["message"] = message;
  serializeJson(jsonSend, Serial);
  Serial.println();
}

void jsonNivel() {
  jsonSend["nivel"] = nivel;
}

void jsonPuntos() {
  jsonSend["puntos"] = puntos;
}

void jsonTiempoRestante() {
  jsonSend["tiempo"]["horas"] = tiempo_restante[horas];
  jsonSend["tiempo"]["minutos"] = tiempo_restante[minutos];
  jsonSend["tiempo"]["segundos"] = tiempo_restante[segundos];
}
