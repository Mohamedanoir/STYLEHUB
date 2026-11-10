Connexion Oracle du projet FashionNova
========================================

Le projet utilise Qt SQL avec le pilote QODBC et le pilote Oracle "Oracle in XE"
déjà installé sur le PC. La connexion ODBC sans DSN cible l'instance locale XE.
La classe Connexion applique le singleton de Meyers et garde une connexion unique.

Au lancement, l'application demande le nom d'utilisateur et le mot de passe du
schéma Oracle. Le mot de passe reste en mémoire pendant l'exécution et n'est pas
enregistré dans les fichiers du projet.

La classe Employe contient les requêtes préparées du CRUD. La vue appelle ces
méthodes et ne contient pas de requêtes SQL. Les opérations utilisent des
paramètres positionnels (? / addBindValue) pour séparer les valeurs du SQL.

La répartition par poste (employés présents), la présence actuelle et les salaires affichent leurs pourcentages; les montants sont calculés depuis les employés chargés d'Oracle. Les statistiques sont recalculées après chaque opération CRUD et changement de présence. Les statistiques de répartition et de salaires sont calculées depuis les
employés chargés d'Oracle. Le graphique Présents/Absents reflète la session
ouverte dans l'application. Le modèle EMPLOYES fourni ne conserve pas
l'historique de présence par jour; aucun graphique historique n'est donc
inventé à partir de données fictives.

La recherche de la barre supérieure et celle de la liste sont synchronisées.
La liste peut être triée par nom, ID, poste ou état de présence. Les contrôles
du formulaire limitent la longueur des champs texte et valident le salaire.
La connexion/déconnexion employé et l'importation de CV texte/CSV sont les
fonctionnalités métier livrées dans ce module.

Le CRUD du module Employés utilise les colonnes du modèle fourni :
EMPLOYES(ID_Employe, Nom_Employe, Prenom_Employe, Poste, Salaire, Mot_de_passe)
et la séquence SEQ_EMPLOYES. L'ID est généré par Oracle et présenté dans
l'interface sous la forme EMP001, EMP002, etc.

Avant de lancer l'application :
1. Démarrer OracleServiceXE et OracleXETNSListener dans les Services Windows.
2. Créer le schéma/tables avec le script validé par l'équipe, si ce n'est pas
   déjà fait.
3. Ouvrir FashionNova_Employes_Oracle.pro dans Qt Creator et choisir un kit 64 bits.
4. Lancer l'application et saisir les identifiants du schéma qui possède
   EMPLOYES et SEQ_EMPLOYES.

Attention : le script fourni commence par supprimer des tables existantes et
leurs contraintes. Ne l'exécutez que dans un schéma vide ou après sauvegarde.

Les champs de présence et la date de dernière connexion de l'interface restent
temporaires en mémoire : ils ne font pas partie de la table EMPLOYES du modèle
fourni.



Suppression des employés
------------------------
L'application vérifie d'abord si une maquette référence l'employé. Si oui, Oracle protège la clé étrangère et la suppression est refusée avec le nombre de maquettes à réaffecter. Aucune maquette n'est supprimée automatiquement.

