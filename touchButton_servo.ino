#include <avr/sleep.h>
#include <TimerOne.h>

// initializing servo position and pin variables
int pos = 0;
int servoPin = 3;

// digital pin 2 has a touchbutton attached to it
int touchButton = 2;

// pot connected to pin A0, to get ON time
int potPin = A0;
int potValue;
int divBy4PotValue;

// intializing led pin and state -- for debug
int ledPin = 13;
bool ledState = 1;

// creating a volatile count variable that gets updated by timer interrupt
volatile unsigned long interruptCount = 0;

// the setup routine runs once when you press reset:
void setup() {
  // make the touchbutton and pot  pin an input; led and servo pin as output
  pinMode(touchButton, INPUT);
  pinMode(potPin,INPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(servoPin,OUTPUT);
  digitalWrite(servoPin,LOW);
  //Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  moveServo_to_turn_off();
  attachInterrupt(0,moveServo_to_turn_on,RISING);
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_cpu();

  // after waking up
  sleep_disable();     
  detachInterrupt(0);
  delay(10);

  potValue=analogRead(potPin);
  divBy4PotValue = potValue >> 2;

  // initializing timer 1 with 8 second period
  Timer1.initialize(4000000); 
  Timer1.attachInterrupt(incrCount,4000000);
  unsigned long countCopy;
  countCopy = interruptCount;
  while(countCopy < divBy4PotValue){
    PRR = PRR | 0b00100000;
    sleep_enable();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_cpu();

    PRR = PRR & 0b00000000;
    noInterrupts();
    countCopy = interruptCount;
    interrupts();
  }
  Timer1.detachInterrupt();
  Timer1.stop();
  interruptCount = 0;
}

void moveServo_to_turn_on (void) {
  for(int i=0;i<100;i++){
    digitalWrite(servoPin,HIGH);
    delayMicroseconds(2600);
    digitalWrite(servoPin,LOW);
    delayMicroseconds(20000-2600);
    delay(15);
  }
}
void moveServo_to_turn_off (void) {
  for(int i=0;i<100;i++){
    digitalWrite(servoPin,HIGH);
    delayMicroseconds(600);
    digitalWrite(servoPin,LOW);
    delayMicroseconds(20000-600);
    delay(15);
  }
}
void incrCount (void) {
  interruptCount = interruptCount + 1;
  digitalWrite(ledPin,ledState);
  ledState=!ledState;
}

