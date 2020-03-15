# Corona-19 Tracker

Vis status på antall smittede, friske og døde etter Covid-19 i Norge. Henter data fra VG.no.

## Oppsett

1. Koble skjermen til mikrokontrolleren. Eksemplet nedenfor viser hvordan det kobles til en Wemos D1 Mini.
    
    | **SSD 1306** | **Wemos D1 Mini** |
    |----------|---------------|
    | VCC/VDD  | 5v            |
    | GND      | GND           |
    | SCL      | D1            |
    | SDA      | D2            |

    ![Kobling mellom Wemos D1 Mini og SSD1306 OLED](https://automatedhome.party/wp-content/uploads/2017/04/WemosOLED.png "Kolbingsskjema")

2. Endre dine wifi-innstillinger i  `config.h`.
   
3. Kjør!


## Kjente feil

1. Scrolling fungerer ikke.4

_PS: Dette er fsørste gang jeg bruker OLED, og koden er veldig WIP, men bidra gjerne._

_PPS: Scriptet henter JSON-data fra VG.no sin statusside. All data tilhører VG, så bruk med omhu._