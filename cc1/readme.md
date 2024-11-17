# Projet de C : CC_1

Le projet est situé dans le dossier cc1/
<br>Le dossier useless/ contient des fichiers inutile au bon fonctionnement du projet mentionné.<br>
## Pré-requis et compilation

#### **Pré-requis**
- **Compilateur** : GCC (GNU Compiler Collection)
- **Outils de débogage** :
  - AddressSanitizer et UndefinedBehaviorSanitizer (inclus avec GCC)
  - Valgrind (analyseur de mémoire)

---
### Shell

Certains IDE ou shells minimalistes, comme le terminal intégré par défaut de Visual Studio Code (`bash 2.5`), peuvent ne pas lancer la commande `make` pour la compilation du programme.<br> 
Par exemples, le terminal pourraient retourner des messages d'erreur ou des problèmes d'incompatibilités avec les outils système comme gcc, make, ou valgrind. Pour éviter ces problèmes, il est recommandé d'utiliser un terminal complet et bien configuré, comme celui de votre système d'exploitation (par exemple, bash, zsh, ou fish) ou un terminal externe comme Windows Terminal, iTerm2 (macOS), ou GNOME Terminal (Linux). Cela garantit un environnement plus robuste pour la compilation et l'exécution de projets complexes.

---

### Vérifier les Bibliothèques

Assurez-vous que les bibliothèques AddressSanitizer et UndefinedBehaviorSanitizer sont installées. Ces paquets sont nécessaire à la commande `make sanitize`.
<br><br>Sur un système Fedora (ou RHEL/CentOS), installez-les avec :

```bash
sudo dnf install libasan libubsan
```

Pour Ubuntu/Debian :
```bash
sudo apt install libasan8 libubsan1
```

Vérifiez également que les versions de vos bibliothèques correspondent à celles utilisées par votre compilateur GCC/Clang:
```bash
sudo dnf install gcc
```

Pour Ubuntu/Debian :
```bash
sudo apt install gcc
```

---

#### **Cibles du Makefile**
Le `Makefile` fourni contient plusieurs cibles pour compiler, exécuter et nettoyer le projet :

1. **`make`**
   - Compile le projet en mode standard (sans activer les outils de débogage avancés).
   - Produit un exécutable nommé `ViewDB`.

2. **`make sanitize`**
   - Compile le projet avec des outils de détection d’erreurs mémoire (AddressSanitizer et UndefinedBehaviorSanitizer).
   - Utile pour identifier les erreurs comme :
     - Les dépassements de tampon.
     - Les accès mémoire invalides.
     - Les comportements indéfinis.

3. **`make valgrind`**
   - Compile le projet sans les outils sanitizers pour garantir la compatibilité avec Valgrind.
   - Exécute l’exécutable avec Valgrind pour détecter :
     - Les fuites de mémoire.
     - Les accès illégaux à la mémoire.
   - Affiche un rapport détaillé des problèmes détectés.

4. **`make clean`**
   - Il se peut que `make` échoue alors exécutez `make clean` au préalable
   - Supprime les fichiers objets (`*.o`) et l’exécutable généré (`ViewDB`).

---

#### **Instructions**
1. **Compiler le projet** :
   ```bash
   make
   ```

2. **Exécuter avec AddressSanitizer et UndefinedBehaviorSanitizer** :
   ```bash
   make sanitize
   ./ViewDB
   ```

3. **Analyser avec Valgrind** :
   ```bash
   make valgrind
   ```

4. **Nettoyer les fichiers générés** :
   ```bash
   make clean
   ```

---

#### **Structure du Projet**
- **Code source** : Tous les fichiers `.c` sont situés dans le dossier `src`.
- **Exécutable** : Produit final nommé `ViewDB`.

---

#### **Exemple d’Utilisation**
1. Lancez le programme avec l’exécutable :
   ```bash
   ./ViewDB
   ```
2. Essayez différentes commandes interactives, comme `CREATE`, `INSERT`, `SHOW TABLES`, etc.
3. Vérifiez la sortie pour vous assurer que les opérations sont correctes.
4. Utilisez `make sanitize` ou `make valgrind` pour analyser les erreurs potentielles dans le code.

---


#### **Explication des Lignes de Configuration**

1. **Définition du Compilateur :**
   ```makefile
   CC = gcc
   ```
   - Spécifie le compilateur utilisé pour ce projet : **GCC** (GNU Compiler Collection).
   - Ce choix garantit une compatibilité avec les outils modernes de débogage comme AddressSanitizer et UndefinedBehaviorSanitizer.

2. **Options de Compilation Générales :**
   ```makefile
   CFLAGS = -Wall -Werror -Wextra -std=gnu99 -g
   ```
   - **`-Wall`** : Active tous les avertissements courants pour détecter d’éventuelles erreurs ou mauvaises pratiques.
   - **`-Werror`** : Traite les avertissements comme des erreurs pour garantir un code sans défauts.
   - **`-Wextra`** : Active des avertissements supplémentaires pour une vérification encore plus stricte.
   - **`-std=gnu99`** : Utilise le standard **C99** avec des extensions GNU en supplément pour une compatibilité accrue.
   - **`-g`** : Ajoute des informations de débogage au binaire, ce qui est essentiel pour utiliser des outils comme **gdb** ou **Valgrind**.

3. **Options pour les Sanitizers :**
   ```makefile
   SANITIZE_FLAGS = -fsanitize=address -fsanitize=undefined
   ```
   - **`-fsanitize=address`** :
     - Active **AddressSanitizer**, un outil qui détecte :
       - Les dépassements de tampon.
       - Les accès mémoire après libération (`use-after-free`).
       - Les lectures/écritures sur des zones mémoire invalides.
   - **`-fsanitize=undefined`** :
     - Active **UndefinedBehaviorSanitizer**, un outil qui détecte :
       - Les comportements indéfinis (par exemple, un débordement d’entier ou un décalage de bits invalide).
       - Les déférencements de pointeurs nuls.

4. **Options pour l'Édition des Liens :**
   ```makefile
   LDFLAGS =
   ```
   - Cette variable est utilisée pour spécifier des options spécifiques à l'éditeur de liens.
   - Dans ce projet, les flags des sanitizers sont souvent ajoutés dynamiquement à cette variable pour garantir que les bibliothèques nécessaires (comme `libasan` pour AddressSanitizer) sont correctement liées.

---

#### **Pourquoi ces Options Sont Importantes**

- **Détection des Erreurs en Amont :** Les options comme `-Wall`, `-Werror` et `-Wextra` aident à repérer et corriger les erreurs potentielles dès la compilation.
- **Débogage Avancé :** L’activation des sanitizers avec `-fsanitize=address` et `-fsanitize=undefined` permet de trouver des erreurs complexes qui pourraient passer inaperçues lors des tests standards.
- **Respect des Standards :** Le standard **C99** est largement utilisé et compatible avec les bonnes pratiques modernes en C.
- **Maintenance Facile :** Ces options rendent le code plus robuste, fiable et facile à maintenir sur le long terme.

---
### Liste des fichiers

Le projet est structuré comme suit :

```
├── database.csv       # Contient les données de la base de données sous forme de texte
├── Makefile           # Fichier de gestion de la compilation
├── readme.md          # Documentation du projet
├── src                # Dossier contenant les fichiers source et les fichiers objets
│   ├── btree.c        # Implémentation des fonctions pour la gestion du B-Tree
│   ├── btree.h        # En-tête pour le B-Tree
│   ├── create.c       # Implémentation des fonctions pour la création de tables
│   ├── create.h       # En-tête pour les fonctions de création de tables
│   ├── delete.c       # Implémentation des fonctions pour supprimer des données
│   ├── delete.h       # En-tête pour les fonctions de suppression
│   ├── describe.c     # Implémentation des fonctions pour décrire les tables
│   ├── describe.h     # En-tête pour les fonctions de description
│   ├── insert.c       # Implémentation des fonctions pour insérer des données
│   ├── insert.h       # En-tête pour les fonctions d'insertion
│   ├── main           # Fichier exécutable temporaire (généré après compilation)
│   ├── main.c         # Point d’entrée principal de l’application
│   ├── repl.c         # Implémentation de la boucle interactive (REPL)
│   ├── repl.h         # En-tête pour les fonctions de la boucle interactive
│   ├── select.c       # Implémentation des fonctions pour sélectionner des données
│   ├── select.h       # En-tête pour les fonctions de sélection
│   └── persistance.c  # Implémentation des fonctions sur la persistyance mémoire
└── ViewDB             # Exécutable final produit après compilation
```

---

#### **Explications**
- **database.csv** : Stocke les données de la base sous forme structurée.
- **Makefile** : Définit les règles de compilation et les commandes associées (`make`, `make clean`, etc.).
- **readme.md** : Documentation complète pour expliquer le projet et son utilisation.
- **src/** : Contient tous les fichiers sources `.c` et `.h`, ainsi que les fichiers objets `.o` générés après la compilation.
- **ViewDB** : Exécutable final de l’application, généré après une compilation réussie.

---

### Fonctionnement des commandes

#### **Cheminement des commandes**

Le fonctionnement des commandes dans ce projet repose sur une architecture bien structurée autour de deux fonctions principales :
1. **`prepare_statement`** : Analyse et prépare la commande entrée par l'utilisateur.
2. **`execute_statement`** : Exécute la commande une fois validée et structurée.

---

#### **Étape 1 : Lecture de la commande**

La boucle interactive (REPL) commence par lire l'entrée utilisateur :
- La fonction `read_input` récupère la commande et la place dans un tampon (`input_buffer`).
- Si la commande commence par un point (par exemple, `.exit`), elle est interprétée comme une commande interne (`do_meta_command`).

Exemple :
```c
if (input_buffer->buffer[0] == '.') {
    switch (do_meta_command(input_buffer)) {
        case META_COMMAND_SUCCESS:
            continue;
        case META_COMMAND_UNRECOGNIZED_COMMAND:
            printf("Unrecognized command '%s'\n", input_buffer->buffer);
            continue;
    }
}
```

---

#### **Étape 2 : Préparation de la commande**

Les commandes reconnues (comme `create`, `select`, etc.) sont analysées et préparées dans `prepare_statement`. Voici le processus pour une commande `create` :

1. Identifie le type de commande avec `strncmp` :
   ```c
   if (strncmp(input_buffer->buffer, "create", 6) == 0) {
       statement->type = STATEMENT_CREATETABLE;
       // Analyse des colonnes et de la table
   }
   ```

2. Parse les paramètres comme le nom de la table et les colonnes via `sscanf` et `strtok`.

3. Les données analysées sont stockées dans une structure `Statement` pour être utilisées par `execute_statement`.

Exemple :
```c
if (sscanf(input_buffer->buffer, "create %s (%[^)])", statement->table_name, columns_definition) == 2) {
    // Analyse des colonnes
    char* token = strtok(columns_definition, ",");
    while (token != NULL) {
        // Création des nœuds pour chaque colonne
    }
}
```

---

#### **Étape 3 : Exécution de la commande**

Une fois le `Statement` préparé, `execute_statement` prend en charge l'exécution.

Le type de commande est déterminé avec un `switch`. Par exemple :
- **Pour `create`** :
   ```c
   case (STATEMENT_CREATETABLE):
       if (create_table(statement->table_name, statement->columns_head, statement->column_count) == 0) {
           printf("Table '%s' créée avec succès.\n", statement->table_name);
       } else {
           printf("Erreur lors de la création de la table '%s'.\n", statement->table_name);
       }
       break;
   ```

- **Pour `show tables`** :
   ```c
   case (STATEMENT_SHOWTABLES):
       printf("Liste des tables disponibles :\n");
       // Lecture et affichage des noms des tables depuis database.csv
       break;
   ```

- **Pour `select`** :
   ```c
   case (STATEMENT_SELECT):
       execute_select(statement, filename);
       break;
   ```

Chaque commande a une implémentation dédiée (`execute_create`, `execute_select`, etc.) qui interagit avec les fichiers ou la structure en mémoire, comme le B-Tree.

---

#### **Étape 4 : Résultats et Boucle**

Après exécution, le résultat est affiché, et la boucle reprend jusqu'à ce que l'utilisateur quitte (`exit` ou `.exit`).

Exemple de boucle complète dans `repl` :
```c
void repl() {
    InputBuffer* input_buffer = new_input_buffer();
    while (1) {
        print_prompt();
        read_input(input_buffer);

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case PREPARE_SUCCESS:
                execute_statement(&statement, input_buffer, "database.csv");
                break;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                break;
        }
    }
}
```

---

Voici un tableau clair et structuré qui récapitule les **macros**, **typedefs**, et **structs** définis dans les fichiers `.h` et utilisés dans les fichiers `.c`.

---

### Tableau des Structs et Macros

| Fichier `.h`         | Inclus dans les fichiers `.c`   | **Macros utilisées**           | **Typedefs utilisés**        | **Structs utilisées**                  |
|-----------------------|---------------------------------|---------------------------------|------------------------------|----------------------------------------|
| `repl.h`         | `repl.c`, `select.c`, `create.c`, `delete.c` | `MAX_LINE_LENGTH`, `MAX_COLUMNS` | `ColumnNode`                | `InputBuffer`, `Statement`            |
| `btree.h`        | `btree.c`, `repl.c`, `delete.c` | `MAX_KEYS`                     | `BTreeNode`                  | `BTreeNode`, `KeyValuePair`           |
| `create.h`       | `create.c`, `repl.c`           | `TYPE_INT`, `TYPE_STRING`      | `Column`                     | `ColumnNode`                          |
| `select.h`      | `select.c`, `repl.c`           | `MAX_SELECTED_COLUMNS`         | `ColumnValueNode`            | `ColumnValueNode`                     |
| `delete.h`       | `delete.c`, `repl.c`           | Aucun                          | Aucun                        | Aucun                                  |
| `insert.h`       | `insert.c`, `repl.c`           | Aucun                          | `ColumnValueNode`            | `ColumnValueNode`                     |

---

### Explications par fichier

#### **`repl.h`**
- **Macros utilisées** :
  - `MAX_LINE_LENGTH` : Taille maximale d’une ligne d’entrée.
  - `MAX_COLUMNS` : Nombre maximal de colonnes dans une commande.
- **Typedefs utilisés** :
  - `ColumnNode` : Liste chaînée pour représenter les colonnes.
- **Structs utilisées** :
  - `InputBuffer` : Gère les tampons d’entrée utilisateur.
  - `Statement` : Définit les commandes (type, table, colonnes, valeurs, etc.).

---

#### **`btree.h`**
- **Macros utilisées** :
  - `MAX_KEYS` : Nombre maximal de clés dans un nœud du B-Tree.
- **Typedefs utilisés** :
  - `BTreeNode` : Nœud de l’arbre B.
- **Structs utilisées** :
  - `BTreeNode` : Structure principale des nœuds d’un B-Tree.

---

#### **`create.h`**
- **Macros utilisées** :
  - `TYPE_INT` : Identifiant pour les colonnes de type entier.
  - `TYPE_STRING` : Identifiant pour les colonnes de type chaîne.
- **Typedefs utilisés** :
  - `Column` : Définit une colonne avec un nom et un type.
- **Structs utilisées** :
  - `ColumnNode` : Liste chaînée des colonnes pour la création d’une table.

---

#### **`select.h`**
- **Macros utilisées** :
  - `MAX_SELECTED_COLUMNS` : Nombre maximal de colonnes pouvant être sélectionnées.
- **Typedefs utilisés** :
  - `ColumnValueNode` : Représente une colonne et sa valeur dans une requête SELECT.
- **Structs utilisées** :
  - `ColumnValueNode` : Liste chaînée des colonnes et valeurs pour SELECT.

---

#### **`delete.h`**
- **Macros utilisées** :
  - Aucun.
- **Typedefs utilisés** :
  - Aucun.
- **Structs utilisées** :
  - Aucun.

---

#### **`insert.h`**
- **Macros utilisées** :
  - Aucun.
- **Typedefs utilisés** :
  - `ColumnValueNode` : Représente les colonnes et valeurs pour INSERT.
- **Structs utilisées** :
  - `ColumnValueNode` : Liste chaînée des colonnes et valeurs.

---

### Utilisation de ce Tableau
- **Débogage :** Repérez rapidement où les éléments sont utilisés et définis.
- **Organisation :** Identifiez les dépendances pour chaque fichier `.h` et `.c`.
- **Documentation :** Fournit une vue d’ensemble des structures et macros dans le projet.

## Description des Structs

---

#### **`InputBuffer`**
**Fichier :** `repl.h`  
**Utilisation :** Gestion des entrées utilisateur dans la boucle REPL.

```c
typedef struct {
    char* buffer;  // Tampon pour stocker la commande utilisateur
    size_t buffer_length;  // Taille allouée pour le tampon
    ssize_t input_length;  // Longueur actuelle de la commande
} InputBuffer;
```

**Rôle :**
- Stocke l’entrée utilisateur pour analyse et exécution.
- Dynamique : utilise un tampon de taille flexible.

---

#### **`Statement`**
**Fichier :** `repl.h`  
**Utilisation :** Représente une commande SQL-like préparée.

```c
typedef struct {
    StatementType type;  // Type de commande (e.g., CREATE, SELECT, DELETE)
    char table_name[MAX_COLUMNS];  // Nom de la table ciblée
    ColumnNode* columns_head;  // Liste chaînée des colonnes (utilisée dans CREATE)
    int column_count;  // Nombre de colonnes
    char values[MAX_COLUMNS][MAX_VALUE_LEN];  // Valeurs associées à la commande
    char formatted_values[MAX_LINE_LENGTH];  // Valeurs formatées pour insertion
} Statement;
```

**Rôle :**
- Organise les informations nécessaires pour exécuter une commande (table, colonnes, valeurs).
- Prend en charge des commandes complexes comme CREATE ou INSERT.

---

#### **`Column`**
**Fichier :** `create.h`  
**Utilisation :** Définit une colonne dans une table.

```c
typedef struct {
    char name[50];  // Nom de la colonne
    int type;       // Type de la colonne (TYPE_INT ou TYPE_STRING, définis par des macros)
} Column;
```

**Macros associées :**
- `TYPE_INT` : Identifie une colonne de type entier.
- `TYPE_STRING` : Identifie une colonne de type chaîne de caractères.

**Rôle :**
- Décrit une colonne avec son nom et son type.
- Utilisée lors de la création d’une table.

---

#### **`ColumnNode`**
**Fichier :** `create.h`, `insert.h`  
**Utilisation :** Liste chaînée pour représenter plusieurs colonnes.

```c
typedef struct ColumnNode {
    char name[50];  // Nom de la colonne
    int type;       // Type de la colonne (TYPE_INT ou TYPE_STRING)
    struct ColumnNode* next;  // Pointeur vers la colonne suivante
} ColumnNode;
```

**Rôle :**
- Représente une série de colonnes, typiquement lors de la création ou de l’insertion.
- Chaînée pour permettre une extensibilité dynamique.

---

#### **`ColumnValueNode`**
**Fichier :** `select.h`, `insert.h`  
**Utilisation :** Associe une colonne à une valeur dans une commande INSERT ou SELECT.

```c
typedef struct ColumnValueNode {
    char column_name[50];  // Nom de la colonne
    char value[MAX_VALUE_LEN];  // Valeur associée
    struct ColumnValueNode* next;  // Pointeur vers le nœud suivant
} ColumnValueNode; 
```

**Rôle :**
- Utilisée pour stocker les colonnes et leurs valeurs lors de l’analyse et de l’exécution des commandes.
- Employée dans SELECT pour organiser les colonnes sélectionnées.

---

#### **`BTreeNode`**
**Fichier :** `btree.h`  
**Utilisation :** Représente un nœud de l’arbre B.

```c
typedef struct BTreeNode {
    int num_keys;                      // Nombre de clés dans le nœud
    int keys[MAX_KEYS];                // Tableau de clés
    char table_names[MAX_KEYS][50];    // Noms des tables associés aux clés
    struct BTreeNode* children[MAX_KEYS + 1];  // Pointeurs vers les enfants
    int is_leaf;                       // Indique si le nœud est une feuille
} BTreeNode;
```

**Macro associée :**
- `MAX_KEYS` : Nombre maximum de clés qu’un nœud peut contenir.

**Rôle :**
- Organise les données dans une structure hiérarchique pour un accès rapide.
- Gère les opérations de recherche, d’insertion et de division dans le B-Tree.

---

#### **`KeyValuePair`** (si définie)
**Fichier :** `btree.h` (optionnel)  
**Utilisation :** Représente une paire clé-valeur pour certaines opérations sur le B-Tree.

```c
typedef struct {
    int key;        // La clé
    char value[50]; // La valeur associée
} KeyValuePair;
```

**Rôle :**
- Utile pour associer une clé à une donnée lors des manipulations de l’arbre.

---

### Structs et leurs relations

- **REPL** :
  - Utilise `InputBuffer` pour lire les commandes.
  - Utilise `Statement` pour organiser les commandes analysées.
- **B-Tree** :
  - Utilise `BTreeNode` pour gérer les nœuds.
  - (Optionnel) Utilise `KeyValuePair` pour manipuler des paires clé-valeur.
- **Commandes CREATE, SELECT, INSERT** :
  - Dépendent fortement de `Column`, `ColumnNode`, et `ColumnValueNode` pour organiser les colonnes et valeurs.

---

##  **Persistance des Données**

Le système de persistance garantit que les données manipulées en mémoire par le programme sont sauvegardées durablement sur le disque dans un fichier nommé `database.csv`. Cela permet de retrouver l'état exact des tables et des données lors des redémarrages.

---

#### **Fonctionnement Général**
1. **Chargement des données (`load_from_disk`)** :
   - Lors du démarrage, le programme charge le contenu de `database.csv` dans la mémoire.
   - Les données sont structurées dans un B-tree et des structures associées pour permettre des manipulations rapides.

2. **Sauvegarde des données (`save_to_disk`)** :
   - Lors de la fermeture du programme ou après certaines commandes, le programme écrit l'état actuel du B-tree (et des données associées) dans `database.csv`.
   - Cela garantit la persistance des modifications effectuées pendant l'exécution.

---

#### **Structure du fichier `database.csv`**
Le fichier `database.csv` est utilisé pour stocker :
- La définition des tables (ID, nom, colonnes et types de colonnes).
- Les lignes insérées dans les tables.

Exemple de contenu :
```
#### Table::1: users
id::INT,name::STRING,email::STRING
1,'Alice','alice@example.com'
2,'Bob','bob@example.com'

#### Table::2: products
id::INT,name::STRING,price::INT
1,'Laptop',1000
2,'Phone',500
```

---

#### **Détails des Fonctions**

##### **1. `load_from_disk`**
- Cette fonction est appelée au début de `main` :
  ```c
  load_from_disk("database.csv", &btree_root);
  ```
- Elle ouvre `database.csv` en mode lecture et charge les données dans la structure en mémoire.
- Les tables sont ajoutées au B-tree, et leurs colonnes et données sont correctement restaurées.

##### **2. `save_to_disk`**
- Cette fonction est appelée à la fin de `main` :
  ```c
  save_to_disk("database.csv", btree_root);
  ```
- Elle ouvre `database.csv` en mode écriture (`w`) et écrit tout le contenu de la structure en mémoire (B-tree, tables et données) dans le fichier.
- Cela garantit que toutes les modifications apportées pendant l'exécution sont sauvegardées.

La fonction save_to_disk est appelée dans les commandes qui intéragissent en écriture avec database.csv :

- `STATEMENT_EXIT` : Sauvegarde lors de la fermeture.
- `STATEMENT_CREATETABLE` : Sauvegarde après la création d'une table.
- `STATEMENT_INSERT` : Sauvegarde après l'insertion d'une ligne.
- `STATEMENT_DELETETABLE` : Sauvegarde après la suppression d'une table.
Cela garantit que les modifications en mémoire sont persistées immédiatement dans le fichier database.csv.
---

#### **Le rôle de `main` dans la persistance**

Voici le rôle clé de `main` dans la persistance des données :
1. **Chargement au démarrage :**
   - `load_from_disk` initialise le B-tree (`btree_root`) et les structures associées à partir de `database.csv`.
2. **Lancement de la REPL :**
   - La boucle REPL (`repl`) permet à l'utilisateur d'interagir avec les données en mémoire.
3. **Sauvegarde à la fermeture :**
   - `save_to_disk` écrit les données mises à jour dans `database.csv` avant la fin du programme.

---

#### **Cas de Test pour la Persistance**
1. **Création de tables et insertion de données :**
   - Vérifiez que les données apparaissent correctement dans `database.csv`.
   ```sql
   create users (id::INT, name::STRING, email::STRING);
   insert into users (id, name, email) values (1, 'Alice', 'alice@example.com');
   ```

2. **Redémarrage du programme :**
   - Après un redémarrage, vérifiez que les données sont rechargées et manipulables :
   ```sql
   select * from users;
   ```

3. **Modification et sauvegarde :**
   - Supprimez ou insérez des données, puis assurez-vous qu'elles sont sauvegardées dans `database.csv`.

---
