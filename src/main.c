#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "eval.h"
#include "node.h"
#include "map.h"
#include "util.h"
#include "parse.h"
#include "value.h"
#include "vector.h"


/* ***************************************************************************
* CONSTANTS
*/

#define PAGESIZE 4096


/* ***************************************************************************
* FORWARD DECLARATION
*/

/* declaring a function that does not exist in wasm as weak import allows wasm to compile */
__attribute__((weak, import_module("env"), import_name("open"))) extern int open(const char *pathname, int flags);
__attribute__((weak, import_module("env"), import_name("close"))) extern int close(int fd);

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
    NODE* ast = parse(code);

    /*
    if(1) {
        char* tree = nodetree(ast);
        printf("%s\n", tree);
        free(tree);
    }
    */

    if(1) {
        VALUE* result = eval(ast, NULL);
        printf("%s\n", strencoded(result));
        freevalue(result);
    }

    return 0;
}
