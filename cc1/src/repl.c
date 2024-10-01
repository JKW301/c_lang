#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // pour ssize_t
#include <stddef.h>
InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void print_prompt() {
    printf("\033[35mdb > \033[0m");  // \033[35m pour magenta, \033[0m pour réinitialiser la couleur
}


void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = '\0';  // Remplacer le caractère de nouvelle ligne
}

void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        close_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "create_table", 12) == 0) {
        statement->type = STATEMENT_CREATETABLE;
        // Extraire le nom de la table après "create table"
        sscanf(input_buffer->buffer + 13, "%s", statement->table_name);
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "view_tables") == 0) {
        statement->type = STATEMENT_VIEWTABLES;
        return PREPARE_SUCCESS;
    }
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        
        // Supposons que la commande est au format "insert nom_table valeur1 valeur2 valeur3"
        // Vous pouvez ajuster le nombre de valeurs selon votre design
        char* tokens = strtok(input_buffer->buffer, " ");
        tokens = strtok(NULL, " ");  // Nom de la table
        strcpy(statement->table_name, tokens);
        
        int i = 0;
        while (tokens != NULL && i < MAX_VALUES) {
            tokens = strtok(NULL, " ");  // Extraire les valeurs
            if (tokens != NULL) {
                strcpy(statement->values[i], tokens);
                i++;
            }
        }

        return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        // Extraction du nom de la table après "select "
        if (sscanf(input_buffer->buffer, "select %s", statement->table_name) == 1) {
            return PREPARE_SUCCESS;
        } else {
            return PREPARE_UNRECOGNIZED_STATEMENT;
        }
    }
    if (strcmp(input_buffer->buffer, "exit") == 0) {
        statement->type = STATEMENT_EXIT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement) {
    static TableList* table_list = NULL;
    static int table_counter = 1; // Compteur pour les tables

    if (table_list == NULL) {
        table_list = create_table_list();
        load_table_list_from_file(table_list, "tables.txt");

        // Mettre à jour le compteur pour éviter les doublons
        Table* current = table_list->head;
        while (current != NULL) {
            if (current->table_number >= table_counter) {
                table_counter = current->table_number + 1;
            }
            current = current->next;
        }
    }

    switch (statement->type) {
        case STATEMENT_CREATETABLE:
            add_table(table_list, table_counter++, statement->table_name);
            printf("Table '%s' created.\n", statement->table_name);
            save_table_list_to_file(table_list, "tables.txt"); // Sauvegarder après création
            break;

        case STATEMENT_VIEWTABLES:
            if (table_list->head == NULL) {
                printf("No tables created yet.\n");
            } else {
                print_table_list(table_list);
            }
            break;

        case STATEMENT_INSERT: 
            // Exemple de données à insérer
            execute_insert(statement, table_list, statement->values);
            break;

        case STATEMENT_SELECT:
            execute_select(statement, table_list);
            break;

        case STATEMENT_EXIT:
            printf("Goodbye!\n");
            close_input_buffer(new_input_buffer());
            exit(EXIT_SUCCESS);
            break;

        default:
            printf("Unrecognized command.\n");
            break;
        }
    }

// void execute_insert(Statement* statement, TableList* table_list, char data[MAX_VALUES][255]) {
//     // Chercher la table par nom
//     struct Table* table = table_list->head;
//     while (table != NULL && strcmp(table->table_name, statement->table_name) != 0) {
//         table = table->suivante;
//     }
//     if (table == NULL) {
//         printf("Table %s non trouvée.\n", statement->table_name);
//         return;
//     }

//     // Parcourir les colonnes et insérer les valeurs correspondantes
//     struct Colonne* colonne = table->colonnes;
//     int i = 0;
//     while (colonne != NULL && i < MAX_VALUES) {
//         struct Valeur* nouvelle_valeur = (struct Valeur*)malloc(sizeof(struct Valeur));
//         strcpy(nouvelle_valeur->data, data[i]);  // Assigner la valeur fournie
//         nouvelle_valeur->suivant = colonne->valeurs;  // Insérer au début de la liste chaînée
//         colonne->valeurs = nouvelle_valeur;  // Mise à jour de la colonne
//         colonne = colonne->suivante;
//         i++;
//     }
//     printf("Données insérées dans la table %s.\n", statement->table_name);
// }

void execute_insert(Statement* statement, TableList* table_list, char data[MAX_VALUES][255]) {
    // Chercher la table par nom
    Table* table = table_list->head;
    while (table != NULL && strcmp(table->table_name, statement->table_name) != 0) {
        table = table->suivante;
    }
    if (table == NULL) {
        printf("Table %s non trouvée.\n", statement->table_name);
        return;
    }

    // Si la table n'a pas encore de colonnes, en créer
    if (table->colonnes == NULL) {
        // Créer des colonnes par défaut pour chaque valeur insérée
        for (int i = 0; i < MAX_VALUES && strlen(data[i]) > 0; i++) {
            struct Colonne* nouvelle_colonne = (struct Colonne*)malloc(sizeof(struct Colonne));
            sprintf(nouvelle_colonne->nom, "colonne%d", i + 1);  // Nommez les colonnes colonne1, colonne2, etc.
            nouvelle_colonne->valeurs = NULL;
            nouvelle_colonne->suivante = table->colonnes;
            table->colonnes = nouvelle_colonne;  // Ajout de la colonne à la table
        }
    }

    // Parcourir les colonnes et insérer les valeurs correspondantes
    struct Colonne* colonne = table->colonnes;
    int i = 0;
    while (colonne != NULL && i < MAX_VALUES && strlen(data[i]) > 0) {
        struct Valeur* nouvelle_valeur = (struct Valeur*)malloc(sizeof(struct Valeur));
        strcpy(nouvelle_valeur->data, data[i]);  // Assigner la valeur fournie
        nouvelle_valeur->suivant = colonne->valeurs;  // Insérer au début de la liste chaînée
        colonne->valeurs = nouvelle_valeur;  // Mise à jour de la colonne
        colonne = colonne->suivante;
        i++;
    }
    printf("Données insérées dans la table %s.\n", statement->table_name);
}


void execute_select(Statement* statement, TableList* table_list) {
    // Chercher la table par nom
    Table* table = table_list->head;
    while (table != NULL && strcmp(table->table_name, statement->table_name) != 0) {
        table = table->suivante;
    }

    // Si la table n'existe pas, afficher un message d'erreur
    if (table == NULL) {
        printf("Table %s non trouvée.\n", statement->table_name);
        return;
    }

    // Parcourir les colonnes et afficher les valeurs
    struct Colonne* colonne = table->colonnes;
    if (colonne == NULL) {
        printf("La table %s n'a pas de colonnes.\n", table->table_name);
        return;
    }

    // Afficher les colonnes et leurs valeurs
    while (colonne != NULL) {
        printf("Colonne %s : ", colonne->nom);
        struct Valeur* valeur = colonne->valeurs;
        if (valeur == NULL) {
            printf("Aucune donnée dans cette colonne.\n");
        } else {
            while (valeur != NULL) {
                printf("%s -> ", valeur->data);
                valeur = valeur->suivant;
            }
            printf("NULL\n");
        }
        colonne = colonne->suivante;
    }
}


// Créer une nouvelle liste de tables
TableList* create_table_list() {
    TableList* list = (TableList*)malloc(sizeof(TableList));
    list->head = NULL;
    return list;
}

// Ajouter une nouvelle table à la liste
void add_table(TableList* list, int table_number, const char* table_name) {
    Table* new_table = (Table*)malloc(sizeof(Table));
    new_table->table_number = table_number;
    strncpy(new_table->table_name, table_name, sizeof(new_table->table_name) - 1);
    new_table->table_name[sizeof(new_table->table_name) - 1] = '\0';
    new_table->next = list->head;
    list->head = new_table;
}

// Sauvegarder la liste de tables dans un fichier
// void save_table_list_to_file(TableList* list, const char* filename) {
//     FILE* file = fopen(filename, "w");
//     if (!file) {
//         printf("Error opening file %s\n", filename);
//         return;
//     }
//     Table* current = list->head;
//     while (current != NULL) {
//         fprintf(file, "%d %s\n", current->table_number, current->table_name);
//         current = current->next;
//     }
//     fclose(file);
//     printf("Table list saved to %s\n", filename);
// }

// Fonction pour sauvegarder les tables, colonnes et valeurs dans un fichier
void save_table_list_to_file(TableList* list, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file %s\n", filename);
        return;
    }

    // Parcourir la liste des tables
    struct Table* current_table = list->head;
    while (current_table != NULL) {
        // Sauvegarder le numéro et le nom de la table
        fprintf(file, "Table %d: %s\n", current_table->table_number, current_table->table_name);

        // Parcourir les colonnes de la table
        struct Colonne* current_colonne = current_table->colonnes;
        while (current_colonne != NULL) {
            // Sauvegarder le nom de la colonne
            fprintf(file, "  Colonne: %s\n", current_colonne->nom);

            // Parcourir les valeurs de la colonne
            struct Valeur* current_valeur = current_colonne->valeurs;
            fprintf(file, "    Valeurs: ");
            while (current_valeur != NULL) {
                fprintf(file, "%s ", current_valeur->data);
                current_valeur = current_valeur->suivant;
            }
            fprintf(file, "\n");

            current_colonne = current_colonne->suivante;
        }

        fprintf(file, "\n");  // Saut de ligne entre les tables
        current_table = current_table->suivante;
    }

    fclose(file);
    printf("Table list with columns and values saved to %s\n", filename);
}

void load_table_list_from_file(TableList* list, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("No existing table file found, starting with an empty list.\n");
        return;
    }
    int table_number;
    char table_name[255];
    while (fscanf(file, "%d %s", &table_number, table_name) != EOF) {
        add_table(list, table_number, table_name);
    }
    fclose(file);
}

/*
void print_table_list(TableList* list) {
    Table* current = list->head;
    while (current != NULL) {
        printf("Table %d: %s\n", current->table_number, current->table_name);
        current = current->next;
    }
}
*/
void print_table_list(TableList* list) {
    Table* current = list->head;
    while (current != NULL) {
        print_table_in_frame(current->table_name);  // Encadrer chaque nom de table
        current = current->next;
    }
}

void print_table_in_frame(const char* table_name) {
    int length = strlen(table_name);
    
    // Affichage de la ligne supérieure
    printf("+");
    for (int i = 0; i < length + 2; i++) {
        printf("-");
    }
    printf("+\n");

    // Affichage du contenu (nom de la table)
    printf("| %s |\n", table_name);

    // Affichage de la ligne inférieure
    printf("+");
    for (int i = 0; i < length + 2; i++) {
        printf("-");
    }
    printf("+\n");
}



void repl() {
    InputBuffer* input_buffer = new_input_buffer();


    while (1) {
        print_prompt();
        read_input(input_buffer);
        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer)) {
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }
        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }
        execute_statement(&statement);
        printf("Executed.\n");
    }
}
