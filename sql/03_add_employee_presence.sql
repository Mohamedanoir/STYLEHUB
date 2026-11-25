-- Add persistent attendance state to the existing EMPLOYES table.
-- Run once while connected as the owner of EMPLOYES (for example SYSTEM
-- only if SYSTEM owns this project table). This script preserves all rows.
SET SERVEROUTPUT ON;
DECLARE
    v_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_count
      FROM USER_TAB_COLUMNS
     WHERE TABLE_NAME = 'EMPLOYES'
       AND COLUMN_NAME = 'ETAT_PRESENCE';

    IF v_count = 0 THEN
        EXECUTE IMMEDIATE
            'ALTER TABLE EMPLOYES ADD (ETAT_PRESENCE VARCHAR2(10) DEFAULT ''ABSENT'' NOT NULL)';
        DBMS_OUTPUT.PUT_LINE('ETAT_PRESENCE added; existing employees default to ABSENT.');
    ELSE
        DBMS_OUTPUT.PUT_LINE('ETAT_PRESENCE already exists.');
    END IF;

    SELECT COUNT(*) INTO v_count
      FROM USER_CONSTRAINTS
     WHERE TABLE_NAME = 'EMPLOYES'
       AND CONSTRAINT_NAME = 'CK_EMP_PRESENCE';

    IF v_count = 0 THEN
        EXECUTE IMMEDIATE
            'ALTER TABLE EMPLOYES ADD CONSTRAINT CK_EMP_PRESENCE CHECK (ETAT_PRESENCE IN (''PRESENT'', ''ABSENT''))';
        DBMS_OUTPUT.PUT_LINE('Presence check constraint added.');
    ELSE
        DBMS_OUTPUT.PUT_LINE('CK_EMP_PRESENCE already exists.');
    END IF;
END;
/

SELECT ID_EMPLOYE, NOM_EMPLOYE, PRENOM_EMPLOYE, ETAT_PRESENCE
  FROM EMPLOYES
 ORDER BY ID_EMPLOYE;
