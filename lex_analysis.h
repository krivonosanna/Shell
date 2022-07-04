struct lexem {
	int type;
	char ** argv;
};

enum  lex {
	CONVEYOR,
	INPUT, 
	OUTPUT, 
	OUTPUT_END,
	AMPERSAND,
    CONJUNCTION,
	UTILITY,
	OPEN_BRACKET,
	CLOSE_BRACKET
};


struct lexem * lexical_analysis(int * count);
