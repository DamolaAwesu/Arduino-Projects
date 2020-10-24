#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x3f,16,2);

#include <EEPROM.h>

#include <Key.h>
#include <Keypad.h>

#include <AddicoreRFID.h>
#include <SPI.h>

#define	uchar	unsigned char
#define	uint	unsigned int

uchar fifobytes;
uchar fifoValue;


AddicoreRFID myRFID; // create AddicoreRFID object to control the RFID module

const int chipSelectPin = 10;
const int NRSTPD = 9;

//Maximum length of the array
#define MAX_LEN 4

#define ledGreen A1
#define relay A2
#define buzzer A3

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = 
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {8,7,6,5}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {4,3,2,A0}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
char key;

uint8_t successRead;
byte pin;

byte readCard[5];
byte storedCard[5];
byte masterCard[5];

boolean match = false;
boolean setupMode = false;


///////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  // put your setup code here, to run once:
  SPI.begin();
  
  lcd.init();
  lcd.backlight();
  
  pinMode(chipSelectPin,OUTPUT);
  digitalWrite(chipSelectPin, LOW);
  pinMode(NRSTPD,OUTPUT);
  digitalWrite(NRSTPD, HIGH);
  
  lcd.setCursor(1,0);
  lcd.print("ACCESS CONTROL");
  lcd.setCursor(5, 1);
  lcd.print("SYSTEM");
  delay(2000);
  
  myRFID.AddicoreRFID_Init();
  
  if (EEPROM.read(1) != 143)
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Scan a card to");
    lcd.setCursor(1,1);
    lcd.print("set as master");
    delay(1000);
    do
    {
      successRead = scanCard();
      delay(5000);
    }
    while (!successRead);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Card scanned");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set master pin");
    lcd.setCursor(12, 1);
    pin = getPasscode();
    readCard[4] = pin;
    for (uint8_t i = 0;i < 5;i++)
    {
      EEPROM.write(2 + i, readCard[i]);
    }
    EEPROM.write(1, 143);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Master Card set");
    delay(1000);
  }
  for (uint8_t j = 0; j < 5; j++)
  {
    masterCard[j] = EEPROM.read(2 + j);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  do
  {
    successRead = scanCard();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Swipe your card");
    delay(1000);
  }
  while(!successRead);
  
  if (setupMode)
  {
    if (masterCheck(readCard))
    {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Master Card ");
      lcd.setCursor(4, 1);
      lcd.print("Scanned!");
      delay(500);
      setupMode = false;
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Program Mode");
      lcd.setCursor(4, 1);
      lcd.print("Exited");
      delay(1000);
      return;
    }
    else
    {
      if (checkID(readCard))
      {
        //delete the card
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("I know this card");
        deleteID(readCard);
        delay(1000);
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Scan a card to add");
        lcd.setCursor(3, 1);
        lcd.print("or remove");
        delay(2000);
      }
      else
      {
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("New Card");
        lcd.setCursor(4, 1);
        lcd.print("Scanned!");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Set card pin");
        lcd.setCursor(12, 1);
        pin = getPasscode();
        writeCode(readCard, pin);
        writeID(readCard);
        return;
      }
    }
  }
  else
  {
    if (masterCheck(readCard))
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enter master pin");
      lcd.setCursor(12, 1);
      pin = getPasscode();
      if (masterCode(readCard))
      {
        setupMode = true;
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Program Mode");
        lcd.setCursor(4, 1);
        lcd.print("Entered!");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Scan a card to add");
        lcd.setCursor(3, 1);
        lcd.print("or remove");
        delay(1000);
        return;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Invalid code!");
        return;
      }
    }
    else
    {
      if (checkID(readCard))
      {
        granted();
      }
      else
      {
        denied();
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
uint8_t scanCard()
{
  uchar i, tmp, checksum1;
  uchar status;
  uchar str[MAX_LEN];
  uchar RC_size;
  uchar blockAddr; //Selection operation block address 0 to 63
  String mynum = "";

  str[1] = 0x4400;
  
  status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str);
  if (status != MI_OK)
  {
    return 0;
  }
  status = myRFID.AddicoreRFID_Anticoll(str);
  if (status != MI_OK)
  {
    return 0;
  }
  for(int z = 0; z < 4; z++)
  {
    readCard[z] = str[z];
  }
  checksum1 = str[0] ^ str[1] ^ str[2] ^ str[3];
  
  myRFID.AddicoreRFID_Halt();
}

/////////////////////////////////////////////////////////////////////////////////////////
void readID(int n)
{
  int pos = ( n* 5) + 2;
  for (int i = 0; i < 5; i++)
  {
    storedCard[i] = EEPROM.read(pos + i);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void writeID(byte a[])
{
  if (!checkID(a))
  {
    int num = EEPROM.read(0);
    int start = (num*5) + 7;
    num++;
    EEPROM.write(0, num);
    for (int j = 0; j < 5; j++)
    {
      EEPROM.write(start + j, a[j]);
    }
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Card added");
    lcd.setCursor(2, 1);
    lcd.print("successfully");
    delay(1000);
  }
  else
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Card not added");
    delay(1000);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void writeCode(byte a[], byte code)
{
  if(a[0] != 0)
  {
    a[4] = code;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void deleteID(byte a[])
{
  if (!checkID( a ))
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Delete failed!");
    delay(1000);
  }
  else
  {
    int num = EEPROM.read(0);
    int slot = getIDslot( a );
    int start = (slot * 5) + 2;
    int looping = ((num - slot) * 5);
    int j;
    num--;
    EEPROM.write(0, num);
    for ( j = 0; j < looping; j++)
    {
      EEPROM.write(start + j, EEPROM.read(start + 5 + j));
    }
    for (int k = 0;k < 5;k++)
    {
      EEPROM.write(start + j + k, 0);
    }
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Card removed");
    lcd.setCursor(2, 1);
    lcd.print("successfully");
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
boolean compareIDs( byte a[], byte b[])
{
  if (a[0] != 0)
  {
    match = true;
  }
  for (int k =0; k<4; k++)
  {
    if (a[k] != b[k])
    {
      match = false;
    }
  }
  
  if (match)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
boolean compareCode(byte b[], byte pin)
{
  if (b[4] != 0)
  {
    match = true;
  }
  byte passCode = b[4];
  if (pin != passCode)
  {
    match = false;
  }
  
  if (match)
    return true;
  else
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
boolean checkID( byte a[])
{
  int count = EEPROM.read(0);
  for (int i = i; i <= count; i++)
  {
    readID(i);
    if (compareIDs(a, storedCard))
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enter card pin");
      lcd.setCursor(12, 1);
      pin = getPasscode();
      if (compareCode(storedCard, pin))
      {
        return true;
        break;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Invalid pin!");
        delay(500);
        return false;
      }
    }
    else
    {
      //Return false
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
boolean masterCheck(byte test[])
{
  if (compareIDs(test, masterCard))
  {
    return true;
  }
  else
  {
    return false;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
boolean masterCode(byte test[])
{
  if (compareCode(test, pin))
  {
    return true;
  }
  else
  {
    return false;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void granted()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Access Granted!");
  digitalWrite(ledGreen, HIGH);
  digitalWrite(relay, HIGH);
  delay(3000);
  digitalWrite(relay, LOW);
  digitalWrite(ledGreen, LOW);
}

/////////////////////////////////////////////////////////////////////////////////////////
void denied()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Access Denied!");
  tone(buzzer, 1000);
  delay(1000);
  noTone(buzzer);
  delay(1000);
}

/////////////////////////////////////////////////////////////////////////////////////////
int getPasscode()
{
  char input[4];
  int n = 0;
  int num = 0;
  key = kpd.getKey();
  while (key != '#')
  {
    switch (key)
    {
      case NO_KEY:
      break;
      
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      input[n] = key;
      lcd.print("*");
      n = n + 1;
      num = num*10 + (key - '0');
      break;
      
      case '*':
      num = 0;
      break;
      
      case 'A': case 'B': case 'C': case 'D':
      input[n] = key;
      lcd.print(input[n]);
      n = n+1;
      break;
    }
   key = kpd.getKey();
  }
  return num;
  n = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
int getIDslot( byte find[] )
{
  int count = EEPROM.read(0);
  for (int i = 1; i <= count; i++)
  {
    readID(i);
    if (compareIDs(find, storedCard))
    {
      return i;
      break;
    }
  }
}
//////////////////////////////////////Program End////////////////////////////////////////
