export class JsonExpr {
    constructor(path) {
        const module = new WasmModule(path);

        this.instance = null;
        this.promise = new WasmInstance(module)
            .then((instance) => {
                this.instance = instance;
            });
    }

    then(onFulfilled, onRejected) {
        return this.promise.then(onFulfilled, onRejected);
    }

    catch(onRejected) {
        return this.promise.catch(onRejected);
    }

    finally(onFinally) {
        return this.promise.finally(onFinally);
    }

    eval(code) {
        const iface = new WasmIface(this.instance);
        const ccode = iface.strdup(code);
        const ast = this.instance.exports.je_parse(ccode);
        const result = this.instance.exports.je_eval(ast, 0);
        const svalue = this.instance.exports.je_valstr(result);

        this.instance.exports.je_freeval(result);
        this.instance.exports.je_freenode(ast);

        return svalue;
    }
}

export class WasmExit {
    constructor(code) {
        this.code = code;
    }
}

export class WasmModule {
    constructor(url) {
        this.url = url;
        this.bytes = null;
        this.promise = fetch(this.url)
            .then((response) => response.arrayBuffer())
            .then((bytes) => this.bytes = bytes);
    }

    then(onFulfilled, onRejected) {
        return this.promise.then(onFulfilled, onRejected);
    }

    catch(onRejected) {
        return this.promise.catch(onRejected);
    }

    finally(onFinally) {
        return this.promise.finally(onFinally);
    }
}

export class WasmInstance {
    constructor(module) {
        const importObject = {
            env : {
                read  : (fd,buf,count) => this.$read(fd,buf,count),
                write : (fd,buf,count) => this.$write(fd,buf,count),
                _exit : (code)         => this.$exit(code),
            },
        };

        this.instance = null;
        this.outbuf = ["", "", ""];
        this.promise = module
            .then((bytes) => WebAssembly.instantiate(bytes, importObject))
            .then((result) => {
                // console.log("exports", result.instance.exports);
                this.instance = result.instance;

                return this.instance;
            });
    }

    destructor(module) {
        for(let i=1; i<3; i++) {
            if(this.outbuf[i].length) console.log(this.outbuf[i]);
        }
    }

    $read(fd, buf, count) {
        console.log(`read(${fd}, ${buf}, ${count})`);
        return 0;
    }

    $write(fd, buf, count) {
        // console.log(`write(${fd}, ${buf}, ${count})`);
        const buffer = new Uint8Array(this.instance.exports.memory.buffer, buf, count);
        const str = new TextDecoder().decode(buffer);
        const lines = str.split("\n");

        for(let i=0; i<lines.length-1; i++) {
            this.outbuf[fd] += lines[i];
            console.log(this.outbuf[fd]);
            this.outbuf[fd] = "";
        }

        if(lines.length) {
            this.outbuf[fd] += lines[lines.length-1];
        }

        return count;
    }

    $exit(code) {
        // console.log(`exit(${code})`);
        throw new WasmExit(code);
    }

    then(onFulfilled, onRejected) {
        return this.promise.then(onFulfilled, onRejected);
    }

    catch(onRejected) {
        return this.promise.catch(onRejected);
    }

    finally(onFinally) {
        return this.promise.finally(onFinally);
    }
}

export class WasmIface {
    constructor(instance) {
        this.instance = instance;
        this.buffer = new Uint8Array(instance.exports.memory.buffer);
        this.allocated = [];
    }

    destructor() {
        for(let addr of this.allocated) {
            this.instance.exports.free(addr);
        }
    }

    strdup(str) {
        const buffer = this.buffer;
        const buf = new TextEncoder().encode(str);
        const addr = this.instance.exports.calloc(1, buf.length+1);

        for(let i=0; i<buf.length; i++) {
            buffer[addr+i] = buf[i];
        }

        return addr;
    }

    strat(addr) {
        const buffer = this.buffer;
        let count = 0;
        let buf = null;
        let str = null;

        while(buffer[addr] != 0) {
            count++;
            addr++;
        }

        buf = new Uint8Array(_instance.exports.memory.buffer, buf, count);
        str = new TextDecoder("utf-8").decode(buf);

        return str;
    }
}
