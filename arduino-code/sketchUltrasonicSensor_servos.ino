#include <Servo.h>  //Load Servo Library
int trigPin = 13; //Sensor Trip pin connected to Arduino pin 13
int echoPin = 11; //Sensor Echo pin connected to Arduino pin 11
int servo1ControlPin = 6; //Servo control line is connected to pin 6
int servo2ControlPin = 5; //Servo control line is connected to pin 6
int pos;

#define filterSamples   12  // filterSamples should  be an odd number, no smaller than 3
float ultrasoundSmoothArray[filterSamples];   // array for holding raw sensor values for ultrasound sensor

float pingTime;  //Duration = time for ping to travel from sensor to target and return
float targetDistance; //Distance to Target in cm
float speedOfSound = 343; //Speed of sound in meters per second when temp is 20 degrees
float servo1Angle = 90; //Variable for the value we want to set servo1 to
float servo2Angle = 90; //Variable for the value we want to set servo2 to
Servo myPointer1;  //Create a servo object called myPointer
Servo myPointer2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(servo1ControlPin, OUTPUT);
  pinMode(servo2ControlPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  myPointer1.attach(servo1ControlPin); //Tell arduino where the servo1 is attached
  myPointer2.attach(servo2ControlPin); //Tell arduino where the servo2 is attached
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(trigPin, LOW); //Set trigger pin low
  delayMicroseconds(2000); //Let signal settle
  digitalWrite(trigPin, HIGH); //Set trigPin high
  delayMicroseconds(15); //Delay in high state
  digitalWrite(trigPin, LOW); //ping has now been sent
  delayMicroseconds(10); //Delay in low state

  pingTime = pulseIn(echoPin, HIGH, 20000);  //pingTime is presented in microceconds CAMBIAR SI NO FUNCIONA
  pingTime = pingTime / 1000000; //Convert pingTime to seconds by dividing by 1000000 (microseconds in a second)
  targetDistance = speedOfSound * pingTime; //This will be in meters per second
  targetDistance = targetDistance / 2; //Remember ping travels to target and back from target, so you must divide by 2 for actual target distance
  targetDistance = targetDistance * 100; //Convert meters to centimeters

  //Pass to function the current RAW ultrasound reading and the accumulator array of previous RAW values
  targetDistance = addValueAndSmooth(targetDistance, ultrasoundSmoothArray);

  //  Serial.print("The Distance to Target is: ");
  //  Serial.print(targetDistance);
  //  Serial.println(" cm");

  Serial.println(targetDistance); // Processing serial port

  //servo1Angle = (150/42) * targetDistance + 15; //Calculate Servo Angle from targetDistance
  servo1Angle = map(targetDistance, 3, 24, 35, 115);
  myPointer1.write(servo1Angle); //write servoAngle to the servo1
  //  Serial.println(servo1Angle);

  //servo2Angle = (150/42) * targetDistance + 15; //Calculate Servo Angle from targetDistance
  servo2Angle =  map(targetDistance, 24, 45, 35, 115);
  myPointer2.write(servo2Angle); //write servoAngle to the servo2
  //  Serial.println(servo2Angle);

  delay(100); //delay tenth of a second to slow things down a little

  if (targetDistance >= 48) {
    //Serial.write(0);
    for (pos = 30; pos < 120; pos += 1) // avanza de 0 a 180 grados
    { myPointer1.write(pos); // mueve a la posición de la variable 'pos'
      myPointer2.write(pos); // mueve a la posición de la variable 'pos'
      delay(15); // espera 15ms
    }
    for (pos = 120; pos >= 1; pos -= 1) // va de 180 a 0 grados
    {
      myPointer1.write(pos);
      myPointer2.write(pos);
      delay(15);
    }
  }
}

float addValueAndSmooth(float rawIn, float *sensSmoothArray) {    // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
  // static int raw[filterSamples];
  static float sorted[filterSamples];
  boolean done;

  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot
  
  // Serial.print("raw = ");

  for (j = 0; j < filterSamples; j++) { // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;                // flag to know when we're done sorting
  while (done != 1) {      // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++) {
      if (sorted[j] > sorted[j + 1]) {    // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j + 1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

  /*
    for (j = 0; j < (filterSamples); j++){    // print the array to debug
      Serial.print(sorted[j]);
      Serial.print("   ");
    }
    Serial.println();
  */

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * 15)  / 100), 1);
  top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j < top; j++) {
    total += sorted[j];  // total remaining indices
    k++;
    // Serial.print(sorted[j]);
    // Serial.print("   ");
  }

  //  Serial.println();
  //  Serial.print("average = ");
  //  Serial.println(total/k);
  return total / k;    // divide by number of samples
}
