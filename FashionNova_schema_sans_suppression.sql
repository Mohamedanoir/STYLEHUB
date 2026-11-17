-- =====================================================================
--  FashionNova - Script de création de la base (Oracle)
-- =====================================================================

-- Script de création uniquement : aucune table ou séquence n'est supprimée.
-- À exécuter une seule fois dans un schéma vide.

-- ---------- 1. CLIENTS ----------
CREATE TABLE CLIENTS (
    ID_Client          NUMBER         CONSTRAINT pk_clients PRIMARY KEY,
    Prenom_Client      VARCHAR2(50)   NOT NULL,
    Telephone          VARCHAR2(20),
    Email              VARCHAR2(100)  CONSTRAINT uq_clients_email UNIQUE,
    Historique_Achats  VARCHAR2(500)
);

-- ---------- 2. EMPLOYES ----------
CREATE TABLE EMPLOYES (
    ID_Employe      NUMBER         CONSTRAINT pk_employes PRIMARY KEY,
    Nom_Employe     VARCHAR2(50)   NOT NULL,
    Prenom_Employe  VARCHAR2(50)   NOT NULL,
    Poste           VARCHAR2(50),
    Salaire         NUMBER(10,3)   CONSTRAINT ck_emp_salaire CHECK (Salaire >= 0),
    Mot_de_passe    VARCHAR2(100)  NOT NULL
);

-- ---------- 3. MACHINES ----------
CREATE TABLE MACHINES (
    ID_Machine                  NUMBER        CONSTRAINT pk_machines PRIMARY KEY,
    Nom_Machine                 VARCHAR2(50)  NOT NULL,
    Type_Machine                VARCHAR2(50),
    Etat                        VARCHAR2(30),
    Date_Prochaine_Maintenance  DATE,
    Heures_Fonctionnement       NUMBER        DEFAULT 0 CHECK (Heures_Fonctionnement >= 0),
    Pieces_Produites            NUMBER        DEFAULT 0 CHECK (Pieces_Produites >= 0),
    Heures_Arret                NUMBER        DEFAULT 0 CHECK (Heures_Arret >= 0)
);

-- ---------- 4. COMMANDES  (FK -> CLIENTS : "passer") ----------
CREATE TABLE COMMANDES (
    ID_Commande        NUMBER         CONSTRAINT pk_commandes PRIMARY KEY,
    Date_Commande      DATE           DEFAULT SYSDATE NOT NULL,
    Statut             VARCHAR2(30),
    Montant_Total      NUMBER(10,3)   CONSTRAINT ck_cmd_montant CHECK (Montant_Total >= 0),
    Adresse_Livraison  VARCHAR2(200),
    Latitude           NUMBER(9,6),
    Longitude          NUMBER(9,6),
    Mode_Paiement      VARCHAR2(30),
    ID_Client          NUMBER         NOT NULL,
    CONSTRAINT fk_cmd_client FOREIGN KEY (ID_Client)
               REFERENCES CLIENTS (ID_Client)
);

-- ---------- 5. MAQUETTES  (FK -> EMPLOYES : "faire") ----------
CREATE TABLE MAQUETTES (
    ID_Maquette       NUMBER         CONSTRAINT pk_maquettes PRIMARY KEY,
    Nom_Maquette      VARCHAR2(50)   NOT NULL,
    Type_Maquette     VARCHAR2(30),
    Date_de_Creation  DATE           DEFAULT SYSDATE,
    Statut            VARCHAR2(30),
    Taille            VARCHAR2(10),
    Largeur           NUMBER(6,2)    CONSTRAINT ck_maq_largeur  CHECK (Largeur  > 0),
    Longueur          NUMBER(6,2)    CONSTRAINT ck_maq_longueur CHECK (Longueur > 0),
    Description       VARCHAR2(500),
    Video             VARCHAR2(255),
    ID_Employe        NUMBER         NOT NULL,
    CONSTRAINT fk_maq_employe FOREIGN KEY (ID_Employe)
               REFERENCES EMPLOYES (ID_Employe)
);

-- ---------- 6. ARTICLES  (FK -> MAQUETTES "possède", FK -> MACHINES "traiter") ----------
CREATE TABLE ARTICLES (
    ID_Article      NUMBER         CONSTRAINT pk_articles PRIMARY KEY,
    Nom_Article     VARCHAR2(50)   NOT NULL,
    Categorie       VARCHAR2(50),
    Prix            NUMBER(10,3)   CONSTRAINT ck_art_prix  CHECK (Prix >= 0),
    Quantite_Stock  NUMBER         DEFAULT 0 CONSTRAINT ck_art_stock CHECK (Quantite_Stock >= 0),
    Date_Ajout      DATE           DEFAULT SYSDATE,
    Statut          VARCHAR2(30),
    Demande         NUMBER         DEFAULT 0,
    Image           VARCHAR2(255),
    Description     VARCHAR2(500),
    ID_Maquette     NUMBER         NOT NULL,
    ID_Machine      NUMBER         NOT NULL,
    CONSTRAINT fk_art_maquette FOREIGN KEY (ID_Maquette)
               REFERENCES MAQUETTES (ID_Maquette),
    CONSTRAINT fk_art_machine  FOREIGN KEY (ID_Machine)
               REFERENCES MACHINES (ID_Machine)
);

-- ---------- 7. CONTENIR  (n,n : FK -> COMMANDES et FK -> ARTICLES) ----------
CREATE TABLE CONTENIR (
    ID_Commande  NUMBER  NOT NULL,
    ID_Article   NUMBER  NOT NULL,
    Quantite     NUMBER  NOT NULL CONSTRAINT ck_cont_qte CHECK (Quantite > 0),
    CONSTRAINT pk_contenir PRIMARY KEY (ID_Commande, ID_Article),
    CONSTRAINT fk_cont_commande FOREIGN KEY (ID_Commande)
               REFERENCES COMMANDES (ID_Commande) ON DELETE CASCADE,
    CONSTRAINT fk_cont_article  FOREIGN KEY (ID_Article)
               REFERENCES ARTICLES (ID_Article)
);

-- ---------- 8. Séquences ----------
CREATE SEQUENCE seq_clients   START WITH 1 INCREMENT BY 1;
CREATE SEQUENCE seq_employes  START WITH 1 INCREMENT BY 1;
CREATE SEQUENCE seq_machines  START WITH 1 INCREMENT BY 1;
CREATE SEQUENCE seq_commandes START WITH 1 INCREMENT BY 1;
CREATE SEQUENCE seq_maquettes START WITH 1 INCREMENT BY 1;
CREATE SEQUENCE seq_articles  START WITH 1 INCREMENT BY 1;

-- ---------- 9. Données de test ----------
INSERT INTO EMPLOYES  VALUES (seq_employes.NEXTVAL, 'Ben Ali', 'Ahmed', 'Styliste', 1800, 'ahmed123');
INSERT INTO MACHINES  VALUES (seq_machines.NEXTVAL, 'Brother X1', 'Couture', 'Fonctionnelle',
                              DATE '2026-12-01', 120, 450, 5);
INSERT INTO MAQUETTES VALUES (seq_maquettes.NEXTVAL, 'Robe d''été', 'Robe', SYSDATE, 'Validée',
                              'M', 60, 110, 'Robe légère en coton', NULL, 1);
INSERT INTO ARTICLES  VALUES (seq_articles.NEXTVAL, 'Robe d''été rouge', 'Robes', 89.900, 20,
                              SYSDATE, 'Disponible', 0, NULL, 'Taille M, rouge', 1, 1);
INSERT INTO CLIENTS   VALUES (seq_clients.NEXTVAL, 'Sarra', '22123456', 'sarra@mail.com', NULL);
INSERT INTO COMMANDES VALUES (seq_commandes.NEXTVAL, SYSDATE, 'En cours', 179.800,
                              'Tunis', 36.806500, 10.181500, 'Carte', 1);
INSERT INTO CONTENIR  VALUES (1, 1, 2);

COMMIT;

-- ---------- 10. Vérification des clés étrangères (6 lignes) ----------
SELECT a.table_name      AS table_fils,
       c.column_name     AS cle_etrangere,
       r.table_name      AS table_pere
FROM   user_constraints  a
JOIN   user_cons_columns c ON c.constraint_name = a.constraint_name
JOIN   user_constraints  r ON r.constraint_name = a.r_constraint_name
WHERE  a.constraint_type = 'R'
ORDER  BY a.table_name;

-- ---------- 11. Vérification des liens (jointures) ----------
-- Employé -> Maquette -> Article -> Machine
SELECT e.Prenom_Employe, m.Nom_Maquette, a.Nom_Article, ma.Nom_Machine
FROM   EMPLOYES  e
JOIN   MAQUETTES m  ON m.ID_Employe  = e.ID_Employe
JOIN   ARTICLES  a  ON a.ID_Maquette = m.ID_Maquette
JOIN   MACHINES  ma ON ma.ID_Machine = a.ID_Machine;

-- Client -> Commande -> Article
SELECT cl.Prenom_Client, co.ID_Commande, a.Nom_Article, ct.Quantite
FROM   CLIENTS   cl
JOIN   COMMANDES co ON co.ID_Client   = cl.ID_Client
JOIN   CONTENIR  ct ON ct.ID_Commande = co.ID_Commande
JOIN   ARTICLES  a  ON a.ID_Article   = ct.ID_Article;

