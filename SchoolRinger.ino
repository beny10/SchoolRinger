#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68
#define DISPLAY_REFRESH_RATE 1
#define MODE_CHANGE_BUTTON 3
#define VALUE_CHANGE_BUTTON 2
#define SOUND_INTERVAL 2000
#define RELAY_PIN A0
int ringingTime[][3] = {{8, 0, 0}, {8, 50, 0}, {9, 0, 0}, {9, 50, 0}, {10, 0, 0}, {10, 50, 0}, {11, 10, 0}, {12, 0, 0}, {12, 10, 0}, {13, 0, 0}, {13, 10, 0}, {13, 59, 0}};//13:59 "sa prinda elevii conventia"
//int ringingTime[][3]={{21,30,00},{21,32,0}};
int ringingCount = 12;
int RtcYear = 0;
int RtcHour = 0;
int RtcMinute = 0;
int RtcSecond = 0;
int RtcMonth = 0;
int RtcDay = 0;
int RtcDayOfWeek = 0;
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
int d0 = 8;
int d1 = 9;
int d2 = 10;
int d3 = 11;
int firstCharcter = 4;
int secondCharcter = 5;
int thirdCharcter = 6;
char numbersInBinary[][4] = {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001"};
void setup() {
  pinMode(firstCharcter, OUTPUT);
  pinMode(secondCharcter, OUTPUT);
  pinMode(thirdCharcter, OUTPUT);
  pinMode(d0, OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  digitalWrite(firstCharcter, HIGH);
  digitalWrite(secondCharcter, HIGH);
  digitalWrite(thirdCharcter, HIGH);
  Wire.begin();
  pinMode(MODE_CHANGE_BUTTON, INPUT_PULLUP);
  pinMode(VALUE_CHANGE_BUTTON, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  Serial.begin(9600);
}
void updateTimeDate()
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  RtcSecond = bcdToDec(Wire.read() & 0x7f);
  RtcMinute = bcdToDec(Wire.read());
  RtcHour = bcdToDec(Wire.read() & 0x3f);
  RtcDayOfWeek = bcdToDec(Wire.read());
  RtcDay = bcdToDec(Wire.read());
  RtcMonth = bcdToDec(Wire.read());
  RtcYear = bcdToDec(Wire.read());
  if (isOraVara(RtcMonth, RtcDay, RtcDayOfWeek))
  {
    RtcHour++;
    if (RtcHour == 24)
      RtcHour = 0;
  }
}
void printOneNumber(int number)
{
  digitalWrite(d0, numbersInBinary[number][3] - 48);
  digitalWrite(d1, numbersInBinary[number][2] - 48);
  digitalWrite(d2, numbersInBinary[number][1] - 48);
  digitalWrite(d3, numbersInBinary[number][0] - 48);
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
  delay(DISPLAY_REFRESH_RATE);
  digitalWrite(firstCharcter, HIGH);
  printCharacter(text[1]);
  digitalWrite(secondCharcter, LOW);
  printCharacter(text[1]);
  delay(DISPLAY_REFRESH_RATE);
  digitalWrite(secondCharcter, HIGH);
  printCharacter(text[2]);
  digitalWrite(thirdCharcter, LOW);
  delay(DISPLAY_REFRESH_RATE);
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
bool isOraVara(int month, int day, int dayOfWeek)
{
  bool isOraVara = false;
  if (month > 3 && month < 10)
  {
    isOraVara = true;
  }
  else if (month == 3)
  {
    isOraVara = false;
    if (day > 24)
    {
      isOraVara = true;
      if (day + (7 - dayOfWeek) <= 31)
      {
        isOraVara = false;
      }
    }
  }
  else if (month == 10)
  {
    isOraVara = true;
    if (day > 24)
    {
      isOraVara = false;
      if (day + (7 - dayOfWeek) <= 31)
      {
        isOraVara = true;
      }
    }
  }
  return isOraVara;
}
void Ring()
{
  digitalWrite(RELAY_PIN, HIGH);
  delay(SOUND_INTERVAL);
  digitalWrite(RELAY_PIN, LOW);
}
void displayTime()
{
  if (isOraVara(RtcMonth, RtcDay, RtcDayOfWeek))
  {
    Serial.print("ora vara ");
  }
  else
  {
    Serial.print("ora iarna ");
  }
  Serial.print(RtcHour);
  Serial.print(":");
  Serial.print(RtcMinute);
  Serial.print(":");
  Serial.print(RtcSecond);
  Serial.print(" ");
  Serial.print(RtcDay);
  Serial.print("/");
  Serial.print(RtcMonth);
  Serial.print("/");
  Serial.print(RtcYear);
  Serial.print(" ");
  Serial.println(RtcDayOfWeek);

}
int *getProgramWizardValue(int progStep)
{
  switch (progStep)
  {
    case 0:
      return &RtcYear;
      break;
    case 1:
      return &RtcMonth;
      break;
    case 2:
      return &RtcDay;
      break;
    case 3:
      return &RtcDayOfWeek;
      break;
    case 4:
      return &RtcHour;
      break;
    case 5:
      return &RtcMinute;
      break;
    case 6:
      return &RtcSecond;
      break;
  }
}
int getMaximValueForProgramming(int progStep)
{
  switch (progStep)
  {
    case 0:
      return 100;
      break;
    case 1:
      return 12;
      break;
    case 2:
      return 31;
      break;
    case 3:
      return 7;
      break;
    case 4:
      return 23;
      break;
    case 5:
      return 59;
      break;
    case 6:
      return 59;
      break;
  }
}
int getMinimValueForProgramming(int progStep)
{
  switch (progStep)
  {
    case 0:
      return 1;
      break;
    case 1:
      return 1;
      break;
    case 2:
      return 1;
      break;
    case 3:
      return 0;
      break;
    case 4:
      return 0;
      break;
    case 5:
      return 0;
      break;
    case 6:
      return 0;
      break;
  }
}
void setDS3232Time(byte second, byte minute, byte hour, byte dayOfWeek, byte
                   dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  if (isOraVara(RtcMonth, RtcDay, RtcDayOfWeek))
  {
    if (hour > 0)
      hour--;
    else
      hour = 23;
  }
  Serial.print("hour=");
  Serial.println(hour);
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void programWizard(int progStep)
{
  int toDisplay ;
  toDisplay = *getProgramWizardValue(progStep);
  toDisplay = progStep * 100 + toDisplay;
  printNumber(toDisplay);
}
void programWizardValueIncremented(int progStep, int step)
{
  int *value = getProgramWizardValue(progStep);
  *value = *value + step;
  if (*value > getMaximValueForProgramming(progStep))
    *value = getMinimValueForProgramming(progStep);
  if (*value < getMinimValueForProgramming(progStep))
    *value = getMinimValueForProgramming(progStep);
}
void programMode()
{
  if (wasTimeModified == 0)
    updateTimeDate();
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
  if (programmingStep < 7)
    programWizard(programmingStep);
  else
  {
    setDS3232Time(RtcSecond, RtcMinute, RtcHour, RtcDayOfWeek, RtcDay, RtcMonth, RtcYear);
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
      while (count++ < 500)
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
    updateTimeDate();
    displayTime();
    if (RtcDayOfWeek == 6 || RtcDayOfWeek == 7)
    {
      //delay(100000);
      return;
    }
    for (int i = 0; i < ringingCount; ++i)
    {
      if (RtcHour == ringingTime[i][0] && RtcMinute == ringingTime[i][1] && RtcSecond >= ringingTime[i][2] - 1 && RtcSecond <= ringingTime[i][2] + 1)
      {
        Serial.println("Sun");
        Ring();
      }
    }
    delay(1000);
  }
}
