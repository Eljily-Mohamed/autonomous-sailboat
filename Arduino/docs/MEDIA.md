# Guide des médias

Ce document explique comment ajouter des images et vidéos à la documentation.

## Structure des dossiers

```
docs/
├── images/          # Images (PNG, JPG, SVG)
│   ├── system-diagram.png
│   ├── boat-photo.jpg
│   ├── wiring-diagram.png
│   └── ...
└── videos/          # Vidéos (MP4, WebM)
    ├── demo-system.mp4
    ├── autonomous-navigation.mp4
    └── ...
```

## Images

### Formats supportés

- **PNG** : Pour les schémas, diagrammes, captures d'écran
- **JPG** : Pour les photos
- **SVG** : Pour les schémas vectoriels (recommandé)

### Taille recommandée

- **Schémas/Diagrammes** : 1200x800px ou plus
- **Photos** : 1920x1080px (Full HD) ou plus
- **Captures d'écran** : Taille native de l'écran

### Comment ajouter une image

1. Placez votre image dans `docs/images/`
2. Utilisez la syntaxe Markdown :

```markdown
![Description de l'image](images/nom-fichier.png)
*Légende de l'image*
```

### Images à ajouter

Voici la liste des images référencées dans la documentation :

#### Introduction
- [ ] `system-diagram.png` - Schéma de l'architecture système
- [ ] `boat-photo.jpg` - Photo du voilier équipé
- [ ] `wiring-diagram.png` - Schéma de connexion des modules
- [ ] `board-mounted.jpg` - Photo de la carte ESP32 montée

#### Fonctionnement
- [ ] `wiring-diagram.png` - Schéma de connexion des modules
- [ ] `lora-communication.png` - Schéma de communication LoRa
- [ ] `data-flow.png` - Diagramme de flux des données
- [ ] `board-boat.jpg` - Photo de la carte embarquée
- [ ] `board-receiver.jpg` - Photo de la carte réceptrice

#### Démarrage
- [ ] `arduino-ide-install.png` - Installation Arduino IDE
- [ ] `esp32-install.png` - Installation support ESP32
- [ ] `board-selection.png` - Sélection de carte

## Vidéos

### Formats supportés

- **MP4** : Format recommandé (H.264)
- **WebM** : Alternative open source

### Taille recommandée

- **Résolution** : 1920x1080 (Full HD) ou 1280x720 (HD)
- **Durée** : 2-5 minutes pour les démonstrations
- **Taille fichier** : < 50 MB si possible

!!! warning "Taille des fichiers"
    Pour les grandes vidéos, utilisez des services externes :
    - YouTube
    - Vimeo
    - GitHub Releases (pour les fichiers)

### Comment ajouter une vidéo

#### Option 1 : Fichier local (petites vidéos)

```html
<iframe width="560" height="315" src="videos/nom-video.mp4" frameborder="0" allowfullscreen></iframe>
```

#### Option 2 : YouTube (recommandé)

```markdown
[![Titre de la vidéo](https://img.youtube.com/vi/VIDEO_ID/0.jpg)](https://www.youtube.com/watch?v=VIDEO_ID)
```

#### Option 3 : Lien direct

```markdown
[Voir la vidéo](videos/nom-video.mp4)
```

### Vidéos à ajouter

- [ ] `demo-system.mp4` - Démonstration complète du système
- [ ] `autonomous-navigation.mp4` - Navigation autonome en action
- [ ] `installation.mp4` - Guide d'installation pas à pas
- [ ] `manual-mode.mp4` - Démonstration du mode manuel
- [ ] `lora-communication.mp4` - Communication LoRa en action

## Optimisation

### Images

Avant d'ajouter des images, optimisez-les :

```bash
# Installer ImageMagick (Mac)
brew install imagemagick

# Optimiser une image
convert image.jpg -quality 85 -resize 1920x1080 image-optimized.jpg
```

### Vidéos

Pour compresser une vidéo :

```bash
# Installer ffmpeg
brew install ffmpeg

# Compresser une vidéo
ffmpeg -i input.mp4 -vcodec h264 -acodec mp2 output.mp4
```

## Exemples de syntaxe

### Image simple
```markdown
![Description](images/photo.jpg)
```

### Image avec légende
```markdown
![Description](images/photo.jpg)
*Légende de la photo*
```

### Vidéo YouTube
```markdown
[![Titre](https://img.youtube.com/vi/VIDEO_ID/0.jpg)](https://www.youtube.com/watch?v=VIDEO_ID)
```

### Vidéo locale
```html
<video width="560" height="315" controls>
  <source src="videos/demo.mp4" type="video/mp4">
  Votre navigateur ne supporte pas la vidéo.
</video>
```

---

*Ajoutez vos médias dans les dossiers correspondants et mettez à jour cette liste au fur et à mesure.*

