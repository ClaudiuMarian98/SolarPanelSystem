#include <Wire.h> // Comunicatie I2C
#include <LiquidCrystal_I2C.h> // LCD
#include <SoftwareSerial.h> // comunicat RX TX pe alte porturi decat cele
dedicate hardware
#include <TinyGPSPlus.h> // GPS
#include <Stepper.h> // librarii motoare pas cu pas
#include <QMC5883LCompass.h> // modul busolă
QMC5883LCompass compass;
const float stpg = 5.69;
const int STEPS = 2048;
Stepper myStepper(STEPS, 12, 10, 11, 9);
static const int RXPin = 3, TXPin = 2;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
// Set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial softSerial(4, 5); // RX, TX
int selev;
int grd;
byte m , d , hr, minut, second;
int yare; // anul
int lstm = 30; // local standard time meridian;
int lt; // local time in minutes
float b;
float eot; //equation of time
float tc; // time correction
float lst; // local solar time
float hra; // hour angle
float g;
int rez;
int bis; // verifica an bisect
int n; // nr zilei din an
float decl;
float latit;// = 44.43278, latitude, latit0;
float longit;// = 26.1521, longitude, longit0;
float elev;
float azim, azi;
float age;
int dat = 0, zero = 0;
double cours;
int elevg, azimg, elevgg;
int v = 0;
#define DIR 6
#define STEP 7
#define SELEV A3
#define UCEL A6
#define ssf A0
#define ssm A1
float angle;
float old_angle;
float dif;
int ii;
int ucel;
int i;
int x;
int af, am;
long contor = 1;
short com;
65
int serv_cor=4;
volatile long temp, counter = 0; //This variable will increase or decrease depending on the
rotation of encoder
void setup() {
 softSerial.begin(38400);
 myStepper.setSpeed(10);
 lcd.begin();
 // Turn on the blacklight and print a message.
 lcd.backlight();
 Serial.begin(9600); //initialize serial channel to 9600 baud rate
 ss.begin(GPSBaud); // connect gps sensor
 Wire.begin(2);
 pinMode(UCEL, INPUT);
 pinMode(8, OUTPUT);
 digitalWrite(8, HIGH);
 pinMode(SELEV, INPUT_PULLUP);
 pinMode(DIR, OUTPUT);
 pinMode(STEP, OUTPUT);
 digitalWrite(DIR, LOW);
 digitalWrite(STEP, LOW);
compass.init();
}
void loop() {
if(v == 0){
 selev = digitalRead(SELEV);
// Serial.println(selev);
 if(selev != 0){
while(selev == 1){
 selev = digitalRead(SELEV);
 grd = stpg;
 /
myStepper.step(grd);
delay(50);
}
 }

 else{
 while(selev == 0){
 selev = digitalRead(SELEV);
 grd = stpg;

 myStepper.step(-grd);
 delay(50);
 }
 }
 compass.read();
 x = compass.getY()/10;
 while(abs(x)!= 0){
 compass.read();
 x = compass.getY()/10;
 Serial.println(x);
 if(x<0){
 digitalWrite(DIR, HIGH);
 }
else digitalWrite(DIR, LOW);
66
 digitalWrite(STEP, LOW);
 digitalWrite(STEP, HIGH);
 delay(5);
}

 v = 1;

}
if(contor = 1){
 dat = 1; // date valide GPS
 smartDelay(1000);
 // Send the value of counter encoder
 if ( counter != temp ) {
 // Serial.println (counter);
 temp = counter;
 }
 if (temp < 0) {
 temp = 360 - temp;
 }
 if (temp > 359) {
 temp = temp - 360;
 }
 if (millis() > 5000 && gps.charsProcessed() < 10)
 dat = 0;
 if (gps.location.isValid() == true) {
 latit = gps.location.lat();
 longit = gps.location.lng();
 // Serial.println(latit);
 //Serial.println(longit);
 }
 else {
 dat = 0;
 }
 if (gps.date.isValid() == true) {
 yare = gps.date.year();
 m = gps.date.month();
 d = gps.date.day();
 // Serial.println(yare);
 //Serial.println(m);
 //Serial.println(d);
 }
 else {
 dat = 0;
 }
 if (gps.time.isValid() == true) {
 hr = gps.time.hour();
 hr = hr + 2;
 minut = gps.time.minute();
 // Serial.println(hr);
 //Serial.println(minut);
 }
 else dat = 0;
67
 if (dat = 1) {
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Lt");
 lcd.setCursor(3, 0);
 lcd.print(latit);
 lcd.setCursor(9, 0);
 lcd.print("Lg");
 lcd.setCursor(12, 0);
 lcd.print(longit);
 lcd.setCursor(0, 1);
 lcd.print(yare);
 lcd.setCursor(4, 1);
 lcd.print("/");
 lcd.setCursor(5, 1);
 if(m < 10){
 lcd.print("0");
 lcd.setCursor(6, 1);
 lcd.print(m);
 }
 else{
 lcd.setCursor(5, 1);
 lcd.print(m);
 }
 lcd.setCursor(7, 1);
 lcd.print("/");
 lcd.setCursor(8, 1);
 if(d < 10){
 lcd.print("0");
 lcd.setCursor(9, 1);
 lcd.print(d);
 }
 else{
 lcd.setCursor(8, 1);
 lcd.print(d);
 }

lcd.setCursor(10, 1);
 lcd.print(" ");
 lcd.setCursor(11, 1);
 lcd.print(hr);
 lcd.setCursor(13, 1);
 lcd.print(":");
 lcd.setCursor(14, 1);
 lcd.print(minut);
 delay(1000);

 //declinatia
 rez = yare % 4;
 if (rez == 0) {
 bis = 1;
 }
 else bis = 2;
 n = (275 * m / 9) - bis * ((m + 9) / 12) + d - 30; // nr zile din an
 // Serial.println(n);
 g = 6.28 * (n - 1 + (hr - 12) / 24) / 365;
 // Serial.println(g);
 // decl = 0.006918 - 0.399912*cos(g)+ 0.070257*sin(g)-
0.006758*cos(2*g)+0.000907*sin(2*g)- 0.002697*cos(3*g )+0.001480*sin(3*g );
 decl = -23.45 * 0.0174 * cos((0.986 * (n + 10) * 0.0174));
 // Serial.println(decl*57.32);
68
 // elevatie
 lt = hr * 60 + minut;
 b = 0.986 * (n - 81) * 0.0174;
 // Serial.println(b);
 eot = 9.87 * sin(2 * b) - 7.53 * cos(b) - 1.5 * sin(b); // in minute
 // Serial.println(eot);
 tc = 4 * (longit - lstm) + eot;
 lst = lt + tc / 60;
 lst = lst / 60;
 //Serial.println(lst);
 hra = 15 * (lst - 12);
 // Serial.println(hra);
 hra = hra * 0.0174;
 elev = asin(sin(latit * 0.01744) * sin(decl) + cos(latit * 0.0174) * cos(decl) *
cos(hra));
 elevg = (int)(elev * 57.32);
 // Serial.println(elev*57.32);
if(elevg <= 0) {
 elevg = 0;
}
ucel = analogRead(UCEL);
ucel = map(ucel, 0, 1023, 0, 100);
float ucell = ucel/10;
 azi = acos((sin(decl) * cos(latit * 0.0174) - cos(decl) * sin(latit * 0.0174) *
cos(hra)) / cos(elev));
 azi = azi * 57.32;
 if (hra > 0) {
 azim = 360 - azi;
 }
 else azim = azi;
 azimg = (int)azim;

 Serial.println(azimg);

 if (elevgg != elevg) {
 grd = - (elevg-elevgg)*stpg;
 //Serial.println(grd);
 myStepper.step(grd);
 delay(500);
elevgg = elevg;
 }

 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Elev");
 lcd.setCursor(5, 0);
 lcd.print(elevg);
 lcd.setCursor(9, 0);
 lcd.print("Azi");
 lcd.setCursor(13, 0);
 lcd.print(azimg);
 lcd.setCursor(0, 1);
 lcd.print("Ucel = ");
 lcd.setCursor(8, 1);
 lcd.print(ucell);
 lcd.setCursor(14, 1);
 lcd.print("V");
delay(1000);
 contor++;
}
}
else{
69
 contor++;
 if(contor>= 80000){
 contor = 1;
 }
}
 get_angle(com);
 old_angle = angle;
 angle = com;
 angle = angle + azimg;
// Serial.print("angle ");
// Serial.println(com);
 dif = angle - old_angle;
 ii = 4.7*dif*2;
 if(ii < 0){
 ii = -ii;
 digitalWrite(DIR, HIGH);
 }
 else digitalWrite(DIR, LOW);

 //digitalWrite(DIR, LOW);
 for(i = 1; i<=ii; i++){
 digitalWrite(STEP, LOW);
 digitalWrite(STEP, HIGH);
 delay(10);

 }
//}
}
static void smartDelay(unsigned long ms)
{
 unsigned long start = millis();
 do
 {
 ss.listen();
 while (ss.available())
 gps.encode(ss.read());
 } while (millis() - start < ms);
}
static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
 if (!d.isValid())
 {
 // Serial.print(F("**** "));
 }
 else
 {
 char sz[32];
 sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
 // Serial.print(sz);
 int year = d.year();
 return year;
 // Serial.print(year);
 }
 if (!t.isValid())
 {
 // Serial.print(F("**** "));
 }
 else
 {
 char sz[32];
70
 sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
 // Serial.print(sz);
 }
 printInt(d.age(), d.isValid(), 5);
 smartDelay(0);
}
static void printInt(unsigned long val, bool valid, int len)
{
 char sz[32] = "*******";
 if (valid)
 sprintf(sz, "%ld", val);
 sz[len] = 0;
 for (int i = strlen(sz); i < len; ++i)
 sz[i] = ' ';
 if (len > 0)
 sz[len - 1] = ' ';
// Serial.print(sz);
 smartDelay(0);
}
//void requestEvent()
//{
// Wire.write((byte *)&data, sizeof data);
//}
void get_angle(short &com)
{
 softSerial.listen();
 serial_flush();
 digitalWrite(8, LOW);
 delay(50);
 if (softSerial.available()) {
 // Serial.println("esteee");
 com = softSerial.read() <<8;
 com = com | softSerial.read();
 com = com<< 8;
 com = com | softSerial.read();
 //Serial.println(com);
}
 digitalWrite(8, HIGH);

}
void serial_flush(void) {
 while (softSerial.available() ) softSerial.read();
 //Serial.println("flushhhhhhhh");
