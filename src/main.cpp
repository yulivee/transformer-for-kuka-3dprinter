#include <Arduino.h>
#include <AccelStepper.h>
#include <QueueList.h>

// Create a Queue for motor_values
QueueList <float> queue;

// Define the stepper and the pins it will use
AccelStepper stepper1(AccelStepper::DRIVER, 10, 8);

// Define our three input button pins
#define  LEFT_PIN  4
#define  STOP_PIN  3
#define  RIGHT_PIN 2

// Define our analog pot input pin
#define  SPEED_PIN 6

// Define our maximum and minimum speed in steps per second (scale pot to these)
#define MAX_SPEED 2000
#define MIN_SPEED 0.1 //0.1
#define MOTOR_CNT 500
#define PWM_CNT 2001
#define QUEUE_DEPTH 20

const int analogInPin = A7;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the poti
int outputValue = 0;        // value output to the PWM (analog out)

static float motor_speed = 0.0;                      // Holds average motor speed in steps/second
static float motor_current_speed = 0.0;              // Holds current reading of motor speed in steps/second
static int   motor_analog_read_counter = MOTOR_CNT;  // Counts down to 0 to fire analog read
static int   pwm_analog_read_counter = PWM_CNT;      // Counts down to 0 to fire analog read
static char  sign = 1;                               // Holds -1, 1 or 0 to turn the motor on/off and control direction
static int   analog_value = 0;                       // Holds raw analog value.

void setup() {
  stepper1.setMaxSpeed(10000.0);
 
  // Set up the three button inputs, with pullups
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(STOP_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);

}

void loop() {

  if (pwm_analog_read_counter > 0) {
	  // wait 2 milliseconds before the next loop
	  // for the analog-to-digital converter to settle
	  // after the last reading:
	  // delay(1);
	  pwm_analog_read_counter--;
  } else {
	  pwm_analog_read_counter = PWM_CNT;
	  // read the analog in value:
	  sensorValue = analogRead(analogInPin);
	  // map it to the range of the analog out:
	  outputValue = map(sensorValue, 0, 1023, 0, 255);
	  // change the analog out value:
	  analogWrite(analogOutPin, outputValue);
  }
 
  // If a switch is pushed down (low), set the sign value appropriately
  if (digitalRead(LEFT_PIN) == 0) {
    sign = 1;
  }
  else if (digitalRead(RIGHT_PIN) == 0) {    
    sign = -1;
  }
  else if (digitalRead(STOP_PIN) == 0) {
    sign = 0;
  }

  // We only want to read the pot every so often (because it takes a long time we don't
  // want to do it every time through the main loop).  
  if (motor_analog_read_counter > 0) {
    motor_analog_read_counter--;
  }
  else {
    motor_analog_read_counter = MOTOR_CNT;
    // Now read the pot (from 0 to 1023)
    analog_value = analogRead(SPEED_PIN);

    // we do not have a clean 0V signal, so to stop the motor we manually
    // set to speed to 0 if we receive below 103 readings of the ADC
    // this is the first ~ 1.005 V we ignore
    if ( analog_value < 103 ) {
	    stepper1.setSpeed(0);
    } else {	   
	    // Give the stepper a chance to step if it needs to
	    stepper1.runSpeed();
	    //  And scale the pot's value from min to max speeds
	    motor_current_speed = sign * ((analog_value/1023.0) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED;
	    queue.push(motor_current_speed);

	    if ( queue.count() == QUEUE_DEPTH + 1 ) {
		queue.pop();
	    }

	    motor_speed = queue.average();

	    // Update the stepper to run at this new speed
	    stepper1.setSpeed(motor_speed);
	    }

  }

  // This will run the stepper at a constant speed
  stepper1.runSpeed();

}
