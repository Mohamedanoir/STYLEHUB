STYLEHUB - Integration 6 modules
================================

Ouvrir CMakeLists.txt avec Qt Creator (Qt 6.7.3 MinGW 64-bit).
Configurer le projet puis Build > Run.

Modules dans une seule application :
- Clients
- Employes
- Commandes
- Maquettes
- Machines
- Articles

Qt WebEngine n'est plus requis. La zone carte des Commandes utilise un apercu de remplacement pour garantir la compilation avec un kit Qt standard.
Les CRUD des modules sources sont conserves; Machines a ete reactive en mode CRUD.


CORRECTIONS (integration fix)
-----------------------------
1. Edition de liens (ld: multiple definition) : les classes DonutChart, LineChart (module Machines)
   et BannerWidget (module Machines) portaient le meme nom que celles de Maquettes / Articles.
   -> renommees en MachDonutChart, MachLineChart, MachBannerWidget (modules/machines/src, .h/.cpp/.ui).
2. Page Maquettes : MaquettesPage etait un squelette (sans style, icones, banniere, apercu image/GIF,
   pagination, statistiques, video, alerte retard, export PDF). Elle est reconstruite
   (integrated/maquettes/maquettespage.h/.cpp) sur l'API existante (Maquette, Banniere, ApercuMaquette,
   DonutChart/LineChart/BarChart). Images ajoutees dans resources.qrc sous le prefixe /maquettes.
3. Page Machines : style.qss n'etait jamais applique -> charge maintenant sur la page uniquement.
4. Ressource en double /images/logo.png (Clients + Machines) : entree inutilisee supprimee de
   modules/machines/resources.qrc.

NB : api_key.txt contient une cle API : ne la partagez pas / ne la poussez pas sur GitHub.


LOGIN (ajout)
-------------
- Au lancement, l'ecran d'authentification FASHIONOVA (dossier login/) s'affiche avant l'application.
- Pour l'instant : n'importe quelle saisie (meme vide) est acceptee -> clic sur "Se connecter" (ou Entree)
  ouvre l'application a 6 modules, puis le login se ferme.
- Le style du login (login/login.qss) est applique a la fenetre de login uniquement, pas a toute l'application.
- Ressources du login sous le prefixe /login (aucun conflit avec les autres modules).
- L'oeil affiche / masque le mot de passe. "Mot de passe oublie" affiche un message.
- Pour activer une vraie verification plus tard : modifier LoginWindow::onConnecter() (login/loginwindow.cpp).


CARTE DES LIVRAISONS (Gestion des Commandes)
--------------------------------------------
- Nouveau widget natif deliverymap.h/.cpp : carte OpenStreetMap SANS Qt WebEngine (Widgets + Network seulement).
- Connexion Internet necessaire pour le fond de carte et l'itineraire (cache disque ensuite pour les tuiles deja vues).
- Deplacement : glisser a la souris | Zoom : molette, double-clic ou boutons + / -
- Un marqueur par commande (vert = Livree, orange = En cours d'expedition, bleu = Prete en atelier) + atelier central.
- Clic sur un marqueur -> la commande est selectionnee dans le tableau et dans le panneau Details.
- Selection d'une commande dans le tableau -> la carte se centre dessus.
- Bouton "Itineraire" (et "Voir l'itineraire sur la carte") : trace atelier -> client (service OSRM) et met a jour
  la distance / les frais reels. Sans Internet : trace en ligne droite en pointilles.
- Bouton "Localiser" : recentre sur l'atelier.
- Position de l'atelier : constantes kAtelierLat / kAtelierLon dans deliverymap.h.
- Le chatbot (mots "route", "carte", "gps"...) declenche aussi l'affichage de l'itineraire.
