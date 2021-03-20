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

Wykorzystuję bibliotekę TinyGPS++: <http://arduiniana.org/libraries/tinygpsplus/>

![Foto 1](https://skaskiewicz.pl/.foto/IMG_20200413_120334.jpg)
![Foto 2](https://skaskiewicz.pl/.foto/IMG_20200413_120346.jpg)
![Foto 3](https://skaskiewicz.pl/.foto/IMG_20200413_120351.jpg)
![Foto 4](https://skaskiewicz.pl/.foto/IMG_20200413_120404.jpg)
![Foto 5](https://skaskiewicz.pl/.foto/IMG_20200413_120408.jpg)
![Foto 6](https://skaskiewicz.pl/.foto/IMG_20200413_120413.jpg)
