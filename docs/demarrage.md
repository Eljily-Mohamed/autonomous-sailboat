# Démarrage du projet

## Prérequis

Avant de commencer, assurez-vous d'avoir :

- **Arduino IDE** (version 1.8.x ou 2.x)
- **ESP32 Board Support** installé
- **Bibliothèques requises** (voir ci-dessous)
- **Matériel** : ESP32, module LoRa, GPS, servomoteurs

## Installation d'Arduino IDE

### Étape 1 : Télécharger Arduino IDE

1. Allez sur [arduino.cc/en/software](https://www.arduino.cc/en/software)
2. Téléchargez la version pour votre système d'exploitation
3. Installez Arduino IDE

![Installation Arduino IDE](images/arduino-ide-install.png)
*Capture d'écran de l'installation Arduino IDE - À ajouter dans `docs/images/arduino-ide-install.png`*

### Étape 2 : Installer le support ESP32

1. Ouvrez Arduino IDE
2. Allez dans **Fichier → Préférences**
3. Dans **URL de gestionnaire de cartes supplémentaires**, ajoutez :
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Allez dans **Outils → Type de carte → Gestionnaire de cartes**
5. Recherchez "ESP32" et installez "esp32 by Espressif Systems"

![Installation ESP32](images/esp32-install.png)
*Capture d'écran de l'installation du support ESP32 - À ajouter dans `docs/images/esp32-install.png`*

## Installation des bibliothèques

### Bibliothèques requises

Installez les bibliothèques suivantes via **Croquis → Inclure une bibliothèque → Gérer les bibliothèques** :

1. **LoRa** par Sandeep Mistry
   - Recherchez "LoRa" et installez la version la plus récente

2. **TinyGPSPlus** par Mikal Hart
   - Recherchez "TinyGPSPlus" et installez

3. **ESP32Servo** par Kevin Harrington
   - Recherchez "ESP32Servo" et installez

### Installation manuelle (si nécessaire)

Si certaines bibliothèques ne sont pas disponibles via le gestionnaire :

1. Téléchargez les bibliothèques depuis GitHub
2. Placez-les dans le dossier `Arduino/libraries/`
3. Redémarrez Arduino IDE

## Configuration du projet

### Étape 1 : Sélectionner la carte

1. Ouvrez le fichier `boat.ino` dans Arduino IDE
2. Allez dans **Outils → Type de carte → ESP32 Arduino**
3. Sélectionnez **ESP32 Dev Module** (ou votre modèle spécifique)

![Sélection de carte](images/board-selection.png)
*Capture d'écran de la sélection de carte ESP32 - À ajouter dans `docs/images/board-selection.png`*

### Étape 2 : Configurer les paramètres

Dans **Outils**, configurez :

- **Upload Speed** : 115200 (ou plus rapide si supporté)
- **CPU Frequency** : 240 MHz (recommandé)
- **Flash Frequency** : 80 MHz
- **Flash Size** : 4MB (ou selon votre ESP32)
- **Partition Scheme** : Default 4MB with spiffs
- **Core Debug Level** : None (pour performance)

### Étape 3 : Configurer les pins

Ouvrez `config_pins.h` et vérifiez/modifiez les pins selon votre configuration :

```cpp
// Pins LoRa
#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_CS 5
#define LORA_RST 14
#define LORA_IRQ 26

// Pins GPS
#define GPS_RX 16
#define GPS_TX 17

// Pins Servos
#define SERVO_SAIL_PIN 25
#define SERVO_RUDDER_PIN 26

// Pins Radio Receiver
#define RADIO_CH1_PIN 34
#define RADIO_CH2_PIN 35
#define RADIO_SEL_PIN 36
```

## Compilation et upload

### Étape 1 : Vérifier la connexion

1. Connectez votre ESP32 via USB
2. Vérifiez que le port série est détecté dans **Outils → Port**
3. Sélectionnez le bon port (ex: `/dev/ttyUSB0` ou `COM3`)

### Étape 2 : Compiler

1. Cliquez sur **Vérifier** (✓) ou appuyez sur `Ctrl+R`
2. Vérifiez qu'il n'y a pas d'erreurs de compilation

### Étape 3 : Uploader

1. Cliquez sur **Téléverser** (→) ou appuyez sur `Ctrl+U`
2. Attendez la fin du téléversement
3. Le moniteur série s'ouvrira automatiquement (ou ouvrez-le via **Outils → Moniteur série**)

!!! warning "Problèmes courants"
    Si l'upload échoue :
    - Appuyez sur le bouton **BOOT** de l'ESP32 pendant l'upload
    - Vérifiez que le bon port est sélectionné
    - Réduisez la vitesse d'upload si nécessaire

## Configuration initiale

### Première exécution

Lors du premier démarrage, le système va :

1. Initialiser LoRa (avec messages de debug)
2. Attendre un fix GPS (peut prendre 30-60 secondes)
3. Initialiser les servomoteurs
4. Détecter le mode (manuel ou autonome)

### Vérification du fonctionnement

Dans le **Moniteur série** (115200 bauds), vous devriez voir :

```
[LoRa] Initialisation...
[LoRa] ✓ Initialisé avec succès
[GPS] Initialisation...
[GPS] ✓ Prêt
[System] ✓ Système prêt
```

## Configuration de la station de contrôle

Si vous utilisez une carte réceptrice pour la station de contrôle :

1. Répétez les étapes d'installation sur la carte réceptrice
2. Utilisez le même code ou un code adapté pour la réception
3. Connectez la carte au PC via USB
4. Configurez l'interface PC pour communiquer via Serial

## Prochaines étapes

Une fois le système installé et fonctionnel :

1. Consultez [Architecture](architecture.md) pour comprendre le code
2. Lisez [API Reference](api.md) pour les fonctions disponibles
3. Consultez [Troubleshooting](troubleshooting.md) en cas de problème

---

*Pour comprendre le fonctionnement, voir [Fonctionnement](fonctionnement.md)*

