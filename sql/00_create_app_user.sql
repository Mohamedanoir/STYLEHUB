-- Run this script once while connected as SYSTEM in Oracle SQL Developer.
-- SQL Developer will prompt for the application user's password.
-- This creates a separate schema; it does not remove or change Oracle's SYSTEM objects.

CREATE USER FASHIONNOVA IDENTIFIED BY "&FASHIONNOVA_PASSWORD"
  DEFAULT TABLESPACE USERS
  TEMPORARY TABLESPACE TEMP
  QUOTA 100M ON USERS;

GRANT CREATE SESSION, CREATE TABLE, CREATE SEQUENCE TO FASHIONNOVA;

-- Then create a SQL Developer connection named FASHIONNOVA using this new password,
-- open 01_create_six_tables.sql under that connection, and run it with F5.
