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
  {39, 38, 10} // Sensor 9
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

int distancia[] = {
  0, // Distancia minima en cm
  10 // Distancia maxima en cm
};
int distanciaMin = 0; // Establece la posicion de la distancia minima como posición en el array
int distanciaMax = 1; // Establece la posicion de la distancia maxima como posición en el array

int nivel = -1;
int sensor_activo = -1;

/*---------------------------------------
  setup
-----------------------------------------*/
   
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  for (int i = 0; i < sensores_length; i++) {
    pinMode(sensores[i][triger], OUTPUT); // Define triger como salida
    pinMode(sensores[i][echo], INPUT); // Define echo como entrada
    pinMode(sensores[i][led], OUTPUT); // Define led como salida
  }
  
  setLevel(3); // Establece el nivel con el cual se iniciara el sketch
  sensor_activo = getSensor();
}

/*---------------------------------------
  loop
-----------------------------------------*/

void loop() {
  if(Serial.available()) {
    String dataReceived = Serial.readString();
    dataReceived.trim();
    int dataNivel = dataReceived.toInt();
    if(dataNivel >= 0 && dataNivel < (sizeof(niveles) / sizeof(int))) {
      setLevel(dataNivel);
    }
  }
  
  if(!enableSensor(sensor_activo)) {
    disableSensor(sensor_activo);
    sensor_activo = getSensor();
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
  if (getDistancia >= distancia[distanciaMin] && getDistancia <= distancia[distanciaMax]) {
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
 void setLevel(int level) {
  nivel = level;
  disableSensor(sensor_activo);
  sensor_activo = getSensor();
 }
