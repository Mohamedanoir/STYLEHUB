-- FashionNova application schema: exactly the six requested business tables.
-- Run as the FASHIONNOVA schema owner. This script does not DROP existing objects.

CREATE TABLE CLIENTS (
    ID_Client         NUMBER         CONSTRAINT PK_CLIENTS PRIMARY KEY,
    Nom_Client       VARCHAR2(50)   NOT NULL,
    Prenom_Client    VARCHAR2(50)   NOT NULL,
    Telephone        VARCHAR2(20),
    Email            VARCHAR2(100)  CONSTRAINT UQ_CLIENTS_EMAIL UNIQUE,
    Historique_Achats VARCHAR2(500)
);

CREATE TABLE EMPLOYES (
    ID_Employe      NUMBER         CONSTRAINT PK_EMPLOYES PRIMARY KEY,
    Nom_Employe     VARCHAR2(50)   NOT NULL,
    Prenom_Employe  VARCHAR2(50)   NOT NULL,
    Poste           VARCHAR2(50),
    Salaire         NUMBER(10,3)   CONSTRAINT CK_EMP_SALAIRE CHECK (Salaire >= 0),
    Mot_de_passe    VARCHAR2(100)  NOT NULL,
    Etat_Presence   VARCHAR2(10) DEFAULT 'ABSENT' NOT NULL
                    CONSTRAINT CK_EMP_PRESENCE CHECK (Etat_Presence IN ('PRESENT', 'ABSENT'))
);

CREATE TABLE MACHINES (
    ID_Machine                  NUMBER        CONSTRAINT PK_MACHINES PRIMARY KEY,
    Nom_Machine                 VARCHAR2(50)  NOT NULL,
    Type_Machine                VARCHAR2(50),
    Etat                        VARCHAR2(30),
    Date_Prochaine_Maintenance  DATE,
    Heures_Fonctionnement       NUMBER DEFAULT 0 CONSTRAINT CK_MACH_HEURES CHECK (Heures_Fonctionnement >= 0),
    Pieces_Produites            NUMBER DEFAULT 0 CONSTRAINT CK_MACH_PIECES CHECK (Pieces_Produites >= 0),
    Heures_Arret                NUMBER DEFAULT 0 CONSTRAINT CK_MACH_ARRET CHECK (Heures_Arret >= 0)
);

CREATE TABLE MAQUETTES (
    ID_Maquette       NUMBER         CONSTRAINT PK_MAQUETTES PRIMARY KEY,
    Nom_Maquette      VARCHAR2(50)   NOT NULL,
    Type_Maquette     VARCHAR2(30),
    Date_de_Creation  DATE DEFAULT SYSDATE,
    Statut            VARCHAR2(30),
    Taille            VARCHAR2(10),
    Largeur           NUMBER(6,2) CONSTRAINT CK_MAQ_LARGEUR CHECK (Largeur > 0),
    Longueur          NUMBER(6,2) CONSTRAINT CK_MAQ_LONGUEUR CHECK (Longueur > 0),
    Description       VARCHAR2(500),
    Video             VARCHAR2(255),
    -- Kept for the maquette module's data; deliberately no FK to EMPLOYES so
    -- the employee module can delete employees independently.
    ID_Employe        NUMBER
);

CREATE TABLE ARTICLES (
    ID_Article      NUMBER         CONSTRAINT PK_ARTICLES PRIMARY KEY,
    Nom_Article     VARCHAR2(50)   NOT NULL,
    Categorie       VARCHAR2(50),
    Prix            NUMBER(10,3) CONSTRAINT CK_ART_PRIX CHECK (Prix >= 0),
    Quantite_Stock  NUMBER DEFAULT 0 CONSTRAINT CK_ART_STOCK CHECK (Quantite_Stock >= 0),
    Date_Ajout      DATE DEFAULT SYSDATE,
    Statut          VARCHAR2(30),
    Demande         NUMBER DEFAULT 0,
    Image           VARCHAR2(255),
    Description     VARCHAR2(500),
    ID_Maquette     NUMBER NOT NULL,
    ID_Machine      NUMBER NOT NULL,
    CONSTRAINT FK_ART_MAQUETTE FOREIGN KEY (ID_Maquette) REFERENCES MAQUETTES(ID_Maquette),
    CONSTRAINT FK_ART_MACHINE FOREIGN KEY (ID_Machine) REFERENCES MACHINES(ID_Machine)
);

CREATE TABLE COMMANDES (
    ID_Commande        NUMBER         CONSTRAINT PK_COMMANDES PRIMARY KEY,
    Date_Commande      DATE DEFAULT SYSDATE NOT NULL,
    Statut             VARCHAR2(30),
    Montant_Total      NUMBER(10,3) CONSTRAINT CK_CMD_MONTANT CHECK (Montant_Total >= 0),
    Adresse_Livraison  VARCHAR2(200),
    Latitude           NUMBER(9,6),
    Longitude          NUMBER(9,6),
    Mode_Paiement      VARCHAR2(30),
    Quantite           NUMBER DEFAULT 1 NOT NULL CONSTRAINT CK_CMD_QUANTITE CHECK (Quantite > 0),
    ID_Client          NUMBER NOT NULL,
    ID_Article         NUMBER NOT NULL,
    CONSTRAINT FK_CMD_CLIENT FOREIGN KEY (ID_Client) REFERENCES CLIENTS(ID_Client),
    CONSTRAINT FK_CMD_ARTICLE FOREIGN KEY (ID_Article) REFERENCES ARTICLES(ID_Article)
);

CREATE SEQUENCE SEQ_CLIENTS START WITH 1 INCREMENT BY 1 NOCACHE;
CREATE SEQUENCE SEQ_EMPLOYES START WITH 1 INCREMENT BY 1 NOCACHE;
CREATE SEQUENCE SEQ_MACHINES START WITH 1 INCREMENT BY 1 NOCACHE;
CREATE SEQUENCE SEQ_MAQUETTES START WITH 1 INCREMENT BY 1 NOCACHE;
CREATE SEQUENCE SEQ_ARTICLES START WITH 1 INCREMENT BY 1 NOCACHE;
CREATE SEQUENCE SEQ_COMMANDES START WITH 1 INCREMENT BY 1 NOCACHE;

-- Check that this schema contains only the six application tables.
SELECT TABLE_NAME FROM USER_TABLES ORDER BY TABLE_NAME;
