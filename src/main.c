#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
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
"Run a jsonexpr script.\n"
"\n"
"Usage: {SCRIPTNAME} [OPTIONS] [FILES]\n"
"\n"
"Options:\n"
"  FILE                  File(s) to evaluate.\n"
"\n"
"  -e,--eval EXPR        Evaluate EXPR.\n"
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


/* ***************************************************************************
* PROGRAM BEGINS HERE
*/

int main(int argc, char* argv[]) {
    int errcount = 0;

    /* Process options */
    static struct option long_options[] = {
        {"eval"   , required_argument, 0, 'e'},
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
    char* code = NULL;
    size_t offset = 0;
    size_t size = 0;
    int result = 0;

    while(1) {
        code = realloc(code, _PAGESIZE);
        if(!code) {
            perror("doMyFdThing");
            exit(1);
        }

        size = read(fd, code+offset, _PAGESIZE);
        if(size <= 0) break;

        offset += size;
    }

    result = doMyCodeThing(code);
    free(code);

    return result;
}

int doMyCodeThing(const char* code) {
    JE_AST ast = JE_Parse(code);

    if(ast.isok) {
        JE_MAP* symmap = JE_SymmapCreate(JE_SYMBOLS_DEFAULT);
        JE_VALUE result = JE_AstEval(ast, symmap);

        if(!quiet) {
            char* cstr = JE_VCALL(&result,toQstr);

            printf("%s\n", cstr);
            free(cstr);
        }

        JE_VCALL(&result,destroy);
        JE_SymmapDestroy(symmap);
    }

    JE_AstDestroy(&ast);

    return 0;
}

/* vim:ft=c:
*/
