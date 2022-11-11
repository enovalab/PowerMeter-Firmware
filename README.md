# PowerMeter

## Dokumentation
### Datenstrukturierung
Die Datenstrukturierung legt fest, wie Daten kategorisiert werden.

#### Flüchtige Daten (werden bei Anfrage erhoben)
- Momentane Messwerte
```json
{
    "voltage": 234.3,
    "current": 0.23,
    "active": 67.32,
    "apparent": 70.59,
    "reactive": 21.18,
    "factor": 0.88
}
```
#### Nichtflüchtige Daten (gespeichert im SPIFFS)
- Konfiguration
```json
{
    "name": "Default Name",
    "relay": true,
    "calibration": {
        "voltage": 0.4908,
        "current": 0.01596,
        "phase": 12
    },
    "wifi": {
        "sta": {
            "ssid": "YourWiFiNetwork",
            "password": "SuperSecretStuff",
            "staticip": "0.0.0.0",
            "gateway": "0.0.0.0",
            "subnet": "0.0.0.0",
            "dns1": "0.0.0.0",
            "dns2": "0.0.0.0"
        },
        "ap": {
            "ssid": "Power Meter AP",
            "password": "Katek1976"
        },
        "accespoint": true
    }
}
```
- Verlaufsdaten (neuster Messwert am Ende des Arrays)
```json
[
    69.0,
    420.0,
    122.6,
    84.5,
    89.4,
    543.3,
    102.1,
    455.5,
    32.03,
    10.0,
    323.1,
    122.4
]
```
## Hardware
Schaltung und Leiterplattenlayout findest du unter folgendem Link:
[Power Meter - Easy EDA](https://u.easyeda.com/join?type=project&key=1f3c904dbd0d79bb1bfd9a29f802ef85&inviter=85809e691e9d409fb29755267ff12244)


## Anleitung zur Teilname

### a) Repository klonen
Um das Projekt auf deinen Computer zu bekommen und es auf den ESP hochzuladen brauchst du:
1. VS Code installiert https://code.visualstudio.com/download
2. In VS Code die Extension PlatformIO
2. Git installiert https://git-scm.com/download/win
3. Einen GitHub Account

**Wichtig**: Vor dem Klonen musst du noch:
1. In VS Code auf die PlatformIO-Home-Seite gehen und unter Platforms -> Embedded "Espressif32" ins Suchfeld eingeben und installieren.
2. Nach abgeschlossener Installation musst du ein Dummy-Projekt mit der ESP32 Platform erstellen, falls du noch nie ein ESP32 Projekt in PlatformIO erstellt hast. Gehe dazu auf die PlatformIO-Home-Seite -> New Project. 
    - Name: z.B. DummyProjekt
    - Board: DOIT ESP32 DEVKIT V1
    - Framework: Arduino
    - Haken bei "Use default Location" muss gesetzt sein
    
    Klicke dann "Finish". Dadurch werden alle Dateien generiert um mit ESP32 Projekten zu arbeiten.


Jetzt kannst du das Repository klonen:
1. Klicke in VS Code an der linken Seite auf das Alien-Symbol.
2. Gehe hier oben auf Code und kopiere den Link
3. Unter Quick Access -> Miscellaneous -> Clone Git Project
4. Füge den Link ein und drücke Enter
5. Wähle als Speicherort: Dokumente -> PlatformIO -> Projects
6. Klicke auf Öffnen

_Fertig!_ Du hast erfolgreich das Repository geklont.

## Entwicklung
### Software
- [ ] Log Makros für Serial und File
- [ ] Strukturierung der Software

### Hardware
- [ ] Leiterplattenlayout
- [ ] Upload ohne USB ermöglichen