//真服了，debug模型在window下，relase模式在Module下,
window._malloc = window._malloc || Module._malloc || Module._webidl_malloc;
window._free = window._free || Module._free || Module._webidl_free;
