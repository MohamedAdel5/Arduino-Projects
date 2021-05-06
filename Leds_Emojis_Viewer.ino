/*
Notes:
1)This code is slightly slow so in order to make sure that 
a key is pressed you should wait till you see any change 
in the LEDs.
(You have to wait about 500 ms after each keypad press)

2)You cannot enter 1212 as a password.

3)The audio component does not work properly, although it
works in tinkerCad simulator.
(I added an Oscope but the wave duration is not precise)
*/

#define DATA_PIN A5
#define DATA_ENABLE 13
#define DRAW 1
#define CLEAR 0
#define KEYPAD_ANALOG_PIN A4
#define DEBOUNCE_TIME 250
#define THRESHOLD 4
#define AUDIO_PIN 9
#define NUMERIC_MODE 0
#define EMOTIONS_MODE 1
#define NONE -1
#define DRAWING 1
#define NOT_DRAWING 0
#define NOT_MATCH 0
#define MATCH 1
#define INVERSE_MATCH 2
#define CHNG_PASSWORD 3

//TONES
#define a 3830 // 261 Hz
#define b 3400 // 294 Hz
#define c 3038 // 329 Hz
#define d 2864 // 349 Hz
#define e 2550 // 392 Hz
#define f 2272 // 440 Hz
#define g 2028 // 493 Hz
#define h 1912 // 523 Hz
#define k 1800 // 523 Hz
#define l 1700 // 523 Hz
unsigned short tonesArr[] = {a, b, c, d, e, f, g, h, k, l};

unsigned long lastValue = 0;
byte mode = NUMERIC_MODE;
byte drawingState = NOT_DRAWING;
byte drawingObject;
byte drawIndex = 0;
byte palindromeCheck = false;

byte password[] = {1, 2, 3, 4};
byte changingPass = false;
byte input[4];
byte inputIndex = 0;

//100 is a terminating character
byte num0[] = {5, 6, 14, 17, 23, 28, 33, 38, 43, 48, 53, 58, 63, 68, 73, 78, 84, 87, 95, 96, 100};
byte num1[] = {5, 6, 14, 15, 16, 23, 24, 25, 26, 33, 34, 35, 36, 45, 46, 55, 56, 65, 66, 75, 76, 85, 86, 95, 96, 100};
byte num2[] = {5, 6, 7, 14, 15, 16, 17, 18, 23, 24, 28, 33, 38, 47, 48, 56, 57, 65, 66, 74, 75, 83, 84, 92, 93, 94, 95, 96, 97, 98, 99, 100};
byte num3[] = {4, 5, 6, 13, 14, 15, 16, 17, 23, 27, 28, 37, 38, 46, 47, 56, 57, 67, 68, 73, 77, 78, 83, 84, 85, 86, 87, 94, 95, 96, 100};
byte num4[] = {6, 7, 15, 16, 17, 24, 25, 26, 27, 33, 34, 36, 37, 43, 46, 47, 52, 56, 57, 62, 63, 64, 65, 66, 67, 68, 76, 77, 86, 87, 96, 97, 100};
byte num5[] = {3, 4, 5, 6, 7, 8, 9, 13, 23, 33, 34, 35, 36, 37, 47, 48, 49, 59, 69, 79, 88, 87, 97, 96, 95, 94, 83, 100};
byte num6[] = {5, 6, 14, 15, 16, 17, 23, 28, 33, 43, 53, 54, 55, 56, 57, 63, 68, 73, 78, 83, 88, 94, 95, 96, 97, 100};
byte num7[] = {2, 3, 4, 5, 6, 7, 8, 9, 19, 28, 38, 37, 46, 56, 65, 75, 74, 83, 92, 100};
byte num8[] = {5, 6, 14, 17, 23, 28, 33, 38, 44, 47, 55, 56, 64, 67, 73, 78, 84, 87, 95, 96, 100};
byte num9[] = {5, 6, 14, 17, 23, 28, 33, 38, 44, 48, 45, 47, 46, 58, 68, 78, 87, 96, 95, 94, 83, 100};
byte emoji0[] = {4, 5, 6, 7, 18, 29, 39, 50, 60, 69, 79, 88, 97, 96, 95, 94, 83, 72, 62, 51, 41, 32, 22, 13, 34, 37, 54, 65, 66, 57, 100};
byte emoji1[] = {4, 5, 6, 7, 18, 29, 39, 50, 60, 69, 79, 88, 97, 96, 95, 94, 83, 72, 62, 51, 41, 32, 22, 13, 34, 37, 64, 55, 56, 67, 100};
byte emoji2[] = {4, 5, 6, 7, 18, 29, 39, 50, 60, 69, 79, 88, 97, 96, 95, 94, 83, 72, 62, 51, 41, 32, 22, 13, 34, 37, 64, 65, 66, 67, 100};
byte emoji3[] = {4, 5, 6, 7, 18, 29, 39, 50, 60, 69, 79, 88, 97, 96, 95, 94, 83, 72, 62, 51, 41, 32, 22, 13, 34, 37, 65, 66, 76, 75, 100};
byte emoji4[] = {4, 5, 6, 7, 18, 29, 39, 50, 60, 69, 79, 88, 97, 96, 95, 94, 83, 72, 62, 51, 41, 32, 22, 13, 34, 37, 100, 43, 34, 37, 48, 64, 65, 66, 67, 75, 76, 100};
byte emoji5[] = {4, 5, 6, 7, 18, 29, 39, 50, 60, 69, 79, 88, 97, 96, 95, 94, 83, 72, 62, 51, 41, 32, 22, 13, 34, 37, 64, 65, 66, 67, 75, 76, 85, 86, 100};
byte emoji6[] = {4, 5, 6, 7, 18, 29, 39, 50, 60, 69, 79, 88, 97, 96, 95, 94, 83, 72, 62, 51, 41, 32, 22, 13, 34, 35, 37, 54, 65, 66, 57, 100};
byte emoji7[] = {4, 5, 6, 7, 18, 29, 39, 50, 60, 69, 79, 88, 97, 96, 95, 94, 83, 72, 62, 51, 41, 32, 22, 13, 33, 34, 44, 43, 35, 36, 37, 38, 47, 48, 64, 75, 76, 67, 100};
byte emoji8[] = {4, 5, 6, 7, 18, 29, 39, 50, 60, 69, 79, 88, 97, 96, 95, 94, 83, 72, 62, 51, 41, 32, 22, 13, 34, 37, 100};
byte emoji9[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50, 60, 69, 79, 88, 97, 96, 95, 94, 83, 72, 62, 51, 41, 31, 21, 11, 23, 34, 45, 46, 37, 28, 43, 48, 75, 76, 100};

byte *numArr[] = {num0, num1, num2, num3, num4, num5, num6, num7, num8, num9};
byte *emojiArr[] = {emoji0, emoji1, emoji2, emoji3, emoji4, emoji5, emoji6, emoji7, emoji8, emoji9};
void setup()
{
	pinMode(DATA_ENABLE, OUTPUT);
	digitalWrite(DATA_ENABLE, HIGH);

	pinMode(DATA_PIN, OUTPUT);
	digitalWrite(DATA_PIN, LOW);

	DDRD = 0xFF;
	PORTD = 0;

	pinMode(AUDIO_PIN, OUTPUT);
	analogWrite(AUDIO_PIN, 0);
	pinMode(KEYPAD_ANALOG_PIN, INPUT);

	//Turn off all leds (CLEAR all D-Latches because they might be SET from previous runs)
	clearDisplay();

	for (byte i = 0; i < 10; ++i)
	{
		mapNumbersToMatrix(numArr[i]);
		mapNumbersToMatrix(emojiArr[i]);
	}
	startSelfTesting();
}

void loop()
{
	char pressedKey = NONE;
	if (millis() - lastValue > 250)
	{

		pressedKey = getKeypadPressedKey();
		if (pressedKey != NONE)
		{
			playAudio(tonesArr[pressedKey], 0);
			input[inputIndex] = pressedKey;
			++inputIndex;
			if (inputIndex >= 4)
			{
				if (changingPass == true)
				{
					setNewPassword();
					changingPass = false;

					inputIndex = 0;
				}
				else
				{
					inputIndex = 0;
					byte check = checkInputPassword();
					if (check == MATCH)
					{

						if (palindromeCheck == true)
						{
							mode = (mode == EMOTIONS_MODE) ? NUMERIC_MODE : EMOTIONS_MODE;
						}
						else
						{
							mode = EMOTIONS_MODE;
						}
						playAudio(tonesArr[pressedKey], 1);
					}
					else if (check == INVERSE_MATCH)
					{
						playAudio(tonesArr[pressedKey], 1);
						mode = NUMERIC_MODE;
					}
					else if (check == CHNG_PASSWORD)
					{
						changingPass = true;
					}
					else
					{
						for (byte i = 0; i < 3; ++i)
						{
							input[i] = input[i + 1];
						}
						inputIndex = 3;
					}
				}
			}
		}
	}

	if (drawingState != DRAWING)
	{

		if (pressedKey != NONE && pressedKey != drawingObject)
		{
			drawingState = DRAWING;
			drawingObject = pressedKey;
			clearDisplay();
		}
	}
	else // If drawingState == DRAWING
	{
		//If the user pressed another key while drawing --> clear the previous drawing and start drawing the new object.
		if (pressedKey != NONE && pressedKey != drawingObject)
		{
			clearDisplay();
			drawingObject = pressedKey;
			drawIndex = 0;
		}

		byte **objectArr = (mode == NUMERIC_MODE) ? numArr : emojiArr;
		digitalWrite(DATA_PIN, HIGH);
		PORTD = objectArr[drawingObject][drawIndex];
		delayMicroseconds(10);
		digitalWrite(DATA_ENABLE, LOW);
		delayMicroseconds(10);
		digitalWrite(DATA_ENABLE, HIGH);
		delay(100);
		++drawIndex;
		if (objectArr[drawingObject][drawIndex] == 100)
		{
			drawIndex = 0;
			drawingState = NOT_DRAWING;
		}
	}
}

/*
	This function maps numbers calculated from the design to numbers the fits in my code.
*/
void mapNumbersToMatrix(byte *arr)
{

	byte i = 0;
	while (arr[i] != 100)
	{
		byte mod = arr[i] % 10;
		switch (mod)
		{
		case 1:
			arr[i] += 8;
			break;
		case 2:
			arr[i] += 6;
			break;
		case 3:
			arr[i] += 4;
			break;
		case 4:
			arr[i] += 2;
			break;
		case 5:
			arr[i] += 0;
			break;
		case 6:
			arr[i] -= 2;
			break;
		case 7:
			arr[i] -= 4;
			break;
		case 8:
			arr[i] -= 6;
			break;
		case 9:
			arr[i] -= 8;
			break;
		case 0:
			arr[i] -= 10;
			break;
		}
		++i;
	}
}
//command => DRAW or CLEAR
//row => 0-9
void drawRow(byte row, byte command)
{
	row *= 10;
	digitalWrite(DATA_PIN, (command == DRAW) ? HIGH : LOW);
	for (byte i = 0; i < 10; ++i)
	{
		PORTD = row + i;
		delay(1);
		digitalWrite(DATA_ENABLE, LOW);
		delay(1);
		digitalWrite(DATA_ENABLE, HIGH);
	}
}
//command => DRAW or CLEAR
//col => 0-9
void drawColumn(byte col, byte command)
{
	digitalWrite(DATA_PIN, (command == DRAW) ? HIGH : LOW);
	for (byte i = 0; i < 10; ++i)
	{
		PORTD = col + i * 10;
		delay(1);
		digitalWrite(DATA_ENABLE, LOW);
		delay(1);
		digitalWrite(DATA_ENABLE, HIGH);
	}
}
void startSelfTesting()
{
	for (byte i = 0; i < 10; ++i)
	{
		drawColumn(i, DRAW);
		delay(200);
		drawColumn(i, CLEAR);
	}
	for (byte i = 0; i < 10; ++i)
	{
		drawRow(i, DRAW);
		delay(200);
		drawRow(i, CLEAR);
	}
}
char getKeypadPressedKey()
{
	int analogInput = analogRead(KEYPAD_ANALOG_PIN);
	if ((analogInput > 0) && (millis() - lastValue >= DEBOUNCE_TIME))
	{
		lastValue = millis();
		if ((analogInput > (719 - THRESHOLD)) && (analogInput < (719 + THRESHOLD)))
			return 0;
		if ((analogInput > (929 - THRESHOLD)) && (analogInput < (929 + THRESHOLD)))
			return 1;
		if ((analogInput > (911 - THRESHOLD)) && (analogInput < (911 + THRESHOLD)))
			return 2;
		if ((analogInput > (894 - THRESHOLD)) && (analogInput < (894 + THRESHOLD)))
			return 3;
		if ((analogInput > (852 - THRESHOLD)) && (analogInput < (852 + THRESHOLD)))
			return 4;
		if ((analogInput > (837 - THRESHOLD)) && (analogInput < (837 + THRESHOLD)))
			return 5;
		if ((analogInput > (822 - THRESHOLD)) && (analogInput < (822 + THRESHOLD)))
			return 6;
		if ((analogInput > (786 - THRESHOLD)) && (analogInput < (786 + THRESHOLD)))
			return 7;
		if ((analogInput > (773 - THRESHOLD)) && (analogInput < (773 + THRESHOLD)))
			return 8;
		if ((analogInput > (761 - THRESHOLD)) && (analogInput < (761 + THRESHOLD)))
			return 9;
	}
	else
	{
		return NONE;
	}
}

void clearDisplay()
{
	byte j = 0;
	digitalWrite(DATA_PIN, LOW);
	while (j != 100)
	{
		PORTD = j;
		delayMicroseconds(50);
		digitalWrite(DATA_ENABLE, LOW);
		delayMicroseconds(50);
		digitalWrite(DATA_ENABLE, HIGH);
		++j;
	}
}
byte checkInputPassword()
{
	if (input[0] == 1 && input[1] == 2 && input[2] == 1 && input[3] == 2)
	{
		return CHNG_PASSWORD;
	}
	byte matchState = MATCH;
	for (byte i = 0; i < 4; ++i)
	{
		if (password[i] != input[i])
		{
			matchState = NOT_MATCH;
			break;
		}
	}
	if (matchState == MATCH)
	{
		return MATCH;
	}

	matchState = INVERSE_MATCH;
	byte j = 0;
	for (char i = 3; i >= 0; --i)
	{
		if (password[i] != input[j])
		{
			matchState = NOT_MATCH;
			break;
		}
		++j;
	}
	if (matchState == INVERSE_MATCH)
	{
		return INVERSE_MATCH;
	}
	else
		return NOT_MATCH;
}

bool setNewPassword()
{

	if (input[0] == 1 && input[1] == 2 && input[2] == 1 && input[3] == 2)
		return false;

	if (input[0] == input[3] && input[1] == input[2])
	{
		palindromeCheck = true;
	}
	else
		palindromeCheck = false;
	for (byte i = 0; i < 4; ++i)
	{
		password[i] = input[i];
	}
	return true;
}
void playAudio(byte key, byte time)
{
	unsigned long duration = 16 * 10000;
	if (time == 1)
		duration *= 2;
	unsigned long startTime = micros();
	while (micros() - startTime < duration)
	{
		digitalWrite(AUDIO_PIN, HIGH);
		delayMicroseconds(key);
		digitalWrite(AUDIO_PIN, LOW);
		delayMicroseconds(key);
	}
}