# RC-Buggy
// muss alles noch angepasst werden

Easy to use RC-Car Lösung

Ziele dieses Projekts ist die Umsetzung eines vollständig ferngesteuerten Autos mit möglichst geringem Aufwand insbesondere in Bezug auf die Energieversorgung und Fernsteuerung.
![image](https://github.com/user-attachments/assets/6d364af6-8729-486a-9fd6-0fee1ced2c6f)

Das Projekt gliedert sich in folgende Teilprojekte:
- 3D-Design (Entwurf und 3D-Druck des Autos)
- PCB-Design (Entwurf einer Platine)
- Mikrocontroller-Software (Programmierung der Ansteuerung der Motoren und Empfang der Fahrbefehle über Bluetooth Low Energy)
- WebApp (Webseite zur Fernsteuerung des Autos)

## 3D-Design
Das Chassis kann mit jedem 3D-CAD Programm entworfen werden. Ursprünglich ist es in Autodesk Fusion360 entwickelt worden.
Die Bodengruppe kann unabhängig vom gestalteten Chassis verwendet werden.
![image](https://github.com/user-attachments/assets/9915c54e-8df8-4378-97af-516384eb4df5)


## PCB-Design
Die Hardware wird auf Basis des Mikrocontroller Boards **Seed XIAO ESP32C3** umgesetzt. Dazu exisitiert eine einfache Steuerplatine. Sie dient dazu folgende Aktoren anzusteuern:
- 4 Motoren über zwei H-Brücken vom Typ MX1508
- einen Servo
- 2 transistorgesteuerte Lasten (z.B. für LEDs)

Als Energiequelle wird eine Standard 5V-Powerbank benutzt.

Die Platine wurde mit einem DC/DC-Wandler (MT3608) ergänzt, um die Spannung der Motoren anzuheben und damit die Fahrgeschwindigkeit des Modells zu erhöhen. 

Der Schaltplan der Platine
![image](https://github.com/user-attachments/assets/1cf77ce4-5130-4cb7-8478-ee43e15c6c76)

3D-Darstellung
![image](https://github.com/user-attachments/assets/b8c841bd-829f-4cba-bef2-268f2a5c381d)

## Mikrocontroller-Software
Zur Verwendung der Mikrocontroller Software kann am einfachsten die Arduino IDE genutzt werden. In der IDE müssen unter Tools/Boards/Boardmanager die esp32-Boards ergänzt werden.
![image](https://github.com/user-attachments/assets/33c61387-2fd0-4319-98e0-e48e7ce7a221)

Dann kann das Programm für das Zielboard XIAO_ESP32C3 übersetzt werden
![image](https://github.com/user-attachments/assets/b8718e6f-c345-40b3-a95b-f238c2b7cce1)

Der folgende Ausschnitt zeigt den wesentlichen Deklaration/setup-Bereich des Programms. Erkennbar leicht lassen sich die Aktoren ansprechen. Das Verhalten lässt sich nach eigenen Wünschen anpassen.  
![image](https://github.com/user-attachments/assets/b1a4fc9b-f44e-455f-808c-995cd18fedea)

## WebApp
Zur Fernsteuerung wird auf Bluetooth Low Energy (BLE) gesetzt. Damit ist der Mikrocontroller direkt mit Hilfe einer Web-App ansteuerbar. Voraussetzung dafür ist, dass die Webseite mit einem BLE-fähigen Browser (z.B. Google Chrome) genutzt wird.
Die WebApp kann auf einem Server oder direkt als Datei auf dem Smartphone oder jedem anderen BLE-fähigen Device betrieben werden. Zur Steuerung werden die Neigungssensoren des Devices genutzt.

![image](https://github.com/user-attachments/assets/c30d9081-db10-4b51-a62a-15ac75144089)
