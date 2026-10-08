FASHIONOVA — SmartMarket : intégration des 6 modules (méthode du « Guide d'intégration »)
=========================================================================================

OUVRIR LE PROJET : Qt Creator -> ouvrir CMakeLists.txt (Qt 6.7.3 MinGW 64-bit) -> Configure -> Build -> Run.

DÉROULEMENT DE L'APPLICATION
  1. Page de connexion (PLogin). Pour l'instant, N'IMPORTE QUELLE saisie (même vide) + « Se connecter » fonctionne.
     « Mot de passe oublié ? » ouvre la page PMotDePasse.
  2. MENU PRINCIPAL (PMenu) : un bouton par module (Clients, Employés, Commandes, Maquettes, Articles, Machines).
     On clique sur un module : son interface s'ouvre.
  3. Dans un module, le menu latéral permet de passer à un autre module ; « Déconnexion » ramène à la page de connexion
     (puis, après une nouvelle connexion, au menu principal).

1) CE QUE DEMANDE LE GUIDE  ->  OÙ C'EST DANS LE PROJET
-------------------------------------------------------
- Un seul UI « SmartMarket.ui »                      -> smartmarket.ui
- Un QStackedWidget principal                        -> SWSmartMarket
- Une page par module (noms explicites)              -> PLogin(0) PMotDePasse(1) PMenu(2) PClient(3) PEmpl(4) PCommande(5)
                                                        PMaquette(6) PMachine(7) PArticle(8)       (enum SmartMarket::Page)
- Chaque module a son stackedWidget interne          -> SMClient, SMEmpl, SMCommande, SMMaquette, SMMachine, SMArticle
                                                        (page « CRUD » : PClientCRUD, PEmplCRUD, ...)
- Widgets des modules copiés dans les pages          -> Clients, Maquettes, Machines, Articles : widgets dans smartmarket.ui
                                                        Employés, Commandes : interface construite en code, insérée dans PEmplCRUD / PCommandeCRUD
- Noms d'objets significatifs                        -> suffixe du module : btnAjouterClient, editNomMaquette, tableMachine...
- Slots copiés dans SmartMarket.cpp / SmartMarket.h  -> toute la logique des modules est dans la classe SmartMarket
                                                        (sections « MODULE CLIENTS », « MODULE ARTICLES », ... de smartmarket.cpp ;
                                                        les méthodes portent le nom du module : ajouterClient(), onAjouterMaquette(), ...)
- Boutons de menu + navigation                       -> on_btnMenuClients_clicked() ... (menu principal), on_btnMClient_clicked() ... (menu latéral),
                                                        on_btnDeconnexion_clicked()
- Auto-connexion des slots (on_<objet>_<signal>)     -> utilisée pour la navigation et la connexion, comme dans le guide

2) ORGANISATION DU CODE
-----------------------
smartmarket.h/.cpp/.ui   fenêtre unique : navigation, connexion, menu principal + logique de TOUS les modules
smartsidebar.h           menu latéral (widget promu dans le .ui)
deliverymap.h/.cpp       carte des livraisons (OpenStreetMap, sans Qt WebEngine)
integrated/clients/client.h, integrated/articles/article.h, integrated/employes/employee.h    types de données des modules
maquette.* (modèle), widgets.*, statwidgets.*, icones.*, connexion.*                           Maquettes
integrated/articles/articlewidgets.*   widgets des Articles       integrated/employes/logindialog.*   connexion employé
modules/machines/src/*   base de données, dialogue de détails, graphiques et bannière des Machines
login/                   style, images et polices de la page de connexion
menu/                    style et images du menu principal
docs/ui_originaux_modules/   les anciens .ui séparés (archivés, plus compilés)
docs/anciens_projets/        anciens fichiers .pro (obsolètes : utiliser CMakeLists.txt)

3) AJOUTER UN NOUVEAU MODULE
----------------------------
1. smartmarket.ui : page « PXxx » (+ stackedWidget interne SMXxx) dans SWSmartMarket, bouton « btnMXxx » dans le menu latéral
   et bouton « btnMenuXxx » dans le menu principal.
2. smartmarket.h : ajouter PXxx à l'enum Page et les slots on_btnMXxx_clicked() / on_btnMenuXxx_clicked() ; smartmarket.cpp : setCurrentIndex(PXxx).
3. Copier les slots du module dans SmartMarket.cpp/.h (noms des widgets uniques : suffixe du module).

4) CARTE DES LIVRAISONS (Gestion des Commandes)
-----------------------------------------------
Fond de carte OpenStreetMap et itinéraire : connexion Internet nécessaire (tuiles mises en cache ensuite).
Glisser = déplacer ; molette / double-clic / boutons + et - = zoom ; clic sur un marqueur = sélectionne la commande ;
« Itinéraire » = trace atelier -> client (service OSRM, repli en ligne droite hors-ligne) ; « Localiser » = recentre sur l'atelier.
Position de l'atelier : kAtelierLat / kAtelierLon dans deliverymap.h.

5) DIVERS
---------
- api_key.txt contient une clé API : ne la partagez pas / ne la poussez pas sur GitHub.
- Pour activer une vraie vérification de connexion : modifier SmartMarket::on_btnConnecter_clicked() (smartmarket.cpp).
- Les bases SQLite (maquettes_fashionova.db, fashionova_machines.db) sont créées automatiquement.
