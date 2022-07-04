#include "lex_analysis.h"
#include "parser.h"
#include "execution.h"
#include "free.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>




struct lexem * lexical_analysis(int * count) {
	struct lexem * array = NULL;
	int c, i = -1, j = -1, k = -1;
	c = getchar();
	while (c != EOF) {
		if (isspace(c)) {
			c = getchar();
			continue;
		}
		i++;
		if (i == 0)
			array = malloc(sizeof(struct lexem));
		else
			array = realloc(array, (i+1)*sizeof(struct lexem));
        
        array[i].argv = NULL;
        if (c == '(') {
        	array[i].type = OPEN_BRACKET;
			c = getchar();
			continue;
        }
        
        if (c == ')') {
        	array[i].type = CLOSE_BRACKET;
			c = getchar();
			continue;
        }

		if (c == '|') {
			array[i].type = CONVEYOR;
			c = getchar();
			continue;
		}
        
        if (c == '<') {
			array[i].type = INPUT;
			c = getchar();
			continue;
        }

        if (c == '>') {
        	c = getchar();
        	if (c == '>') {
			    array[i].type = OUTPUT_END;
			    c = getchar();
        	}
			else
				array[i].type = OUTPUT;
			continue;
        }

        if (c == '&') {
       	    c = getchar();
        	if (c == '&') {
			    array[i].type = CONJUNCTION;
			    c = getchar();
        	}
			else
				array[i].type = AMPERSAND;
			continue;
        }
        array[i].type = UTILITY;
        j = -1;
        do {
        	j++;
        	k = -1;
        	if (j == 0) {
        		array[i].argv = malloc(sizeof(char *));
        	}
        	else {
        		array[i].argv = realloc(array[i].argv, (j+1)*sizeof(char *));
        	}
            do {
            	k++;
            	if (k == 0)
            		array[i].argv[j] = malloc(sizeof(char));
            	else
            	    array[i].argv[j] = realloc(array[i].argv[j], (k+1)*sizeof(char));
            	array[i].argv[j][k] = c;
            	c = getchar();
            } while(c != '|' && c != '>' && c != '<' && c != '&' && !isspace(c) && c != EOF && c != '(' && c != ')');
            k++;
            array[i].argv[j] = realloc(array[i].argv[j], (k+1)*sizeof(char));
            array[i].argv[j][k] = '\0';
            while (isspace(c))
            	c = getchar();

        } while (c != '|' && c != '>' && c != '<' && c != '&' && c != EOF && c != '(' && c != ')');
        j++;
        array[i].argv = realloc(array[i].argv, (j+1)*sizeof(char *));
        array[i].argv[j] = NULL;
    }
    *count = i+1;
    return array;
}