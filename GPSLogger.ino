/*
   GPS LOGGER ORAZ ODBIORNIK GPS (fork https://github.com/CheapskateProjects/GPSLogger)

   Program realizuje dwie funkcje: zapisuje aktualne położenie do pliku lub wysyła dane na port wyjściowy (nie jednocześnie).
   Jest to zależne od obecniości karty SD: jest - zapisuje do pliku, brak - wysyła na port w standardzei NMEA.
   Logowane są następujące informacje:
   długość geog.,szerokość geog,wysokość_do_decymetra,data(rrrr.mm.dd),czas GMT(00:00:00),rodzaj_fix'a,hdop,pdop,ilosc_satelit_z_fixem
   Dane są logowane standardowo co 5 sekund.
   Nazwa pliku z danymi to data (rrrr.mm.dd).
   Takie dane można przekonwertować na stronie https://www.gpsvisualizer.com/
   Dioda czderwona sygnalizuje 3 stany:
   świeci -> brak karty - gdy karta wyjęta wysyła dane na serial tak, że można podłączyć do PC i korzystać
   miga -> inicjalizuje się, czeka na FIX; błąd
   zgaszona -> wszystko OK! i zapala zieloną :)
*/

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

//piny do komunikacji z GPS
#define RXPin 2
#define TXPin 3

/*
   pin do diody inforamcyjnej:
   czerwona świeci -> brak karty - gdy karta wyjęta wysyła dane na serial tak, ze mozna podłączyć do PC i korzystać
   miga -> inicjalizuje się, czeka na FIX; błąd
   zgaszona -> wszystko OK! zielona miga :)
*/
#define GpsLedPinRed 7

//dioda zielona dla czytelnosci, że OK. bo brak swiecacej diody niektorych moze wprawiac w zaklopotanie
#define GpsLedPinGreen 6

// szybkość transmisji danych.
#define GPSBaud 9600

// częstotliwość zapisu lokalizacji (w milisekundach)
#define frequency 5000

// gps object
TinyGPSPlus gps;

//będzie sprawdzać czy odbiornik ma FIX: 1-brak, 2-fix 2D (można wykorzystać), 3-fix 3D
TinyGPSCustom quality(gps, "GNGSA", 2);

//PDOP
TinyGPSCustom pdop(gps, "GNGSA", 15);

//HDOP
TinyGPSCustom hdop(gps, "GNGSA", 16);

//ilość satelit musi być minimum 3 dla poprawnej pozycji. 4 dla pozycji z poprawką zegara GPS co podnosi dokładnosc
String sat = "0";

// serial
SoftwareSerial ss(RXPin, TXPin);

// true, gdy znaleźliśmy lokalizację od ostatniego uruchomienia i plik został otwarty
boolean opened = false;

// aktualny plik danych
File dataFile;

// nazwa pliku
String fileName;

// kontrolka czasu do sprawdzania odstępu 5 sekund
unsigned long previous = 0;

//kontrolka obecnosci karty SD
boolean sdPresent = false;

void setup()
{
  //zaninicjalizuj serial
  Serial.begin(GPSBaud);
  ss.begin(GPSBaud);

  //zainicjalizuj diody
  pinMode(GpsLedPinRed, OUTPUT);
  digitalWrite(GpsLedPinRed, LOW);
  pinMode(GpsLedPinGreen, OUTPUT);
  digitalWrite(GpsLedPinGreen, LOW);

  //sprawdź kartę SD
  sdPresent = SD.begin(10);
  //brak SD - zapal diodę
  if (sdPresent == false)
  {
    digitalWrite(GpsLedPinRed, HIGH);
  }
}

void loop()
{
  //brak karty - wysyłaj wszystko na serial, bez kontroli, bez diody. weryfikacja poprawnosci następuje w dedykowanych programach
  if (sdPresent == false) {
    while (ss.available()) {
      byte c = ss.read();
      Serial.print((char)c);
    }
  }
  else {
    // jesli cos przychodzi z seriala to działaj
    while (ss.available()) {
      if (gps.encode(ss.read())) {
        //odczytaj info o fix'ie
        String  fix = quality.value();
        //odczytaj info o horyzontalnej precyzji
        String horizPrec = hdop.value();
        //odczytaj info o ilosci satelit
        sat = gps.satellites.value();
        // Mrugaj czerwoną aż do ustalenia poprawnej pozycji (nawet do 5-7 minut przy zimnym starcie w złych warunkach)
        if (gotFIX(fix, horizPrec, sat) == 0)
        {
          blinkLed(false);
          return;
        }
        //działaj jeśli ma fix'a
        else {
          logInfo();
        }
      }
    }
  }
}
/*
   funkcja sprawdzajaca czy odbiornik GPS osiągnał założoną dokładnosć
*/
int gotFIX(String fix, String horizPrec, String sat) {
  //zamiana ze stringa otrzymanego z biblioteki TinyGPS++ na inty
  int fixInt = fix.toInt();
  int satInt = sat.toInt();
  int horizPrecInt = horizPrec.toInt();
  /*
     sprawdzenie dokładnosci otrzymanej pozycji z odbiornika GPS
     fixInt:
     - 1 brak fixa
     - 2 fix 2D
     - 3 fix 3D
     horizPrecInt:
     - inaczej hdop - poniżej 10 dla rozwiązania 3D, poniżej 5 dla rozwiązania 2D
     satInt:
     - minimum do wyznaczenia pozycji w 2D to 3 satelity. do dokładnej są 4 ponieważ jest to poprawka czasu.
     dlatego zawsze warto mieć 4 bo jest pewność poprawności wyzanczenia
  */
  if (fixInt == 3 && horizPrecInt < 10 && satInt > 3 || fixInt == 2 && horizPrecInt < 5 && satInt > 3)
    return 1;
  else
    return 0;
}
/*
   funkcaj migajaca diodami
   false - czerwona
   true - zielona
*/
void blinkLed(boolean led) {
  if (led == false) {
    digitalWrite(GpsLedPinGreen, LOW);
    digitalWrite(GpsLedPinRed, HIGH);
    delay(20);
    digitalWrite(GpsLedPinRed, LOW);
  }
  else {
    digitalWrite(GpsLedPinRed, LOW);
    digitalWrite(GpsLedPinGreen, HIGH);
    delay(20);
    digitalWrite(GpsLedPinGreen, LOW);
  }
}
// funkcja drukująca "0" przy jednocyfrowych liczbach
void printIntValue(int value)
{
  if (value < 10)
  {
    dataFile.print(F("0"));
  }
  dataFile.print(value);
}

// funkcja realizująca zapis danych
void logInfo()
{
  if (!opened)
  {
    // tworzy plik z nazwa w formie rrrr.mm.dd.txt jeśli nie istnieje
    fileName = "";
    fileName += gps.date.year();
    if (gps.date.month() < 10) fileName += "0";
    fileName += gps.date.month();
    if (gps.date.day() < 10) fileName += "0";
    fileName += gps.date.day();
    fileName += ".txt";
    opened = true;
    /*
       jesli plik nie istnieje tworzy naglowek
       jesli istnieje plik zgodny z datą pomiarów następuje dopisywanie
    */
    if (!SD.exists(fileName)) {
      dataFile = SD.open(fileName, FILE_WRITE);
      dataFile.print("latitude,longitude,altitude,date,time,fix,hdop,pdop,sattelites in fix");
      dataFile.println();
      dataFile.close();
    }
  }

  // jesli ok migaj na zielono
  blinkLed(true);

  //sprawdź czy minęło 5 sekund i zapisz
  if (millis() - previous > frequency)
  {
    //ustaw czas kontrolny
    previous = millis();
    //otwórz plik i zapisuj
    dataFile = SD.open(fileName, FILE_WRITE);
    //długosć geograficzna
    dataFile.print(gps.location.lat(), 9);
    dataFile.print(F(","));
    //szerokość geograficzna
    dataFile.print(gps.location.lng(), 9);
    dataFile.print(F(","));
    //wysokosc w metrach nad poziomem morza
    dataFile.print(gps.altitude.meters(), 1);
    dataFile.print(F(","));
    //rok w formacie rrrr
    dataFile.print(gps.date.year());
    dataFile.print(F("."));
    //miesiąc w formacie mm
    printIntValue(gps.date.month());
    dataFile.print(F("."));
    //dzień w formacie dd
    printIntValue(gps.date.day());
    dataFile.print(F(","));
    //godzina w formacie hh
    printIntValue(gps.time.hour());
    dataFile.print(F(":"));
    //minuta w formacie mm
    printIntValue(gps.time.minute());
    dataFile.print(F(":"));
    //secunda w formacie ss
    printIntValue(gps.time.second());
    dataFile.print(F(","));
    //dokładność fix'a (2 lub 3)
    dataFile.print(quality.value());
    dataFile.print(F(","));
    //parametr hdop
    dataFile.print(hdop.value());
    dataFile.print(F(","));
    //paramtetr pdop
    dataFile.print(pdop.value());
    dataFile.print(F(","));
    //ilosc satelit
    dataFile.print(gps.satellites.value());
    dataFile.println();
    dataFile.close();
  }
}
