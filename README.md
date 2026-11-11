# FashioNova – Gestion des Machines (Qt / C++)

Application de bureau Qt Widgets reproduisant l'interface « Gestion des Machines » de FashioNova
(menu latéral avec logo, bandeau, formulaire, liste, statistiques), avec SQLite par défaut et une connexion Oracle optionnelle.

## Prérequis
- Qt **5.15** ou **Qt 6.x** avec les modules **Widgets, Sql (pilote QSQLITE; QODBC pour Oracle), Svg**
- Un compilateur C++17 (MinGW, MSVC, GCC ou Clang)

## Compilation
**Avec Qt Creator** : ouvrir `GestionMachines.pro` (qmake) ou `CMakeLists.txt` (CMake), choisir un kit, puis *Exécuter*.

**En ligne de commande (qmake)** :
```
qmake GestionMachines.pro
make            # ou mingw32-make / nmake
```

**En ligne de commande (CMake)** :
```
cmake -S . -B build
cmake --build build
```

## Fonctionnalités (cahier des charges)
Entité **Machine** : `ID_Machine`, `Nom_Machine`, `Type_Machine`, `Etat`, `Date_Prochaine_Maintenance`, heures de fonctionnement, pièces produites et heures d'arrêt.

Le tableau affiche les trois compteurs machine pour une même période de suivi. Le métier Performance calcule le rendement en pièces par heure et la disponibilité (`fonctionnement / (fonctionnement + arrêt)`), puis compare les types. Le planning de maintenance tient compte de la date prévue, de l'état et des compteurs : les arrêts élevés et les heures de fonctionnement importantes font remonter une machine dans les priorités.

Les compteurs fournis avec les machines de démonstration sont des exemples fictifs à remplacer par les mesures réelles de l'atelier. Lors de la mise à niveau d'une ancienne base, ils sont initialisés une seule fois seulement si les trois valeurs sont encore nulles.

| Fonction | Où |
|---|---|
| Ajouter | bouton *Ajouter* (ID généré : MCH001, MCH002…) |
| Afficher | bouton *Afficher* (détails + état de la maintenance) |
| Modifier | sélectionner une ligne, changer les champs, *Modifier* |
| Supprimer | bouton *Supprimer* (avec confirmation) |
| Trier | liste déroulante *Trier par* : Nom_Machine, Type_Machine, Etat |
| Rechercher | champ de recherche (ID_Machine, Nom_Machine, Type_Machine) – la barre du haut et celle de la liste sont synchronisées |
| Exporter PDF | bouton *Exporter PDF* (liste filtrée/triée courante) |
| Détection automatique de la maintenance | une machine est signalée si l'état est « En maintenance » ou « Indisponible », si la date est dépassée, ou si la maintenance est à moins de 7 jours : icône ⚠ et date colorée dans la liste, bandeau « n machine(s) nécessitent une maintenance » (cliquable pour filtrer), message dans le formulaire et cloche avec compteur |
| Alerte d'indisponibilité | quand une machine devient *Indisponible*, une notification rouge s'affiche ; la cloche liste toutes les alertes (un clic ouvre la machine) |

Statistiques : répartition par type (anneau), **performance par type de machine** (métier 1) et
**planning de maintenance optimisé** (métier 2).

### Métiers innovants et données réelles
Les boutons de la barre « Statistiques » permettent de saisir les relevés et de planifier/appliquer les interventions. Le calcul est dans `src/metiers.h/.cpp` ; l'affichage est dans `PerformanceBars` et `PlanningList` (`src/chartwidgets.*`).

| Métier | Règle |
|---|---|
| **1 – Mesurer la performance du parc par type de machine** | Les relevés des 30 derniers jours alimentent les taux : disponibilité = heures de fonctionnement ÷ heures prévues ; production atteinte = quantité produite ÷ objectif. Score d'un type = moyenne des deux taux agrégés. La carte affiche le score, la disponibilité, la production et le nombre de relevés. Un type sans relevés n'obtient pas de score. Barres vertes ≥ 80 %, orange ≥ 50 %, rouge sinon. |
| **2 – Optimiser le planning de maintenance** | Les machines indisponibles, en retard et en maintenance passent en priorité. Ensuite, si leur prochaine échéance est dans les 30 jours, les machines d'un type sous 70 % de performance sont avancées. Le planning évite les week-ends, deux interventions du même type le même jour et dépassements de 8 heures d'interventions par jour. Les durées déjà saisies sont réutilisées. « Appliquer le planning » enregistre les nouvelles dates et conserve les précédentes dans l'historique. |

Le bouton **+ Relevé réel** ouvre un formulaire quotidien. Il limite les heures de fonctionnement et d'arrêt au nombre d'heures prévues et interdit un deuxième relevé pour la même machine et la même date. **Planifier entretien** enregistre une intervention manuelle ; **Appliquer le planning** enregistre en une fois les propositions de l'algorithme après confirmation.

## Données
La base SQLite `fashionova_machines.db` est créée au premier lancement dans le dossier de données de l'application
(`QStandardPaths::AppDataLocation`), avec 12 machines de démonstration et les tables suivantes :

- `machine` : `ID_Machine`, `Nom_Machine`, `Type_Machine`, `Etat`, `Date_Prochaine_Maintenance`, `Photo`.
- `releve_machine` : une mesure par machine et par jour : date, heures prévues, heures de fonctionnement/arrêt, quantité produite, objectif et cause d'arrêt. La clé étrangère pointe vers `machine` et le couple machine/date est unique.
- `maintenance` : interventions prévues et historisées : machine, date prévue, type, priorité, durée estimée, statut, description, date et durée réelles, coût.

Les nouveaux relevés et interventions sont conservés dans cette base. Supprimez ce fichier pour repartir de zéro.
Le bouton ✎ (ou « + ») sous la photo permet d'associer une image à une machine (à valider avec *Modifier* / *Ajouter*).

### Connexion Oracle (optionnelle)
La connexion Oracle utilise le pilote Qt `QODBC` et un DSN ODBC Windows 64 bits nommé `Oracle_XE`.
Le mode SQLite reste actif si aucune variable d'environnement n'est définie. Pour activer Oracle dans Qt Creator,
ajoutez ces variables dans **Projets > Exécution > Environnement** :

| Variable | Valeur par défaut / exemple |
|---|---|
| `FASHIONOVA_DB_DRIVER` | `QODBC` |
| `FASHIONOVA_ORACLE_DSN` | `Oracle_XE` |
| `FASHIONOVA_DB_USER` | `system` |
| `FASHIONOVA_DB_PASSWORD` | le mot de passe défini lors de l'installation Oracle XE |

Le premier lancement crée les tables `MACHINE`, `RELEVE_MACHINE`, `MAINTENANCE` et les séquences nécessaires dans le schéma Oracle de l'utilisateur connecté. Il faut que le service Oracle XE et le DSN soient accessibles avant de lancer l'application.

## Structure
L'interface est décrite dans des fichiers **Qt Designer (`.ui`)** ; les `.cpp` ne contiennent que la logique.
```
GestionMachines.pro / CMakeLists.txt   projet
resources.qrc                          images, icônes SVG, feuille de style
resources/style.qss                    thème (couleurs, boutons, tableau…) modifiable sans recompiler le code

src/mainwindow.ui    + mainwindow.h/.cpp     fenêtre : menu latéral, barre du haut, pile de pages
src/sidebarwidget.h/.cpp                menu latéral avec image de fond (resources/images/sidebar_bg.jpg), widget promu dans mainwindow.ui
src/machinespage.ui  + machinespage.h/.cpp   page Machines : formulaire, liste, statistiques + CRUD, recherche, tri, PDF, alertes
src/detailsdialog.ui + detailsdialog.h/.cpp   boîte « Afficher » (détails d'une machine)
src/main.cpp                           démarrage, thème, ouverture de la base
src/database.*                         accès SQLite (table machine)
src/machine.h                          entité Machine + règles de détection de maintenance
src/metiers.*                          les deux métiers innovants : performance par type et planning de maintenance
src/chartwidgets.*                     graphiques : anneau, performance par type, planning – widgets promus dans machinespage.ui
src/bannerwidget.*                     bandeau d'en-tête – widget promu dans machinespage.ui
src/uihelpers.*                        icônes SVG, couleurs, images arrondies
```

### Modifier l'interface avec Qt Designer
Ouvrir `src/mainwindow.ui`, `src/machinespage.ui` ou `src/detailsdialog.ui` (double-clic dans Qt Creator).
- `MachinesPage`, `BannerWidget`, `DonutChart`, `PerformanceBars` et `PlanningList` sont des **widgets promus** (conteneurs vides dans Designer, dessinés par le code).
- Les couleurs/bordures viennent de `style.qss` ; les widgets répétés (cartes, champs, libellés…) sont repérés par la propriété dynamique `role` (ex. `role = "card"`).
- Les icônes SVG sont recolorées à l'exécution : elles sont affectées dans le code (`setupIcons()`), pas dans Designer.
- Les listes *Type* et *État* du formulaire se modifient dans Designer, mais restent à garder cohérentes avec `machine.h` (`typesMachine()`, `Etat`).

## Personnalisation
- Noms du menu : boutons `btnNav…` dans `src/mainwindow.ui` (libellés « Gestion … »).
- Seuil d'alerte (7 jours) : `Machine::raisonMaintenance()` dans `src/machine.h`.
- Couleurs : `resources/style.qss`.


Les boutons **Ajouter, Afficher, Modifier, Supprimer** sont actifs dans cette version.
# Données métier de la fiche machine

La fiche machine contient trois compteurs cumulés sur la même période de suivi :
heures de fonctionnement, pièces produites et heures d'arrêt. La performance par
type compare le rendement (pièces/heure de fonctionnement) et la disponibilité
(fonctionnement / (fonctionnement + arrêt)). Le planning utilise ces compteurs
pour faire remonter les machines les plus sollicitées ou les plus arrêtées, en
plus de l'état et de la date de maintenance déjà enregistrés.



## Intégration du module Gestion des Machines

Le module Machines est chargé dans la page Machines de la fenêtre commune. Le CRUD réutilise la connexion Oracle partagée `Connexion::instance().base()`. Les ressources Machines sont préfixées `:/machines/` pour éviter les collisions avec les ressources globales.
