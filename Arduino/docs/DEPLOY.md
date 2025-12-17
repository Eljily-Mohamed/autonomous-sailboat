# Guide de déploiement de la documentation

## Configuration GitHub Pages

Pour que la documentation soit accessible, vous devez activer GitHub Pages dans les paramètres de votre dépôt :

### Étapes à suivre :

1. **Allez sur votre dépôt GitHub** : https://github.com/Eljily-Mohamed/autonomous-sailboat

2. **Cliquez sur "Settings"** (en haut du dépôt)

3. **Dans le menu de gauche, cliquez sur "Pages"**

4. **Dans la section "Source"** :
   - Sélectionnez **"GitHub Actions"** comme source
   - **Ne sélectionnez pas** une branche (comme `gh-pages` ou `main`)

5. **Sauvegardez** les paramètres

### Vérification du déploiement

1. **Vérifiez que le workflow s'est exécuté** :
   - Allez dans l'onglet **"Actions"** de votre dépôt
   - Vous devriez voir un workflow "Deploy Documentation"
   - Vérifiez qu'il s'est terminé avec succès (icône verte ✓)

2. **Si le workflow a échoué** :
   - Cliquez sur le workflow pour voir les détails
   - Vérifiez les logs pour identifier l'erreur

3. **Attendez quelques minutes** :
   - Après le premier déploiement, GitHub Pages peut prendre 5-10 minutes pour être accessible
   - L'URL sera : `https://eljily-mohamed.github.io/autonomous-sailboat/`

### Déploiement manuel

Si vous voulez déclencher le déploiement manuellement :

1. Allez dans l'onglet **"Actions"**
2. Sélectionnez le workflow **"Deploy Documentation"**
3. Cliquez sur **"Run workflow"**
4. Sélectionnez la branche `main`
5. Cliquez sur **"Run workflow"**

### Test local

Pour tester la documentation localement avant de la déployer :

```bash
# Installer les dépendances
pip install -r requirements.txt

# Lancer le serveur local
mkdocs serve

# La documentation sera accessible sur http://127.0.0.1:8000
```

### Problèmes courants

#### Erreur 404
- Vérifiez que GitHub Pages est activé avec "GitHub Actions" comme source
- Vérifiez que le workflow s'est exécuté avec succès
- Attendez quelques minutes après le déploiement

#### Le workflow ne se déclenche pas
- Vérifiez que vous avez poussé sur la branche `main`
- Vérifiez que les fichiers modifiés sont dans `Arduino/docs/` ou `mkdocs.yml`

#### Erreur de build
- Vérifiez que tous les fichiers Markdown référencés dans `mkdocs.yml` existent
- Testez localement avec `mkdocs build --strict`
