#include <stdio.h>
#include <stdlib.h>
#include "lex_analysis.h"
#include "parser.h"
#include "execution.h"
#include "free.h"  


int main(void) {
	int count;
	struct lexem * array = NULL;
	array = lexical_analysis(&count);
    if (array == NULL) {
        return 0;
    }
    struct list * root = parser(array, &count);
    free_array(array, &count);
    if (root == NULL) {
        exit(1);
    } 
    execution(root, root);
} 