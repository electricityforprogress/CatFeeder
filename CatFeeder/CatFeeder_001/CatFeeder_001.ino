//CatFeeder v 001
//Modification of Lusmo cat feeder, I was unable to replace the 4xD batteries 
//with a power supply and have opted to interface directly with the Motor via
//a MOSFET, and read the feed motor rotations using the built in 'clicker'.


byte motorPin = 9; // this pin controls the LED
byte analogInput = A0; // this is the analog input pin for the potentiometer
byte buttonPin = 7; // this pin controls the button
bool buttonState = LOW;
bool prevButton = LOW;
byte clickCount = 0;
byte clicks = 10; //cycles to continue turning motor, many clicks per cycle
byte runMotor = 0;
unsigned long currentMillis = 0;
unsigned long prevMillis = 0;
// length of time between cycles
int feedingCycle =  20000; //28800000; // 8 hours = 2.88e7 milliseconds
int maxCycle = 5000; // max length of time for any feeding cycle to run (in case clicks fails)
int fade = 2;
bool fadeDir = 1;
byte LEDpins[] = {3,5,6};

void setup() {
  Serial.begin(9600); // start serial for debugging
  
  pinMode(motorPin, OUTPUT); // set motor pin for output
  for(byte i=0;i<3;i++){
    pinMode(LEDpins[i], OUTPUT); // LED  pin
  }
  pinMode(buttonPin, INPUT); // set button pin for input

  TXLED1; // turn off onboard leds on pro micro
  RXLED1; // turn off onboard leds on pro micro

  // startup LED light show
  for(byte i=0;i<3;i++){
    digitalWrite(LEDpins[i],HIGH); delay(250);
    digitalWrite(LEDpins[i],LOW);  delay(250);
    digitalWrite(LEDpins[i],HIGH); delay(250);
    digitalWrite(LEDpins[i],LOW);  delay(250);
  }
 
  // option to press button and feed 'now'
       currentMillis = millis(); //current time
       prevMillis = currentMillis;
   while(currentMillis - prevMillis < 10000) { //ten seconds
     buttonState = digitalRead(buttonPin);
        if(buttonState != prevButton) {
          if(!buttonState) { //from On to Off, press and release
            prevMillis = feedingCycle; //trigger cycle on first loop
          }
        }
      prevButton = buttonState;
      currentMillis = millis(); //current time

      // pulse the LED, really fast

      analogWrite(LEDpins[0],fade); 
      if(fadeDir == 0) fade++; else fade--;
      if(fade > 150) fadeDir = 0;
      if(fade < 2) fadeDir  = 1;
      delay(3);
   }
    digitalWrite(3,LOW); //turn off led
  // continue to main program using feedingCycle of 8 hours

}




void loop() {  
  int analogReading = analogRead(analogInput); // read from the potentiometer

  currentMillis = millis(); //current time

  if((unsigned long)(currentMillis - prevMillis) > feedingCycle) {
    runMotor = 1;
    prevMillis = currentMillis; // track time
    while(runMotor && (unsigned long)(currentMillis - prevMillis) < maxCycle) {
      currentMillis = millis(); //update time
      digitalWrite(motorPin, HIGH); // turn on motor
      digitalWrite(LEDpins[2],HIGH); // turn on LED
      analogReading = analogRead(analogInput); // read from the potentiometer
      buttonState = digitalRead(buttonPin);
      if(buttonState != prevButton) {
        if(!buttonState) { //from On to Off, press and release
          clickCount++; // increase clickCount after button release
          Serial.println(clickCount);
          digitalWrite(LEDpins[1],HIGH); delay(20); digitalWrite(LEDpins[1],LOW); //flash LED
        }
      }
      prevButton = buttonState; // store last value
      if(clickCount > clicks) {
        clickCount = 0; //reset clicks
        runMotor = 0;
        break; // exit while loop after motor running cycle
      }
    }
    clickCount = 0; 
  }
  
  digitalWrite(motorPin,LOW); //turn off motor    
  digitalWrite(LEDpins[2],LOW); // turn off LED
//  Serial.print(clickCount); Serial.print(",");
//  Serial.print(runMotor); Serial.print(",");
//  Serial.print((unsigned long)(currentMillis - prevMillis)); Serial.print(",");
//  Serial.print(currentMillis); Serial.print(","); 
//  Serial.println(prevMillis);
//  

}
