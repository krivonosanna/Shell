struct list {
    char ** argv;
    char * input;
    char * output;
    char * output_end;
    int ampersand;
    struct list * psubcmd;
    struct list * pipe;
    struct list * and;
    struct list * next;
    int dec_input;
    int dec_output;
};


struct list * parser(struct lexem * array, int * count);
struct list * ampersand(struct list * p, struct lexem * array, int * pos, int * count);
struct list * conjunction(struct list * p, struct lexem * array, int * pos, int * count);
struct list *  conveyor(struct list * p, struct lexem * array, int * pos, int * count);
struct list *  input_and_output(struct list * p, struct lexem * array, int * pos, int *count);
struct list *  new_list(struct list * p, struct lexem * array, int * pos, int * count, int file);
