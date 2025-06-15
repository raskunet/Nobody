#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

typedef struct CONFIG_LINE {
    char* name;
    union {
        int* iptr;
        double *dptr;
    } ptr;
} config_line;

extern int line;
extern char* fileDat;
extern int fileSize;
extern config_line properties[];

char* trimLine(char*);
int readConfig(char*);

#endif /* config_parser.h */