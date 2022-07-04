#include "lex_analysis.h"
#include "parser.h"
#include "execution.h"
#include "free.h"
#include "free.h"
#include <unistd.h>
#include <stdlib.h>



void free_array(struct lexem * array, int * count) {
    if (array == NULL)
        return;
    int i, j;
    for (i = 0; i < *count; i++) {
        j = 0;
        if (array[i].argv != NULL) {
            while (array[i].argv[j] != NULL) {
                free(array[i].argv[j]);
                j++;
            }
            free(array[i].argv);
        }  
    }    
    free(array);
}

void free_root(struct list * root) {
    if (root == NULL) {
        return;
    }
    if (root->psubcmd != NULL) {
        free_root(root->psubcmd);
    }
    if (root->and != NULL) {
        free_root(root->and);
    }
    if (root->next != NULL) {
        free_root(root->next);
    }
    if (root->pipe != NULL) {
        free_root(root->pipe);
    }
    free(root->input);
    free(root->output);
    free(root->output_end);
    int j;
    if (root->argv != NULL) {
        j = 0;
        while (root->argv[j] != NULL) {
            free(root->argv[j]);
            j++;
        }
        free(root->argv);
    }
    free(root);
}