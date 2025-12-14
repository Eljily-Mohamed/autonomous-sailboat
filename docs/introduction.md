# Introduction au projet

## Vue d'ensemble

AutoBoat est un projet de navigation autonome pour voilier développé dans le cadre d'un projet académique. Le système permet de contrôler un voilier à distance via LoRa et de le faire naviguer automatiquement vers des waypoints GPS prédéfinis.

## Objectifs du projet

Le projet AutoBoat vise à :

1. **Navigation autonome** : Permettre au voilier de naviguer automatiquement vers des waypoints GPS
2. **Adaptation au vent** : Adapter la navigation en fonction des conditions de vent
3. **Communication longue portée** : Utiliser LoRa pour la communication entre le bateau et la station de contrôle
4. **Contrôle manuel** : Permettre le basculement vers un contrôle manuel via radiocommande
5. **Télémetrie** : Envoyer en temps réel la position, le cap, et les informations de navigation

## Composants principaux

### Matériel

- **ESP32** : Microcontrôleur principal
- **Module LoRa SX1278** : Communication longue portée
- **Module GPS** : Localisation et navigation
- **Récepteur radio** : Détection du mode manuel/autonome
- **Servomoteurs** : Contrôle de la voile et du gouvernail
- **Moteur ESC** : Propulsion du bateau

### Logiciel

- **Firmware Arduino** : Code principal embarqué
- **Communication JSON** : Format de messages standardisé
- **Protocole LoRa** : Transmission des données

## Images et vidéos

!!! info "Contenu multimédia"
    Les images et vidéos du projet sont stockées dans :
    - **Images** : `docs/images/`
    - **Vidéos** : `docs/videos/`
    
    Vous pouvez ajouter :
    - Schéma de montage du système
    - Photos du bateau équipé
    - Vidéos de démonstration
    - Captures d'écran de l'interface de contrôle

### Images du projet

#### Schéma de l'architecture système
![Schéma du système](images/system-diagram.png)
*Schéma de l'architecture système - À ajouter dans `docs/images/system-diagram.png`*

#### Photo du bateau
![Photo du bateau](images/boat-photo.jpg)
*Photo du voilier équipé - À ajouter dans `docs/images/boat-photo.jpg`*

#### Schéma de connexion
![Schéma de connexion](images/wiring-diagram.png)
*Schéma de connexion des modules - À ajouter dans `docs/images/wiring-diagram.png`*

#### Carte embarquée
![Carte embarquée](images/board-mounted.jpg)
*Photo de la carte ESP32 montée sur le bateau - À ajouter dans `docs/images/board-mounted.jpg`*

### Vidéos du projet

#### Démonstration du système
<iframe width="560" height="315" src="videos/demo-system.mp4" frameborder="0" allowfullscreen></iframe>
*Vidéo de démonstration - À ajouter dans `docs/videos/demo-system.mp4`*

Ou utilisez un lien externe (YouTube, Vimeo, etc.) :
```markdown
[![Démonstration du système](https://img.youtube.com/vi/VIDEO_ID/0.jpg)](https://www.youtube.com/watch?v=VIDEO_ID)
```

#### Navigation autonome
<iframe width="560" height="315" src="videos/autonomous-navigation.mp4" frameborder="0" allowfullscreen></iframe>
*Vidéo de navigation autonome - À ajouter dans `docs/videos/autonomous-navigation.mp4`*

#### Installation et configuration
<iframe width="560" height="315" src="videos/installation.mp4" frameborder="0" allowfullscreen></iframe>
*Vidéo d'installation - À ajouter dans `docs/videos/installation.mp4`*

## Cas d'usage

Le système AutoBoat peut être utilisé pour :

- **Recherche et sauvetage** : Navigation automatique vers des zones spécifiques
- **Surveillance maritime** : Patrouille autonome
- **Recherche scientifique** : Collecte de données en mer
- **Éducation** : Apprentissage de la robotique et de la navigation

## Prochaines étapes

Pour commencer avec le projet :

1. Lire la section [Fonctionnement](fonctionnement.md) pour comprendre l'architecture
2. Suivre le guide [Démarrage](demarrage.md) pour installer et configurer
3. Consulter l'[Architecture](architecture.md) pour comprendre le code

---

*Dernière mise à jour : Automatique via Git*

