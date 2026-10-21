-- Executer dans SQL Developer avec la connexion FASHIONOVA (F5).
-- Supprime definitivement les autres tables et leurs donnees.
-- Conserve MAQUETTES et ses donnees.
SET SERVEROUTPUT ON;

DECLARE
    nb_maquettes PLS_INTEGER;
BEGIN
    IF SYS_CONTEXT('USERENV', 'SESSION_USER') <> 'FASHIONOVA' THEN
        RAISE_APPLICATION_ERROR(-20001, 'Utiliser la connexion FASHIONOVA.');
    END IF;

    SELECT COUNT(*) INTO nb_maquettes
    FROM user_tables
    WHERE table_name = 'MAQUETTES';

    IF nb_maquettes <> 1 THEN
        RAISE_APPLICATION_ERROR(-20002, 'MAQUETTES introuvable : aucune suppression effectuee.');
    END IF;

    FOR t IN (
        SELECT table_name FROM user_tables
        WHERE table_name <> 'MAQUETTES'
          AND nested = 'NO'
          AND secondary = 'N'
          AND (iot_type IS NULL OR iot_type = 'IOT')
        ORDER BY table_name
    ) LOOP
        EXECUTE IMMEDIATE 'DROP TABLE ' ||
            DBMS_ASSERT.ENQUOTE_NAME(t.table_name, FALSE) ||
            ' CASCADE CONSTRAINTS PURGE';
        DBMS_OUTPUT.PUT_LINE('Table supprimee : ' || t.table_name);
    END LOOP;
END;
/

SELECT table_name FROM user_tables ORDER BY table_name;
SELECT * FROM MAQUETTES;
