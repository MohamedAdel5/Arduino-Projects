#define HEATER A5
#define RED_DECORATE A4
#define YELLOW_DECORATE A3

#define KEYPAD_PIN A0
#define KEYPAD_DEBOUNCE_TIME 250
#define KEYPAD_THRESHOLD 4

#define DECORATING_ARM 8
#define EGG_VALVE 9
#define VANILLA_VALVE 10
#define SUGAR_VALVE 11
#define FLOUR_VALVE 12

#define STEPPER_PIN_COUNT 4
#define STEPPER_STEPS_COUNT 32
// clang-format off
#define STEPPER_1_PINS {2, 0, 3, 1}
#define STEPPER_2_PINS {2, 0, 3, 1}
#define STEPPER_3_PINS {6, 4, 7, 5}
// clang-format on

#define STEPPER_SELECTION_PIN 13
#define SELECT_STEPPER_1 1
#define SELECT_STEPPER_2 0

//if(machine_current_state < 10) 		making cake phase
//else if(machine_current_state < 20) 	baking cake phase
//else if(machine_current_state < 30) 	decorating cake phase
#define MAKING_CAKE 10
#define BAKING_CAKE 20
#define DECORATING_CAKE 30

#define MAKING_CAKE_CRACK_EGG 0
#define MAKING_CAKE_MIXER_ONLY_PHASE_1_START 1
#define MAKING_CAKE_MIXER_ONLY_PHASE_1_END 2
#define MAKING_CAKE_ADD_VANILLA_START 3
#define MAKING_CAKE_ADD_VANILLA_END 4
#define MAKING_CAKE_ADD_SUGAR 5
#define MAKING_CAKE_MIXER_ONLY_PHASE_2_START 6
#define MAKING_CAKE_MIXER_ONLY_PHASE_2_END 7
#define MAKING_CAKE_REDUCE_MIXER_SPEED 8
#define MAKING_CAKE_ADDING_FLOUR 9

#define BAKING_CAKE_START_PRODUCTION_LINE 10
#define BAKING_CAKE_START_HEATER 11
#define BAKING_CAKE_COOLING_DOWN 12

#define DECORATING_CAKE_WAIT_FOR_USER_INPUT 20
#define DECORATING_CAKE_DECORATION 21

#define SLEEP_FOREVER 100

#define MIXER_SPEED 600 //rpm

unsigned long STEPPER_1_LAST_STEP_TIME = 0;
unsigned short STEPPER_1_SPEED = 0;
byte STEPPER_1_STEPS_COUNT = 0;
byte STEPPER_1_REV_COUNT = 0;
byte STEPPER_1_CURRENT_STATE = 0;

unsigned long STEPPER_2_LAST_STEP_TIME = 0;
unsigned short STEPPER_2_SPEED = 0;
byte STEPPER_2_STEPS_COUNT = 0;
byte STEPPER_2_REV_COUNT = 0;
byte STEPPER_2_CURRENT_STATE = 0;

unsigned long STEPPER_3_LAST_STEP_TIME = 0;
unsigned short STEPPER_3_SPEED = 0;
byte STEPPER_3_STEPS_COUNT = 0;
byte STEPPER_3_REV_COUNT = 0;
byte STEPPER_3_CURRENT_STATE = 0;

unsigned long VANILLA_VALVE_OPEN_TIME = 0;
byte VANILLA_VALVE_CLOSED = false;

byte DECORATION_MODE = 0;

byte CURRENT_PHASE = 0;

unsigned long KEYPAD_LAST_VALUE = millis();

void setup()
{
	DDRB = 0xFF;
	DDRD = 0xFF;
	DDRC = 0xFE;
	//Serial.begin(9600);

	servo_rotate(0, DECORATING_ARM);
	servo_rotate(0, EGG_VALVE);
	servo_rotate(0, VANILLA_VALVE);
	servo_rotate(0, SUGAR_VALVE);
	servo_rotate(0, FLOUR_VALVE);
	delay(2000);
}

void loop()
{

	switch (CURRENT_PHASE)
	{

	case MAKING_CAKE_CRACK_EGG:
		crackEgg();
		CURRENT_PHASE = MAKING_CAKE_MIXER_ONLY_PHASE_1_START;
		break;

	case MAKING_CAKE_MIXER_ONLY_PHASE_1_START:
		mixerStart(5, MIXER_SPEED); // 10 rev/s = 600 rev/min
		CURRENT_PHASE = MAKING_CAKE_MIXER_ONLY_PHASE_1_END;
		break;

	case MAKING_CAKE_MIXER_ONLY_PHASE_1_END:
		if (is_mixer_idle())
			CURRENT_PHASE = MAKING_CAKE_ADD_VANILLA_START;
		break;

	case MAKING_CAKE_ADD_VANILLA_START:
		mixerStart(4, MIXER_SPEED); //Continue for 4 more rotations
		addVanilla();
		CURRENT_PHASE = MAKING_CAKE_ADD_VANILLA_END;
		break;

	case MAKING_CAKE_ADD_VANILLA_END:
		if (is_vanilla_time_over())
		{
			if(!VANILLA_VALVE_CLOSED)
			{
				closeValve(VANILLA_VALVE);
				VANILLA_VALVE_CLOSED = true;
			}
			if (is_mixer_idle())
			{
				delay(200);
				CURRENT_PHASE = MAKING_CAKE_ADD_SUGAR;
			}
		}
		break;

	case MAKING_CAKE_ADD_SUGAR:
		addSugar();
		CURRENT_PHASE = MAKING_CAKE_MIXER_ONLY_PHASE_2_START;
		break;

	case MAKING_CAKE_MIXER_ONLY_PHASE_2_START:
		mixerStart(10, MIXER_SPEED);
		CURRENT_PHASE = MAKING_CAKE_MIXER_ONLY_PHASE_2_END;
		break;

	case MAKING_CAKE_MIXER_ONLY_PHASE_2_END:
		if (is_mixer_idle())
			CURRENT_PHASE = MAKING_CAKE_ADDING_FLOUR;
		break;

	case MAKING_CAKE_ADDING_FLOUR:
		addingFlour();
		CURRENT_PHASE = BAKING_CAKE_START_PRODUCTION_LINE;
		break;

	case BAKING_CAKE_START_PRODUCTION_LINE:
		production_line_full_rotation();
		CURRENT_PHASE = BAKING_CAKE_START_HEATER;
		break;

	case BAKING_CAKE_START_HEATER:
		heater();
		CURRENT_PHASE = BAKING_CAKE_COOLING_DOWN;
		break;

	case BAKING_CAKE_COOLING_DOWN:
		delay(1000);
		CURRENT_PHASE = DECORATING_CAKE_WAIT_FOR_USER_INPUT;
		break;

	case DECORATING_CAKE_WAIT_FOR_USER_INPUT:
		production_line_full_rotation();
		getSelectedDecoration();
		CURRENT_PHASE = DECORATING_CAKE_DECORATION;
		break;

	case DECORATING_CAKE_DECORATION:
		applyDecoration();
		CURRENT_PHASE = SLEEP_FOREVER;
		break;

	case SLEEP_FOREVER:
		while (1)
			;
		break;
	}

	mixer_movement();
}
void applyDecoration()
{
	switch (DECORATION_MODE)
	{
	case 1:
		servo_rotate(135, DECORATING_ARM);
		delay(500);
		digitalWrite(RED_DECORATE, 1);
		rotating_plate(1);
		digitalWrite(RED_DECORATE, 0);
		servo_rotate(150, DECORATING_ARM);
		delay(500);
		digitalWrite(YELLOW_DECORATE, 1);
		rotating_plate(1);
		digitalWrite(YELLOW_DECORATE, 0);
		servo_rotate(0, DECORATING_ARM);
		delay(500);

		break;

	case 2:
		for (byte i = 0; i < 8; ++i)
		{
			if (i % 2 == 0)
			{
				servo_rotate(135, DECORATING_ARM);
				delay(500);
				digitalWrite(RED_DECORATE, 1);
				rotating_plate(8);
				digitalWrite(RED_DECORATE, 0);
			}
			else
			{
				servo_rotate(180, DECORATING_ARM);
				delay(500);
				digitalWrite(YELLOW_DECORATE, 1);
				rotating_plate(8);
				digitalWrite(YELLOW_DECORATE, 0);
			}
		}
		servo_rotate(0, DECORATING_ARM);
		delay(500);
		break;

	case 3:
		servo_rotate(135, DECORATING_ARM);
		delay(500);
		for (byte i = 0; i < 16; ++i)
		{
			digitalWrite(RED_DECORATE, 1);
			rotating_plate(16);
			digitalWrite(RED_DECORATE, 0);
			delay(300);
		}
		servo_rotate(180, DECORATING_ARM);
		delay(500);
		digitalWrite(YELLOW_DECORATE, 1);
		delay(500);
		digitalWrite(YELLOW_DECORATE, 0);
		servo_rotate(0, DECORATING_ARM);
		delay(500);
		break;

	case 4:
		servo_rotate(135, DECORATING_ARM);
		delay(500);
		for (byte i = 0; i < 16; ++i)
		{
			if (i % 2 == 0)
			{
				digitalWrite(RED_DECORATE, 1);
				delay(200);
				digitalWrite(RED_DECORATE, 0);
			}
			else
			{
				digitalWrite(YELLOW_DECORATE, 1);
				delay(200);
				digitalWrite(YELLOW_DECORATE, 0);
			}
			rotating_plate(16);
		}

		servo_rotate(180, DECORATING_ARM);
		delay(500);
		digitalWrite(RED_DECORATE, 1);
		rotating_plate(1);
		digitalWrite(RED_DECORATE, 0);

		servo_rotate(170, DECORATING_ARM);
		delay(500);
		digitalWrite(RED_DECORATE, 1);
		rotating_plate(1);
		digitalWrite(RED_DECORATE, 0);

		servo_rotate(0, DECORATING_ARM);
		delay(500);
		break;
	}
}
void getSelectedDecoration()
{
	byte key = keypad_getKey();
	while (!key)
	{
		key = keypad_getKey();
	}
	DECORATION_MODE = key;
}

void heater()
{
	digitalWrite(HEATER, HIGH);
	delay(1000);
	digitalWrite(HEATER, LOW);
}

void rotating_plate(byte revolutionDivisor)
{

	STEPPER_3_SPEED = 100;
	STEPPER_3_REV_COUNT = 1;
	STEPPER_3_STEPS_COUNT = STEPPER_STEPS_COUNT / revolutionDivisor;
	while (STEPPER_3_REV_COUNT != 0)
	{
		byte stepper_pins[4] = STEPPER_3_PINS;
		stepper_step(stepper_pins, &STEPPER_3_CURRENT_STATE, &STEPPER_3_LAST_STEP_TIME, STEPPER_3_SPEED, &STEPPER_3_STEPS_COUNT);
		if (STEPPER_3_STEPS_COUNT == 0)
		{
			STEPPER_3_REV_COUNT--;
			if (STEPPER_3_REV_COUNT != 0)
				STEPPER_3_STEPS_COUNT = STEPPER_STEPS_COUNT;
		}
	}
}

void production_line_full_rotation()
{
	digitalWrite(STEPPER_SELECTION_PIN, SELECT_STEPPER_2);
	STEPPER_2_SPEED = 100;
	STEPPER_2_REV_COUNT = 1;
	STEPPER_2_STEPS_COUNT = STEPPER_STEPS_COUNT;
	while (STEPPER_2_REV_COUNT != 0)
	{
		byte stepper_pins[4] = STEPPER_2_PINS;
		stepper_step(stepper_pins, &STEPPER_2_CURRENT_STATE, &STEPPER_2_LAST_STEP_TIME, STEPPER_2_SPEED, &STEPPER_2_STEPS_COUNT);
		if (STEPPER_2_STEPS_COUNT == 0)
		{
			STEPPER_2_REV_COUNT--;
			if (STEPPER_2_REV_COUNT != 0)
				STEPPER_2_STEPS_COUNT = STEPPER_STEPS_COUNT;
		}
	}
}
void addingFlour()
{
	for (byte i = 0; i < 3; ++i)
	{
		openValve(FLOUR_VALVE);
		delay(100);
		closeValve(FLOUR_VALVE);

		mixerStart(4, MIXER_SPEED / 2);
		while (!is_mixer_idle())
			mixer_movement();
	}
}

void addSugar()
{
	openValve(SUGAR_VALVE);
	delay(200);
	closeValve(SUGAR_VALVE);
}

void mixerStart(byte revolutions, unsigned short speed)
{
	digitalWrite(STEPPER_SELECTION_PIN, SELECT_STEPPER_1);
	STEPPER_1_SPEED = speed;
	STEPPER_1_REV_COUNT = revolutions;
	STEPPER_1_STEPS_COUNT = STEPPER_STEPS_COUNT;
}

void mixer_movement()
{
	if (STEPPER_1_REV_COUNT != 0)
	{
		byte stepper_pins[4] = STEPPER_1_PINS;
		stepper_step(stepper_pins, &STEPPER_1_CURRENT_STATE, &STEPPER_1_LAST_STEP_TIME, STEPPER_1_SPEED, &STEPPER_1_STEPS_COUNT);
		if (STEPPER_1_STEPS_COUNT == 0)
		{
			STEPPER_1_REV_COUNT--;
			if (STEPPER_1_REV_COUNT != 0)
				STEPPER_1_STEPS_COUNT = STEPPER_STEPS_COUNT;
		}
	}
}

bool is_mixer_idle()
{
	return STEPPER_1_REV_COUNT == 0;
}

bool is_vanilla_time_over()
{
	return millis() - VANILLA_VALVE_OPEN_TIME >= 100;
}

void addVanilla()
{
	openValve(VANILLA_VALVE);
	VANILLA_VALVE_OPEN_TIME = millis();
}

void crackEgg()
{
	openValve(EGG_VALVE);
	delay(500);
	closeValve(EGG_VALVE);
}

void openValve(byte valve)
{
	servo_rotate(180, valve);
}

void closeValve(byte valve)
{
	servo_rotate(0, valve);
}

void servo_rotate(byte angle, byte servo_pin)
{
	unsigned short delayTest = angle * 5.556 + 1000;
	digitalWrite(servo_pin, 1);
	delayMicroseconds(delayTest);
	digitalWrite(servo_pin, 0);
	delay(20 - (delayTest / 1000));
}

void stepper_step(byte *stepper_pins, byte *stepper_current_state, unsigned long *stepper_last_step_time, unsigned short stepper_speed, byte *stepper_steps_count)
{
	unsigned long delay = 60L * 1000L * 1000L / STEPPER_STEPS_COUNT / stepper_speed;

	// Serial.print("delay: ");
	// Serial.println(delay);

	if (micros() - *stepper_last_step_time >= delay)
	{
		switch (*stepper_current_state)
		{
		case 0: // 1010
			// Serial.println(1);

			digitalWrite(stepper_pins[0], 1);
			digitalWrite(stepper_pins[1], 0);
			digitalWrite(stepper_pins[2], 1);
			digitalWrite(stepper_pins[3], 0);
			break;
		case 1: // 0110
			// Serial.println(2);

			digitalWrite(stepper_pins[0], 0);
			digitalWrite(stepper_pins[1], 1);
			digitalWrite(stepper_pins[2], 1);
			digitalWrite(stepper_pins[3], 0);
			break;
		case 2: //0101
			// Serial.println(3);

			digitalWrite(stepper_pins[0], 0);
			digitalWrite(stepper_pins[1], 1);
			digitalWrite(stepper_pins[2], 0);
			digitalWrite(stepper_pins[3], 1);
			break;
		case 3: //1001
			// Serial.println(4);

			digitalWrite(stepper_pins[0], 1);
			digitalWrite(stepper_pins[1], 0);
			digitalWrite(stepper_pins[2], 0);
			digitalWrite(stepper_pins[3], 1);
			break;
		}
		(*stepper_steps_count) -= 1;
		(*stepper_current_state) += 1;
		if ((*stepper_current_state) == 4)
			(*stepper_current_state) = 0;
		(*stepper_last_step_time) = micros();
	}
}

char keypad_getKey()
{
	int analogInput = analogRead(KEYPAD_PIN);
	// Serial.println(analogInput);
	if ((analogInput > 0) && (millis() - KEYPAD_LAST_VALUE >= KEYPAD_DEBOUNCE_TIME))
	{
		KEYPAD_LAST_VALUE = millis();
		if ((analogInput > (931 - KEYPAD_THRESHOLD)) && (analogInput < (931 + KEYPAD_THRESHOLD)))
			return 1;
		if ((analogInput > (855 - KEYPAD_THRESHOLD)) && (analogInput < (855 + KEYPAD_THRESHOLD)))
			return 2;
		if ((analogInput > (790 - KEYPAD_THRESHOLD)) && (analogInput < (790 + KEYPAD_THRESHOLD)))
			return 3;
		if ((analogInput > (735 - KEYPAD_THRESHOLD)) && (analogInput < (735 + KEYPAD_THRESHOLD)))
			return 4;
	}
	else
	{
		return 0;
	}
}