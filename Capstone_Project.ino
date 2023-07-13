//Library yang digunakan
#include <DHT.h> // Library Sensor DHT11
#include <GravityTDS.h> // Library Sensor TDS
#include <EEPROM.h> // Library EEPROM untuk Sensor TDS
#include <Blynk.h> //Library Bylnk
#include <WiFi.h> //Library WiFi
#include <WiFiClient.h> //Library WiFi
#include <BlynkSimpleEsp32.h> //Library ESP Blynk untuk ESP 32

//Definisi Pin dan Sensor
#define DHTPIN 23 // Mendefinisikan pin 23 untuk sensor DHT
#define DHTTYPE DHT11 //Mendefinisikan tipe DHT11
#define TdsSensorPin 35 //Mendefinisikan pin 35 untuk sensor TDS
#define trig_pin 18 // Mendefinisikan pin 18 untuk Trigger pada sensor ultrasonik
#define echo_pin 19 // Mendefinisikan pin 19 untuk Echo pada sensor ultrasonik
#define pH_Pin 34 // Mendefinisikan pin 34 untuk pH_pin pada sensor pH
#define BLYNK_TEMPLATE_ID "TMPL6BAUxzblG"
#define BLYNK_TEMPLATE_NAME "MonitoringTanamanHidroponik"
#define BLYNK_AUTH_TOKEN "4aBWofZvHYYx3ugIlL6MJ4LYADRs6S4a"
#define BLYNK_PRINT Serial

//Deklarasi Variable
long echotime; // Variable echotime untuk sensor ultrasonik
float temperature = 25; // Variable temperature untuk sensor TDS
float tdsValue = 0; // Variable tdsValue untuk sensor TDS (Nilai Awal)
float luas_alas = 698.5; // Variable luas_alas untuk sensor ultrasonik (luas alas wadah)
float tinggi_wadah = 11.5; // Variable tinggi_wadah untuk sensor ultrasonik (tinggi wadah)
float volume; // Variable volume untuk sensor ultrasonik
float tinggi_air; // Variable tinggi_air untuk sensor ultrasonik
float jarak_pantul; // Variable jarak_pantul untuk sensor ultrasonik
float h; // Variable h untuk sensor DHT11 
float t; // Variable t untuk sensor DHT11
float pH; // Variable pH untuk sensor pH (Nilai pH)
float pHValue=0; // Variable pHValue untuk sensor pH (Nilai ADC)
float voltagepH; // Variable voltagepH untuk sensor pH
float ppm_r = 560.0;
float ppm_t = 860.0;
float pH_Set_r = 6.00;
float pH_Set_t = 7.00;
float air_set = 9.0;
char ssid[] = "RedmiNote8";
char pass[] = "13082001";
char auth[] = BLYNK_AUTH_TOKEN;
const int IN1 = 27;
const int IN2 = 26;
const int IN3 = 25;
const int IN4 = 33;
const int IN5 = 15;
const int IN6 = 5; 

// Membuat Objek
DHT dht(DHTPIN, DHTTYPE); // Membuat objek dari class DHT
GravityTDS gravityTds; // Membuat objek dari class GravityTDS
BlynkTimer timer;

void setup(){
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(IN1, OUTPUT); // Pompa PH Up
  pinMode(IN2, OUTPUT); // Pompa PH Down
  pinMode(IN3, OUTPUT); // Pompa Larutan A
  pinMode(IN4, OUTPUT); // Pompa Larutan B
  pinMode(IN5, OUTPUT); // Pompa Air
  pinMode(IN6, OUTPUT); // Pompa Air Netral
  pinMode(pH_Pin, INPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  digitalWrite(trig_pin, LOW);
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(3.3);
  gravityTds.setAdcRange(4096);
  gravityTds.begin();
  dht.begin();
}
void loop(){
  Blynk.run();
  timer.run();
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  echotime     = pulseIn(echo_pin, HIGH);
  jarak_pantul = (0.034*(float)echotime)/2;
  tinggi_air   = tinggi_wadah - jarak_pantul;
  volume       = luas_alas*tinggi_air;
  Serial.print("====================\n");
  Serial.println("Sensor Ultrasonik ");
  Serial.print("Jarak Permukaan = ");
  Serial.print(jarak_pantul);
  Serial.println(" cm");
  Serial.print("Tinggi Air      = ");
  Serial.print(tinggi_air);
  Serial.println(" cm");
  Serial.print("Volume Air      = ");
  Serial.print(volume);
  Serial.println(" cm3");
  Serial.print("====================\n");
  Blynk.virtualWrite(V4, tinggi_air);
  delay(2000);
  
  Serial.print("====================\n");
  Serial.println("Sensor DHT11 ");
  delay(2000);
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)){
    Serial.println("Failed to read form DHT sensor!");
    return;
  }
  Serial.print("Suhu : ");
  Serial.print(t);
  Serial.print("|| Kelembapan : ");
  Serial.print(h);
  Serial.print("\n====================");
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  if (t>30){
    //Blynk.email("galihbaskorohidayat@gmail.com","Alert","Temperature Over 30 C !");
    Blynk.logEvent("notifikasi", "Suhu diatas 30 Derajat celcius");
  }

  gravityTds.setTemperature(temperature);
  gravityTds.update();
  tdsValue = gravityTds.getTdsValue();
  Serial.print("====================\n");
  Serial.println("Sensor TDS");
  Serial.print(tdsValue, 0);
  Serial.println(" ppm");
  Serial.print("====================\n");
  Blynk.virtualWrite(V2, tdsValue);
  delay(1000);

  Serial.print("====================\n");
  Serial.println("Sensor pH ");
  pHValue= analogRead(pH_Pin);
  Serial.print("Nilai adc : ");
  Serial.println(pHValue);
  voltagepH = pHValue*(3.3/4095.0);
  pH = (3.3*voltagepH);
  Serial.print("Nilai pH : ");
  Serial.println(pH);
  Serial.print("Nilai Tegangan : ");
  Serial.println(voltagepH);
  Serial.print("====================\n");
  Blynk.virtualWrite(V3, pH);
  delay(500);
  // If-Else pH
  if(pH < pH_Set_r){
    delay(1000);
    digitalWrite(IN1, HIGH);
    delay(2000);
    digitalWrite(IN1, LOW);
    delay(30000);
  }else if(pH > pH_Set_t){
    delay(1000);
    digitalWrite(IN2, HIGH);
    delay(2000);
    digitalWrite(IN2, LOW);
    delay(30000);
  }else{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
  // If-Else TDS
  if(tdsValue < ppm_r){
    delay(1000);
    digitalWrite(IN3, HIGH);
    delay(2000);
    digitalWrite(IN3, LOW);
    delay(2000);
    digitalWrite(IN4, HIGH);
    delay(2000);
    digitalWrite(IN4, LOW);
    delay(2000);
    delay(30000);
  }else if(tdsValue > ppm_t){
    delay(1000);
    digitalWrite(IN6, HIGH);
    delay(2000);
    digitalWrite(IN6, LOW);
    delay(2000);
    delay(30000);
  }else{
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    digitalWrite(IN6, LOW);
  }
  // If-Else Tinggi Air
  if(tinggi_air < air_set){
    delay(1000);
    digitalWrite(IN5, HIGH);
    delay(3000);
    digitalWrite(IN5, LOW);
    delay(3000);
  }else{
    digitalWrite(IN5, LOW);
  }
}


