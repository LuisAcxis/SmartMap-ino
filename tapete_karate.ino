/*---------------------------------------
  Atributos
-----------------------------------------*/
int sensores[][3] = { // Array con los puertos del/los sensores
//{triger, echo, led}
  {53, 52, 51}, // Sensor 1
  {50, 49, 48}, // Sensor 2
  {47, 46, 45}, // Sensor 3
  {44, 43, 42}, // Sensor 4
  {41, 40, 39} // Sensor 5
};
int distancia[] = {
  0, // Distancia minima en cm
  10 // Distancia maxima en cm
};

int triger = 0; // Establece la posicion triger como posición en el array
int echo = 1; // Establece la posicion echo como posición en el array
int led = 2; // Establece la posicion del/los leds en el array
int sensores_length = (sizeof(sensores) / sizeof(int)) / 3;

int distanciaMin = 0; // Establece la posicion de la distancia minima como posición en el array
int distanciaMax = 1; // Establece la posicion de la distancia maxima como posición en el array

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
  sensor_activo = getRandomSensor();
}

/*---------------------------------------
  loop
-----------------------------------------*/

void loop() {
  if(!enableSensor(sensor_activo)) {
    disableSensor(sensor_activo);
    sensor_activo = getRandomSensor();
  }
}

/*---------------------------------------
    Functions
-----------------------------------------*/

/**
 * @deprecated Obtiene un numero aleatorio entre 0 y el total de sensores -1
 * @return int Numero aleatorio
 */
int getRandomSensor() {
  int min = 0;
  int max = sensores_length;
  while(true) {
    int random_number = random(min, max);
    if(random_number != sensor_activo || sensor_activo == -1) {
      return random_number;
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
  delayMicroseconds(10); 
  digitalWrite(sensores[sensor][triger], LOW);
  long tiempo = pulseIn(sensores[sensor][echo], HIGH);
  return tiempo * 0.0343 / 2; // Calcula la distancia obtenida en centimetros
 }
