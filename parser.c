#include "lex_analysis.h"
#include "parser.h"
#include "execution.h"
#include "free.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


struct list * parser(struct lexem * array, int * count) {
	int  pos = 0;
    struct list * p = NULL;
    p = ampersand(p, array, &pos, count);
    if (p == NULL)
        return NULL;
    if (pos != *count) {
        free_root(p);
        free_array(array, count);
    	fprintf(stderr, "%s\n", "Error\n");
    	exit(1);
    }
    return p;
}

struct list * ampersand(struct list * p, struct lexem * array, int * pos, int * count) {
    p = conjunction(p, array, pos, count);
    if (p == NULL)
        return NULL;
    struct list * q = p;
    while (*pos < *count) {
    	if (array[*pos].type != AMPERSAND) 
    	    break;
    	q->ampersand = 1;
    	(*pos)++;
        if (*pos == *count || array[*pos].type == CLOSE_BRACKET) {
            q->next = NULL;
            break;
        }
        else
    	    q->next = conjunction(q->next, array, pos, count);
        q = q->next;
        if (q == NULL) {
            free_root(p);
            return NULL;
        }
    }
    return p;
}

struct list * conjunction(struct list * p, struct lexem * array, int * pos, int * count) {
	p = conveyor(p, array, pos, count);
    if (p == NULL)
        return NULL;
    struct list * q = p;
	while (*pos < *count) {
		if(array[*pos].type != CONJUNCTION)
			break;
		(*pos)++;
		q->and = conveyor(q->and, array, pos, count);
        q = q->and;
        if (q == NULL) {
            free_root(p);
            return NULL;
        }
	}
	return p;
}

struct list * conveyor(struct list * p, struct lexem * array, int * pos, int * count) {
	p = input_and_output(p, array, pos, count);
    if (p == NULL)
        return NULL;
    struct list * q = p;
	while (*pos < *count) {
		if (array[*pos].type != CONVEYOR) 
			break;
        (*pos)++;
        q->pipe = input_and_output(q->pipe, array, pos, count);
        q = q->pipe;
        if (q == NULL) {
            free_root(p);
            return NULL;
        }
    }
    return p;
}

struct list * input_and_output(struct list * p, struct lexem * array, int * pos, int *count) {
	p = new_list(p, array, pos, count, 0);
    if (p == NULL)
        return NULL;
    while (*pos < *count) {
    	if (array[*pos].type != INPUT && array[*pos].type != OUTPUT && array[*pos].type != OUTPUT_END) 
            break;
        (*pos)++;
        p = new_list(p, array, pos, count, array[(*pos)-1].type);
        if (p == NULL)
            return NULL;
    }
    return p;
}

struct list * new_list(struct list * p, struct lexem * array, int * pos, int * count, int file) {
    if (*pos == *count) {
        free_root(p);
        return NULL;
    	fprintf(stderr, "%s\n", "Error\n");
    	exit(1);
    }

    if (p == NULL) {
        p = malloc(sizeof(struct list));
        p->argv = NULL;
        p->input = NULL;
        p->output = NULL;
        p->output_end = NULL;
        p->ampersand = 0;
        p->psubcmd = NULL;
        p->pipe = NULL;
        p->and = NULL;
        p->next = NULL;
        p->dec_input = 0;
        p->dec_output = 1;
    }    
    
    if (array[*pos].type == OPEN_BRACKET) {
    	(*pos)++;
    	p->psubcmd = ampersand(p->psubcmd, array, pos, count);
        if (p->psubcmd == NULL) {
            free_root(p);
            return NULL;
        }
    	if (*pos < *count && array[*pos].type == CLOSE_BRACKET) {
    		(*pos)++;
    		return p;
    	}
        free_root(p);
    	fprintf(stderr, "%s\n", "Error\n");
    	return NULL;
    }
    
    if (array[*pos].type != UTILITY) {
        free_root(p);
    	fprintf(stderr, "%s\n", "Error\n");
    	return NULL;
    }
    
    int j = 0;

    if (file == INPUT) {
        p->input = malloc(strlen(array[*pos].argv[j]) + 1);
        strcpy(p->input, array[*pos].argv[j]);
        j++;
        if (array[*pos].argv[j] != NULL) {
            free_root(p);
        	fprintf(stderr, "%s\n", "Error\n");
    	    return NULL;
        }
        (*pos)++;
        return p;
    } 

    if (file == OUTPUT) {
        p->output = malloc(strlen(array[*pos].argv[j]) + 1);
        strcpy(p->output, array[*pos].argv[j]);
        j++;
        if (array[*pos].argv[j] != NULL) {
            free_root(p);
        	fprintf(stderr, "%s\n", "Error\n");
    	    return NULL;
        }
        (*pos)++;
        return p;
    } 

    if (file == OUTPUT_END) {
        p->output_end = malloc(strlen(array[*pos].argv[j]) + 1);
        strcpy(p->output_end, array[*pos].argv[j]);
        j++;
        if (array[*pos].argv[j] != NULL) {
            free_root(p);
        	fprintf(stderr, "%s\n", "Error\n");
    	    return NULL;
        }
        (*pos)++;
        return p;
    } 

  
    while (array[*pos].argv[j] != NULL) {
        if (p->argv == NULL) {
            p->argv = malloc(sizeof(char *));
        } 
        else {
            p->argv = realloc(p->argv, (j+1) * sizeof(char *));
        }
        p->argv[j] = malloc(strlen(array[*pos].argv[j]) + 1);
        strcpy(p->argv[j], array[*pos].argv[j]);
        j++;
    }
    p->argv = realloc(p->argv, (j+1) * sizeof(char *));
    p->argv[j] = NULL;
    (*pos)++;
    return p;
}