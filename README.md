# 🏆 ClanTracker - Clash Royale Clan Analytics

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)

Un outil d'analyse de performance pour clans Clash Royale, générant automatiquement des rapports détaillés à partir de l'API officielle Supercell.

## 📋 Table des matières

- [Aperçu](#-aperçu)
- [Fonctionnalités](#-fonctionnalités)
- [Prérequis](#-prérequis)
- [Installation](#-installation)
- [Configuration](#-configuration)
- [Utilisation](#-utilisation)
- [Exemples de rapports](#-exemples-de-rapports)
- [Architecture](#-architecture)
- [Contributions](#-contributions)
- [Licence](#-licence)

## 🎯 Aperçu

**ClanTracker** est un programme C++ qui récupère les données d'un clan Clash Royale via l'API officielle et génère deux types de rapports :

- **📊 Rapport Public** : Classements motivants (top joueurs, donateurs, performances en guerre)
- **🔒 Rapport Privé** : Analyses pour leaders (inactivité, non-participation, recommandations d'exclusion)

Idéal pour les leaders de clan souhaitant suivre les performances et maintenir une communauté active.

## ✨ Fonctionnalités

### Analyses automatiques

- ✅ **Top 3 joueurs globaux** (formule pondérée : trophées + dons + guerre - inactivité)
- ✅ **Top 3 guerriers** (River Race points)
- ✅ **Top 3 donateurs** (cartes données)
- ✅ **Meilleure progression** (montée de rang)
- ✅ **"Rat de la semaine"** (profiteurs : reçoivent beaucoup, donnent peu)
- ✅ **Détection d'inactivité** (≥3 jours sans connexion)
- ✅ **Participation aux guerres** (River Race)
- ✅ **Recommandations de kick** (membres inactifs/non-participants)

### Technique

- 🔐 Authentification sécurisée (token Bearer + SSL/TLS)
- 📝 Parsing JSON robuste
- 🗓️ Gestion automatique des dates (ISO 8601)
- 📁 Export horodaté (fichiers par date)
- ⚠️ Gestion d'erreurs complète
- 🚀 Performance optimale (C++17)

## 🛠 Prérequis

### Système

- **OS** : Linux (Ubuntu/Debian recommandé)
- **Compilateur** : g++ ou clang++ avec support C++17
- **Make** : GNU Make

### Bibliothèques

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential pkg-config libcurl4-openssl-dev nlohmann-json3-dev

# Fedora
sudo dnf install @development-tools pkgconfig libcurl-devel nlohmann-json-devel

# Arch Linux
sudo pacman -Syu base-devel pkgconf curl nlohmann-json
```

**Détails** :
- **libcurl** (≥7.x) : Requêtes HTTP(S)
- **nlohmann/json** (≥3.x) : Parsing JSON moderne
- **C++ Standard Library** : `<filesystem>`, `<chrono>`, `<algorithm>`

## 📥 Installation

### 1. Cloner le dépôt

```bash
git clone https://github.com/votre-username/ClanTracker.git
cd ClanTracker
```

### 2. Compiler

```bash
make
```

Le binaire `ClanTracker` sera créé à la racine du projet.

### 3. Vérifier l'installation

```bash
./ClanTracker --help  # (si implémenté)
# ou
ls -lh ClanTracker
```

## ⚙️ Configuration

### Obtenir un token API

1. Créez un compte développeur : [https://developer.clashroyale.com](https://developer.clashroyale.com)
2. Connectez-vous avec votre compte Supercell
3. Créez une nouvelle clé API :
   - **Nom** : ClanTracker (ou autre)
   - **Description** : Analyse de clan
   - **IP autorisées** : Votre IP publique (ou `0.0.0.0/0` pour tests)
4. Copiez le token généré

### Configurer la variable d'environnement

```bash
# Méthode 1 : Export temporaire (session actuelle)
export CLASH_TOKEN="Bearer eyJ0eXAiOiJKV1QiLCJhbGc..."

# Méthode 2 : Ajout permanent dans ~/.bashrc ou ~/.zshrc
echo 'export CLASH_TOKEN="Bearer eyJ0eXAi..."' >> ~/.bashrc
source ~/.bashrc

# Méthode 3 : Fichier .env (recommandé pour production)
echo 'CLASH_TOKEN="Bearer eyJ0eXAi..."' > .env
source .env
```

⚠️ **Sécurité** : Ne commitez JAMAIS votre token dans Git !

```bash
# Ajoutez à .gitignore
echo ".env" >> .gitignore
```

### Activer l'envoi automatique par email (optionnel)

Le programme peut expédier chaque rapport par SMTP juste après leur génération. Exportez les variables suivantes avant d'exécuter le binaire :

```bash
export SMTP_HOST="smtp.example.com"   # hôte SMTP
export SMTP_PORT="587"               # facultatif, sinon 25
export SMTP_USER="bot@example.com"   # login SMTP
export SMTP_PASS="app-password"      # mot de passe / App password
export SMTP_FROM="Clan Tracker <bot@example.com>"
export SMTP_TO="example@mail.com"  # ou liste séparée par des virgules
```

Notes :
- Utilisez un **app password** (ou mot de passe spécifique) si votre fournisseur l'exige (Gmail, Outlook, etc.).
- Si l'une des variables manque, l'envoi est simplement ignoré et un warning apparaît dans la sortie.
- `SMTP_PORT` est optionnel ; ajoutez `465` pour SMTPS si votre relais le demande (libcurl détecte le schéma `smtp://`).
- Pour tester sans expédier, laissez les variables non définies : les rapports sont toujours écrits sur disque.

### Configurer le tag du clan

Modifiez dans `srcs/main.cpp` (ligne ~17) :

```cpp
std::string clanTag = "LQY80L9P";  // Remplacez par votre tag (sans le #)
```

Ou compilez avec le tag en argument (amélioration future).

## 🚀 Utilisation

### Exécution simple

```bash
./ClanTracker
```

### Sortie attendue

```
✅ Rapports générés :
   - files/rapport_public_2025-10-27.txt
   - files/rapport_prive_2025-10-27.txt
```

### Consulter les rapports

```bash
# Rapport public
cat files/rapport_public_$(date +%Y-%m-%d).txt

# Rapport privé
cat files/rapport_prive_$(date +%Y-%m-%d).txt
```

### Automatisation (cron)

Générez automatiquement les rapports chaque semaine :

```bash
# Ouvrir crontab
crontab -e

# Ajouter (tous les lundis à 8h)
0 8 * * 1 cd /path/to/ClanTracker && ./ClanTracker
```

## 📊 Exemples de rapports

### Rapport Public

```
Semaine - Rapport de Clan

Top du Clan 🏆 (performance globale)
1. PlayerOne — score 8500 | 6000 trophées | 150 dons | 800 pts guerre | actif il y a 0j
2. PlayerTwo — score 7200 | 5500 trophées | 120 dons | 600 pts guerre | actif il y a 1j
3. PlayerThree — score 6800 | 5000 trophées | 180 dons | 400 pts guerre | actif il y a 0j

Top Guerre de Clan ⚔
1. WarriorOne — 1200 pts guerre
2. WarriorTwo — 1000 pts guerre
3. WarriorThree — 850 pts guerre

Top Donateurs 🤝
1. GenerousPlayer — 250 cartes données
2. NiceGuy — 200 cartes données
3. Helper — 180 cartes données

Montée de la semaine 📈
PlayerX a gagné 5 places (rang 15 → 10)

Rat de la semaine 🐀
LazyPlayer a donné 2 cartes mais a reçu 150 cartes 😭

Rappel ⚠
Faites vos guerres, restez actifs, et go top ladder ❤️
```

### Rapport Privé

```
Rapport interne (chefs / co-leaders seulement)

Inactivité (>=3 jours sans se connecter) :
- InactivePlayer1 — inactif depuis 7 jours — rôle member — 4200 trophées
- InactivePlayer2 — inactif depuis 5 jours — rôle elder — 3800 trophées

A PAS fait la guerre ⚔ (0 participation) :
- LazyWarrior — 0 pts guerre — inactif 2j — rôle member

Candidats kick 🚫 (member uniquement) :
- PlayerToKick — 3500 trophées — 10j inactif — PAS de guerre
- AnotherOne — 4000 trophées — 4j inactif — PAS de guerre
```

## 🏗️ Architecture

```
ClanTracker/
├── srcs/
│   ├── main.cpp              # Point d'entrée
│   ├── HTTP.cpp              # Communication API (libcurl)
│   ├── Parse_Members.cpp     # Parsing JSON membres
│   ├── War.cpp               # Gestion River Race
│   ├── Data_Members.cpp      # Analyses statistiques
│   ├── Kick.cpp              # Recommandations kick
│   ├── Time.cpp              # Gestion temps/dates
│   └── Report.cpp            # Génération rapports
│
├── includes/
│   └── Clash.hpp             # Header unique (structures + prototypes)
│
├── Makefile                  # Build system
├── README.md                 # Ce fichier
├── ARCHITECTURE.md           # Documentation technique détaillée
└── files/                    # Rapports générés (auto-créé)
```

Pour plus de détails techniques, consultez [ARCHITECTURE.md](ARCHITECTURE.md).

## 🔧 Développement

### Compilation en mode debug

```bash
# Ajoutez -g pour symboles de débogage
make CFLAGS="-Wall -Wextra -Werror -std=c++17 -g"
```

### Nettoyage

```bash
make clean   # Supprime les .o
make fclean  # Supprime les .o et le binaire
make re      # Nettoyage complet + recompilation
```

### Tests

```bash
# Tester avec un clan spécifique
export CLASH_TOKEN="Bearer YOUR_TOKEN"
./ClanTracker

# Vérifier les erreurs
echo $?  # 0 = succès, 1 = erreur
```

### Debugging

```bash
# Mode verbeux cURL (décommenter dans HTTP.cpp)
curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

# Valgrind (fuites mémoire)
valgrind --leak-check=full ./ClanTracker
```

## 🤝 Contributions

Les contributions sont les bienvenues ! Voici comment participer :

1. **Forkez** le projet
2. Créez votre branche (`git checkout -b feature/AmazingFeature`)
3. Commitez vos changements (`git commit -m 'Add some AmazingFeature'`)
4. Pushez vers la branche (`git push origin feature/AmazingFeature`)
5. Ouvrez une **Pull Request**

### Guidelines

- Code en **C++17** minimum
- Respectez le style existant (indentation, nommage)
- Commentez les fonctions complexes
- Testez avant de soumettre
- Mettez à jour la documentation si nécessaire

## 🐛 Problèmes connus

### Token invalide (HTTP 403)

```
[ERREUR] L'API a repondu avec un code HTTP != 200 : 403
Body: {"reason":"accessDenied","message":"Invalid authorization"}
```

**Solutions** :
- Vérifiez que `CLASH_TOKEN` contient bien "Bearer " + token
- Régénérez le token sur le portail développeur
- Vérifiez les restrictions IP

### Compilation échoue

```
fatal error: nlohmann/json.hpp: No such file or directory
```

**Solution** :
```bash
sudo apt install nlohmann-json3-dev
```

### Segfault / Crash

- Vérifiez que le JSON reçu est valide
- Testez avec `valgrind` pour détecter erreurs mémoire
- Vérifiez que libcurl est bien installée

## 📝 Roadmap

- [x] Génération rapports texte
- [x] Analyse multi-critères
- [x] Gestion River Race
- [ ] Support multi-clans
- [ ] Export JSON/CSV
- [ ] Interface web (REST API)
- [ ] Graphiques d'évolution
- [ ] Notifications Discord
- [ ] Base de données historique
- [ ] Dashboard interactif

## 📜 Licence

Ce projet est sous licence **MIT**. Voir le fichier [LICENSE](LICENSE) pour plus de détails.

```
MIT License

Copyright (c) 2025 Votre Nom

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

[...]
```

## 👤 Auteur

**Votre Nom**
- GitHub: [@VinyBoy](https://github.com/VinyBoy)

## 🙏 Remerciements

- **Supercell** pour l'API Clash Royale
- **nlohmann** pour la bibliothèque JSON C++
- **libcurl** pour la gestion HTTP(S)
- La communauté Clash Royale

## ⚠️ Disclaimer

Ce projet n'est **pas affilié, endorsé ou sponsorisé par Supercell**. Clash Royale est une marque déposée de Supercell Oy. Utilisez ce projet conformément aux [conditions d'utilisation de l'API Supercell](https://developer.clashroyale.com/terms).

---

<p align="center">
  Fait avec ❤️ pour la communauté Clash Royale
</p>
