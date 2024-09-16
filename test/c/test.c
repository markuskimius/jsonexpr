#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "je/je.h"


/* ***************************************************************************
* CONSTANTS
*/

#define PAGESIZE 4096


/* ***************************************************************************
* FORWARD DECLARATION
*/

int doMyFdThing(int fd);
int doMyCodeThing(const char* code);


/* ***************************************************************************
* PROGRAM BEGINS HERE
*/

int main(int argc, const char* argv[]) {
    int last = 0;

    if(argc <= 1) last = doMyFdThing(0);

    for(int i=1; i<argc; i++) {
        int fd = open(argv[i], 0);

        if(fd < 0) {
            perror(argv[i]);
            continue;
        }

        last = doMyFdThing(fd);
        close(fd);
    }

    return last;
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
    JE_VAL* result = je_eval(ast, NULL);

    /*
    char* tree = je_nodetree(ast);
    printf("%s\n", tree);
    free(tree);
    */

    // printf("%s\n", je_valqstr(result));

    je_freeval(result);
    je_freenode(ast);

    return 0;
}