#include "FastLED.h"
#include <EEPROM.h>
#include <VirtualWire.h>

#define NUM_PEOPLE 10//this needs to be looked at
#define NUM_TOTAL_PEOPLE 30
#define LED_DATA_PIN A5
#define SENSOR_PIN A0
#define LIGHT_THRESH 600
#define LIGHT_HIST 50
#define LED_BRIGHTNESS 63//WAS 63 ----------------------------------------------
#define LED_RPT_BRIGHTNESS 23//was 23

#define OUT 0 //red
#define IN 1 //green
#define HOL 2 //blue
#define ILL 3 //yellow
#define NULL 4 //black

#define PRESS_SHORT 50//in milliseconds
#define PRESS_LONG 1000
#define PRESS_SUPER_LONG 2500
#define PRESS_ULTRA_LONG 4000

#define RF_DAT 12
#define ROLE_PIN A1

char *controller;//array for sending packets

unsigned long buttonPressedTimer = 0;
byte buttonPressed = 200;
boolean buttonPressedFlag = false;
byte buttonPressedOrigState = 0;

int easterEggCounter = 0;//presscounter for easter egg
unsigned long easterEggTimer = 0;//timer for easter egg delay
boolean easterEggFlag = false;//flag for timer
int easterEggTimeout = 5000;//5 second delay before effect
int easterEggPresses =50;//after the 50th press

unsigned long timerRF = 0;
int timeRF = 2000;

//position is LED number
typedef struct
{
  byte state;
} stateArray;

stateArray myArray[NUM_PEOPLE];
stateArray myRPTArray[NUM_TOTAL_PEOPLE];

CRGB leds[NUM_TOTAL_PEOPLE];

int buttons[]={2,3,4,5,6,7,8,16,17,18};
                                      
// The various roles supported by this sketch
typedef enum {ROLE_INVALID = 0, 
              ROLE_RPT, 
              ROLE_SWPOD,
              ROLE_POD1,
              ROLE_POD2,
              ROLE_POD3,
              ROLE_POD4,
              ROLE_POD5} role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Repeater", "PodSW", "Pod 1", "Pod 2", "Pod 3", "Pod 4", "Pod 5"};

// The role of the current running sketch
role_e myRole;

void setup()
{ 
  //define role
  pinMode(ROLE_PIN, INPUT);
  delay(20);//wait for analogue pins to stabalise
  int role = analogRead(ROLE_PIN);
  //set role
  myRole = (role_e)map(role,0,1024,1,8);
  pinMode(LED_DATA_PIN,OUTPUT);
  loadFromEEPROM();
  //radio stuff
  pinMode(13, OUTPUT);
  vw_set_ptt_inverted(true); // Required for DR3100  
  if(myRole == ROLE_RPT)
  {
    pinMode(RF_DAT, INPUT);
    vw_set_rx_pin(RF_DAT);
    vw_setup(4000);  // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
  }
  else
  {
    pinMode(RF_DAT, OUTPUT);
    vw_set_tx_pin(RF_DAT);
    vw_setup(4000);  // Bits per sec
  }

  //other setup
  if(myRole == ROLE_RPT)
  {
    FastLED.addLeds<NEOPIXEL, LED_DATA_PIN, GRB>(leds, NUM_TOTAL_PEOPLE);//strip has different colour order
    LEDS.setBrightness(LED_RPT_BRIGHTNESS);
  }
  else//everyone else
  {
    FastLED.addLeds<NEOPIXEL, LED_DATA_PIN, RGB>(leds, NUM_PEOPLE);
    //set up inputs
    for(int i = 0; i < 10; i++)
    {
      pinMode(buttons[i], INPUT);           // set pin to input
      digitalWrite(buttons[i], HIGH);       // turn on pullup r
    }
    pinMode(A7, INPUT);
    randomSeed(analogRead(7));
    LEDS.setBrightness(LED_BRIGHTNESS);
  }
  Serial.begin(9600);
  Serial.print("I am ");
  Serial.println(role_friendly_name[myRole]);
  rainbowCycle(5);  
}

void loop()
{
  if(myRole != ROLE_RPT)
  {
    checkButtons();
    easterEgg();
  }
  checkRF();
  showStatus();//do this every time
}

void checkRF()
{
  if ( myRole == ROLE_RPT )//if we are a RPT
  {
    delay(250);//just to be open to recieve messages
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
      digitalWrite(13,1);//data recieved  
      Serial.print("From: ");
      Serial.println(role_friendly_name[buf[0] - '0']);
      CRGB myColour = Wheel(random(0,256));
      int arraySize = 0;
      switch(buf[0] - '0')//from...
      {
        case ROLE_SWPOD:
  	  arraySize = 10;
  	  break;
        case ROLE_POD1:
  	case ROLE_POD2:
  	case ROLE_POD3:
  	case ROLE_POD4:
  	case ROLE_POD5:
  	  arraySize = 4;
  	  break;
	case 42: rainbowCycle(5); break;
        case 43: colorWipe(myColour,50); colorWipe2(CRGB::Black,50); break;
        case 44: rainbow(5); break;
        case 45: colorWipe2(myColour,50); colorWipe(CRGB::Black,50); break;
        case 46: allRainbow(5); break;
        case 47: LEDS.setBrightness(0xFF); runSparkle(myColour, 20, 50); LEDS.setBrightness(LED_RPT_BRIGHTNESS); break;
  	default:
  	  //do nothing
  	  break;
      }
      int bufferOffset = 0;//these are array offsets for the RPT array
      switch(buf[0] - '0')//from...
      {
        case ROLE_SWPOD: bufferOffset = 0; break;
  	case ROLE_POD1: bufferOffset = 10+4+4+4+4; break;
  	case ROLE_POD2: bufferOffset = 10+4+4+4; break;
  	case ROLE_POD3: bufferOffset = 10+4+4; break;
  	case ROLE_POD4: bufferOffset = 10+4; break;
  	case ROLE_POD5: bufferOffset = 10; break;
  	default:
  	  //do nothing
  	  break;
      }	
      for(int i = 0; i < arraySize;i++)
      {
  	myRPTArray[i+bufferOffset].state = (buf[i+1] - '0');
      }
    }
    else
    {
      Serial.print(".");//nothing to to report
      digitalWrite(13,0);//no data
    }
  }  
  else//then we must be a POD
  {
    if(millis() > (timerRF + timeRF + (myRole * 10)))//this will un-sync them a bit
    {
      timerRF = millis();
      //so we need to transmit stuff
      controller="00000000000";//blank it for now
      int arraySize = 4;
      if(myRole == ROLE_SWPOD)
      {
        arraySize = 10;
      } 
      controller[0] = (myRole + '0');//set ID char
      for(int i = 0; i < arraySize;i++)//set array
      {
        controller[i+1] = (myArray[i].state + '0');
      }
      Serial.print("Transmitting: ");
      Serial.println(controller[0]);
      vw_send((uint8_t *)controller, strlen(controller));
      vw_wait_tx(); // Wait until the whole message is gone
    }  
  }
}

void loadFromEEPROM()
{
  if(myRole == ROLE_RPT)
  {
    //set up array as blank
    for(int i = 0; i < NUM_TOTAL_PEOPLE; i++)
    {
      myRPTArray[i].state = OUT;
    }
    Serial.println("Here");
  }
  else
  {
    for(int i = 0; i < NUM_PEOPLE; i++)
    {
      myArray[i].state = EEPROM.read(i);
    }
  }
}

void showStatus()
{
  if(myRole != ROLE_RPT)//if POD
  {
    if(analogRead(SENSOR_PIN) > (LIGHT_THRESH + LIGHT_HIST))
    {
      LEDS.setBrightness(1);//off
    }
    if(analogRead(SENSOR_PIN) < (LIGHT_THRESH - LIGHT_HIST))
    {
      LEDS.setBrightness(LED_BRIGHTNESS);//back to whatever
    }
  }

  if(myRole == ROLE_RPT)
  {
    for(int i = 0; i < NUM_TOTAL_PEOPLE; i++)//set up all LED's
    { 
      switch(myRPTArray[i].state)
      {
        case OUT: leds[i] = CRGB::Red; break;
        case IN: leds[i] = CRGB::Lime; break;
        case HOL: leds[i] = CRGB::Blue; break;
        case ILL: leds[i] = CRGB::Gold; break;//if too close to green, try Gold
        case NULL: leds[i] = CRGB::Black; break;
        default: leds[i] = CRGB::Red; break;
      }
    }
  } 
  else
  {  
    for(int i = 0; i < NUM_PEOPLE; i++)//set up all LED's
    { 
      switch(myArray[i].state)
      {
        case OUT: leds[i] = CRGB::Red; break;
        case IN: leds[i] = CRGB::Lime; break;
        case HOL: leds[i] = CRGB::Blue; break;
        case ILL: leds[i] = CRGB::Gold; break;//if too close to green, try Gold
        case NULL: leds[i] = CRGB::Black; break;
        default: leds[i] = CRGB::Red; break;
      }
    }
  }    
  FastLED.show();
}

void checkButtons()//writes state to eeprom on change
{
  for(int i = 0; i < 10; i++)//for each input
  {
    if(!digitalRead(buttons[0+i]))//if the one being checked is pressed
    {
      if(buttonPressedFlag == false)//if its first time press
      {
        //Serial.print("Button ");
        //Serial.print(i);
        //Serial.println(" Pressed");
        
        //log details and start time
        buttonPressedFlag = true;
        buttonPressedTimer = millis();
        buttonPressed = i;
        buttonPressedOrigState = myArray[i].state;
      }
	  
      else//(buttonPressedFlag == true)
      {//this means this button has been pressed before and is being pressed again
        //Serial.print(i);
        unsigned long timeTemp = (millis()-buttonPressedTimer);//how long it was pressed for
        if(timeTemp < PRESS_SHORT)
        {
          //Serial.println("INVALID");
          //do sod all
        }
        else if(timeTemp < PRESS_LONG)//so its longer then short, if shorter than long then
        {
          //Serial.println("TOGGLE");
          switch(buttonPressedOrigState)//based on original state
          {
            case IN: myArray[i].state = OUT; break;
            case OUT: myArray[i].state = IN; break;
            default: myArray[i].state = IN; break;//e.g. ILL or HOL
          }
        }
        else if((timeTemp < PRESS_SUPER_LONG) && (timeTemp > PRESS_LONG))//timeTemp must be longer than PRESS_SHORT and PRESS_LONG
        {
          //Serial.println("SPECIAL");
          myArray[i].state = HOL;
        }
        else if((timeTemp < PRESS_ULTRA_LONG) && (timeTemp > PRESS_SUPER_LONG))//timeTemp must be longer than PRESS_SUPER_LONG and PRESS_ULTRA_LONG
        {
          //Serial.println("SPECIAL2");
          myArray[i].state = ILL;
        }
        else //timeTemp must be longer than PRESS_ULTRA_LONG
        {
          //Serial.println("SPECIAL3");
          myArray[i].state = NULL;
        }
      }
    }
    else//if the one being checked is NOT pressed
    {			
      if((buttonPressed == i) && (buttonPressedFlag == true))//if it WAS pressed
      {
        //Serial.println("");
        //Serial.println("unpressed");
        easterEggCounter++;
        buttonPressedFlag = false;
        //write value to eeprom
        EEPROM.write(i, myArray[i].state);
      }
    }
  }	
}

//------------------------Easter egg-------------------------------------

void easterEgg()
{
  if(easterEggCounter >= easterEggPresses)
  {
    //reset counter
    easterEggCounter = 0;
    //set timer for 5 seconds time
    easterEggTimer = (millis() + easterEggTimeout);
    easterEggFlag = true;//flag to go
    //Serial.println("Easter Egg Triggered");
  }
  if(easterEggFlag)//if we are flagged to go
  {
    if(easterEggTimer <= millis())//if its time
    {
      easterEggFlag = false;//clear and off we go
      //generate a random colour from the wheel
      CRGB myColour = Wheel(random(0,256));
      //choose a sequence
      int sequence = random(0,5);
      //tell us what it is
      //Serial.print("Sequence:");
      //Serial.println(sequence);
      switch(sequence)
      {
        case 0: rainbowCycle(10); break;
        case 1: colorWipe(myColour,100); colorWipe2(CRGB::Black,100); break;
        case 2: rainbow(10); break;
        case 3: colorWipe2(myColour,100); colorWipe(CRGB::Black,100); break;
        case 4: allRainbow(10); break;
        //case 5: runSparkle(myColour, 20, 100); break;
      }
    }
  }
}

//-------------------------Easter egg sequences------------------------------------------  
  
void runSparkle(CRGB myColour, int wait, int times) 
{
  for(int i = 0; i < times; i++)
  {
    sparkle(myColour.r,myColour.g ,myColour.b,wait);
  }
}
  
void sparkle(byte C0, byte C1, byte C2, int wait) 
{
  for(int i = 0; i < NUM_TOTAL_PEOPLE; i++)//make a strips-worth of data
  {
    byte whiteChance = random(0,256);
    byte FC0 = 0;
    byte FC1 = 0;
    byte FC2 = 0;
    if(whiteChance>252)//occasionally
    {
      FC0=255;
      FC1=255;
      FC2=255;
    }
    else
    {
      //dull down by a random ammount
      FC0=C0/(255/whiteChance);
      FC1=C1/(255/whiteChance);
      FC2=C2/(255/whiteChance);		
      //to dull down the colourd bit so the white looks brighter	
      FC0=FC0/4;
      FC1=FC1/4;
      FC2=FC2/4;
    }
    //leds[i] = CRGB(i,FC0,FC1,FC2);//shimmer effect
    leds[i] = CRGB(FC0,FC1,FC2);
  }
  delay(wait);
  LEDS.show();
}

void allRainbow(uint8_t wait) {
	uint16_t i, j;
	for(j=0; j<256; j++)//one cycle
	{
		for(i=0; i< NUM_TOTAL_PEOPLE; i++)
		{
			leds[i] = CRGB(Wheel(j));
		}
		LEDS.show();
		delay(wait);
	}
}

//rainbow but spread evenly over strip
void rainbowCycle(uint8_t wait) {
	uint16_t i, j;
	for(j=0; j<256; j++) {
		for(i=0; i< NUM_TOTAL_PEOPLE; i++) {
			leds[i] = CRGB(Wheel(((i * 256 / NUM_TOTAL_PEOPLE) + j) & 255));
		}
		LEDS.show();
		delay(wait);
	}
}

void rainbow(uint8_t wait) {
	uint16_t i, j;

	for(j=0; j<256; j++) {
		for(i=0; i<NUM_TOTAL_PEOPLE; i++) {
			leds[i] = CRGB(Wheel((i+j) & 255));
		}
		LEDS.show();
		delay(wait);
	}
}

// Fill the dots one after the other with a colour
void colorWipe(CRGB c, uint8_t wait) {
	for(uint16_t i=0; i<NUM_TOTAL_PEOPLE; i++) {
		leds[i] = CRGB(c);
		LEDS.show();
		delay(wait);
	}
}

void colorWipe2(CRGB c, uint8_t wait) {
	for(uint16_t i=0; i<NUM_TOTAL_PEOPLE; i++) {
		leds[NUM_TOTAL_PEOPLE-i-1] = CRGB(c);
		LEDS.show();
		delay(wait);
	}
}

CRGB Wheel(byte WheelPos) {
	if(WheelPos < 85)//in first 3rd
	{
		return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);//no blue component
	}
	else if(WheelPos < 170) //in second 3rd
	{
		WheelPos -= 85;
		return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);//no green component
	}
	else//in last 3rd
	{
		WheelPos -= 170;
		return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);//no red component
	}
}

byte Wheel_R(byte WheelPos) {
	if(WheelPos < 85)//in first 3rd
	{
		return (WheelPos * 3);
	}
	else if(WheelPos < 170) //in second 3rd
	{
		WheelPos -= 85;
		return (255 - WheelPos * 3);
	}
	else//in last 3rd
	{
		WheelPos -= 170;
		return (0);
	}
}

byte Wheel_G(byte WheelPos) {
	if(WheelPos < 85)//in first 3rd
	{
		return (255 - WheelPos * 3);
	}
	else if(WheelPos < 170) //in second 3rd
	{
		WheelPos -= 85;
		return (0);
	}
	else//in last 3rd
	{
		WheelPos -= 170;
		return ( WheelPos * 3);
	}
}

byte Wheel_B(byte WheelPos) {
	if(WheelPos < 85)//in first 3rd
	{
		return (0);
	}
	else if(WheelPos < 170) //in second 3rd
	{
		WheelPos -= 85;
		return (WheelPos * 3);//no green component
	}
	else//in last 3rd
	{
		WheelPos -= 170;
		return (255 - WheelPos * 3);//no red component
	}
}
