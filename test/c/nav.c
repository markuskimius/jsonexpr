#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "je/je.h"
#include "je/je_nav.h"
#include "je/je_util.h"
#include "je/je_token.h"


/* ***************************************************************************
* CONSTANTS
*/

#define PAGESIZE 4096


/* ***************************************************************************
* GLOBALS
*/

char commands[1024];


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

int doMyFdThing(int fd);
int doMyFileThing(const char* file);
int doMyCodeThing(const char* code);


/* ***************************************************************************
* PROGRAM BEGINS HERE
*/

int main(int argc, char* argv[]) {
    int errcount = 0;

    /* Process options */
    static struct option long_options[] = {
        {"enter"  , no_argument      , 0, 'e'},
        {"exit"   , no_argument      , 0, 'x'},
        {"next"   , no_argument      , 0, 'n'},
        {"prev"   , no_argument      , 0, 'p'},
//        {"remove" , no_argument      , 0, 'r'},
        {"output" , no_argument      , 0, 'o'},
        {"tree"   , no_argument      , 0, 't'},
        {"navtree", no_argument      , 0, 'v'},
        {"eval"   , no_argument      , 0, 'l'},
        {"Eval"   , no_argument      , 0, 'L'},
        {0, 0, 0, 0}
    };

    while(1) {
        int option_index = 0;
        int c = 0;

        c = getopt_long(argc, argv, "exnprotvlL", long_options, &option_index);
        if(c == -1) break;

        switch(c) {
            case 'e' : strcat(commands, "e"); break;
            case 'x' : strcat(commands, "x"); break;
            case 'n' : strcat(commands, "n"); break;
            case 'p' : strcat(commands, "p"); break;
//            case 'r' : strcat(commands, "r"); break;
            case 'o' : strcat(commands, "o"); break;
            case 't' : strcat(commands, "t"); break;
            case 'v' : strcat(commands, "v"); break;
            case 'l' : strcat(commands, "l"); break;
            case 'L' : strcat(commands, "L"); break;
            default  : errcount++;            break;
        }
    }

    if(errcount) {
        fprintf(stderr, "Usage: %s [-exnprotvlL] <file.json>\n", argv[0]);
        exit(1);
    }

    for(int i=optind; i<argc; i++) {
        doMyFileThing(argv[i]);
    }

    return 0;
}


int doMyFileThing(const char* file) {
    int fd = open(file, 0);
    int result = 0;

    if(fd < 0) {
        perror(file);
        return 1;
    }

    result = doMyFdThing(fd);
    close(fd);

    return result;
}


int doMyFdThing(int fd) {
    char* code = calloc(1, PAGESIZE);
    size_t offset = 0;
    int result = 0;

    while(1) {
        ssize_t size = read(fd, code+offset, PAGESIZE);
        if(size <= 0) break;

        offset += size;
        code = realloc(code, offset + PAGESIZE);
    }

    result = doMyCodeThing(code);
    free(code);

    return result;
}


void output(JE_NODE* ast) {
    char* text = je_astrtoken(ast->head, NULL);

    printf("%s", text);
    free(text);
}


void tree(JE_NODE* ast) {
    char* nodetree = je_nodetree(ast);

    fprintf(stderr, "%s\n", nodetree);

    free(nodetree);
}


void navtree(JE_NAV* nav) {
    char* navtree = je_navnodetree(nav->root);

    fprintf(stderr, "%s\n", navtree);

    free(navtree);
}


void eval(JE_NODE* ast, int output) {
    JE_VAL* result = je_eval(ast, NULL);

    if(output) {
        printf("%s\n", je_valqstr(result));
    }

    je_freeval(result);
}


int doMyCodeThing(const char* code) {
    JE_NODE* ast = je_parse(code);
    JE_NAV* nav = je_newnav(ast);
    char* cp = commands;

    while(*cp) {
        switch(*cp) {
            case 'e' : je_naventer(nav); break;
            case 'x' : je_navexit(nav); break;
            case 'n' : je_navnext(nav); break;
            case 'p' : je_navprev(nav); break;
//            case 'r' : je_navremove(nav); break;
            case 'o' : output(ast); break;
            case 't' : tree(ast); break;
            case 'v' : navtree(nav); break;
            case 'l' : eval(ast, 0); break;
            case 'L' : eval(ast, 1); break;
            default  : fprintf(stderr, "Invalid command: %c\n", *cp);
        }

        cp++;
    }

    je_freenav(nav);
    je_freenode(ast);

    return 0;
}

/* vim:ft=c:
*/
