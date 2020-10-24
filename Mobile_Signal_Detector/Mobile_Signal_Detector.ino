#include <virtuabotixRTC.h>
#include <memorysaver.h>
#include <UTFT.h>
#include <URTouch.h>
#include <URTouchCD.h>

//Creating objects
UTFT myTFT(ILI9341_16, 38,39,40,41);
URTouch myTouch(6, 5, 4, 3, 2);

virtuabotixRTC myRTC(8, 9, 10);

//Initialising variables
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

extern unsigned int AlarmButton[0x1040]; //Alarm button bitmap definition

char currentPage; //determines what page we are on

int x, y; //keep track of the screen coordinates

int r_beam = 200; //radar variable

//Alarm variables
int aHours = 7;
int aMinutes = 30;
int hoursRTC, minutesRTC, secondsRTC;
String alarmString, timeString;
boolean alarmNotSet;


void setup() {
  pinMode(11, INPUT);
  pinMode(12, OUTPUT);
  randomSeed(analogRead(0));
  //digitalWrite(12, LOW);
  Serial.begin(9600);
  //Initialising the screen
  myTFT.InitLCD();
  myTFT.clrScr();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  //Drawing the HomeScreen on start-up
  drawHomeScreen();
  currentPage = '0';

  //Initialising the Real-Time Clock
  //myRTC.setDS1302Time(00, 26, 18, 6, 23, 2, 2019); This is used to set the default time and date of the RTC. Uncomment and alter the values in the parentheses to change the default time and date
}

//========================================================================
void loop() {
  //Main loop that controls page switching and data processing
  myRTC.updateTime();
  if (currentPage == '0')
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();

      if ((x>=35) && (x<=285) && (y>=90) && (y<=130))
      {
        drawFrame(35,90,285,130);
        currentPage = '1';
        myTFT.clrScr();
        drawRadarDisplay();
      }

      if ((x>=35) && (x<=285) && (y>=140) && (y<=180))
      {
        drawFrame(35,140,285,180);
        currentPage = '2';
        myTFT.clrScr();
        drawTimerPage();
      }
    }
  }

  if (currentPage == '1')
  {
    sweepRadar();    
  }
  
  if (currentPage == '2') 
  {  
    alarmConfig();
  }
  
  if (myTouch.dataAvailable())
  {
    myTouch.read();
    x = myTouch.getX();
    y = myTouch.getY();
    if ((x>=10) && (x<=40) && (y>=10) && (y<=36))
    {
      drawFrame(10,10,40,36);
      currentPage = '0';
      myTFT.clrScr();
      drawHomeScreen();
    }
  }
  
}

//==========================================================================
//Function to draw the main interface
void drawHomeScreen() {
  myTFT.setBackColor(0,0,0);
  myTFT.setColor(255,255,255);
  myTFT.setFont(BigFont);
  myTFT.print("GSM Signal Detector", CENTER, 10);
  myTFT.setColor(255,0,0);
  myTFT.drawLine(0,32,319,32);
  myTFT.setColor(255,255,255);
  myTFT.setFont(SmallFont);
  myTFT.print("by Jemilat", CENTER, 41);
  myTFT.setFont(BigFont);
  myTFT.print("Select Mode", CENTER, 64);

  myTFT.setColor(100,155,203);
  myTFT.fillRoundRect(35,90,285,130);
  myTFT.setColor(255,255,255);
  myTFT.drawRoundRect(35,90,285,130);
  myTFT.setFont(BigFont);
  myTFT.setBackColor(100,155,203);
  myTFT.print("DETECTOR MODE", CENTER, 102);

  myTFT.setColor(100,155,203);
  myTFT.fillRoundRect(35,140,285,180);
  myTFT.setColor(255,255,255);
  myTFT.drawRoundRect(35,140,285,180);
  myTFT.setFont(BigFont);
  myTFT.setBackColor(100,155,203);
  myTFT.print("EXAM TIMER", CENTER, 152);

  myTFT.setColor(221,216,148);
  myTFT.fillRect(0, 215, 319, 239);
  myTFT.setColor(47,175,68);
  myTFT.fillRect(0, 205, 319, 214);
  myTFT.setColor(0, 0, 0);
  myTFT.setBackColor(221, 216, 148);
  myTFT.setFont(SmallFont);
  myTFT.print("Final Project Class of 2019", 10, 220);
  
}

//============================================================================
//Function to highlight pressed button
void drawFrame(int x1, int y1, int x2, int y2) {
  myTFT.setColor(255,0,0);
  myTFT.drawRoundRect(x1,y1,x2,y2);
  
  while (myTouch.dataAvailable())
  {
    myTouch.read();
    myTFT.setColor(255,0,0);
    myTFT.drawRoundRect(x1,y1,x2,y2);
  }
}

//============================================================================
//Simulate the sweeping motion of the radar
void sweepRadar() {
  while (currentPage == '1')
  { 
    //Simulates forward sweep of the radar
    long r = random(0, r_beam + 2);
    for (int i = 0; i < 180; i++)
    {
      if (myTouch.dataAvailable())
      {
        myTouch.read();
        x = myTouch.getX();
        y = myTouch.getY();
        if ((x>=10) && (x<=40) && (y>=10) && (y<=36))
        {
          drawFrame(10,10,40,36);
          break;
          currentPage = '0';
          myTFT.clrScr();
          drawHomeScreen();
        }
      }
      else
      {
        myTFT.setColor(0,255,0);
        myTFT.drawLine(160,239, 160 + r_beam * cos((360 - i)* 3.14/180), 239 + r_beam * sin((360 - i)* 3.14/180));
        delay(50);
        if ( digitalRead(11) == 0)
        {
          myTFT.setColor(255, 0, 0);
          myTFT.drawLine(160, 239, 160 + r * cos((360 - i) * 3.14/180), 239 + r * sin((360 - i) * 3.14/180));
        }
      }
    }

    if ((x>=10) && (x<=40) && (y>=10) && (y<=36))
    {
      currentPage = '0';
      myTFT.clrScr();
      drawHomeScreen();
    }
    else
    {
      myTFT.clrScr();
      drawRadarDisplay();
      
      //Simulates reverse sweep of the radar
      for (int i = 180; i > 0; i--)
      {
        if (myTouch.dataAvailable())
        {
          myTouch.read();
          x = myTouch.getX();
          y = myTouch.getY();
          if ((x>=10) && (x<=40) && (y>=10) && (y<=36))
          {
            drawFrame(10,10,40,36);
            break;
            currentPage = '0';
            myTFT.clrScr();
            drawHomeScreen();
          }
        }
        else
        {
          myTFT.setColor(0,255,0);
          myTFT.drawLine(160,239, 160 + r_beam * cos((360 - i)* 3.14/180), 239 + r_beam * sin((360 - i)* 3.14/180));
          if ( digitalRead(11) == 0)
          {
            myTFT.setColor(255, 0, 0);
            myTFT.drawLine(160, 239, 160 + r * cos((360 - i) * 3.14/180), 239 + r * sin((360 - i) * 3.14/180));
          }
          delay(50);
        }
      }

      if ((x>=10) && (x<=40) && (y>=10) && (y<=36))
      {
        currentPage = '0';
        myTFT.clrScr();
        drawHomeScreen();
      }
      else
      {
        myTFT.clrScr();
        drawRadarDisplay();
      }
    }
    

  }
}

//============================================================================
//Function that controls the setting, clearing and activation of the alarm
void alarmConfig() {
  alarmNotSet = true;
        
  while (alarmNotSet)
  {
    if (myTouch.dataAvailable()) 
    {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      //Set Hours
      if ((x >= 42) && (x <= 82) && (y >= 115) && (y <= 145)) 
      {
        drawRectFrame(42, 115, 82, 145);
        aHours++;
        if(aHours >=24)
        {
          aHours = 0;
        }
        myTFT.setFont(SevenSegNumFont);
        myTFT.setColor(0, 255, 0);
        myTFT.printNumI(aHours, 32, 50, 2, '0');
      }
      // Set Minutes
      if ((x >= 138) && (x <= 178) && (y >= 115) && (y <= 145)) 
      {
        drawRectFrame(138, 115, 178, 145);
        aMinutes++;
        if(aMinutes >=60)
        {
          aMinutes = 0;
        }
        myTFT.setFont(SevenSegNumFont);
        myTFT.setColor(0, 255, 0);
        myTFT.printNumI(aMinutes, 128, 50, 2, '0');
      }
      // Set Alarm
      if ((x >= 215) && (x <= 303) && (y >= 60) && (y <= 80)) 
      {
        drawRectFrame(215, 60, 303, 90);
        if (aHours < 10 && aMinutes < 10)
        {
          alarmString = "0"+(String)aHours + ":" + "0"+ (String)aMinutes + ":" + "00";
        }
        else if (aHours < 10 && aMinutes > 9)
        {
          alarmString = "0"+(String)aHours + ":" + (String)aMinutes + ":" + "00";
        }
        else if (aHours > 9 && aMinutes < 10)
        {
          alarmString = (String)aHours + ":" + "0"+ (String)aMinutes + ":" + "00";
        }
        else 
        {
          alarmString = (String)aHours + ":" + (String)aMinutes + ":" + "00";
        }
        myTFT.setFont(BigFont);
        myTFT.print("Alarm set for:", CENTER, 165);
        myTFT.print(alarmString, CENTER, 191);
      
      }
      // Clear Alarm
      if ((x >= 215) && (x <= 303) && (y >= 115) && (y <= 145)) 
      {
        drawRectFrame(215, 115, 303, 145);
        //alarmString="";
        aHours = 7;
        aMinutes = 30;
        myTFT.setFont(SevenSegNumFont);
        myTFT.setColor(0, 255, 0);
        myTFT.printNumI(aHours, 32, 50, 2, '0');
        myTFT.printNumI(aMinutes, 128, 50, 2, '0');
        myTFT.setColor(0, 0, 0);
        myTFT.fillRect(45, 165, 275, 210); 
      }
      // Go back to HomeScreen
      if ((x >= 0) && (x <= 40) && (y >= 0) && (y <= 36)) 
      {
        alarmNotSet = false;
        currentPage = '0';
        myTFT.clrScr();
        drawHomeScreen();  // Draws the Home Screen
      }
    }
  }
  
  // When Alarm is activated
  
  if (alarmNotSet == false) 
  {
    hoursRTC = myRTC.hours;
    minutesRTC = myRTC.minutes;
    secondsRTC = myRTC.seconds;
    //timeString = String(hoursRTC) + ":" + String(minutesRTC) + ":" + String(secondsRTC);
    //Serial.println(timeString);
    if (aHours == hoursRTC && aMinutes == minutesRTC)
    {
      boolean alarmOn = true;
      myTFT.clrScr();
      myTFT.setFont(BigFont);
      myTFT.setColor(255, 255, 255);
      myTFT.print("ALARM", CENTER, 90);
      myTFT.drawBitmap (127, 10, 65, 64, AlarmButton);
      myTFT.print(alarmString, CENTER, 114);
      myTFT.drawRoundRect (94, 146, 226, 170);
      myTFT.print("DISMISS", CENTER, 150);
      while (alarmOn)
      {
        digitalWrite(12, HIGH);
        if (myTouch.dataAvailable()) 
        {
          myTouch.read();
          x = myTouch.getX();
          y = myTouch.getY();
      
          if ((x >= 94) && (x <= 226) && (y >= 146) && (y <= 170)) 
          {
            drawRectFrame(94, 146, 226, 170);
            alarmOn = false;
            alarmString="";
            digitalWrite(12, LOW);
            myTFT.clrScr();
            delay(100);
            currentPage = '0';  
            drawHomeScreen();
          }
        }
      }
    }
  }
}
//============================================================================
//Function to draw the interface for the alarm clock
void drawTimerPage() {
  drawBackButton();
  myTFT.setBackColor(0,0,0);
  myTFT.setColor(255,255,255);
  myTFT.print("Set Alarm", CENTER, 10);
    
  // Draws a colon between the hours and the minutes
  myTFT.setColor(0, 255, 0);
  myTFT.fillCircle (112, 65, 4);
  myTFT.setColor(0, 255, 0);
  myTFT.fillCircle (112, 85, 4);
  myTFT.setFont(SevenSegNumFont);
  myTFT.setColor(0, 255, 0);
  myTFT.printNumI(aHours, 32, 50, 2, '0');
  myTFT.printNumI(aMinutes, 128, 50, 2, '0');
  myTFT.setColor(255, 255, 255);
  myTFT.drawRoundRect (42, 115, 82, 145);
  myTFT.drawRoundRect (138, 115, 178, 145);
  myTFT.setFont(BigFont);    
  myTFT.print("H", 54, 122);
  myTFT.print("M", 150, 122);
    
  myTFT.drawRoundRect (215, 60, 303, 90);
  myTFT.print("SET", 236, 67);
  myTFT.drawRoundRect (215, 115, 303, 145);
  myTFT.print("CLEAR", 220, 122);
}

//============================================================================
//Function to draw the Back button
void drawBackButton() {
  myTFT.setBackColor(0,0,0);
  myTFT.setColor(100, 155, 203);
  myTFT.fillRoundRect (10, 10, 40, 36);
  myTFT.setColor(255, 255, 255);
  myTFT.drawRoundRect (10, 10, 40, 36);
  myTFT.setFont(SmallFont);
  myTFT.setBackColor(100, 155, 203);
  myTFT.print("<-", 20, 18);
}

//===========================================================================
//Function to draw the initial interface of the radar display page
void drawRadarDisplay() {
  drawBackButton();
  myTFT.setColor(0,255,0);
  myTFT.drawCircle(160,239,r_beam + 2);
}

//===========================================================================
//Function to draw a rectangular frame around a pressed screen area
void drawRectFrame(int x1, int y1, int x2, int y2) {
  myTFT.setColor(255, 0, 0);
  myTFT.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myTFT.setColor(255, 255, 255);
  myTFT.drawRoundRect (x1, y1, x2, y2);
}

