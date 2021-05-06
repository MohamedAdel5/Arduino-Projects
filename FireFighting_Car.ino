/*
Notes:
1) To rotate right: the right motor stops and the left motor rotates two rotations. [Rotating left does the same logic]
2) To trigger fire fan, open the fire generator switch for more than half a second. [if you close switch before this duration the process will be dismissed] [It does not matter when you close the switch after the fire fan starts]
3) To turn off the fire fan, you have to open the switch for any interval. [As soon as the switch is open the fan will turn off]
4) All the #define preprocessors (Except the last 7 ) are static configurations for the robto design. You can change them as desired.
*/

#include <Stepper.h>

#define robotStepsPerRev 32
#define robotSpeed 60
#define fanSpeed 200

#define maxDistance 40
#define leftUltrasonicEcho 9
#define rightUltrasonicEcho 10
#define midUltrasonicEcho 11
#define ultrasonicTrigger 8


#define ledPin A0
#define powerPin A1
#define firePin 3
#define powerOn 0
#define powerOff 1

#define ON 1
#define OFF 0
#define SUSPICION 2


#define LEFT 1
#define RIGHT 2
#define BACK 3
#define NONE 0

Stepper leftWheel = Stepper(robotStepsPerRev, A4 , A2, A5, A3);
Stepper rightWheel = Stepper(robotStepsPerRev, 1, 13, 0, 2);
Stepper fan = Stepper(robotStepsPerRev, 5, 7, 4, 6);


unsigned long fireCatchInstant;
byte fireAlarm = OFF;
unsigned long time = millis();
unsigned long timeMicros;
unsigned long distance;
byte rotationSteps = 0;
byte rotatingWheel = NONE;
byte currentUltrasonic = 1;
unsigned long ultrasonicTriggerTime = 0;

void setup() {
  	leftWheel.setSpeed(robotSpeed);
	leftWheel.setSpeed(robotSpeed);
  	fan.setSpeed(fanSpeed);
  	pinMode(powerPin, INPUT_PULLUP);
	pinMode(firePin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(firePin), catchFire, CHANGE);
	pinMode(ledPin, OUTPUT);
	pinMode(ultrasonicTrigger, OUTPUT);
	pinMode(leftUltrasonicEcho, INPUT);
	pinMode(rightUltrasonicEcho, INPUT);
	pinMode(midUltrasonicEcho, INPUT);
}

void loop() {
		if(millis() >= time + 1000) {
			digitalWrite(ledPin, !digitalRead(ledPin));
			time = millis();
		}
		if(fireAlarm == SUSPICION && millis() >= fireCatchInstant + 500){
			fireAlarm = ON;		
		}
		if(fireAlarm != ON){	
			if(digitalRead(powerPin) == powerOn){
				timeMicros = micros();
				if(rotatingWheel != NONE){
					if(rotationSteps > 0){
						if(rotatingWheel == LEFT) leftWheel.step(1);
						else rightWheel.step(1);
						rotationSteps--;
					}else{
						rotatingWheel = NONE;					
						leftWheel.setSpeed(robotSpeed);
						rightWheel.setSpeed(robotSpeed);
					}
				}
				else{
					if(ultrasonicTriggerTime == 0){
					digitalWrite( ultrasonicTrigger, LOW );
					ultrasonicTriggerTime = micros();
					}
					else if(timeMicros >= ultrasonicTriggerTime + 5 && digitalRead(ultrasonicTrigger) == LOW){
						digitalWrite( ultrasonicTrigger, HIGH );
						ultrasonicTriggerTime = micros();
					}
					else if(timeMicros >= ultrasonicTriggerTime + 10 && digitalRead(ultrasonicTrigger) == HIGH){
						digitalWrite( ultrasonicTrigger, LOW );
						ultrasonicTriggerTime = 0;
						if(currentUltrasonic == 1){
							distance = getDistance( midUltrasonicEcho);
							if(distance < maxDistance)
							{
								currentUltrasonic = 2;
								leftWheel.setSpeed(0);
								rightWheel.setSpeed(0);
							}else
							{
								rightWheel.step(1);
								leftWheel.step(1);
							}
						}else if (currentUltrasonic == 2){
							distance = getDistance( rightUltrasonicEcho);
							if(distance > maxDistance){
								leftWheel.setSpeed(robotSpeed);
								rotatingWheel = LEFT;
								rotationSteps = robotStepsPerRev*2; 
								currentUltrasonic = 1;
							}
							else{
								currentUltrasonic = 3;
							}
						}else if(currentUltrasonic == 3){
							distance = getDistance( leftUltrasonicEcho );
							if(distance > maxDistance){
								rightWheel.setSpeed(robotSpeed);
								rotatingWheel = RIGHT;
								rotationSteps = robotStepsPerRev*2; 
								currentUltrasonic = 1;
							}
							else{
								rightWheel.setSpeed(robotSpeed);
								leftWheel.setSpeed(robotSpeed);
								rightWheel.step(-1);
								leftWheel.step(-1);
								currentUltrasonic = 2;
							}
						}
					}
				}
			}
		}
		else
		{
			fan.step(10);
		}
}
void catchFire(){
	if(digitalRead(firePin) == HIGH){
		if(fireAlarm == OFF){
			fireCatchInstant = millis();		
			fireAlarm = SUSPICION;
		}
		else{
			fireAlarm = OFF;
		}
	}
	else{
		if(fireAlarm == SUSPICION && millis() < fireCatchInstant + 500){
			fireAlarm = OFF;
		}
	}
}

unsigned long getDistance(byte pin){
	int counter = 0;
	float duration = 0;
	float distance = 0;
	unsigned long time = 0;
	unsigned long then = millis();
	while(  --counter!=0  ){
		if( digitalRead(pin) != 0 ){
		 	time = micros();
			break;
		}
		else{
			if(millis() > then + 100) return 0;
		}
	}
	while( --counter!=0 ){
		if( digitalRead(pin) == 0 ){
		 	duration = micros()-time;
			 break;
		}
		else{
			if(millis() > then + 100) return 0;
		}
	}
	return ( duration/2 ) * 0.0344;
}