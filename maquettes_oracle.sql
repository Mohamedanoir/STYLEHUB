-- =====================================================================
--  FashioNova - Module MAQUETTES : script SANS DANGER
--  A executer dans SQL Developer avec la connexion FASHIONOVA (F5).
--  Ce script NE SUPPRIME AUCUNE TABLE : les modules des autres membres
--  (CLIENTS, COMMANDES, MACHINES, EMPLOYES, ARTICLES, CONTENIR) restent intacts.
--  Il ne touche qu'a la table MAQUETTES.
-- =====================================================================
SET SERVEROUTPUT ON;

-- 1) Structure de MAQUETTES (11 colonnes + 8 colonnes de mesures, voir partie 7 ; + NB_VUES optionnelle)
SELECT column_name, data_type, nullable
FROM user_tab_columns
WHERE table_name = 'MAQUETTES'
ORDER BY column_id;

-- 2) OPTIONNEL : colonne NB_VUES pour le "Top 5 des maquettes les plus consultees".
--    Sans elle l'application fonctionne (les consultations sont comptees en memoire
--    pendant la session). Pour l'ajouter, enlevez les deux tirets devant la ligne :
-- ALTER TABLE MAQUETTES ADD NB_VUES NUMBER DEFAULT 0;

-- 3) ID_EMPLOYE est obligatoire dans MAQUETTES : il faut au moins un employe.
SELECT * FROM EMPLOYES;

-- 4) OPTIONNEL : 6 maquettes de test, ajoutees SEULEMENT si MAQUETTES est vide
--    et si EMPLOYES contient au moins un employe.
DECLARE
    v_nb      PLS_INTEGER;
    v_emp     NUMBER;
    v_validee VARCHAR2(30) := UNISTR('Valid\00E9e');   -- "Validee" avec accent, quel que soit l'encodage du fichier
BEGIN
    SELECT COUNT(*) INTO v_nb FROM MAQUETTES;
    SELECT MIN(ID_EMPLOYE) INTO v_emp FROM EMPLOYES;

    IF v_nb = 0 AND v_emp IS NOT NULL THEN
        INSERT INTO MAQUETTES (ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_DE_CREATION, STATUT,
                               TAILLE, LARGEUR, LONGUEUR, DESCRIPTION, ID_EMPLOYE)
        VALUES (1, 'Robe de soiree', 'Robe', TRUNC(SYSDATE) - 12, 'En cours', 'M', 45, 140, 'Robe longue en satin', v_emp);
        INSERT INTO MAQUETTES (ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_DE_CREATION, STATUT,
                               TAILLE, LARGEUR, LONGUEUR, DESCRIPTION, ID_EMPLOYE)
        VALUES (2, 'Veste en lin', 'Veste', TRUNC(SYSDATE) - 30, v_validee, 'L', 52, 70, 'Veste ete coupe droite', v_emp);
        INSERT INTO MAQUETTES (ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_DE_CREATION, STATUT,
                               TAILLE, LARGEUR, LONGUEUR, DESCRIPTION, ID_EMPLOYE)
        VALUES (3, 'Jupe plissee', 'Jupe', TRUNC(SYSDATE) - 3, 'En attente', 'S', 36, 60, 'Jupe midi plissee', v_emp);
        INSERT INTO MAQUETTES (ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_DE_CREATION, STATUT,
                               TAILLE, LARGEUR, LONGUEUR, DESCRIPTION, ID_EMPLOYE)
        VALUES (4, 'Robe cocktail', 'Robe', TRUNC(SYSDATE) - 45, v_validee, 'S', 40, 95, 'Robe courte en crepe', v_emp);
        INSERT INTO MAQUETTES (ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_DE_CREATION, STATUT,
                               TAILLE, LARGEUR, LONGUEUR, DESCRIPTION, ID_EMPLOYE)
        VALUES (5, 'Blazer classique', 'Veste', TRUNC(SYSDATE) - 9, 'En attente', 'M', 50, 72, 'Blazer croise', v_emp);
        INSERT INTO MAQUETTES (ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_DE_CREATION, STATUT,
                               TAILLE, LARGEUR, LONGUEUR, DESCRIPTION, ID_EMPLOYE)
        VALUES (6, 'Jupe crayon', 'Jupe', TRUNC(SYSDATE) - 1, 'En cours', 'XS', 34, 58, 'Jupe droite taille haute', v_emp);
        COMMIT;
        DBMS_OUTPUT.PUT_LINE('6 maquettes de test ajoutees (employe ' || v_emp || ').');
    ELSE
        DBMS_OUTPUT.PUT_LINE('Rien ajoute : MAQUETTES contient deja des lignes, ou EMPLOYES est vide.');
    END IF;
END;
/

-- 5) Resultat
SELECT * FROM MAQUETTES ORDER BY ID_MAQUETTE;

-- 6) Types acceptes par l'application : Robe, Veste, Jupe, Pantalon, T-shirt, Chapeau, Chemise homme
--    Statuts proposes : En cours, Validee  (les anciennes lignes "En attente" restent affichees).
--    Si Oracle refuse un nouveau type (erreur ORA-02290), une contrainte CHECK limite
--    TYPE_MAQUETTE. Cette requete affiche les contraintes de MAQUETTES :
SELECT constraint_name, search_condition
FROM user_constraints
WHERE table_name = 'MAQUETTES' AND constraint_type = 'C';
--    Remplacer la contrainte (enlever les tirets et mettre le vrai nom de la contrainte) :
-- ALTER TABLE MAQUETTES DROP CONSTRAINT NOM_DE_LA_CONTRAINTE;
-- ALTER TABLE MAQUETTES ADD CONSTRAINT CK_MAQUETTES_TYPE
--     CHECK (TYPE_MAQUETTE IN ('Robe','Veste','Jupe','Pantalon','T-shirt','Chapeau','Chemise homme'));

-- 7) Mesures en cm qui dependent du type (le formulaire n'affiche que celles du type choisi) :
--      Robe          : Tour de poitrine, Tour de taille, Tour de hanches
--      Veste         : Tour de poitrine, Tour de taille, Largeur d'epaules, Longueur de manche
--      Jupe          : Tour de taille, Tour de hanches
--      Pantalon      : Tour de taille, Tour de hanches, Entrejambe
--      T-shirt       : Tour de poitrine, Largeur d'epaules, Longueur de manche
--      Chapeau       : Tour de tete
--      Chemise homme : Tour de cou, Tour de poitrine, Largeur d'epaules, Longueur de manche
--    (Largeur et Longueur : pour tous les types.)
--    L'application ajoute ces colonnes toute seule au demarrage si elles manquent
--    (colonnes vides, aucune donnee modifiee). Pour les ajouter a la main, enlevez les tirets
--    (une colonne qui existe deja donne l'erreur ORA-01430 : enlevez-la de la liste) :
-- ALTER TABLE MAQUETTES ADD (TOUR_COU NUMBER(6,2), TOUR_POITRINE NUMBER(6,2), TOUR_TAILLE NUMBER(6,2),
--     TOUR_HANCHES NUMBER(6,2), LARGEUR_EPAULES NUMBER(6,2), LONGUEUR_MANCHE NUMBER(6,2),
--     ENTREJAMBE NUMBER(6,2), TOUR_TETE NUMBER(6,2));

-- IMPORTANT : apres toute modification faite a la main dans SQL Developer
-- (onglet Donnees, INSERT, UPDATE...), faites COMMIT, sinon Qt ne voit pas
-- les changements et peut rester bloque sur la ligne modifiee.
COMMIT;
