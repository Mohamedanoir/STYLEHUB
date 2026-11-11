-- Adapte la table CLIENTS visible dans Connexion_aziz au module Qt.
-- Les donnees actuelles (ID_CLIENT, NOM_CLIENT, PRENOM_CLIENT, TELEPHONE,
-- EMAIL et HISTORIQUE_ACHATS) sont conservees.
-- Executer dans SQL Developer connecte au schema utilise par l'application.

DECLARE
    v_count PLS_INTEGER;
    v_next_id NUMBER;

    PROCEDURE ajouter_colonne(p_nom VARCHAR2, p_type VARCHAR2) IS
    BEGIN
        SELECT COUNT(*) INTO v_count
          FROM USER_TAB_COLUMNS
         WHERE TABLE_NAME = 'CLIENTS' AND COLUMN_NAME = UPPER(p_nom);
        IF v_count = 0 THEN
            EXECUTE IMMEDIATE 'ALTER TABLE CLIENTS ADD (' || p_nom || ' ' || p_type || ')';
        END IF;
    END;
BEGIN
    ajouter_colonne('ID_APP', 'VARCHAR2(10)');
    ajouter_colonne('NOM_CLIENT', 'VARCHAR2(50)');
    ajouter_colonne('ADRESSE', 'VARCHAR2(200)');
    ajouter_colonne('TYPE_CLIENT', 'VARCHAR2(10)');
    ajouter_colonne('DATE_INSCRIPTION', 'DATE');
    ajouter_colonne('REMARQUES', 'VARCHAR2(500)');
END;
/

-- Bloc separe : Oracle connait maintenant les colonnes ajoutees ci-dessus.
DECLARE
    v_count PLS_INTEGER;
    v_next_id NUMBER;
BEGIN
    UPDATE CLIENTS
       SET ID_APP = 'CL-' || LPAD(TO_CHAR(ID_CLIENT), 3, '0')
     WHERE ID_APP IS NULL;
    UPDATE CLIENTS SET TYPE_CLIENT = 'Standard' WHERE TYPE_CLIENT IS NULL;
    UPDATE CLIENTS SET DATE_INSCRIPTION = SYSDATE WHERE DATE_INSCRIPTION IS NULL;

    SELECT NVL(MAX(ID_CLIENT), 0) + 1 INTO v_next_id FROM CLIENTS;
    BEGIN
        EXECUTE IMMEDIATE 'CREATE SEQUENCE FASHIONOVA_CLIENT_SEQ START WITH ' ||
                          TO_CHAR(v_next_id) || ' INCREMENT BY 1 NOCACHE';
    EXCEPTION
        WHEN OTHERS THEN
            IF SQLCODE != -955 THEN RAISE; END IF;
    END;

    SELECT COUNT(*) INTO v_count FROM USER_CONSTRAINTS
     WHERE TABLE_NAME = 'CLIENTS' AND CONSTRAINT_NAME = 'UK_CLIENTS_ID_APP';
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE 'ALTER TABLE CLIENTS ADD CONSTRAINT UK_CLIENTS_ID_APP UNIQUE (ID_APP)';
    END IF;
END;
/

-- Verification : affiche les colonnes requises par le module.
SELECT COLUMN_NAME, DATA_TYPE, DATA_LENGTH, NULLABLE
  FROM USER_TAB_COLUMNS
 WHERE TABLE_NAME = 'CLIENTS'
 ORDER BY COLUMN_ID;
