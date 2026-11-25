-- Run as the owner of EMPLOYES and MAQUETTES.
-- Removes only the FK from MAQUETTES to EMPLOYES, so employee CRUD is independent.
-- Existing maquette rows and their ID_Employe values are preserved.

BEGIN
  FOR c IN (
    SELECT child_fk.constraint_name
    FROM user_constraints child_fk
    JOIN user_cons_columns child_col
      ON child_col.constraint_name = child_fk.constraint_name
    JOIN user_constraints parent_pk
      ON parent_pk.constraint_name = child_fk.r_constraint_name
    WHERE child_fk.table_name = 'MAQUETTES'
      AND child_fk.constraint_type = 'R'
      AND child_col.column_name = 'ID_EMPLOYE'
      AND parent_pk.table_name = 'EMPLOYES'
  ) LOOP
    EXECUTE IMMEDIATE 'ALTER TABLE MAQUETTES DROP CONSTRAINT ' || c.constraint_name;
  END LOOP;
END;
/

SELECT child_fk.constraint_name, child_fk.delete_rule
FROM user_constraints child_fk
JOIN user_cons_columns child_col
  ON child_col.constraint_name = child_fk.constraint_name
JOIN user_constraints parent_pk
  ON parent_pk.constraint_name = child_fk.r_constraint_name
WHERE child_fk.table_name = 'MAQUETTES'
  AND child_fk.constraint_type = 'R'
  AND child_col.column_name = 'ID_EMPLOYE'
  AND parent_pk.table_name = 'EMPLOYES';
-- The final query should return no rows.
