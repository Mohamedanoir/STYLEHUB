Gestion des Machines — version regroupée

Le dossier src contient exactement les cinq fichiers demandés :
machine.cpp, machine.h, machine.ui, connection.cpp et connection.h.
Les anciennes déclarations et implémentations de la page, du tableau, des statistiques,
des métiers et de la fenêtre principale ont été regroupées dans machine.h/machine.cpp.

Les images, icônes et le style d'origine sont conservés dans resources/ et resources.qrc.
Les deux formulaires complémentaires de la fenêtre principale et de la boîte de détails
sont rangés dans forms/ afin de conserver leur affichage Qt Designer. La page Machines
reste dans src/machine.ui.

Dans Qt Creator, ouvrir GestionMachines.pro ou CMakeLists.txt et choisir un kit Qt.
Oracle est utilisé via QODBC avec le DSN FASHIONOVA_XE et l'utilisateur FASHIONOVA
par défaut. Le mot de passe est demandé au lancement si FASHIONOVA_DB_PASSWORD n'est
pas défini. Voir oracle_schema.sql pour les colonnes nécessaires à MACHINES.
