
#include "Clock.h"
#define DS3231_I2C_ADDRESS 0x68
#define DISPLAY_REFRESH_RATE 5000
#define MODE_CHANGE_BUTTON A2
#define VALUE_CHANGE_BUTTON A3
#define SOUND_INTERVAL 2000
#define RELAY_PIN 9
#define CS  10
int ringingTime[][3] = {{8, 0, 0}, {8, 50, 0}, {9, 0, 0}, {9, 50, 0}, {10, 0, 0}, {10, 50, 0}, {11, 10, 0}, {12, 0, 0}, {12, 10, 0}, {13, 0, 0}, {13, 10, 0}, {13, 59, 0}};//13:59 "sa prinda elevii conventia"
//int ringingTime[][3]={{21,30,00},{21,32,0}};
int ringingCount = 12;

int isInProgrammingMode = 0;
int programmingStep = 0;
int wasTimeModified = 0;
int consecutiveIncrementsCount = 0;
byte decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val / 16 * 10) + (val % 16) );
}
int latch = 2;
int d0 = A4;
int d1 = 4;
int d2 = 3;
int d3 = A5;
int firstCharcter = 6;
int secondCharcter = 7;
int thirdCharcter = 8;
char numbersInBinary[][4] = {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001"};

Clock clock;

void setup() {
  clock.InitClock(CS);

  pinMode(firstCharcter, OUTPUT);
  pinMode(secondCharcter, OUTPUT);
  pinMode(thirdCharcter, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(d0, OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  digitalWrite(firstCharcter, HIGH);
  digitalWrite(secondCharcter, HIGH);
  digitalWrite(thirdCharcter, HIGH);
  digitalWrite(latch, LOW);

  pinMode(MODE_CHANGE_BUTTON, INPUT_PULLUP);
  pinMode(VALUE_CHANGE_BUTTON, INPUT_PULLUP);





  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  Serial.begin(9600);
}
void printOneNumber(int number)
{
  digitalWrite(d0, numbersInBinary[number][3] - 48);
  digitalWrite(d1, numbersInBinary[number][2] - 48);
  digitalWrite(d2, numbersInBinary[number][1] - 48);
  digitalWrite(d3, numbersInBinary[number][0] - 48);

  digitalWrite(latch, LOW);
  digitalWrite(latch, HIGH);
}
void printCharacter(char c)
{
  switch (c)
  {
    case '0':
      printOneNumber(0);
      break;
    case '1':
      printOneNumber(1);
      break;
    case '2':
      printOneNumber(2);
      break;
    case '3':
      printOneNumber(3);
      break;
    case '4': case 'y':
      printOneNumber(4);
      break;
    case '5': case 's':
      printOneNumber(5);
      break;
    case '6':
      printOneNumber(6);
      break;
    case '7':
      printOneNumber(7);
      break;
    case '8':
      printOneNumber(8);
      break;
    case '9':
      printOneNumber(9);
      break;
  }
}
void printText(char text[])
{
  printCharacter(text[0]);
  digitalWrite(firstCharcter, LOW);
  delayMicroseconds(DISPLAY_REFRESH_RATE);
  printCharacter(text[1]);
  digitalWrite(firstCharcter, HIGH);

  digitalWrite(secondCharcter, LOW);
  printCharacter(text[1]);
  delayMicroseconds(DISPLAY_REFRESH_RATE);
  printCharacter(text[2]);
  digitalWrite(secondCharcter, HIGH);

  digitalWrite(thirdCharcter, LOW);
  delayMicroseconds(DISPLAY_REFRESH_RATE);
  digitalWrite(thirdCharcter, HIGH);
}
void printNumber(int number)
{
  char text[3];
  text[2] = number % 10 + 48;
  text[1] = number / 10 % 10 + 48;
  text[0] = number / 100 % 10 + 48;
  printText(text);
}

void Ring()
{
  digitalWrite(RELAY_PIN, HIGH);
  delay(SOUND_INTERVAL);
  digitalWrite(RELAY_PIN, LOW);
}
void displayTime()
{
  if (clock.IsOraVara(clock.GetMonth(), clock.GetDay(), clock.GetDayOfWeek()))
  {
    Serial.print("ora vara ");
  }
  else
  {
    Serial.print("ora iarna ");
  }
  Serial.print(clock.GetHour());
  Serial.print(":");
  Serial.print(clock.GetMinute());
  Serial.print(":");
  Serial.print(clock.GetSecond());
  Serial.print(" ");
  Serial.print(clock.GetDay());
  Serial.print("/");
  Serial.print(clock.GetMonth());
  Serial.print("/");
  Serial.print(clock.GetYear());
  Serial.print(" ");
  Serial.println(clock.GetDayOfWeek());

}
int getProgramWizardValue(int progStep)
{
  switch (progStep)
  {
    case 0:
      return clock.GetYear();
      break;
    case 1:
      return clock.GetMonth();
      break;
    case 2:
      return clock.GetDay();
      break;
    case 3:
      return clock.GetHour();
      break;
    case 4:
      return clock.GetMinute();
      break;
    case 5:
      return clock.GetSecond();
      break;
  }
}

void programWizard(int progStep)
{
  int toDisplay ;
  toDisplay = getProgramWizardValue(progStep);
  toDisplay = progStep * 100 + toDisplay;
  printNumber(toDisplay);
}
void programWizardValueIncremented(int progStep, int step)
{
  switch (progStep)
  {
    case 0:
      clock.IncrementYear();
      break;
    case 1:
      clock.IncrementMonth();
      break;
    case 2:
      clock.IncrementDay();
      break;
    case 3:
      clock.IncrementHour();
      break;
    case 4:
      clock.IncrementMinute();
      break;
    case 5:
      clock.IncrementSecond();
      break;
  }
}
void programMode()
{
  if (wasTimeModified == 0)
    clock.UpdateTimeDate();
  displayTime();
  if (digitalRead(VALUE_CHANGE_BUTTON) == 0)
  {
    Serial.println("Value changed");
    programWizardValueIncremented(programmingStep, 1);
    wasTimeModified = 1;
    if (consecutiveIncrementsCount++ < 10)
      delay(500);
    else
      delay(200);
  }
  else
  {
    consecutiveIncrementsCount = 0;
  }
  if (6 > programmingStep)
    programWizard(programmingStep);
  else
  {
    clock.SetTime();
    programmingStep = 0;
    isInProgrammingMode = 0;
    wasTimeModified = 0;
  }
}
void checkProgramModeButtons()
{
  if (digitalRead(MODE_CHANGE_BUTTON) == 0)
  {
    if (isInProgrammingMode == 0)
    {
      Serial.println("program init");
      int count = 0;
      while (count++ < 100)
      {
        printText("888");
      }
      delay(1000);
      Serial.println("mode");
      isInProgrammingMode = 1;
      programmingStep = 0;
      wasTimeModified = 0;
    }
    else
    {
      Serial.println("modeChanged");
      programmingStep++;
      delay(1000);
    }
  }
}
void loop()
{
  checkProgramModeButtons();
  if (isInProgrammingMode == 1)
  {
    programMode();
  }
  else
  {
    clock.UpdateTimeDate();
    displayTime();
    if (clock.GetDayOfWeek() == 6 || clock.GetDayOfWeek() == 7)
    {
      //delay(100000);
      return;
    }
    for (int i = 0; i < ringingCount; ++i)
    {
      if (clock.GetHour() == ringingTime[i][0] &&
          clock.GetMinute() == ringingTime[i][1] &&
          clock.GetSecond() >= ringingTime[i][2] - 1 && clock.GetSecond() <= ringingTime[i][2] + 1)
      {
        Serial.println("Sun");
        Ring();
      }
    }
    delay(1000);
  }
}
