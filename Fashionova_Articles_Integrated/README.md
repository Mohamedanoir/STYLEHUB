# Fashionova Articles CRUD Oracle

Projet Qt Widgets/Qt SQL intégré aux modules Clients et Employés du dépôt STYLEHUB.

## Ouvrir et compiler

1. Ouvrir `Fashionova_Articles.pro` dans Qt Creator.
2. Sélectionner Qt 6.11.2 MinGW 64-bit (ou un kit Qt 6 compatible avec QODBC).
3. Configurer le projet, puis Build (`Ctrl+B`) et Run (`Ctrl+R`).

## Connexion Oracle

La connexion reste en QODBC. Pour ne pas publier le mot de passe Oracle, le projet lit ces variables d'environnement dans Qt Creator, sous Projects > Run > Environment:

- `FASHIONOVA_DSN` (par défaut `fashionnova`)
- `FASHIONOVA_DB_USER` (par défaut `fashionnova`)
- `FASHIONOVA_DB_PASSWORD` (à renseigner localement)

Le script `verifier_fashionova_base_existante.sql` vérifie les tables, séquences et colonnes attendues sans modifier la base.
