// Ce code est celui du groupe 6
// Les auteurs sont:
//                   BABAALI AMINE 
//                   BAHA ALILI 
//                   KAHLI MOHAMED SAMI
//                   OMAR BELAIDI
#include <BH1750.h>
#include <DHT.h>
#include <DHT_U.h>
#include <RTClib.h>
#include <SdFat.h>
#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <EEPROM.h>

#define DHTPIN 8 
#define DHTTYPE DHT11  
#define RLED 7
#define GLED 6
#define BLED 5
#define GREENBUT 3
#define REDBUT 2

RTC_DS1307 RTC;
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;
DateTime tm;
SoftwareSerial gpsSerial(4,9);//rx,tx 
TinyGPS gps; 
SdFat SD;

//float lat = 28.5458, lon = 77.1703;
int triger;
int prev_mode;
int mode = 1;
int tiktok;
int mtime;
int triger_R;
int triger_G;
const int  Carte = 10;
String dataString;
struct Mesure {                        //structure mesure pour prendre en compte les différents paramètres
  unsigned long int LOG_INTERVAL;
  unsigned int FILE_MAX_SIZE;
  unsigned int TIME_OUT;
  bool LUMIN;
  unsigned short int LUMIN_LOW;
  unsigned short int LUMIN_HIGH;
  bool TEMP_AIR;
  int8_t MIN_TEMP_AIR;
  int8_t MAX_TEMP_AIR;
  bool HYGR;
  uint8_t HYGR_MINT;
  uint8_t HYGR_MAXT;
};
typedef struct{                    // structure qui va prendre en compte les adresses des différentes mesures des capteurs
  unsigned char temperature;
  unsigned char humidite;
  String tempe;
  String hum;
  unsigned char lux;
  String luxe;
} Cap;
Cap cap;
Cap *p = &cap;
String *pdata = &dataString;
File dataFile;                             // on crée un objet file 
unsigned int second_counter,current_mode;
Mesure param;
Mesure *pr = &param;
///////////////////////
void initialisation_interrup(){                      // fonction qui va initialiser les interruptions
  attachInterrupt(digitalPinToInterrupt(GREENBUT),GREEN,HIGH);
  attachInterrupt(digitalPinToInterrupt(REDBUT),RED,HIGH);
}
///////////////////////
void RED(){                                     // fonction interruption bouton rouge
  triger_R = digitalRead (REDBUT);
  if(triger_R == 0){
               // mtime est la période sur laquelle le BTN est appuyé 
    mtime = (millis() - tiktok);
   if(mtime >= 5000)
    if (mode == 4){
     // pour sortir du mode maitenance 
      mode = prev_mode;
     
    }else{ 
               // Accéder au mode maintenance et sauvegarder le mode précedent
    prev_mode = mode;
    mode = 4;
  }
}else {
               // enregistrer le moment d'appuyage du BTN sur la variable tiktok 
    tiktok=millis();
 }
}
///////////////////////
void GREEN(){                                           // fonction interruption bouton vert
  triger_G = digitalRead(GREENBUT);
  if(triger_G == 0){
    mtime=(millis() - tiktok);
    if(mtime >=5000)
    {
      if(mode == 3){
            // pour sortir du mode ECONOMIQUE
        mode =1;
      }else {
            // pour accéder au mode ECONOMIQUE
        mode =3;
      }
    }
  }else {
    tiktok = millis();
  }
}
///////////////////////
void RGBcolor(byte r, byte g, byte b) {                         // fonction contrôler la RGB
  analogWrite(RLED, r);
  analogWrite(GLED, g);
  analogWrite(BLED, b);
}
///////////////////////
void Capteurs() {                                                 // fonction qui va lancer l'acquisition des différents capteurs
  DateTime now = RTC.now();
  // mesure temperature et humidité
  (*p).temperature = dht.readTemperature();
  (*p).humidite = dht.readHumidity();
  (*p).lux = lightMeter.readLightLevel ();
  
  if (isnan((*p).humidite) || isnan((*p).temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  };
  //while(gpsSerial.available()){ // check for gps data 
  //  if(gps.encode(gpsSerial.read()))// encode gps data 
  //  {  
  //  gps.f_get_position(&lat,&lon); // get latitude and longitude 
  // }
  //}
   *pdata = "Temperature : " + String((*p).temperature) + "°C " + " Humidite : " + String((*p).humidite) + "% " + "Luminosite: " + String((*p).lux) + "Lxs "  + now.timestamp();
   //*pdata += "Lon"+ String(lon) + "Lat" + String(lat);
}
///////////////////////
void ecrtSD(){                      // fonction qui va écrire sur les fichiers 
  dataFile = SD.open("date.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(*pdata);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    delay(1000);
    Serial.println("error opening date.txt");
  }
}
///////////////////////
void sd(){                                // fonction qui va initiliaser et vérifier la carte SD
  Serial.print(F("Initializing SD card..."));
  while (!SD.begin(Carte)) {
    Serial.println(F("initialization failed!"));
    RGBcolor(255, 0, 0); // Red
    delay(1000);
    RGBcolor(255, 255, 255); // White
    delay(2000);
    while (1);
  }
  Serial.println(F("initialization done."));
  // Donne le nombre de clusters disponibles dans la carte SD
  uint32_t freeClusters = SD.vol()->freeClusterCount();
  // Calcule l'espace disponible en KB
  float freeKB = .512 * freeClusters * SD.vol()->sectorsPerCluster();
  if (freeKB <= (*pr).FILE_MAX_SIZE) {  
    Serial.println(F("Carte SD pleine"));
    RGBcolor(255, 0, 0); // Red
    delay(1000);
    RGBcolor(255, 255, 255); // White
    delay(1000);
  }
//  else {Serial.print("Espace disponible ");
//  Serial.print(freeKB);
//  Serial.println(" kB");
//  return;
//  }
}
///////////////////////
void reset_param(){                // fonction reset qui va mettre par défaut les différents parametres
  param.LOG_INTERVAL = 10;
  param.FILE_MAX_SIZE = 2048;
  param.TIME_OUT = 30;
  param.LUMIN = 1;
  param.LUMIN_LOW = 255;
  param.LUMIN_HIGH = 768;
  param.TEMP_AIR = 1;
  param.MIN_TEMP_AIR = -10;
  param.MAX_TEMP_AIR = 60;
  param.HYGR = 1;
  param.HYGR_MINT = 0; 
  param.HYGR_MAXT = 50;
  update_param(); 
}
///////////////////////
void mConfig(){          // mode config
  RGBcolor(255, 255, 0);
  String commande;
  second_counter = 0;
  while (Serial.available()==0){
    if(second_counter >= 30*60){
      current_mode = 1;
      break;
    }
  }
  commande = Serial.readStringUntil('\n');
  delay(500);
  if(commande == "version"){
      Serial.println(F("29.0.6"));
      Serial.println(F("E15NY"));
  }else if (commande == "reset"){
    reset_param();
  }else if (commande == "clock"){
    set_clock();
  }else if (commande == "date"){
    set_date();
  }else if (commande == "day"){
    //set_day();
  }else{
    if (commande.indexOf("=")>0){
      int value = atoi(commande.substring(commande.indexOf("=")+1).c_str());
      if(commande.startsWith("LOG_INTERVAL")){
        param.LOG_INTERVAL = value;
      }
      else if(commande.startsWith("FILE_MAX_SIZE")){
        param.FILE_MAX_SIZE = (unsigned int) value;
      }
      else if(commande.startsWith("TIME_OUT")){
        param.TIME_OUT = (unsigned int)value; 
      }
      else if(commande.startsWith("LUMIN")){
        param.LUMIN = (bool)value;
        
      }
      else if(commande.startsWith("LUMIN_LOW")){
        param.LUMIN_LOW = (unsigned short int)value;
        Serial.print(param.LUMIN_LOW);
      }
      else if(commande.startsWith("LUMIN_HIGH")){
        param.LUMIN_HIGH = (unsigned short int)value;
      }
      else if(commande.startsWith("TEMP_AIR")){
        param.TEMP_AIR = (bool)value;
      }
      else if(commande.startsWith("MAX_TEMP_AIR")){
        param.MAX_TEMP_AIR = (int8_t)value;
      }
      else if(commande.startsWith("MIN_TEMP_AIR")){
        param.MIN_TEMP_AIR = (int8_t)value ;
      }
      else if(commande.startsWith("HYGR")){
        param.HYGR = (bool)value;
      }
      else if(commande.startsWith("HYGR_MAXT")){
        param.HYGR_MAXT = (uint8_t)value;
      }
      else if(commande.startsWith("HYGR_MINT")){
        param.HYGR_MINT = (uint8_t)value;
      }else if(commande.startsWith("TIME_OUT")){
        param.TIME_OUT = (uint8_t)value;
      }
      update_param();
    }else {Serial.println(F("[erreur] : Verifiez votre commande {PARAMETRES_A_MODIFIER = VALEUR}"));mConfig();}
  } 
}
///////////////////////
void mStandard(){ // mode standard
  Capteurs();
  ecrtSD();
  dataFile = SD.open("date.txt");
}
///////////////////////
void mEco(){ // mode economique
  Capteurs();
  ecrtSD();
}
///////////////////////
void mMaint(){ // mode maintenance
  Capteurs();
  Serial.println(dataString);
  }
///////////////////////
void update_param(){
  EEPROM.put(0,param); // fonction qui va renvoyer les valeurs dans l'eprom
}
///////////////////////
void set_date(){ // fonction qui va ns permettre de changer la date sur le mode config
  Serial.println(F("Donnez la date au format ( MOIS{1-12},JOUR{1-31},ANNEE{2000-2099})"));
  while (Serial.available() == 0);
  second_counter = 0;
  String date = Serial.readStringUntil('\n');
  delay(500);
  if((date.length()!= 10) && (atoi(date.substring(3,4).c_str()) <= 12)  && (atoi(date.substring(3,4).c_str()) >= 1 )&& (atoi(date.substring(6,10).c_str()) >= 2000) && (atoi(date.substring(6,10).c_str())<= 2099) && (atoi(date.substring(0,1).c_str()) <= 31) && (atoi(date.substring(0,1).c_str())>= 1)){
    Serial.println(F("[erreur : date non valide"));
    set_date();
  }
  if(RTC.isrunning()){
    tm = RTC.now();
  }else{
    tm = DateTime();
    Serial.println(F("veuillez regler l'heure"));
  }
  RTC.adjust(DateTime((uint16_t)(atoi(date.substring(6,10).c_str())),(uint8_t)(atoi(date.substring(3,4).c_str())),(uint8_t)(atoi(date.substring(0,1).c_str())),tm.hour(),tm.minute(),tm.second())); 
}
///////////////////////
void set_clock(){ // fonction qui va ns permettre de changer la valeur de l'heure sur le mode config
  Serial.println(F("Donnez l'heure au format (HEURE{00-23}:MINUTE{00-59}:SECONDE{00-59})"));
  while(Serial.available() == 0);
  second_counter = 0;
  String clock = Serial.readStringUntil('\n');
  delay(500);
  if(clock.length() != 8 ){
    Serial.println(F("[erreur] : heure non reconue"));
    set_clock();
  }
  if(RTC.isrunning()){
    tm = RTC.now();
  }else
  {
    tm = DateTime();
    Serial.println(F("veuillez regler la date"));
  }
  RTC.adjust(DateTime(tm.year(),tm.month(),tm.day(),(uint8_t)(atoi(clock.substring(0,1).c_str())),(uint8_t)(atoi(clock.substring(3,4).c_str())),(uint8_t)(atoi(clock.substring(6,7).c_str()))));
}
///////////////////////
void setup(){                 // initialisation des différents capteurs et paramètres
  reset_param();
  EEPROM.get(0,param);
  Wire.begin();
  Serial.begin(9600);
// gpsSerial.begin(9600); 
  dht.begin();
  lightMeter.begin();
  sd();
 if (!RTC.begin()) {
   Serial.println(F("Couldn't find RTC"));
   while (1);
 }
 if (!RTC.isrunning()) {
   Serial.println(F("RTC is NOT running!"));
   RTC.adjust(DateTime(__DATE__, __TIME__));
 }
 pinMode(RLED, OUTPUT);
 pinMode(GLED, OUTPUT);
 pinMode(BLED, OUTPUT);
 pinMode(REDBUT, INPUT);
 pinMode(GREENBUT, INPUT);
 initialisation_interrup();
}
///////////////////////
void loop(){            // appel des différents modes
  mode=1;
  if(mode == 2){
    Serial.println(F("Mode Config"));

    RGBcolor(255, 255, 0);
    mConfig();
  }else if (mode == 1){
    RGBcolor(0, 255, 0);
    Serial.println(F("Mode Standard"));
    mStandard();
  }else if (mode == 4){
    RGBcolor(255, 127, 0);
    Serial.println(F("Mode Maintenance"));
    mMaint();
  }else if (mode == 3){
    RGBcolor(0, 0, 255);
    Serial.println(F("Mode Economie")); 
    mEco();
  }
  
}
