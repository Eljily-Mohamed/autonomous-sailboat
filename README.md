# Projets Arduino

Ce dépôt contient plusieurs projets Arduino, dont le projet principal **AutoBoat**.

## 📁 Structure

```
Arduino/
├── boat/              # Projet AutoBoat - Navigation autonome pour voilier
├── docs/              # Documentation MkDocs
├── mkdocs.yml         # Configuration MkDocs
└── README.md          # Ce fichier
```

## 📚 Documentation

La documentation complète du projet AutoBoat est disponible dans le dossier `docs/` et peut être visualisée avec MkDocs.

### Installation de MkDocs

```bash
pip install mkdocs mkdocs-material
```

### Lancer la documentation en local

```bash
cd /Users/eljily/Documents/Arduino
mkdocs serve
```

La documentation sera accessible sur `http://127.0.0.1:8000`

### Construire la documentation

```bash
mkdocs build
```

Les fichiers HTML seront générés dans le dossier `site/`

### Déployer sur GitHub Pages

```bash
mkdocs gh-deploy
```

## 🚤 Projet AutoBoat

Le projet AutoBoat est un système de navigation autonome pour voilier basé sur ESP32.

Pour plus d'informations, consultez la [documentation complète](docs/index.md).
