
//真服了，debug模型在window下，relase模式在Module下,
window._malloc = window._malloc || Module._malloc || Module._webidl_malloc;
window._free = window._free || Module._free || Module._webidl_free;
window.UTF8ToString = UTF8ToString;
window.stringToUTF8OnStack = stringToUTF8OnStack;
window.__ATPOSTRUN__ = __ATPOSTRUN__;
window.__ATINIT__ = __ATINIT__;
