#include <VirtualWire.h>

#define RF_DAT 12
#define LED_PIN 2

#define NUM_PEOPLE 10//this needs to be looked at

#define eEggInterval 600000//10 mins

char *controller;//array for sending packets

//position is LED number
typedef struct
{
  byte state;
} stateArray;

stateArray myArray[NUM_PEOPLE];

int buttons[]={3,4,5,6,7,8};
#define BUTTON_SEND 11

const char* pattern_friendly_name[] = { " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",//0-9
                                     " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",//10-19
                                     " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",//20-29
                                     " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",//30-39
                                     " ", " ", " ", " ", " ", " ", " ", " ", " ", "ColourWheel",//40-49
                                     "ColourDown", "Pulse", "ColourUp", "FullFade", "Sparkle", " ", " ", " ", " ", " ",//50-59
                                    };

void setup()
{ 
  pinMode(LED_PIN, OUTPUT);
  vw_set_ptt_inverted(true); // Required for DR3100  
  pinMode(RF_DAT, OUTPUT);
  vw_set_tx_pin(RF_DAT);
  vw_setup(4000);  // Bits per sec
  Serial.begin(9600);
  Serial.println("I am Easter Egg TX");
  for(int i = 0; i < 6; i++)//set up inputs
  {
    pinMode(buttons[i], INPUT);           // set pin to input
    digitalWrite(buttons[i], HIGH);       // turn on pullup r
  } 
  pinMode(BUTTON_SEND, INPUT);
  digitalWrite(BUTTON_SEND, HIGH);
  printMenu();
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(50);
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}

void loop()
{
  checkInputWait(eEggInterval);
  manySendRF(49);
  checkInputWait(eEggInterval);
  manySendRF(50);
  checkInputWait(eEggInterval);
  manySendRF(51);
  checkInputWait(eEggInterval);
  manySendRF(52);
  checkInputWait(eEggInterval);
  manySendRF(53);
  checkInputWait(eEggInterval);
  manySendRF(54);
}

void checkInputWait(unsigned long waitTime)
{
  //delay(waitTime);
  unsigned long delayStart = millis();
  while(millis() < (delayStart + waitTime))
  {
	//check serial input
    if (Serial.available() > 0)
    {
      int thingie = Serial.read();
      thingie = thingie - '0';
      manySendRF(thingie);
    }
    //check physical buttons too	  
    if (!digitalRead(BUTTON_SEND))
    {
      for(int i = 0; i < 6; i++)//for each input
      {
        if(!digitalRead(buttons[i]))//if the one being checked is pressed
        {
          switch(i)
  	  {
  	    case 0: manySendRF(49); break;//these are numbers in the buttons array
  	    case 1: manySendRF(50); break;
  	    case 2: manySendRF(51); break;
  	    case 3: manySendRF(52); break;
  	    case 4: manySendRF(53); break;
  	    case 5: manySendRF(54); break;
  	    default: /*Do Nothing*/ break;
  	  }
        }
      }
    }
  }
}

void manySendRF(byte eEggNumber)
{
  Serial.print("Transmitting: ");
  Serial.println(pattern_friendly_name[eEggNumber]);
  Serial.println();
    digitalWrite(LED_PIN, HIGH);
  for(int i = 0; i < 30;i++)
  {
    sendRF(eEggNumber);
  }
  digitalWrite(LED_PIN, LOW);
  printMenu();
}

void printMenu()
{
  Serial.println(" - EasterEgg Dongle Menu - ");
  String line1 = "a = "; line1 += pattern_friendly_name[49]; Serial.println(line1);
  String line2 = "b = "; line2 += pattern_friendly_name[50]; Serial.println(line2);
  String line3 = "c = "; line3 += pattern_friendly_name[51]; Serial.println(line3);
  String line4 = "d = "; line4 += pattern_friendly_name[52]; Serial.println(line4);
  String line5 = "e = "; line5 += pattern_friendly_name[53]; Serial.println(line5);
  String line6 = "f = "; line6 += pattern_friendly_name[54]; Serial.println(line6);
}

void sendRF(byte eEggNumber)
{
  //so we need to transmit stuff
  controller="00000000000";//blank it for now
  int arraySize = 1;
     
  controller[0] = (eEggNumber + '0');//set ID char
  for(int i = 0; i < arraySize;i++)//set array
  {
    controller[i+1] = (myArray[i].state + '0');
  }
     
//  Serial.print("Transmitting: ");
//  Serial.println(controller[0]);
     
  vw_send((uint8_t *)controller, strlen(controller));
  vw_wait_tx(); // Wait until the whole message is gone
}

