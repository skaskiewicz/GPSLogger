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
zgaszona -> wszystko OK! i zapala zieloną :-)

Wykorzystuję bibliotekę TinyGPS++: <http://arduiniana.org/libraries/tinygpsplus/>

Poniżej wersja testowa urządzenia na Arduino (docelowo przeniesienie na płytkę prototypową oraz wykorzystanie platformy 3,3V):

![IMG_20200413_120413](https://github.com/user-attachments/assets/b461d621-0bfa-4769-ae05-7f1d095d0a53)
![IMG_20200413_120408](https://github.com/user-attachments/assets/fef021db-b1e7-4b9f-b6e4-2d54b0a747cb)
![IMG_20200413_120404](https://github.com/user-attachments/assets/98396da6-47a2-47bf-aa7b-96b81cbecba5)
![IMG_20200413_120354](https://github.com/user-attachments/assets/f9fd685e-47b3-4551-a022-bcbdbab6e098)
![IMG_20200413_120351](https://github.com/user-attachments/assets/df1147b6-8524-4e8e-ac2e-eed6cb497030)
![IMG_20200413_120346](https://github.com/user-attachments/assets/93a89f97-b5cc-492f-b496-e9d88cfaf907)
![IMG_20200413_120334](https://github.com/user-attachments/assets/228caa0e-0b4d-4bd1-a214-e266dc255180)
