#include <Stepper.h>
#define KEYPAD_ANALOG_PIN A3
#define DEBOUNCE_TIME 250
#define THRESHOLD 4
#define LED_PIN A4
#define WEIGHT_PIN A5
#define ELEVATOR_SPEED 60
#define ELEVATOR_MOTOR_STEPS_PER_REV 32
#define ON 1
#define OFF 0


#define DOWN 0
#define UP 1
#define IDLE 2



#define WAITING 1
#define NOT_WAITING 0



unsigned long lastValue = millis();
Stepper elevatorMotor = Stepper(ELEVATOR_MOTOR_STEPS_PER_REV, 2, 0, 8, 1);
unsigned char currentFloor = 1;
unsigned char callQueue[8] = {0,0,0,0,0,0,0,0};
byte weightAlarm = OFF;
byte elevatorState = IDLE;

byte keypadIsPressed = false;
byte pushButtonIsPressed = false;
unsigned long keypadPressTime;
unsigned long pushButtonPressTime;

byte waitState = NOT_WAITING;
unsigned long waitTime;

byte groundCall = false;

unsigned short elevatorCurrentStep = 0;

void setup(){

	DDRD |= 0xF7;
	PORTD |= 0x08;

	DDRB &= 0xE0;
	PORTB |= 0x1F;
	pinMode(A0, INPUT_PULLUP);
	pinMode(A1, INPUT_PULLUP);
	pinMode(A2, INPUT_PULLUP);


	pinMode(WEIGHT_PIN, INPUT);
	pinMode(LED_PIN, OUTPUT);
	pinMode(KEYPAD_ANALOG_PIN, INPUT);

	attachInterrupt(digitalPinToInterrupt(3), interruptHandler, CHANGE);
	elevatorMotor.setSpeed(ELEVATOR_SPEED);
	sevenSegmentDisplay(currentFloor);
}

void loop()
{
	unsigned char  weight = calculateWeight();
	if(weight > 70){
		digitalWrite(LED_PIN, HIGH);
		weightAlarm = ON;
	}
	else{
		digitalWrite(LED_PIN, LOW);
		weightAlarm = OFF;
	}

	unsigned char calledFloorByKeypad = getKeypadPressedKey();
	if(calledFloorByKeypad != 0){

		if(keypadIsPressed == true){
			if(millis() >= keypadPressTime + 200){
				addCallToQueue(calledFloorByKeypad);
			}
		}else{
			keypadIsPressed = true;
			keypadPressTime = millis();
		}
	}
	else{
		keypadIsPressed = false;
	}

	unsigned char calledFloorByPushButtons = getPressedPushButton();
	if(groundCall){
		calledFloorByPushButtons = 1;
	}
	if(calledFloorByPushButtons != 0 || groundCall){
		if(pushButtonIsPressed == true){
			if(millis() >= pushButtonPressTime + 200){
				addCallToQueue(calledFloorByPushButtons);
			}
		}else{
			pushButtonIsPressed = true;
			pushButtonPressTime = millis();
		}
	}
	else{
		pushButtonIsPressed = false;
	}


	if(elevatorState != IDLE && weightAlarm == OFF && waitState != WAITING)
	{

		if(elevatorCurrentStep < ELEVATOR_MOTOR_STEPS_PER_REV){
			if(elevatorState == UP){
				elevatorMotor.step(1);
			}else if(elevatorState == DOWN){
				elevatorMotor.step(-1);
			}
			++elevatorCurrentStep;
		}
		else{
			elevatorCurrentStep = 0;
			currentFloor = (elevatorState == UP)?currentFloor+1:currentFloor-1;
			sevenSegmentDisplay(currentFloor);
			if(floorExistsInCallQueue(currentFloor)){
				releaseCallFromQueue(currentFloor);
				waitState = WAITING;
				waitTime = millis();
			}

			if(!callQueueIsEmpty()){
				getNextFloor(currentFloor);
			}else{
				elevatorState = IDLE;
			}

		}
	}

	if(waitState == WAITING && millis() > waitTime + 1000){
		waitState = NOT_WAITING;
	}

}

void addCallToQueue(unsigned char floorNumber){
	if(elevatorState == IDLE) {
		if(floorNumber != currentFloor){
			if(floorNumber > currentFloor) elevatorState = UP;
			else if(floorNumber < currentFloor) elevatorState = DOWN;
			callQueue[floorNumber - 1] = 1;
		}
	}
	else{
		callQueue[floorNumber - 1] = 1;
	}

}

void releaseCallFromQueue(unsigned char floorNumber){
	callQueue[floorNumber - 1] = 0;
}

bool floorExistsInCallQueue(unsigned char floorNumber){
	return callQueue[floorNumber - 1];
}

bool callQueueIsEmpty(){
	for(byte i = 0; i < 8; ++i)
	{
		if(callQueue[i] == 1)
		{
			return false;
		}
	}
	return true;
}

unsigned char getNextFloor(unsigned char currentFloor){
	if(elevatorState == UP){

		for(char i = currentFloor;  i < 8; ++i){
			if(callQueue[i] == 1) return i+1;
		}


		for(char i = currentFloor-2;  i >= 0; --i){
			if(callQueue[i] == 1) {
				elevatorState = DOWN;
				return i+1;
			}
		}

	}else{


		for(char i = currentFloor-2;  i >= 0; --i){
			if(callQueue[i] == 1) return i+1;
		}


		for(char i = currentFloor;  i < 8; ++i){
			if(callQueue[i] == 1){
					elevatorState = UP;
					return i+1;
			}
		}
	}
}


void sevenSegmentDisplay(unsigned char number){
	if(number > 8) return;
	PORTD &= 0x0F;
	PORTD |= (number << 4);
}

char getKeypadPressedKey(){
	int analogInput = analogRead(KEYPAD_ANALOG_PIN);
	if((analogInput > 0)&&(millis() - lastValue >= DEBOUNCE_TIME)){
		lastValue = millis();
		if((analogInput > (929 -THRESHOLD)) &&(analogInput < (929 + THRESHOLD)))
			return 1;
		if((analogInput > (911 -THRESHOLD)) &&(analogInput < (911 + THRESHOLD)))
			return 2;
		if((analogInput > (852 -THRESHOLD)) &&(analogInput < (852 + THRESHOLD)))
			return 3;
		if((analogInput > (837 -THRESHOLD)) &&(analogInput < (837 + THRESHOLD)))
			return 4;
		if((analogInput > (786 -THRESHOLD)) &&(analogInput < (786 + THRESHOLD)))
			return 5;
		if((analogInput > (773 -THRESHOLD)) &&(analogInput < (773 + THRESHOLD)))
			return 6;
		if((analogInput > (730 -THRESHOLD)) &&(analogInput < (730 + THRESHOLD)))
			return 7;
		if((analogInput > (719 -THRESHOLD)) &&(analogInput < (719 + THRESHOLD)))
			return 8;
	}else{
		return 0;
	}
}

char getPressedPushButton(){

	if(!digitalRead(9)){
		return 2;
	}
	else if(!digitalRead(10)){
		return 3;
	}
	else if(!digitalRead(11)){
		return 4;
	}
	else if(!digitalRead(12)){
		return 5;
	}
	else if(!digitalRead(A2)){
		return 6;
	}
	else if(!digitalRead(A1)){
		return 7;
	}
	else if(!digitalRead(A0)){
		return 8;
	}
	else{
		return 0;
	}
}

unsigned char calculateWeight(){
	return (float)analogRead(WEIGHT_PIN)*100/1023;
}

void interruptHandler(){
	if(digitalRead(3) == LOW){
		groundCall = true;
	}else{
		groundCall = false;
	}
}
