# Troubleshooting

## Problèmes courants et solutions

### GPS

#### GPS ne trouve pas de satellites

**Symptômes :**
- Statut GPS reste à 0 ou 1
- Pas de position valide

**Solutions :**
1. Vérifier que le GPS est bien connecté (Serial1)
2. Vérifier les pins GPS dans `config_pins.h`
3. Attendre 30-60 secondes (première acquisition)
4. Vérifier que le GPS a une vue dégagée du ciel
5. Vérifier l'alimentation du module GPS

#### Position GPS incorrecte

**Symptômes :**
- Position GPS saute ou est erronée
- Cap instable

**Solutions :**
1. Vérifier le nombre de satellites (minimum 4 recommandé)
2. Le filtrage GPS devrait lisser les positions
3. Vérifier que le GPS n'est pas à l'intérieur

---

### LoRa

#### LoRa ne s'initialise pas

**Symptômes :**
- Messages d'erreur au démarrage
- `loraInitialized = false`

**Solutions :**
1. Vérifier les connexions SPI (MISO, MOSI, SCK, CS)
2. Vérifier le pin RST (reset matériel)
3. Vérifier la fréquence LoRa (868 MHz pour Europe)
4. Vérifier l'alimentation du module LoRa
5. Le système tentera automatiquement de réinitialiser

#### Pas de communication LoRa

**Symptômes :**
- Aucun message reçu
- Messages non envoyés

**Solutions :**
1. Vérifier que les deux cartes utilisent la même fréquence
2. Vérifier la distance (portée LoRa limitée)
3. Vérifier les paramètres LoRa (SF, BW, CR)
4. Vérifier les antennes LoRa
5. Consulter les messages de debug dans le moniteur série

#### Erreur "Pas de communication SPI"

**Solutions :**
1. Vérifier les connexions SPI
2. Vérifier que le CS (Chip Select) est correct
3. Vérifier l'alimentation du module
4. Tester avec un autre module LoRa si possible

---

### Servomoteurs

#### Servos ne bougent pas

**Symptômes :**
- Pas de mouvement des servos
- Servos restent à une position

**Solutions :**
1. Vérifier les pins des servos dans `config_pins.h`
2. Vérifier l'alimentation des servos (5V)
3. Vérifier les connexions des signaux PWM
4. Vérifier que les servos sont bien initialisés dans `setup()`
5. Vérifier les amplificateurs 3.3V → 5V si présents

#### Servos bougent de manière erratique

**Symptômes :**
- Mouvements saccadés
- Vibrations

**Solutions :**
1. Vérifier l'alimentation (courant suffisant)
2. Vérifier les connexions (mauvais contact)
3. Vérifier que le PWM hardware est utilisé (pas software)
4. Vérifier les limites d'angles (ne pas dépasser -45°/+45°)

#### Servos ne copient pas le signal radiocommande

**Solutions :**
1. Vérifier que le mode radiocommande est détecté (SEL=1)
2. Vérifier les pins du récepteur radio
3. Vérifier que `handleServoControl()` est appelé dans `loop()`
4. Vérifier les valeurs PWM lues depuis le récepteur

---

### Récepteur Radio

#### Mode non détecté

**Symptômes :**
- Le système reste toujours en mode autonome
- Pas de basculement manuel/autonome

**Solutions :**
1. Vérifier le pin SEL dans `config_pins.h`
2. Vérifier que le récepteur radio est alimenté
3. Vérifier que la radiocommande est allumée
4. Vérifier les connexions du récepteur
5. Tester avec un multimètre le signal SEL

#### Signaux PWM incorrects

**Solutions :**
1. Vérifier les pins PWM1 et PWM2
2. Vérifier que `radioReceiver.update()` est appelé régulièrement
3. Vérifier les valeurs PWM min/max dans `config_pins.h`
4. Vérifier que le récepteur reçoit bien les signaux

---

### Compilation

#### Erreurs de compilation

**Symptômes :**
- Erreurs lors de la compilation
- Bibliothèques manquantes

**Solutions :**
1. Vérifier que toutes les bibliothèques sont installées :
   - LoRa
   - TinyGPSPlus
   - ESP32Servo
2. Vérifier que le support ESP32 est installé
3. Vérifier la version d'Arduino IDE
4. Vérifier que tous les fichiers `.hpp` et `.cpp` sont présents

#### Erreurs de pins

**Solutions :**
1. Vérifier `config_pins.h` pour les définitions de pins
2. Vérifier qu'aucun pin n'est utilisé deux fois
3. Vérifier la compatibilité avec votre ESP32

---

### Upload

#### Upload échoue

**Symptômes :**
- Erreur lors du téléversement
- Timeout

**Solutions :**
1. Appuyer sur le bouton **BOOT** pendant l'upload
2. Vérifier que le bon port série est sélectionné
3. Réduire la vitesse d'upload (115200 ou moins)
4. Vérifier le câble USB
5. Essayer un autre câble USB

#### Port série non détecté

**Solutions :**
1. Vérifier que le câble USB est connecté
2. Installer les drivers USB pour ESP32
3. Vérifier dans le Gestionnaire de périphériques (Windows)
4. Vérifier les permissions (Linux/Mac)

---

### Performance

#### Système lent ou bloqué

**Symptômes :**
- Réactions lentes
- Blocages

**Solutions :**
1. Vérifier qu'il n'y a pas de `delay()` dans la boucle principale
2. Vérifier que les fonctions sont non-bloquantes
3. Vérifier la fréquence CPU (240 MHz recommandé)
4. Vérifier la mémoire disponible

#### GPS bloque au démarrage

**Solutions :**
1. Le GPS attend un fix (peut prendre 30-60 secondes)
2. Vérifier que le GPS a une vue du ciel
3. Le timeout est de 30 secondes, puis le système continue

---

## Messages de debug

### Interprétation des messages

#### `[LoRa] ✓ Initialisé avec succès`
LoRa fonctionne correctement.

#### `[LoRa] ERREUR: Initialisation échouée`
Problème d'initialisation LoRa. Vérifier les connexions.

#### `[GPS] ✓ Prêt`
GPS a obtenu un fix et est prêt.

#### `[GPS] ⚠ Timeout - Continuation sans GPS fix`
GPS n'a pas obtenu de fix en 30 secondes. Le système continue mais sans GPS.

#### `[System] ✓ Système prêt`
Tous les modules sont initialisés.

---

## Support

Si vous rencontrez d'autres problèmes :

1. Vérifier les messages de debug dans le moniteur série
2. Vérifier la configuration dans `config_pins.h`
3. Consulter la documentation des modules
4. Vérifier les connexions matérielles

---

*Pour plus d'informations, voir [Architecture](architecture.md) et [API Reference](api.md)*

