
window.WebAssembly = WXWebAssembly;
window.Module = Module;
Module.instantiateWasm = function(imports, callback){
    WebAssembly.instantiate("/code/sqwasm.wasm", imports).then((result) => {
        Module.instantiateWasm = null;
        callback(result.instance);
    })
}