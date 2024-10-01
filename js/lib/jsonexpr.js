/* ***************************************************************************
* GLOBALS
*/

const JE_WASM = fetch(new URL("./je.wasm", import.meta.url))
    .then((response) => response.arrayBuffer());


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

async function _eval(code, symbols=null) {
    const compiled = await compile(code);

    compiled.setSymbols(symbols);

    return compiled.eval();
}

async function evalurl(codeUrl, symbols=null) {
    const code = await fetch(codeUrl).then((response) => response.text());

    return await _eval(code, symbols);
}

async function compile(code) {
    const compiled = new Compiled(code);

    await compiled.promise;

    return compiled;
}

async function compileurl(codeUrl) {
    const code = await fetch(codeUrl).then((response) => response.text());

    return await compile(code);
}


/* ***************************************************************************
* HELPER CLASSES
*/

class Compiled {
    static rid = new RegExp("^[_a-zA-Z][_a-zA-Z0-9]*$", "m");

    constructor(code) {
        this.adapter = new Adapter();
        this.instance = null;
        this.symtbl = null;
        this.iface = null;
        this.wtree = null;
        this.wsymtbl = null;
        this.promise = JE_WASM
            .then((bytes) => WebAssembly.instantiate(bytes, {
                env : {
                    read  : (fd,buf,count) => this.adapter.read(fd,buf,count),
                    write : (fd,buf,count) => this.adapter.write(fd,buf,count),
                    _exit : (exitcode)     => this.adapter.exit(exitcode),
                },
            }))
            .then((result) => {
                const instance = result.instance;
                const iface = new Interface(instance);
                const wcode = iface.strdup(code);
                const wtree = instance.exports.je_parse(wcode);
                const wsymtbl = instance.exports.je_newtable(0);
                const wsymmap = instance.exports.je_getobject(instance.exports.je_tableget(wsymtbl,0));

                this.adapter.instance = instance;
                this.instance = instance;
                this.wsymtbl = wsymtbl;
                this.wtree = wtree;
                this.iface = iface;
                this.symtbl = new Proxy({}, new ObjectHandler(instance, wsymmap));

                this.iface.free(wcode);

                return instance;
            });
    }

    destructor(code) {
        this.promise
            .then(() => delete this.iface);
    }

    eval() {
        const wresult = this.instance.exports.je_eval(this.wtree, this.wsymtbl);
        const wquoted = this.instance.exports.je_valqstr(wresult);
        const quoted = this.iface.strat(wquoted);
        const result = json_parse(quoted, this.instance, wresult);

        this.instance.exports.je_freeval(wresult)

        return result;
    }
}

class Adapter {
    constructor() {
        this.instance = null;
        this.fdbuf = ["", "", ""]
    }

    read(fd, buf, count) {
        console.log(`read(${fd}, ${buf}, ${count})`);
        return 0;
    }

    write(fd, buf, count) {
        const buffer = new Uint8Array(this.instance.exports.memory.buffer, buf, count);
        const str = new TextDecoder().decode(buffer);
        const lines = str.split("\n");

        for(let i=0; i<lines.length-1; i++) {
            this.fdbuf[fd] += lines[i];
            console.log(this.fdbuf[fd]);
            this.fdbuf[fd] = "";
        }

        if(lines.length) {
            this.fdbuf[fd] += lines[lines.length-1];
        }

        return count;
    }

    exit(code) {
        throw new Exit(code);
    }
}

class Interface {
    constructor(instance) {
        this.instance = instance;
        this.wallocated = [];
    }

    destructor() {
        for(let waddr of this.wallocated) {
            this.instance.exports.free(waddr);
        }
    }

    strat(waddr) {
        const buffer = new Uint8Array(this.instance.exports.memory.buffer);
        let count = 0;
        let buf = null;
        let str = null;

        while(buffer[waddr+count] != 0) {
            count++;
        }

        buf = new Uint8Array(this.instance.exports.memory.buffer, waddr, count);
        str = new TextDecoder("utf-8").decode(buf);

        return str;
    }

    strdup(string) {
        const buffer = new Uint8Array(this.instance.exports.memory.buffer);
        const buf = new TextEncoder().encode(string);
        const waddr = this.instance.exports.calloc(1, buf.length+1);

        for(let i=0; i<buf.length; i++) {
            buffer[waddr+i] = buf[i];
        }

        this.wallocated.push(waddr);

        return waddr;
    }

    free(waddr) {
        const index = this.wallocated.indexOf(waddr);

        if(index >= 0) {
            this.wallocated.splice(index, 1);
        }

        this.instance.exports.free(waddr);
    }
}

class ObjectHandler {
    constructor(instance, wmap) {
        this.instance = instance;
        this.wmap = wmap;
        this.iface = new Interface(instance);
    }

    has(map, name) {
        const wname = this.iface.strdup(name);
        const wresult = this.instance.exports.je_mapget(this.wmap, wname);
        const result = wresult ? true : false;

        this.iface.free(wname);

        return result;
    }

    ownKeys(map) {
        const keys = [];
        let iter = this.wmap;

        while((iter = this.instance.exports.je_mapnext(iter))) {
            const wkey = this.instance.exports.je_mapkey(iter);
            const key = this.iface.strat(wkey);

            keys.push(key);
        }

        return keys;
    }

    getOwnPropertyDescriptor(k) {
        return {
            enumerable: true,
            configurable: true,
        };
    }

    get(map, name, receiver) {
        const wname = this.iface.strdup(name);
        const wresult = this.instance.exports.je_mapget(this.wmap, wname);
        const wquoted = this.instance.exports.je_valqstr(wresult);
        const quoted = this.iface.strat(wquoted);
        const result = json_parse(quoted);

        this.iface.free(wname);

        return this.#promote(result, wresult);
    }

    set(map, name, value, receiver) {
        const wname = this.iface.strdup(name);
        const wexpr = this.iface.strdup(json_encode(value));
        const wtree = this.instance.exports.je_parse(wexpr);
        const wresult = this.instance.exports.je_eval(wtree,0);

        this.instance.exports.je_mapset(this.wmap, wname, wresult);

        this.instance.exports.je_freenode(wtree);
        this.iface.free(wexpr);
        this.iface.free(wname);

        return this.#promote(value, wresult);
    }

    #promote(value, wvalue) {
        if(value instanceof ObjectHandler)     { /* Pass */ }
        else if(value instanceof ArrayHandler) { /* Pass */ }
        else if(value instanceof Array) {
            const warray = this.instance.exports.je_getarray(wvalue);

            value = new Proxy(value, new ArrayHandler(this.instance, warray));
        }
        else if(value instanceof Object) {
            const wobject = this.instance.exports.je_getobject(wvalue);

            value = new Proxy(value, new ObjectHandler(this.instance, wobject));
        }

        return value;
    }
}

class ArrayHandler {
    constructor(instance, wvec) {
        this.instance = instance;
        this.wvec = wvec;
    }

    get(target, property, receiver) {
// TODO
        return [];
    }

    set(target, property, value, receiver) {
// TODO
        return value;
    }
}


/* ***************************************************************************
* HELPER FUNCTIONS
*/

function json_encode(value) {
    return JSON.stringify(value);
}

function json_parse(quoted) {
    return JSON.parse(quoted);
}


/* ***************************************************************************
* EXCEPTIONS
*/

export class Exit {
    constructor(code) {
        this.code = code;
    }
}

export class SyntaxError {
    constructor(text) {
        this.text = text;
    }
}


/* ***************************************************************************
* TEST FUNCTIONS
*/

async function main() {
    let compiled = await compile(`PRINT(a);`);
    let result = null;

    compiled.symtbl["a"] = {
        "b" : {
            "c" : {
                "d" : [ 1, 2, 3, 4 ],
            }
        }
    };

    compiled.symtbl["a"]["b"] = { "c" : { "d" : [ 2, 3, 4, 5 ] } };
    compiled.symtbl["a"]["b"]["c"]["d"][1] = 5;
//    console.log(Array.isArray(compiled.symtbl["a"]["b"]["c"]["d"]));
//    console.log(compiled.symtbl["a"]["b"]["c"]["d"].length);
    result = compiled.eval();
//    console.log("result", JSON.stringify(result));
}


/* ***************************************************************************
* EXPORT
*/

export default {
    main        : main,
    eval        : _eval,
    evalurl     : evalurl,
    compile     : compile,
    compileurl  : compileurl,
};
