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


/* ***************************************************************************
* CONSTANTS
*/

#define PAGESIZE 4096


/* ***************************************************************************
* GLOBALS
*/

int tree = 0;
char* files[1024];


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
        {"tree"   , no_argument      , 0, 't'},
        {0, 0, 0, 0}
    };

    while(1) {
        int option_index = 0;
        int c = 0;

        c = getopt_long(argc, argv, "t", long_options, &option_index);
        if(c == -1) break;

        switch(c) {
            case 't' : tree = 1;   break;
            default  : errcount++; break;
        }
    }

    if(errcount) {
        fprintf(stderr, "Usage: %s [-t] <file.json>\n", argv[0]);
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


int doMyCodeThing(const char* code) {
    JE_NODE* ast = je_parse(code);
    JE_NAV* nav = je_newnav(ast);
    JE_VAL* result = je_eval(ast, NULL);

    if(tree) {
        char* nodetree = je_nodetree(ast);
        char* navtree = je_navnodetree(nav->root);

        fprintf(stderr, "%s\n", nodetree);
        fprintf(stderr, "%s\n", navtree);

        free(nodetree);
        free(navtree);
    }

    je_freeval(result);
    je_freenav(nav);
    je_freenode(ast);

    return 0;
}


int doMyCodeThingMultiCatTest(const char* code) {
    const char* s1 = "Hello\nworld!";
    const char* s2 = "Bye,\ncruel world!";

    char* text1 = je_amcat(s1, " -> ");
    char* text2 = je_amcat(text1, s2);

    printf("%s\n", text2);

    free(text1);
    free(text2);

    return 0;
}


int doMyCodeThingIteratorTest(const char* code) {
    JE_LINE_ITER* iter = je_newlineiter("Hello\nworld!\nBye!");
    const char* line = NULL;

    for(int i=0; i<5; i++) {
        const char* line = je_nextline(iter);

        printf("(%s)\n", line);
    }

    je_freelineiter(iter);

    return 0;
}

/* vim:ft=c:
*/
