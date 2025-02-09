#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include "je.h"


/* ***************************************************************************
* CONSTANTS
*/

#define _PAGESIZE 4096


/* ***************************************************************************
* USAGE
*/

void usage() {
    printf("%s\n",
"Run a JsonExpr script.\n"
"\n"
"Usage: {SCRIPTNAME} [OPTIONS] [FILES]\n"
"\n"
"Options:\n"
"  FILE                  File(s) to run.\n"
"\n"
"  -e,--eval EXPR        Evaluate EXPR.\n"
"  -t,--tree             Display the syntax tree.\n"
"  -q,--quiet            Do not output the result.\n"
"  -v,--version          Display version and quit.\n"
    );
}

void version() {
    printf("jsonexpr %s\n", JE_VERSION);
}


/* ***************************************************************************
* GLOBALS
*/

int doMyFdThing(int fd);
int doMyCodeThing(const char* code);

char* evalv[256];
int evalc = 0;
int quiet = 0;
int tree = 0;


/* ***************************************************************************
* PROGRAM BEGINS HERE
*/

int main(int argc, char* argv[]) {
    int errcount = 0;

    /* Process options */
    static struct option long_options[] = {
        {"eval"   , required_argument, 0, 'e'},
        {"tree"   , no_argument      , 0, 't'},
        {"quiet"  , no_argument      , 0, 'q'},
        {"version", no_argument      , 0, 'v'},
        {"help"   , no_argument      , 0, 'h'},
        {0, 0, 0, 0}
    };

    while(1) {
        int option_index = 0;
        int c = 0;

        c = getopt_long(argc, argv, "e:tqvh", long_options, &option_index);
        if(c == -1) break;

        switch(c) {
            case 'e' : evalv[evalc++] = optarg; break;
            case 't' : tree = 1; break;
            case 'q' : quiet = 1; break;
            case 'v' : version(); exit(0); break;
            case 'h' : usage(); exit(0); break;
            default  : errcount++; break;
        }

        if(evalc == sizeof(evalv)/sizeof(char*)) {
            fprintf(stderr, "Too many -e or --eval\n");
            errcount++;
            evalc--;
        }
    }

    /* Sanity check */
    if(errcount) {
        fprintf(stderr, "Type '%s --help' for help.\n", argv[0]);
        exit(1);
    }

    for(int i=0; i<evalc; i++) {
        doMyCodeThing(evalv[i]);
    }

    if(argc <= optind && evalc == 0) {
        doMyFdThing(0);
    }

    for(int i=optind; i<argc; i++) {
        int fd = open(argv[i], 0);

        if(fd < 0) {
            perror(argv[i]);
            continue;
        }

        doMyFdThing(fd);
        close(fd);
    }

    return 0;
}


int doMyFdThing(int fd) {
    char* code = calloc(1, _PAGESIZE);
    size_t offset = 0;
    int result = 0;

    while(1) {
        ssize_t size = read(fd, code+offset, _PAGESIZE);
        if(size <= 0) break;

        offset += size;
        code = realloc(code, offset + _PAGESIZE);
    }

    result = doMyCodeThing(code);
    free(code);

    return result;
}


int doMyCodeThing(const char* code) {
    JE_NODE* ast = JE_Parse(code);
    JE_VAL* result = JE_EvalByNode(ast, NULL);

    if(tree) {
        char* tree = JE_NodeTreeAstr(ast);

        fprintf(stderr, "%s\n", tree);
        free(tree);
    }

    if(!quiet) {
        printf("%s\n", JE_ValToQstr(result));
    }

    JE_ValDelete(result);
    JE_NodeDelete(ast);

    return 0;
}

/* vim:ft=c:
*/
