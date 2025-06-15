#include "config_parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

double G_CONST = 0.005;
double dt = 0.25;
int MAX_BODIES = 1 << 12;

static int array_size = 3;
config_line properties[] = {(config_line){"Bodies", .ptr.iptr = &MAX_BODIES},
                            (config_line){"g", .ptr.dptr = &G_CONST},
                            (config_line){"dt", .ptr.dptr = &dt}};

char *trimLine(char *line) {
    char *end;
    while (isspace((unsigned char)*line)) line++;
    if (*line == 0) return line;
    end = line + strlen(line) - 1;
    while (end > line && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return line;
}

int readConfig(char *path) {
    FILE *fp = fopen(path, "r");
    char buff[256];
    while (fgets(buff, 256, fp) != NULL) {
        buff[strcspn(buff, "\n")] = 0;
        // printf("%s\n", buff);
        char *trimmed = trimLine(buff);
        char *tok = strtok(trimmed, "=");
        char *property = strtok(NULL, "=");
        
        tok = trimLine(tok);
        property = trimLine(property);
        printf("%s=%s\n", tok, property);
        for (int i = 0; i < array_size; i++) {
            if (!strcmp(tok, properties[i].name)) {
                if (strstr(property, ".") != NULL) {
                    *(properties[i].ptr.dptr) = atof(property);
                }
                if (strstr(property, "<<") != NULL) {
                    int num, shift;
                    sscanf(property, "%d << %d", &num, &shift);
                    *(properties[i].ptr.iptr) = num << shift;
                }
            }
        }
    }
    //printf("%f %f %d", dt, G_CONST, MAX_BODIES);
    return 1;
}