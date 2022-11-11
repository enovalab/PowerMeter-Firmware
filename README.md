# PowerMeter

## Dokumentation
### Datenstrukturierung
Die Datenstrukturierung legt fest, wie Daten kategorisiert werden.
#### Flüchtige Daten
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
    - Metadaten
```json
{
    "name": "Default Name",
    "relay": true
}
```
    - WiFi Konfiguration
```json
{
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
        "password": "SuperSecretStuff"
    },
    "accespoint": true
}
```
    - Verlaufsdaten
```json
[
    0.0, // ältester Messwert
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0  // neuster Messwert
]
```

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



