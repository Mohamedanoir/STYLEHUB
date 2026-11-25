Correctif CRUD Employes
=======================

Le FK entre MAQUETTES.ID_Employe et EMPLOYES.ID_Employe bloquait la suppression
d'un employe utilise par une maquette. Le correctif 02_detach_employee_fk_from_maquettes.sql
retire uniquement ce FK. Il conserve la table MAQUETTES, ses lignes et les valeurs
de ses colonnes; il ne supprime aucun enregistrement.

Dans SQL Developer, ouvrir une feuille de calcul liee au schema qui possede les
tables du projet, ouvrir ce script, puis l'executer avec F5. Verifier que la
requete finale ne retourne aucune ligne. Ne pas executer ce correctif depuis un
compte qui n'est pas proprietaire des tables du projet.

La modification du CRUD affiche maintenant le code Oracle, le texte de la base
et le texte du pilote separes. Si Modifier echoue encore, ce detail permet de
voir la cause Oracle exacte au prochain essai.
