/*---------------------------------------
  Atributos
-----------------------------------------*/
int sensores[][3] = { // Array con los puertos del/los sensores
//{triger, echo, led}
  {2, 3, 4}, // Sensor 1
  {2, 5, 6}, // Sensor 2
  {2, 7, 8}, // Sensor 3
  {2, 9, 10}, // Sensor 4
  {2, 11, 12}, // Sensor 5
  {2, 13, 14}, // Sensor 6
  {2, 15, 16}, // Sensor 7
  {2, 17, 18}, // Sensor 8
  {2, 19, 20}, // Sensor 9
  {2, 21, 22}, // Sensor 10
  {2, 23, 24}, // Sensor 11
  {2, 25, 26} // Sensor 12
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

int sensor_activo;

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
    if(random_number != sensor_activo) {
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
  digitalWrite(sensores[sensor][led], HIGH);
  digitalWrite(sensores[sensor][triger], LOW);
  delayMicroseconds(5);
  digitalWrite(sensores[sensor][triger], HIGH); // Genera el pulso de triger
  delayMicroseconds(10); 
  digitalWrite(sensores[sensor][triger], LOW);

  long tiempo = pulseIn(sensores[sensor][echo], HIGH);
  long getDistancia = tiempo * 0.0343 / 2; // Calcula la distancia obtenida en centimetros
  if (getDistancia >= distancia[distanciaMin] && getDistancia <= distancia[distanciaMax]) {
    return false;
  }
  return true;
 }

 void disableSensor(int sensor) {
  digitalWrite(sensores[sensor][led], LOW);
 }
