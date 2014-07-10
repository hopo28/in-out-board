#include <VirtualWire.h>

#define RF_DAT 12

#define NUM_PEOPLE 10//this needs to be looked at

#define eEggInterval 600000//10 mins

char *controller;//array for sending packets

//position is LED number
typedef struct
{
  byte state;
} stateArray;

stateArray myArray[NUM_PEOPLE];


const char* pattern_friendly_name[] = { " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",//0-9
                                     " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",//10-19
                                     " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",//20-29
                                     " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",//30-39
                                     " ", " ", "ColourWheel", "ColourDown", "ColourFade", "ColourUp", "FullFade", "Sparkle", " ", " ",//40-49
                                    };

void setup()
{ 
  pinMode(13, OUTPUT);
  vw_set_ptt_inverted(true); // Required for DR3100  
  pinMode(RF_DAT, OUTPUT);
  vw_set_tx_pin(RF_DAT);
  vw_setup(4000);  // Bits per sec
  Serial.begin(9600);
  Serial.println("I am Easter Egg TX");
}

void loop()
{
  manySendRF(42);
  checkInputWait(eEggInterval);
  manySendRF(43);
  checkInputWait(eEggInterval);
  manySendRF(44);
  checkInputWait(eEggInterval);
  manySendRF(45);
  checkInputWait(eEggInterval);
  manySendRF(46);
  checkInputWait(eEggInterval);
  manySendRF(47);
  checkInputWait(eEggInterval);
}

void checkInputWait(unsigned long waitTime)
{
  //delay(waitTime);
  unsigned long delayStart = millis();
  while(millis() < (delayStart + waitTime))
  {
    if (Serial.available() > 0)
    {
      int thingie = Serial.read();
      thingie = thingie - '0';
      manySendRF(thingie);
    }
  }
}

void manySendRF(byte eEggNumber)
{
  Serial.print("Transmitting: ");
  Serial.println(pattern_friendly_name[eEggNumber]);
  for(int i = 0; i < 30;i++)
  {
    sendRF(eEggNumber);
  }
  String line1 = "Z = "; line1 += pattern_friendly_name[42]; Serial.println(line1);
  String line2 = "[ = "; line2 += pattern_friendly_name[43]; Serial.println(line2);
  String line3 = "\\ = ";line3 += pattern_friendly_name[44]; Serial.println(line3);
  String line4 = "] = "; line4 += pattern_friendly_name[45]; Serial.println(line4);
  String line5 = "^ = "; line5 += pattern_friendly_name[46]; Serial.println(line5);
  String line6 = "_ = "; line6 += pattern_friendly_name[47]; Serial.println(line6);
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
