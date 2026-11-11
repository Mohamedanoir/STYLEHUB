# Connexion Oracle

Le DSN ODBC est `Source_Projet2A`. Au lancement, l'application demande le nom
d'utilisateur Oracle (propose `fashionova` par défaut) et le mot de passe.
Saisissez les identifiants du schéma qui possède les tables utilisées par
l'application (par exemple `aziz` si vos tables sont dans le schéma AZIZ).
Le mot de passe est utilisé uniquement en mémoire pendant la session et n'est
pas enregistré dans les fichiers du projet.

Le module Clients est intégré à l'application. Avant sa première utilisation,
exécuter une fois `modules/fashionova-v2/migration_ajout_remarques.sql` dans le
schéma Oracle utilisé par l'application. Cette migration complète CLIENTS avec
les colonnes attendues par le module, sans supprimer les lignes existantes.
