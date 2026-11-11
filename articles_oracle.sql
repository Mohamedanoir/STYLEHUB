-- Script Oracle SQL Developer : table ARTICLES (conforme au modèle relationnel validé)
-- NB : ID_Maquette et ID_Machine sont NULLABLES pour que le CRUD Articles fonctionne
--      même si les tables MAQUETTES et MACHINES ne sont pas encore remplies.
CREATE TABLE ARTICLES (
    ID_Article      VARCHAR2(20)  PRIMARY KEY,
    Nom_Article     VARCHAR2(100) NOT NULL,
    Categorie       VARCHAR2(50),
    Prix            NUMBER(10,3),
    Quantite_Stock  NUMBER(8)     DEFAULT 0,
    Date_Ajout      DATE,
    Statut          VARCHAR2(30),
    Demande         NUMBER(3)     DEFAULT 0,
    Image           VARCHAR2(300),
    Description     VARCHAR2(500),
    ID_Maquette     NUMBER,
    ID_Machine      NUMBER
    -- Quand MAQUETTES et MACHINES existent, ajouter :
    -- ,CONSTRAINT fk_art_maq FOREIGN KEY (ID_Maquette) REFERENCES MAQUETTES(ID_Maquette)
    -- ,CONSTRAINT fk_art_mac FOREIGN KEY (ID_Machine)  REFERENCES MACHINES(ID_Machine)
);

-- Donnees de depart : les 8 articles qui s'affichaient dans l'interface
INSERT INTO ARTICLES (ID_Article,Nom_Article,Categorie,Prix,Quantite_Stock,Date_Ajout,Statut,Demande,Image,Description) VALUES ('ART001','Soie naturelle','Tissus',45,120,TO_DATE('2026-01-12','YYYY-MM-DD'),'Disponible',85,':/images/art001.png','Soie fluide pour robes de soirée');
INSERT INTO ARTICLES (ID_Article,Nom_Article,Categorie,Prix,Quantite_Stock,Date_Ajout,Statut,Demande,Image,Description) VALUES ('ART002','Dentelle française','Tissus',68.5,35,TO_DATE('2026-02-08','YYYY-MM-DD'),'Disponible',60,':/images/art002.png','Dentelle fine ivoire');
INSERT INTO ARTICLES (ID_Article,Nom_Article,Categorie,Prix,Quantite_Stock,Date_Ajout,Statut,Demande,Image,Description) VALUES ('ART003','Fil polyester noir','Fils',2.9,0,TO_DATE('2026-02-20','YYYY-MM-DD'),'Rupture de stock',90,':/images/art003.png','Cône de 1000 m');
INSERT INTO ARTICLES (ID_Article,Nom_Article,Categorie,Prix,Quantite_Stock,Date_Ajout,Statut,Demande,Image,Description) VALUES ('ART004','Boutons nacre','Boutons',0.8,900,TO_DATE('2026-03-05','YYYY-MM-DD'),'Disponible',40,':/images/art004.png','Lot de 100 pièces');
INSERT INTO ARTICLES (ID_Article,Nom_Article,Categorie,Prix,Quantite_Stock,Date_Ajout,Statut,Demande,Image,Description) VALUES ('ART005','Fermeture invisible','Fermetures',1.5,240,TO_DATE('2026-03-18','YYYY-MM-DD'),'Disponible',30,':/images/art005.png','Longueur 50 cm');
INSERT INTO ARTICLES (ID_Article,Nom_Article,Categorie,Prix,Quantite_Stock,Date_Ajout,Statut,Demande,Image,Description) VALUES ('ART006','Satin duchesse','Tissus',39,18,TO_DATE('2026-04-02','YYYY-MM-DD'),'Disponible',55,':/images/art006.png','Satin épais brillant');
INSERT INTO ARTICLES (ID_Article,Nom_Article,Categorie,Prix,Quantite_Stock,Date_Ajout,Statut,Demande,Image,Description) VALUES ('ART007','Ruban velours','Accessoires',6.2,75,TO_DATE('2026-04-25','YYYY-MM-DD'),'Archivé',5,':/images/art007.png','Ancienne collection');
INSERT INTO ARTICLES (ID_Article,Nom_Article,Categorie,Prix,Quantite_Stock,Date_Ajout,Statut,Demande,Image,Description) VALUES ('ART008','Fil de soie','Fils',4.4,310,TO_DATE('2026-05-14','YYYY-MM-DD'),'Disponible',70,':/images/art008.png','Fil à broder');
COMMIT;
