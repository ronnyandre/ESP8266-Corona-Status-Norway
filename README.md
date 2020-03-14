# Norsk Corona-tracker

Siden du har litt tid til overs om dagen, og skulle du tilfeldigvis ha en ESP8266-kontroller og en SSD1306 OLED liggende har jeg en idé til deg.

## Oppsett

1. Koble skjermen til mikrokontrolleren. Eksemplet nedenfor viser hvordan det kobles til en Wemos D1 Mini.
    
    | **SSD 1306** | **Wemos D1 Mini**s |
    |----------|---------------|
    | VCC/VDD  | 5v            |
    | GND      | GND           |
    | SCL      | D1            |
    | SDA      | D2            |

    ![Kobling mellom Wemos D1 Mini og SSD1306 OLED](https://automatedhome.party/wp-content/uploads/2017/04/WemosOLED.png "Kolbingsskjema")

2. Endre variablene `ssid` og `pass` i koden for å matche navnet på ditt wifi og passordet.

3. Kjør!


## Kjente feil

1. Scrolling fungerer ikke.

_PS: Dette er fsørste gang jeg bruker OLED, og koden er veldig WIP, men bidra gjerne._

_PPS: Scriptet henter JSON-data fra VG.no sin statusside. All data tilhører VG, og bruk med omhu._