// include relevant libraries
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

// initialize the temperature sensor, fan and heater pins and needed variables
int sampleTime = 1000;
double temp;
int set1; int set2;
int fanPin = 10;
int heaterPin = 26;

// initialize the  LCD library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 22, 23, 24, 25);
int cursorColumn = 14;

// initialize the keypad library
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3};

// make the keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
char key;


void setup() 
{
  // set input and output pins
  pinMode(fanPin, OUTPUT);
  pinMode(heaterPin, OUTPUT);
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(2,0);
  lcd.print("TEMP CONTROL");
  lcd.setCursor(5,1);
  lcd.print("SYSTEM");
  delay(1000);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Enter min temp");
  lcd.setCursor(2,1);
  lcd.print("and max temp");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("MinTemp: ");
  lcd.setCursor(12,0);
  // get desired temperature
  set1 = getNumber();
  lcd.setCursor(0,1);
  lcd.print("MaxTemp: ");
  lcd.setCursor(12,1);
  set2 = getNumber();
  delay(2000);
  lcd.clear();
}

void loop() 
{
  double FTemp0 = ((5.0/1024.0)* analogRead(A0)*100);
  double FTemp = FTemp0;
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  // print the current temperature
  lcd.print("ROOMTEMP: ");
  lcd.print(FTemp);
  lcd.print("C");
  delay(sampleTime);

  // set the cursor to column 8, line 1
  lcd.setCursor(0,1);
    // print the tempersture set by the user
  lcd.print("RANGE: ");
  lcd.setCursor(7,1);
  lcd.print(set1);
  lcd.print("C");
  lcd.setCursor(11,1);
  lcd.print("~ ");
  lcd.print(set2);
  lcd.print("C");
  
//  if (buttonstate == true)
//  {
//    getRange();
//  }
  
  if (set2 < FTemp)
  {
    digitalWrite(fanPin, HIGH);
  }
  else
  {
    digitalWrite(fanPin, LOW);
  }
  
  if (set1 > FTemp)
  {
    digitalWrite(heaterPin, HIGH);
  }
  else
  {
    digitalWrite(heaterPin, LOW);
  }
}

int getNumber()
{
  char input[2];
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
      lcd.print(input[n]);
      n = n + 1;
      num = num*10 + (key - '0');
      break;
      
      case '*':
      num = 0;
      break;
    }
   key = kpd.getKey();
 }
  return num;
  n = 0;
}

//int getRange()
//{
//  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("MinTemp: ");
//  lcd.setCursor(12,0);
//  // get desired temperature
//  set1 = getNumber();
//  lcd.setCursor(0,1);
//  lcd.print("MaxTemp: ");
//  lcd.setCursor(12,1);
//  set2 = getNumber();
//  delay(2000);
//  lcd.clear();
//} 
