FASHIONOVA ORDERS - Qt Creator
=================================

Projet Qt Widgets + Qt WebEngine.

IMPORTANT:
Le projet est préparé pour utiliser directement les images de votre dossier
Documents/FashionovaOrders/images.

Copiez ces fichiers dans le dossier images/ du projet :
- sidebar.png
- image_tete.png
- prod_veste.png
- prod_bijoux.png
- prod_chaussure.png
- prod_lunette.png
- prod_montre.png
- prod_sac.png

Le programme fonctionne même si une image manque : un emplacement de secours est affiché.

CARTE:
La carte est une vraie carte interactive basée sur Leaflet + OpenStreetMap.
Une connexion Internet est nécessaire pour charger les tuiles.
- zoom / dézoom
- déplacement
- marqueurs clients
- couleurs selon le statut
- atelier central
- clic sur un marqueur -> informations de livraison
- bouton "Localiser" recentre la carte sur l'atelier
- bouton "Itinéraire" dessine un trajet visuel atelier -> client sélectionné

STATUTS:
Vert  = Livrée
Orange = En cours d'expédition
Bleu = Prête en atelier

DISTANCE / FRAIS:
Les distances d'exemple sont calculées côté C++ avec la formule de Haversine
à partir des coordonnées de l'atelier et des clients. Les frais et délais sont
des estimations de démonstration et peuvent être remplacés par votre logique métier.

OUVERTURE:
1. Décompressez le ZIP.
2. Ouvrez FashionovaOrders.pro dans Qt Creator.
3. Configurez un kit Qt 6 avec Qt WebEngineWidgets installé.
4. Lancez qmake/CMake selon votre kit, puis Build + Run.

Si votre installation Qt ne contient pas WebEngineWidgets, installez le composant
Qt WebEngine via l'installateur/maintenance tool Qt.
