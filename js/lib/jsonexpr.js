/* ***************************************************************************
* GLOBALS
*/

const JE_WASM = fetch(new URL("./je.wasm", import.meta.url))
    .then((response) => response.arrayBuffer());


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

async function _eval(code, symbols={}) {
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
                const wtree = iface.parse(wcode);
                const wsymtbl = iface.symnew();
                const wsymmap = iface.valmap(iface.symget(wsymtbl,0));

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

    get(name) {
        return this.symtbl[name];
    }

    set(name, value) {
        this.symtbl[name] = value;
    }

    setSymbols(symbols) {
        for(const [k,v] of Object.entries(symbols)) {
            this.set(k, v);
        }
    }

    destructor(code) {
        this.promise
            .then(() => delete this.iface);
    }

    eval() {
        const wresult = this.iface.eval(this.wtree, this.wsymtbl);
        const wquoted = this.iface.valqstr(wresult);
        const quoted = this.iface.strat(wquoted);
        const result = json_parse(quoted, this.instance, wresult);

        this.iface.valfree(wresult)

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

    parse(wcode) {
        return this.instance.exports.je_parse(wcode);
    }

    eval(wtree, wsymtbl) {
        return this.instance.exports.je_eval(wtree, wsymtbl);
    }

    nodefree(wnode) {
        this.instance.exports.je_freenode(wnode);
    }

    symnew() {
        return this.instance.exports.je_newtable(0);
    }

    symget(wsymtbl, wkey) {
        return this.instance.exports.je_tableget(wsymtbl, wkey);
    }

    mapset(wmap, wkey, wval) {
        return this.instance.exports.je_mapset(wmap, wkey, wval);
    }

    mapget(wmap, wkey) {
        return this.instance.exports.je_mapget(wmap, wkey);
    }

    mapkey(wmap) {
        return this.instance.exports.je_mapkey(wmap);
    }

    mapnext(wmap) {
        return this.instance.exports.je_mapnext(wmap);
    }

    vecset(wvec, index, wval) {
        return this.instance.exports.je_vecset(wvec, index, wval);
    }

    vecget(wvec, index) {
        return this.instance.exports.je_vecget(wvec, index);
    }

    veclen(wvec) {
        return this.instance.exports.je_veclen(wvec);
    }

    valmap(wval) {
        return this.instance.exports.je_getobject(wval);
    }

    valvec(wval) {
        return this.instance.exports.je_getarray(wval);
    }

    valstr(wval) {
        return this.instance.exports.je_valstr(wval);
    }

    valqstr(wval) {
        return this.instance.exports.je_valqstr(wval);
    }

    valfree(wresult) {
        this.instance.exports.je_freeval(wresult);
    }
}

class HandlerBase {
    constructor(instance) {
        this.instance = instance;
        this.iface = new Interface(instance);
    }

    _promote(value, wvalue) {
        if(value instanceof ObjectHandler)     { /* Pass */ }
        else if(value instanceof ArrayHandler) { /* Pass */ }
        else if(value instanceof Array) {
            const warray = this.iface.valvec(wvalue);

            value = new Proxy(value, new ArrayHandler(this.instance, warray));
        }
        else if(value instanceof Object) {
            const wobject = this.iface.valmap(wvalue);

            value = new Proxy(value, new ObjectHandler(this.instance, wobject));
        }

        return value;
    }
}

class ObjectHandler extends HandlerBase {
    constructor(instance, wmap) {
        super(instance);

        this.wmap = wmap;
    }

    has(map, name) {
        const wname = this.iface.strdup(name);
        const wresult = this.iface.mapget(this.wmap, wname);
        const result = wresult ? true : false;

        this.iface.free(wname);

        return result;
    }

    ownKeys(map) {
        const keys = [];
        let iter = this.wmap;

        while((iter = this.iface.mapnext(iter))) {
            const wkey = this.iface.mapkey(iter);
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
        const wresult = this.iface.mapget(this.wmap, wname);
        const wquoted = this.iface.valqstr(wresult);
        const quoted = this.iface.strat(wquoted);
        const result = json_parse(quoted);

        this.iface.free(wname);

        return this._promote(result, wresult);
    }

    set(map, name, value, receiver) {
        const wname = this.iface.strdup(name);
        const wexpr = this.iface.strdup(json_encode(value));
        const wtree = this.iface.parse(wexpr);
        const wresult = this.iface.eval(wtree,0);

        this.iface.mapset(this.wmap, wname, wresult);

        this.iface.nodefree(wtree);
        this.iface.free(wexpr);
        this.iface.free(wname);

        return this._promote(value, wresult);
    }
}

class ArrayHandler extends HandlerBase {
    constructor(instance, wvec) {
        super(instance);

        this.wvec = wvec;
    }

    get(vec, property, receiver) {
        const index = Number(property);

        if(Number.isInteger(index)) {
            const wresult = this.iface.vecget(this.wvec, index);
            const wquoted = this.iface.valqstr(wresult);
            const quoted = this.iface.strat(wquoted);
            const result = json_parse(quoted);

            return this._promote(result, wresult);
        }

        switch(property) {
            case "length": return this.iface.veclen(this.wvec); break;
        }
    }

    set(vec, index, value, receiver) {
        const wexpr = this.iface.strdup(json_encode(value));
        const wtree = this.iface.parse(wexpr);
        const wresult = this.iface.eval(wtree,0);

        this.iface.vecset(this.wvec, index, wresult);

        this.iface.nodefree(wtree);
        this.iface.free(wexpr);

        return this._promote(value, wresult);
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
    const symbols = { students : [ "Alice", "Bob", "Charlie" ] };
    const compiled = await compile(`
        FOR(i = 0, i < LEN(students), i++,
            note = "";

            IF(students[i] == "Alice",
                note = " (she's my favorite)"
            );

            PRINT("Student #" + (i+1) + "'s name is " + students[i] + note)
        );

        LEN(students);
    `);

    compiled.setSymbols(symbols);

    console.log("result =", JSON.stringify(compiled.eval()));
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

    Exit        : Exit,
    SyntaxError : SyntaxError,
};
