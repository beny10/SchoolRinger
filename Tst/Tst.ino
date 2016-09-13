#define SOUND_INTERVAL 2000
#define RELAY_PIN 10
#include <virtuabotixRTC.h>
int ringingTime[][3]={{8,0,0},{8,50,0},{9,0,0},{9,50,0},{10,0,0},{10,50,0},{11,10,0},{12,0,0},{12,10,0},{13,0,0},{13,10,0},{13,59,0}};
//int ringingTime[][3]={{21,30,00},{21,32,0}};
int ringingCount=12;
virtuabotixRTC RTC(5,6,7);
void setup()
{
  pinMode(RELAY_PIN,OUTPUT);
  digitalWrite(RELAY_PIN,LOW);
  Serial.begin(9600);
  pinMode(A0,INPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  digitalWrite(3,HIGH);
  digitalWrite(4,LOW);
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //RTC.setDS1302Time(00, 29,14, 7,20,3, 2016);
}
bool isOraVara(int month,int day,int dayOfWeek)
{
  bool isOraVara=false;
  if(month>3 && month<10)
  {
    isOraVara=true;
  }
  else if(month==3)
  {
    if(day>24)
    {
      isOraVara=true;
      if(day+(7-dayOfWeek)<=31)
      {
        isOraVara=false;
      }
    }
  }
  else if(month==10)
  {
    if(day>24)
    {
      isOraVara=false;
      if(day+(7-dayOfWeek)<=31)
      {
        isOraVara=true;
      }
    }
  }
  return isOraVara;
}
bool ZiAdaugareOra(int month,int day,int dayOfWeek,int hour)
{
  if(hour!=1 || month!=3)
  {
    return false;
  }
  if(day>24)
  {
    if(dayOfWeek==7 && day+7>31)
    {
      return true;
    }
    return false;
  }
}
bool ZiScadereOra(int month,int day,int dayOfWeek,int hour)
{
  if(hour!=1 || month!=10)
  {
    return false;
  }
  if(day>24)
  {
    if(dayOfWeek==7 && day+7>31)
    {
      return true;
    }
    return false;
  }
}
void Ring()
{
  digitalWrite(RELAY_PIN,HIGH);
  delay(SOUND_INTERVAL);
  digitalWrite(RELAY_PIN,LOW);
}
void loop()
{
  RTC.updateTime();
  int RtcYear=RTC.year;
  int RtcHour=RTC.hours;
  int RtcMinute=RTC.minutes;
  int RtcSecond=RTC.seconds;
  int RtcMonth=RTC.month;
  int RtcDay=RTC.dayofmonth;
  int RtcDayOfWeek=RTC.dayofweek;
  if(isOraVara(RtcMonth,RtcDay,RtcDayOfWeek))
  {
    RtcHour++;
  }
  Serial.print(RtcYear);
  Serial.print("/");
  Serial.print(RtcMonth);
  Serial.print("/");
  Serial.print(RtcDay);
  Serial.print(" ");
  Serial.print(RtcDayOfWeek);
  Serial.print("    ");
  Serial.print(RtcHour);
  Serial.print(" ");
  Serial.print(RtcMinute);
  Serial.print(" ");
  Serial.println(RtcSecond);
  if(RtcHour==0 && RtcMinute==0 && RtcSecond==5)
  {
    RTC.setDS1302Time(0,0 ,RtcHour, RtcDayOfWeek, RtcDay, RtcMonth, RtcYear);
  }
  if(RtcDayOfWeek==6 || RtcDayOfWeek==7)
  {
    //delay(100000);
    return;
  }
  for(int i=0;i<ringingCount;++i)
  {
    if(RtcHour==ringingTime[i][0] && RtcMinute==ringingTime[i][1] && RtcSecond>=ringingTime[i][2]-1 && RtcSecond<=ringingTime[i][2]+1)
    {
      Serial.println("Sun");
      Ring();
    }
  }
  delay(1000);
}
