//CatFeeder v 002
//Modification of Lusmo cat feeder, I was unable to replace the 4xD batteries 
//with a power supply and have opted to interface directly with the Motor via
//a MOSFET, and read the feed motor rotations using the built in 'clicker'.


byte motorPin = 9; // this pin controls the LED
byte analogInput = A0; // this is the analog input pin for the potentiometer
byte buttonPin = 7; // this pin controls the button
byte clickPin = 8; // this pin controls the motor clicker
bool buttonState = LOW;
bool prevButton = LOW;
byte clickCount = 0;
byte clicks = 10; //cycles to continue turning motor, many clicks per cycle
byte runMotor = 0;
unsigned long currentMillis = 0;
unsigned long prevMillis = 0;
// length of time between cycles
unsigned long feedingCycleTime =  28800000; // 8 hours = 2.88e7 milliseconds
int maxCycle = 10000; // max length of time for any feeding cycle to run (in case clicks fails)
byte LEDpins[] = {3,5,6};
byte LEDfade[] = {2,2,2}; 
byte LEDdir[]  = {1,1,1};
byte maxBrightness = 230;
byte minBrightness = 0;
long ledMillis[] = {0,0,0};



void setup() {
  Serial.begin(9600); // start serial for debugging
  Serial.println("Cat Feeder");
  
  pinMode(motorPin, OUTPUT); // set motor pin for output
  for(byte i=0;i<3;i++){
    pinMode(LEDpins[i], OUTPUT); // LED  pins
  }
  pinMode(buttonPin, INPUT); // set button pin for input
  pinMode(clickPin, INPUT); // set clicker pin for input
  
  randomSeed(analogRead(analogInput));
  for(byte i=0;i<3;i++){ //set all LEDs random
    LEDfade[i] = random(minBrightness,maxBrightness);
  }

  TXLED1; // turn off onboard leds on pro micro
  RXLED1; // turn off onboard leds on pro micro

  // startup LED light show
  // option to press button and feed 'now'
       currentMillis = millis(); //current time
       prevMillis = currentMillis;
       for(byte i=0;i<3;i++){ ledMillis[i] = currentMillis; }
       
   while(currentMillis - prevMillis < 10000) { // run lightshow for ten seconds

    if(checkButton(buttonPin)) { // trigger feeding if button pressed
      feedingCycle(clicks);
      ledsOFF();
      break; //exit startup lightshow
    }
 
     //pulse LEDs othewise
      checkLEDshow(5);

      currentMillis = millis(); //current time
   } //while ten seconds
   
   ledsOFF();

  // continue to main program using feedingCycle

}




void loop() {  
  //int analogReading = analogRead(analogInput); // read from the potentiometer

  currentMillis = millis(); //current time

  if((unsigned long)(currentMillis - prevMillis) > feedingCycleTime) {
    Serial.println("FeedingTime");
    feedingCycle(clicks);
  }
  
  digitalWrite(motorPin,LOW); // turn off motor    
  
  checkLEDfade(0,10); // fade green LED

      if(checkButton(buttonPin)) { // on button press, run feeding
        feedingCycle(clicks/2); //half of a feeding amount
        ledsOFF();
      }
//  

}

bool checkButton(int inputPin) {
  buttonState = digitalRead(inputPin);
    if(buttonState != prevButton) { // change in button state
      if(!buttonState) { //from On to Off, press and release
        prevButton = buttonState;
        Serial.println("Button Pressed");
        return 1; // returns true for button 'clicked' and released
      }
    }
  prevButton = buttonState;
  return 0; // return false for no click
}

void checkLEDshow(int fadeTime) {
    for(byte i=0;i<3;i++){
      checkLEDfade(i, random(2,25));
     }
}

void ledsOFF() {
     for(byte i=0;i<3;i++){ //set all LEDs off
        analogWrite(LEDpins[i], minBrightness); 
     }
}

void checkLEDfade(byte ledIndex, int fadeRateTime) {
  if(currentMillis - ledMillis[ledIndex] > fadeRateTime) {
      if(LEDfade[ledIndex] >= maxBrightness) LEDdir[ledIndex] = 0;
      if(LEDfade[ledIndex] <= minBrightness) LEDdir[ledIndex]  = 1;
      if(LEDdir[ledIndex] == 1) LEDfade[ledIndex]++; else LEDfade[ledIndex]--;
      analogWrite(LEDpins[ledIndex],LEDfade[ledIndex]); 
      ledMillis[ledIndex] = currentMillis; //reset fade time
  }
}

void feedingCycle(int clickSize) {
    runMotor = 1;
    ledsOFF(); 
    prevMillis = currentMillis; // track time
    while(runMotor && (unsigned long)(currentMillis - prevMillis) < maxCycle) {
      currentMillis = millis(); //update time
      digitalWrite(motorPin, HIGH); // turn on motor
      checkLEDfade(2, 8);
       if(checkButton(clickPin)) { //clicks from feeder 
         analogWrite(LEDpins[1], 100); delay(20); analogWrite(LEDpins[1], 0);
         clickCount++;
       }
        if(clickCount > clickSize) {
          clickCount = 0; //reset clicks
          runMotor = 0;
          ledsOFF();
          break; // exit while loop after motor running cycle
        }
    }
    
    clickCount = 0; 
}
