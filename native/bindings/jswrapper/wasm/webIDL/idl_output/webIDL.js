
// Bindings utilities

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function WrapperObject() {
}
WrapperObject.prototype = Object.create(WrapperObject.prototype);
WrapperObject.prototype.constructor = WrapperObject;
WrapperObject.prototype.__class__ = WrapperObject;
WrapperObject.__cache__ = {};
Module['WrapperObject'] = WrapperObject;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant)
    @param {*=} __class__ */
function getCache(__class__) {
  return (__class__ || WrapperObject).__cache__;
}
Module['getCache'] = getCache;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant)
    @param {*=} __class__ */
function wrapPointer(ptr, __class__) {
  var cache = getCache(__class__);
  var ret = cache[ptr];
  if (ret) return ret;
  ret = Object.create((__class__ || WrapperObject).prototype);
  ret.ptr = ptr;
  return cache[ptr] = ret;
}
Module['wrapPointer'] = wrapPointer;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function castObject(obj, __class__) {
  return wrapPointer(obj.ptr, __class__);
}
Module['castObject'] = castObject;

Module['NULL'] = wrapPointer(0);

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function destroy(obj) {
  if (!obj['__destroy__']) throw 'Error: Cannot destroy object. (Did you create it yourself?)';
  obj['__destroy__']();
  // Remove from cache, so the object can be GC'd and refs added onto it released
  delete getCache(obj.__class__)[obj.ptr];
}
Module['destroy'] = destroy;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function compare(obj1, obj2) {
  return obj1.ptr === obj2.ptr;
}
Module['compare'] = compare;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function getPointer(obj) {
  return obj.ptr;
}
Module['getPointer'] = getPointer;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function getClass(obj) {
  return obj.__class__;
}
Module['getClass'] = getClass;

// Converts big (string or array) values into a C-style storage, in temporary space

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
var ensureCache = {
  buffer: 0,  // the main buffer of temporary storage
  size: 0,   // the size of buffer
  pos: 0,    // the next free offset in buffer
  temps: [], // extra allocations
  needed: 0, // the total size we need next time

  prepare() {
    if (ensureCache.needed) {
      // clear the temps
      for (var i = 0; i < ensureCache.temps.length; i++) {
        Module['_webidl_free'](ensureCache.temps[i]);
      }
      ensureCache.temps.length = 0;
      // prepare to allocate a bigger buffer
      Module['_webidl_free'](ensureCache.buffer);
      ensureCache.buffer = 0;
      ensureCache.size += ensureCache.needed;
      // clean up
      ensureCache.needed = 0;
    }
    if (!ensureCache.buffer) { // happens first time, or when we need to grow
      ensureCache.size += 128; // heuristic, avoid many small grow events
      ensureCache.buffer = Module['_webidl_malloc'](ensureCache.size);
      assert(ensureCache.buffer);
    }
    ensureCache.pos = 0;
  },
  alloc(array, view) {
    assert(ensureCache.buffer);
    var bytes = view.BYTES_PER_ELEMENT;
    var len = array.length * bytes;
    len = alignMemory(len, 8); // keep things aligned to 8 byte boundaries
    var ret;
    if (ensureCache.pos + len >= ensureCache.size) {
      // we failed to allocate in the buffer, ensureCache time around :(
      assert(len > 0); // null terminator, at least
      ensureCache.needed += len;
      ret = Module['_webidl_malloc'](len);
      ensureCache.temps.push(ret);
    } else {
      // we can allocate in the buffer
      ret = ensureCache.buffer + ensureCache.pos;
      ensureCache.pos += len;
    }
    return ret;
  },
  copy(array, view, offset) {
    offset /= view.BYTES_PER_ELEMENT;
    for (var i = 0; i < array.length; i++) {
      view[offset + i] = array[i];
    }
  },
};

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureString(value) {
  if (typeof value === 'string') {
    var intArray = intArrayFromString(value);
    var offset = ensureCache.alloc(intArray, HEAP8);
    ensureCache.copy(intArray, HEAP8, offset);
    return offset;
  }
  return value;
}

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureInt8(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAP8);
    ensureCache.copy(value, HEAP8, offset);
    return offset;
  }
  return value;
}

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureInt16(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAP16);
    ensureCache.copy(value, HEAP16, offset);
    return offset;
  }
  return value;
}

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureInt32(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAP32);
    ensureCache.copy(value, HEAP32, offset);
    return offset;
  }
  return value;
}

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureFloat32(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAPF32);
    ensureCache.copy(value, HEAPF32, offset);
    return offset;
  }
  return value;
}

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureFloat64(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAPF64);
    ensureCache.copy(value, HEAPF64, offset);
    return offset;
  }
  return value;
}

// Interface: VoidPtr

/** @suppress {undefinedVars, duplicate} @this{Object} */
function VoidPtr() { throw "cannot construct a VoidPtr, no constructor in IDL" }
VoidPtr.prototype = Object.create(WrapperObject.prototype);
VoidPtr.prototype.constructor = VoidPtr;
VoidPtr.prototype.__class__ = VoidPtr;
VoidPtr.__cache__ = {};
Module['VoidPtr'] = VoidPtr;

/** @suppress {undefinedVars, duplicate} @this{Object} */
VoidPtr.prototype['__destroy__'] = VoidPtr.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_VoidPtr___destroy___0(self);
};

// Interface: IAttributeInfo

/** @suppress {undefinedVars, duplicate} @this{Object} */
function IAttributeInfo() {
  this.ptr = _emscripten_bind_IAttributeInfo_IAttributeInfo_0();
  getCache(IAttributeInfo)[this.ptr] = this;
};

IAttributeInfo.prototype = Object.create(WrapperObject.prototype);
IAttributeInfo.prototype.constructor = IAttributeInfo;
IAttributeInfo.prototype.__class__ = IAttributeInfo;
IAttributeInfo.__cache__ = {};
Module['IAttributeInfo'] = IAttributeInfo;
/** @suppress {undefinedVars, duplicate} @this{Object} */
IAttributeInfo.prototype['setName'] = IAttributeInfo.prototype.setName = function(name) {
  var self = this.ptr;
  ensureCache.prepare();
  if (name && typeof name === 'object') name = name.ptr;
  else name = ensureString(name);
  _emscripten_bind_IAttributeInfo_setName_1(self, name);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IAttributeInfo.prototype['get_format'] = IAttributeInfo.prototype.get_format = function() {
  var self = this.ptr;
  return _emscripten_bind_IAttributeInfo_get_format_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IAttributeInfo.prototype['set_format'] = IAttributeInfo.prototype.set_format = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_IAttributeInfo_set_format_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(IAttributeInfo.prototype, 'format', { get: IAttributeInfo.prototype.get_format, set: IAttributeInfo.prototype.set_format });

/** @suppress {undefinedVars, duplicate} @this{Object} */
IAttributeInfo.prototype['__destroy__'] = IAttributeInfo.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_IAttributeInfo___destroy___0(self);
};

// Interface: ISamplerTextureInfo

/** @suppress {undefinedVars, duplicate} @this{Object} */
function ISamplerTextureInfo() {
  this.ptr = _emscripten_bind_ISamplerTextureInfo_ISamplerTextureInfo_0();
  getCache(ISamplerTextureInfo)[this.ptr] = this;
};

ISamplerTextureInfo.prototype = Object.create(WrapperObject.prototype);
ISamplerTextureInfo.prototype.constructor = ISamplerTextureInfo;
ISamplerTextureInfo.prototype.__class__ = ISamplerTextureInfo;
ISamplerTextureInfo.__cache__ = {};
Module['ISamplerTextureInfo'] = ISamplerTextureInfo;
/** @suppress {undefinedVars, duplicate} @this{Object} */
ISamplerTextureInfo.prototype['setName'] = ISamplerTextureInfo.prototype.setName = function(name) {
  var self = this.ptr;
  ensureCache.prepare();
  if (name && typeof name === 'object') name = name.ptr;
  else name = ensureString(name);
  _emscripten_bind_ISamplerTextureInfo_setName_1(self, name);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
ISamplerTextureInfo.prototype['get_binding'] = ISamplerTextureInfo.prototype.get_binding = function() {
  var self = this.ptr;
  return _emscripten_bind_ISamplerTextureInfo_get_binding_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
ISamplerTextureInfo.prototype['set_binding'] = ISamplerTextureInfo.prototype.set_binding = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_ISamplerTextureInfo_set_binding_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(ISamplerTextureInfo.prototype, 'binding', { get: ISamplerTextureInfo.prototype.get_binding, set: ISamplerTextureInfo.prototype.set_binding });
/** @suppress {undefinedVars, duplicate} @this{Object} */
ISamplerTextureInfo.prototype['get_type'] = ISamplerTextureInfo.prototype.get_type = function() {
  var self = this.ptr;
  return _emscripten_bind_ISamplerTextureInfo_get_type_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
ISamplerTextureInfo.prototype['set_type'] = ISamplerTextureInfo.prototype.set_type = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_ISamplerTextureInfo_set_type_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(ISamplerTextureInfo.prototype, 'type', { get: ISamplerTextureInfo.prototype.get_type, set: ISamplerTextureInfo.prototype.set_type });
/** @suppress {undefinedVars, duplicate} @this{Object} */
ISamplerTextureInfo.prototype['get_count'] = ISamplerTextureInfo.prototype.get_count = function() {
  var self = this.ptr;
  return _emscripten_bind_ISamplerTextureInfo_get_count_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
ISamplerTextureInfo.prototype['set_count'] = ISamplerTextureInfo.prototype.set_count = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_ISamplerTextureInfo_set_count_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(ISamplerTextureInfo.prototype, 'count', { get: ISamplerTextureInfo.prototype.get_count, set: ISamplerTextureInfo.prototype.set_count });
/** @suppress {undefinedVars, duplicate} @this{Object} */
ISamplerTextureInfo.prototype['get_buildin'] = ISamplerTextureInfo.prototype.get_buildin = function() {
  var self = this.ptr;
  return !!(_emscripten_bind_ISamplerTextureInfo_get_buildin_0(self));
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
ISamplerTextureInfo.prototype['set_buildin'] = ISamplerTextureInfo.prototype.set_buildin = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_ISamplerTextureInfo_set_buildin_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(ISamplerTextureInfo.prototype, 'buildin', { get: ISamplerTextureInfo.prototype.get_buildin, set: ISamplerTextureInfo.prototype.set_buildin });

/** @suppress {undefinedVars, duplicate} @this{Object} */
ISamplerTextureInfo.prototype['__destroy__'] = ISamplerTextureInfo.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_ISamplerTextureInfo___destroy___0(self);
};

// Interface: IUniform

/** @suppress {undefinedVars, duplicate} @this{Object} */
function IUniform() {
  this.ptr = _emscripten_bind_IUniform_IUniform_0();
  getCache(IUniform)[this.ptr] = this;
};

IUniform.prototype = Object.create(WrapperObject.prototype);
IUniform.prototype.constructor = IUniform;
IUniform.prototype.__class__ = IUniform;
IUniform.__cache__ = {};
Module['IUniform'] = IUniform;
/** @suppress {undefinedVars, duplicate} @this{Object} */
IUniform.prototype['setName'] = IUniform.prototype.setName = function(name) {
  var self = this.ptr;
  ensureCache.prepare();
  if (name && typeof name === 'object') name = name.ptr;
  else name = ensureString(name);
  _emscripten_bind_IUniform_setName_1(self, name);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IUniform.prototype['get_type'] = IUniform.prototype.get_type = function() {
  var self = this.ptr;
  return _emscripten_bind_IUniform_get_type_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IUniform.prototype['set_type'] = IUniform.prototype.set_type = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_IUniform_set_type_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(IUniform.prototype, 'type', { get: IUniform.prototype.get_type, set: IUniform.prototype.set_type });
/** @suppress {undefinedVars, duplicate} @this{Object} */
IUniform.prototype['get_count'] = IUniform.prototype.get_count = function() {
  var self = this.ptr;
  return _emscripten_bind_IUniform_get_count_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IUniform.prototype['set_count'] = IUniform.prototype.set_count = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_IUniform_set_count_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(IUniform.prototype, 'count', { get: IUniform.prototype.get_count, set: IUniform.prototype.set_count });

/** @suppress {undefinedVars, duplicate} @this{Object} */
IUniform.prototype['__destroy__'] = IUniform.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_IUniform___destroy___0(self);
};

// Interface: IBlockInfo

/** @suppress {undefinedVars, duplicate} @this{Object} */
function IBlockInfo() {
  this.ptr = _emscripten_bind_IBlockInfo_IBlockInfo_0();
  getCache(IBlockInfo)[this.ptr] = this;
};

IBlockInfo.prototype = Object.create(WrapperObject.prototype);
IBlockInfo.prototype.constructor = IBlockInfo;
IBlockInfo.prototype.__class__ = IBlockInfo;
IBlockInfo.__cache__ = {};
Module['IBlockInfo'] = IBlockInfo;
/** @suppress {undefinedVars, duplicate} @this{Object} */
IBlockInfo.prototype['pushMembers'] = IBlockInfo.prototype.pushMembers = function(uniform) {
  var self = this.ptr;
  if (uniform && typeof uniform === 'object') uniform = uniform.ptr;
  _emscripten_bind_IBlockInfo_pushMembers_1(self, uniform);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IBlockInfo.prototype['setBlockName'] = IBlockInfo.prototype.setBlockName = function(name) {
  var self = this.ptr;
  ensureCache.prepare();
  if (name && typeof name === 'object') name = name.ptr;
  else name = ensureString(name);
  _emscripten_bind_IBlockInfo_setBlockName_1(self, name);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IBlockInfo.prototype['get_binding'] = IBlockInfo.prototype.get_binding = function() {
  var self = this.ptr;
  return _emscripten_bind_IBlockInfo_get_binding_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IBlockInfo.prototype['set_binding'] = IBlockInfo.prototype.set_binding = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_IBlockInfo_set_binding_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(IBlockInfo.prototype, 'binding', { get: IBlockInfo.prototype.get_binding, set: IBlockInfo.prototype.set_binding });
/** @suppress {undefinedVars, duplicate} @this{Object} */
IBlockInfo.prototype['get_buildin'] = IBlockInfo.prototype.get_buildin = function() {
  var self = this.ptr;
  return !!(_emscripten_bind_IBlockInfo_get_buildin_0(self));
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IBlockInfo.prototype['set_buildin'] = IBlockInfo.prototype.set_buildin = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_IBlockInfo_set_buildin_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(IBlockInfo.prototype, 'buildin', { get: IBlockInfo.prototype.get_buildin, set: IBlockInfo.prototype.set_buildin });

/** @suppress {undefinedVars, duplicate} @this{Object} */
IBlockInfo.prototype['__destroy__'] = IBlockInfo.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_IBlockInfo___destroy___0(self);
};

// Interface: IShaderDefine

/** @suppress {undefinedVars, duplicate} @this{Object} */
function IShaderDefine() {
  this.ptr = _emscripten_bind_IShaderDefine_IShaderDefine_0();
  getCache(IShaderDefine)[this.ptr] = this;
};

IShaderDefine.prototype = Object.create(WrapperObject.prototype);
IShaderDefine.prototype.constructor = IShaderDefine;
IShaderDefine.prototype.__class__ = IShaderDefine;
IShaderDefine.__cache__ = {};
Module['IShaderDefine'] = IShaderDefine;
/** @suppress {undefinedVars, duplicate} @this{Object} */
IShaderDefine.prototype['pushBlock'] = IShaderDefine.prototype.pushBlock = function(blockInfo) {
  var self = this.ptr;
  if (blockInfo && typeof blockInfo === 'object') blockInfo = blockInfo.ptr;
  _emscripten_bind_IShaderDefine_pushBlock_1(self, blockInfo);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IShaderDefine.prototype['pushAttribute'] = IShaderDefine.prototype.pushAttribute = function(attributeInfo) {
  var self = this.ptr;
  if (attributeInfo && typeof attributeInfo === 'object') attributeInfo = attributeInfo.ptr;
  _emscripten_bind_IShaderDefine_pushAttribute_1(self, attributeInfo);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IShaderDefine.prototype['pushSamplerTexture'] = IShaderDefine.prototype.pushSamplerTexture = function(sampler) {
  var self = this.ptr;
  if (sampler && typeof sampler === 'object') sampler = sampler.ptr;
  _emscripten_bind_IShaderDefine_pushSamplerTexture_1(self, sampler);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IShaderDefine.prototype['pushMacro'] = IShaderDefine.prototype.pushMacro = function(name, flag) {
  var self = this.ptr;
  ensureCache.prepare();
  if (name && typeof name === 'object') name = name.ptr;
  else name = ensureString(name);
  if (flag && typeof flag === 'object') flag = flag.ptr;
  _emscripten_bind_IShaderDefine_pushMacro_2(self, name, flag);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
IShaderDefine.prototype['__destroy__'] = IShaderDefine.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_IShaderDefine___destroy___0(self);
};

// Interface: IPassInfo

/** @suppress {undefinedVars, duplicate} @this{Object} */
function IPassInfo() {
  this.ptr = _emscripten_bind_IPassInfo_IPassInfo_0();
  getCache(IPassInfo)[this.ptr] = this;
};

IPassInfo.prototype = Object.create(WrapperObject.prototype);
IPassInfo.prototype.constructor = IPassInfo;
IPassInfo.prototype.__class__ = IPassInfo;
IPassInfo.__cache__ = {};
Module['IPassInfo'] = IPassInfo;
/** @suppress {undefinedVars, duplicate} @this{Object} */
IPassInfo.prototype['get_shaderIndex'] = IPassInfo.prototype.get_shaderIndex = function() {
  var self = this.ptr;
  return _emscripten_bind_IPassInfo_get_shaderIndex_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IPassInfo.prototype['set_shaderIndex'] = IPassInfo.prototype.set_shaderIndex = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_IPassInfo_set_shaderIndex_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(IPassInfo.prototype, 'shaderIndex', { get: IPassInfo.prototype.get_shaderIndex, set: IPassInfo.prototype.set_shaderIndex });
/** @suppress {undefinedVars, duplicate} @this{Object} */
IPassInfo.prototype['get_passStateBuffer'] = IPassInfo.prototype.get_passStateBuffer = function() {
  var self = this.ptr;
  return UTF8ToString(_emscripten_bind_IPassInfo_get_passStateBuffer_0(self));
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IPassInfo.prototype['set_passStateBuffer'] = IPassInfo.prototype.set_passStateBuffer = function(arg0) {
  var self = this.ptr;
  ensureCache.prepare();
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  else arg0 = ensureString(arg0);
  _emscripten_bind_IPassInfo_set_passStateBuffer_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(IPassInfo.prototype, 'passStateBuffer', { get: IPassInfo.prototype.get_passStateBuffer, set: IPassInfo.prototype.set_passStateBuffer });

/** @suppress {undefinedVars, duplicate} @this{Object} */
IPassInfo.prototype['__destroy__'] = IPassInfo.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_IPassInfo___destroy___0(self);
};

// Interface: IEffectDefine

/** @suppress {undefinedVars, duplicate} @this{Object} */
function IEffectDefine() {
  this.ptr = _emscripten_bind_IEffectDefine_IEffectDefine_0();
  getCache(IEffectDefine)[this.ptr] = this;
};

IEffectDefine.prototype = Object.create(WrapperObject.prototype);
IEffectDefine.prototype.constructor = IEffectDefine;
IEffectDefine.prototype.__class__ = IEffectDefine;
IEffectDefine.__cache__ = {};
Module['IEffectDefine'] = IEffectDefine;
/** @suppress {undefinedVars, duplicate} @this{Object} */
IEffectDefine.prototype['pushShaderDefine'] = IEffectDefine.prototype.pushShaderDefine = function(info) {
  var self = this.ptr;
  if (info && typeof info === 'object') info = info.ptr;
  _emscripten_bind_IEffectDefine_pushShaderDefine_1(self, info);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
IEffectDefine.prototype['pushPassInfo'] = IEffectDefine.prototype.pushPassInfo = function(info) {
  var self = this.ptr;
  if (info && typeof info === 'object') info = info.ptr;
  _emscripten_bind_IEffectDefine_pushPassInfo_1(self, info);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
IEffectDefine.prototype['__destroy__'] = IEffectDefine.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_IEffectDefine___destroy___0(self);
};

// Interface: EffectAsset

/** @suppress {undefinedVars, duplicate} @this{Object} */
function EffectAsset(id) {
  if (id && typeof id === 'object') id = id.ptr;
  this.ptr = _emscripten_bind_EffectAsset_EffectAsset_1(id);
  getCache(EffectAsset)[this.ptr] = this;
};

EffectAsset.prototype = Object.create(WrapperObject.prototype);
EffectAsset.prototype.constructor = EffectAsset;
EffectAsset.prototype.__class__ = EffectAsset;
EffectAsset.__cache__ = {};
Module['EffectAsset'] = EffectAsset;
/** @suppress {undefinedVars, duplicate} @this{Object} */
EffectAsset.prototype['setKeyUrl'] = EffectAsset.prototype.setKeyUrl = function(url) {
  var self = this.ptr;
  ensureCache.prepare();
  if (url && typeof url === 'object') url = url.ptr;
  else url = ensureString(url);
  _emscripten_bind_EffectAsset_setKeyUrl_1(self, url);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
EffectAsset.prototype['initialize'] = EffectAsset.prototype.initialize = function(info) {
  var self = this.ptr;
  if (info && typeof info === 'object') info = info.ptr;
  _emscripten_bind_EffectAsset_initialize_1(self, info);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
EffectAsset.prototype['__destroy__'] = EffectAsset.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_EffectAsset___destroy___0(self);
};

// Interface: IMaterialInfo

/** @suppress {undefinedVars, duplicate} @this{Object} */
function IMaterialInfo() {
  this.ptr = _emscripten_bind_IMaterialInfo_IMaterialInfo_0();
  getCache(IMaterialInfo)[this.ptr] = this;
};

IMaterialInfo.prototype = Object.create(WrapperObject.prototype);
IMaterialInfo.prototype.constructor = IMaterialInfo;
IMaterialInfo.prototype.__class__ = IMaterialInfo;
IMaterialInfo.__cache__ = {};
Module['IMaterialInfo'] = IMaterialInfo;
/** @suppress {undefinedVars, duplicate} @this{Object} */
IMaterialInfo.prototype['pushMacro'] = IMaterialInfo.prototype.pushMacro = function(name, value) {
  var self = this.ptr;
  ensureCache.prepare();
  if (name && typeof name === 'object') name = name.ptr;
  else name = ensureString(name);
  if (value && typeof value === 'object') value = value.ptr;
  _emscripten_bind_IMaterialInfo_pushMacro_2(self, name, value);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
IMaterialInfo.prototype['__destroy__'] = IMaterialInfo.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_IMaterialInfo___destroy___0(self);
};

// Interface: Material

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Material(id) {
  if (id && typeof id === 'object') id = id.ptr;
  this.ptr = _emscripten_bind_Material_Material_1(id);
  getCache(Material)[this.ptr] = this;
};

Material.prototype = Object.create(WrapperObject.prototype);
Material.prototype.constructor = Material;
Material.prototype.__class__ = Material;
Material.__cache__ = {};
Module['Material'] = Material;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Material.prototype['setKeyUrl'] = Material.prototype.setKeyUrl = function(url) {
  var self = this.ptr;
  ensureCache.prepare();
  if (url && typeof url === 'object') url = url.ptr;
  else url = ensureString(url);
  _emscripten_bind_Material_setKeyUrl_1(self, url);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Material.prototype['setEffectAsset'] = Material.prototype.setEffectAsset = function(effect) {
  var self = this.ptr;
  if (effect && typeof effect === 'object') effect = effect.ptr;
  _emscripten_bind_Material_setEffectAsset_1(self, effect);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Material.prototype['initialize'] = Material.prototype.initialize = function(info) {
  var self = this.ptr;
  if (info && typeof info === 'object') info = info.ptr;
  _emscripten_bind_Material_initialize_1(self, info);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Material.prototype['setPropertyBridgeJsTexture'] = Material.prototype.setPropertyBridgeJsTexture = function(name, textureAssetId) {
  var self = this.ptr;
  ensureCache.prepare();
  if (name && typeof name === 'object') name = name.ptr;
  else name = ensureString(name);
  if (textureAssetId && typeof textureAssetId === 'object') textureAssetId = textureAssetId.ptr;
  _emscripten_bind_Material_setPropertyBridgeJsTexture_2(self, name, textureAssetId);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Material.prototype['setPropertyBridgeJsFloat'] = Material.prototype.setPropertyBridgeJsFloat = function(name, value) {
  var self = this.ptr;
  ensureCache.prepare();
  if (name && typeof name === 'object') name = name.ptr;
  else name = ensureString(name);
  if (value && typeof value === 'object') value = value.ptr;
  _emscripten_bind_Material_setPropertyBridgeJsFloat_2(self, name, value);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Material.prototype['setPropertyBridgeJsMat4'] = Material.prototype.setPropertyBridgeJsMat4 = function(name, value) {
  var self = this.ptr;
  ensureCache.prepare();
  if (name && typeof name === 'object') name = name.ptr;
  else name = ensureString(name);
  if (value && typeof value === 'object') value = value.ptr;
  _emscripten_bind_Material_setPropertyBridgeJsMat4_2(self, name, value);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Material.prototype['__destroy__'] = Material.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Material___destroy___0(self);
};

// Interface: ITexture2DCreateInfo

/** @suppress {undefinedVars, duplicate} @this{Object} */
function ITexture2DCreateInfo() {
  this.ptr = _emscripten_bind_ITexture2DCreateInfo_ITexture2DCreateInfo_0();
  getCache(ITexture2DCreateInfo)[this.ptr] = this;
};

ITexture2DCreateInfo.prototype = Object.create(WrapperObject.prototype);
ITexture2DCreateInfo.prototype.constructor = ITexture2DCreateInfo;
ITexture2DCreateInfo.prototype.__class__ = ITexture2DCreateInfo;
ITexture2DCreateInfo.__cache__ = {};
Module['ITexture2DCreateInfo'] = ITexture2DCreateInfo;
/** @suppress {undefinedVars, duplicate} @this{Object} */
ITexture2DCreateInfo.prototype['get_width'] = ITexture2DCreateInfo.prototype.get_width = function() {
  var self = this.ptr;
  return _emscripten_bind_ITexture2DCreateInfo_get_width_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
ITexture2DCreateInfo.prototype['set_width'] = ITexture2DCreateInfo.prototype.set_width = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_ITexture2DCreateInfo_set_width_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(ITexture2DCreateInfo.prototype, 'width', { get: ITexture2DCreateInfo.prototype.get_width, set: ITexture2DCreateInfo.prototype.set_width });
/** @suppress {undefinedVars, duplicate} @this{Object} */
ITexture2DCreateInfo.prototype['get_height'] = ITexture2DCreateInfo.prototype.get_height = function() {
  var self = this.ptr;
  return _emscripten_bind_ITexture2DCreateInfo_get_height_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
ITexture2DCreateInfo.prototype['set_height'] = ITexture2DCreateInfo.prototype.set_height = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_ITexture2DCreateInfo_set_height_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(ITexture2DCreateInfo.prototype, 'height', { get: ITexture2DCreateInfo.prototype.get_height, set: ITexture2DCreateInfo.prototype.set_height });
/** @suppress {undefinedVars, duplicate} @this{Object} */
ITexture2DCreateInfo.prototype['get_format'] = ITexture2DCreateInfo.prototype.get_format = function() {
  var self = this.ptr;
  return _emscripten_bind_ITexture2DCreateInfo_get_format_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
ITexture2DCreateInfo.prototype['set_format'] = ITexture2DCreateInfo.prototype.set_format = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_ITexture2DCreateInfo_set_format_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(ITexture2DCreateInfo.prototype, 'format', { get: ITexture2DCreateInfo.prototype.get_format, set: ITexture2DCreateInfo.prototype.set_format });
/** @suppress {undefinedVars, duplicate} @this{Object} */
ITexture2DCreateInfo.prototype['get_mipmapLevel'] = ITexture2DCreateInfo.prototype.get_mipmapLevel = function() {
  var self = this.ptr;
  return _emscripten_bind_ITexture2DCreateInfo_get_mipmapLevel_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
ITexture2DCreateInfo.prototype['set_mipmapLevel'] = ITexture2DCreateInfo.prototype.set_mipmapLevel = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_ITexture2DCreateInfo_set_mipmapLevel_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(ITexture2DCreateInfo.prototype, 'mipmapLevel', { get: ITexture2DCreateInfo.prototype.get_mipmapLevel, set: ITexture2DCreateInfo.prototype.set_mipmapLevel });
/** @suppress {undefinedVars, duplicate} @this{Object} */
ITexture2DCreateInfo.prototype['get_samplerHash'] = ITexture2DCreateInfo.prototype.get_samplerHash = function() {
  var self = this.ptr;
  return _emscripten_bind_ITexture2DCreateInfo_get_samplerHash_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
ITexture2DCreateInfo.prototype['set_samplerHash'] = ITexture2DCreateInfo.prototype.set_samplerHash = function(arg0) {
  var self = this.ptr;
  if (arg0 && typeof arg0 === 'object') arg0 = arg0.ptr;
  _emscripten_bind_ITexture2DCreateInfo_set_samplerHash_1(self, arg0);
};

/** @suppress {checkTypes} */
Object.defineProperty(ITexture2DCreateInfo.prototype, 'samplerHash', { get: ITexture2DCreateInfo.prototype.get_samplerHash, set: ITexture2DCreateInfo.prototype.set_samplerHash });

/** @suppress {undefinedVars, duplicate} @this{Object} */
ITexture2DCreateInfo.prototype['__destroy__'] = ITexture2DCreateInfo.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_ITexture2DCreateInfo___destroy___0(self);
};

// Interface: Texture2d

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Texture2d(id) {
  if (id && typeof id === 'object') id = id.ptr;
  this.ptr = _emscripten_bind_Texture2d_Texture2d_1(id);
  getCache(Texture2d)[this.ptr] = this;
};

Texture2d.prototype = Object.create(WrapperObject.prototype);
Texture2d.prototype.constructor = Texture2d;
Texture2d.prototype.__class__ = Texture2d;
Texture2d.__cache__ = {};
Module['Texture2d'] = Texture2d;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Texture2d.prototype['setKeyUrl'] = Texture2d.prototype.setKeyUrl = function(url) {
  var self = this.ptr;
  ensureCache.prepare();
  if (url && typeof url === 'object') url = url.ptr;
  else url = ensureString(url);
  _emscripten_bind_Texture2d_setKeyUrl_1(self, url);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Texture2d.prototype['create'] = Texture2d.prototype.create = function(info) {
  var self = this.ptr;
  if (info && typeof info === 'object') info = info.ptr;
  _emscripten_bind_Texture2d_create_1(self, info);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Texture2d.prototype['uploadData'] = Texture2d.prototype.uploadData = function() {
  var self = this.ptr;
  _emscripten_bind_Texture2d_uploadData_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Texture2d.prototype['resize'] = Texture2d.prototype.resize = function(width, height) {
  var self = this.ptr;
  if (width && typeof width === 'object') width = width.ptr;
  if (height && typeof height === 'object') height = height.ptr;
  _emscripten_bind_Texture2d_resize_2(self, width, height);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Texture2d.prototype['setSamplerFromJs'] = Texture2d.prototype.setSamplerFromJs = function(hash) {
  var self = this.ptr;
  if (hash && typeof hash === 'object') hash = hash.ptr;
  _emscripten_bind_Texture2d_setSamplerFromJs_1(self, hash);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Texture2d.prototype['__destroy__'] = Texture2d.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Texture2d___destroy___0(self);
};

// Interface: RenderTexture

/** @suppress {undefinedVars, duplicate} @this{Object} */
function RenderTexture(id) {
  if (id && typeof id === 'object') id = id.ptr;
  this.ptr = _emscripten_bind_RenderTexture_RenderTexture_1(id);
  getCache(RenderTexture)[this.ptr] = this;
};

RenderTexture.prototype = Object.create(WrapperObject.prototype);
RenderTexture.prototype.constructor = RenderTexture;
RenderTexture.prototype.__class__ = RenderTexture;
RenderTexture.__cache__ = {};
Module['RenderTexture'] = RenderTexture;
/** @suppress {undefinedVars, duplicate} @this{Object} */
RenderTexture.prototype['initiliazeFormJS'] = RenderTexture.prototype.initiliazeFormJS = function(data) {
  var self = this.ptr;
  ensureCache.prepare();
  if (data && typeof data === 'object') data = data.ptr;
  else data = ensureString(data);
  _emscripten_bind_RenderTexture_initiliazeFormJS_1(self, data);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
RenderTexture.prototype['__destroy__'] = RenderTexture.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_RenderTexture___destroy___0(self);
};

// Interface: Spine

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Spine(id) {
  if (id && typeof id === 'object') id = id.ptr;
  this.ptr = _emscripten_bind_Spine_Spine_1(id);
  getCache(Spine)[this.ptr] = this;
};

Spine.prototype = Object.create(WrapperObject.prototype);
Spine.prototype.constructor = Spine;
Spine.prototype.__class__ = Spine;
Spine.__cache__ = {};
Module['Spine'] = Spine;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Spine.prototype['setKeyUrl'] = Spine.prototype.setKeyUrl = function(url) {
  var self = this.ptr;
  ensureCache.prepare();
  if (url && typeof url === 'object') url = url.ptr;
  else url = ensureString(url);
  _emscripten_bind_Spine_setKeyUrl_1(self, url);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Spine.prototype['parseBuffer'] = Spine.prototype.parseBuffer = function(buffer, size) {
  var self = this.ptr;
  ensureCache.prepare();
  if (buffer && typeof buffer === 'object') buffer = buffer.ptr;
  else buffer = ensureString(buffer);
  if (size && typeof size === 'object') size = size.ptr;
  _emscripten_bind_Spine_parseBuffer_2(self, buffer, size);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Spine.prototype['getAnimatoinCount'] = Spine.prototype.getAnimatoinCount = function() {
  var self = this.ptr;
  return _emscripten_bind_Spine_getAnimatoinCount_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Spine.prototype['getAllAnimationNames'] = Spine.prototype.getAllAnimationNames = function() {
  var self = this.ptr;
  return _emscripten_bind_Spine_getAllAnimationNames_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Spine.prototype['setTextures'] = Spine.prototype.setTextures = function(buffer, count) {
  var self = this.ptr;
  ensureCache.prepare();
  if (buffer && typeof buffer === 'object') buffer = buffer.ptr;
  else buffer = ensureString(buffer);
  if (count && typeof count === 'object') count = count.ptr;
  _emscripten_bind_Spine_setTextures_2(self, buffer, count);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Spine.prototype['__destroy__'] = Spine.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Spine___destroy___0(self);
};

// Interface: FreeTypeFont

/** @suppress {undefinedVars, duplicate} @this{Object} */
function FreeTypeFont(id) {
  if (id && typeof id === 'object') id = id.ptr;
  this.ptr = _emscripten_bind_FreeTypeFont_FreeTypeFont_1(id);
  getCache(FreeTypeFont)[this.ptr] = this;
};

FreeTypeFont.prototype = Object.create(WrapperObject.prototype);
FreeTypeFont.prototype.constructor = FreeTypeFont;
FreeTypeFont.prototype.__class__ = FreeTypeFont;
FreeTypeFont.__cache__ = {};
Module['FreeTypeFont'] = FreeTypeFont;
/** @suppress {undefinedVars, duplicate} @this{Object} */
FreeTypeFont.prototype['setKeyUrl'] = FreeTypeFont.prototype.setKeyUrl = function(url) {
  var self = this.ptr;
  ensureCache.prepare();
  if (url && typeof url === 'object') url = url.ptr;
  else url = ensureString(url);
  _emscripten_bind_FreeTypeFont_setKeyUrl_1(self, url);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
FreeTypeFont.prototype['loadFontFile'] = FreeTypeFont.prototype.loadFontFile = function(data, size) {
  var self = this.ptr;
  ensureCache.prepare();
  if (data && typeof data === 'object') data = data.ptr;
  else data = ensureString(data);
  if (size && typeof size === 'object') size = size.ptr;
  _emscripten_bind_FreeTypeFont_loadFontFile_2(self, data, size);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
FreeTypeFont.prototype['__destroy__'] = FreeTypeFont.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_FreeTypeFont___destroy___0(self);
};

// Interface: WebGLDevice

/** @suppress {undefinedVars, duplicate} @this{Object} */
function WebGLDevice() {
  this.ptr = _emscripten_bind_WebGLDevice_WebGLDevice_0();
  getCache(WebGLDevice)[this.ptr] = this;
};

WebGLDevice.prototype = Object.create(WrapperObject.prototype);
WebGLDevice.prototype.constructor = WebGLDevice;
WebGLDevice.prototype.__class__ = WebGLDevice;
WebGLDevice.__cache__ = {};
Module['WebGLDevice'] = WebGLDevice;
/** @suppress {undefinedVars, duplicate} @this{Object} */
WebGLDevice.prototype['initializeCapability'] = WebGLDevice.prototype.initializeCapability = function(data) {
  var self = this.ptr;
  ensureCache.prepare();
  if (data && typeof data === 'object') data = data.ptr;
  else data = ensureString(data);
  _emscripten_bind_WebGLDevice_initializeCapability_1(self, data);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
WebGLDevice.prototype['__destroy__'] = WebGLDevice.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_WebGLDevice___destroy___0(self);
};

// Interface: WebGL2Device

/** @suppress {undefinedVars, duplicate} @this{Object} */
function WebGL2Device() {
  this.ptr = _emscripten_bind_WebGL2Device_WebGL2Device_0();
  getCache(WebGL2Device)[this.ptr] = this;
};

WebGL2Device.prototype = Object.create(WrapperObject.prototype);
WebGL2Device.prototype.constructor = WebGL2Device;
WebGL2Device.prototype.__class__ = WebGL2Device;
WebGL2Device.__cache__ = {};
Module['WebGL2Device'] = WebGL2Device;
/** @suppress {undefinedVars, duplicate} @this{Object} */
WebGL2Device.prototype['initializeCapability'] = WebGL2Device.prototype.initializeCapability = function(data) {
  var self = this.ptr;
  ensureCache.prepare();
  if (data && typeof data === 'object') data = data.ptr;
  else data = ensureString(data);
  _emscripten_bind_WebGL2Device_initializeCapability_1(self, data);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
WebGL2Device.prototype['__destroy__'] = WebGL2Device.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_WebGL2Device___destroy___0(self);
};

// Interface: System

/** @suppress {undefinedVars, duplicate} @this{Object} */
function System() { throw "cannot construct a System, no constructor in IDL" }
System.prototype = Object.create(WrapperObject.prototype);
System.prototype.constructor = System;
System.prototype.__class__ = System;
System.__cache__ = {};
Module['System'] = System;

/** @suppress {undefinedVars, duplicate} @this{Object} */
System.prototype['__destroy__'] = System.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_System___destroy___0(self);
};

// Interface: RenderPipeline

/** @suppress {undefinedVars, duplicate} @this{Object} */
function RenderPipeline() { throw "cannot construct a RenderPipeline, no constructor in IDL" }
RenderPipeline.prototype = Object.create(WrapperObject.prototype);
RenderPipeline.prototype.constructor = RenderPipeline;
RenderPipeline.prototype.__class__ = RenderPipeline;
RenderPipeline.__cache__ = {};
Module['RenderPipeline'] = RenderPipeline;

/** @suppress {undefinedVars, duplicate} @this{Object} */
RenderPipeline.prototype['__destroy__'] = RenderPipeline.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_RenderPipeline___destroy___0(self);
};

// Interface: FireGameRenderPipeline

/** @suppress {undefinedVars, duplicate} @this{Object} */
function FireGameRenderPipeline() {
  this.ptr = _emscripten_bind_FireGameRenderPipeline_FireGameRenderPipeline_0();
  getCache(FireGameRenderPipeline)[this.ptr] = this;
};

FireGameRenderPipeline.prototype = Object.create(WrapperObject.prototype);
FireGameRenderPipeline.prototype.constructor = FireGameRenderPipeline;
FireGameRenderPipeline.prototype.__class__ = FireGameRenderPipeline;
FireGameRenderPipeline.__cache__ = {};
Module['FireGameRenderPipeline'] = FireGameRenderPipeline;
/** @suppress {undefinedVars, duplicate} @this{Object} */
FireGameRenderPipeline.prototype['enableWashRender'] = FireGameRenderPipeline.prototype.enableWashRender = function(b, waterMarkMaterialId) {
  var self = this.ptr;
  if (b && typeof b === 'object') b = b.ptr;
  if (waterMarkMaterialId && typeof waterMarkMaterialId === 'object') waterMarkMaterialId = waterMarkMaterialId.ptr;
  _emscripten_bind_FireGameRenderPipeline_enableWashRender_2(self, b, waterMarkMaterialId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
FireGameRenderPipeline.prototype['__destroy__'] = FireGameRenderPipeline.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_FireGameRenderPipeline___destroy___0(self);
};

// Interface: Application

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Application() {
  this.ptr = _emscripten_bind_Application_Application_0();
  getCache(Application)[this.ptr] = this;
};

Application.prototype = Object.create(WrapperObject.prototype);
Application.prototype.constructor = Application;
Application.prototype.__class__ = Application;
Application.__cache__ = {};
Module['Application'] = Application;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Application.prototype['initialize'] = Application.prototype.initialize = function() {
  var self = this.ptr;
  _emscripten_bind_Application_initialize_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Application.prototype['update'] = Application.prototype.update = function() {
  var self = this.ptr;
  _emscripten_bind_Application_update_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Application.prototype['postUpdate'] = Application.prototype.postUpdate = function() {
  var self = this.ptr;
  _emscripten_bind_Application_postUpdate_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Application.prototype['registerSystem'] = Application.prototype.registerSystem = function(system) {
  var self = this.ptr;
  if (system && typeof system === 'object') system = system.ptr;
  _emscripten_bind_Application_registerSystem_1(self, system);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Application.prototype['setRenderPipeline'] = Application.prototype.setRenderPipeline = function(pipeline) {
  var self = this.ptr;
  if (pipeline && typeof pipeline === 'object') pipeline = pipeline.ptr;
  _emscripten_bind_Application_setRenderPipeline_1(self, pipeline);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Application.prototype['__destroy__'] = Application.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Application___destroy___0(self);
};

// Interface: InputManager

/** @suppress {undefinedVars, duplicate} @this{Object} */
function InputManager() {
  this.ptr = _emscripten_bind_InputManager_InputManager_0();
  getCache(InputManager)[this.ptr] = this;
};

InputManager.prototype = Object.create(WrapperObject.prototype);
InputManager.prototype.constructor = InputManager;
InputManager.prototype.__class__ = InputManager;
InputManager.__cache__ = {};
Module['InputManager'] = InputManager;
/** @suppress {undefinedVars, duplicate} @this{Object} */
InputManager.prototype['setEventDataPointer'] = InputManager.prototype.setEventDataPointer = function(pointer, size) {
  var self = this.ptr;
  ensureCache.prepare();
  if (pointer && typeof pointer === 'object') pointer = pointer.ptr;
  else pointer = ensureString(pointer);
  if (size && typeof size === 'object') size = size.ptr;
  _emscripten_bind_InputManager_setEventDataPointer_2(self, pointer, size);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
InputManager.prototype['update'] = InputManager.prototype.update = function() {
  var self = this.ptr;
  _emscripten_bind_InputManager_update_0(self);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
InputManager.prototype['__destroy__'] = InputManager.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_InputManager___destroy___0(self);
};

// Interface: Screen

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Screen() {
  this.ptr = _emscripten_bind_Screen_Screen_0();
  getCache(Screen)[this.ptr] = this;
};

Screen.prototype = Object.create(WrapperObject.prototype);
Screen.prototype.constructor = Screen;
Screen.prototype.__class__ = Screen;
Screen.__cache__ = {};
Module['Screen'] = Screen;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Screen.prototype['setRenderSize'] = Screen.prototype.setRenderSize = function(width, height) {
  var self = this.ptr;
  if (width && typeof width === 'object') width = width.ptr;
  if (height && typeof height === 'object') height = height.ptr;
  _emscripten_bind_Screen_setRenderSize_2(self, width, height);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Screen.prototype['setDesignSize'] = Screen.prototype.setDesignSize = function(width, height) {
  var self = this.ptr;
  if (width && typeof width === 'object') width = width.ptr;
  if (height && typeof height === 'object') height = height.ptr;
  _emscripten_bind_Screen_setDesignSize_2(self, width, height);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Screen.prototype['__destroy__'] = Screen.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Screen___destroy___0(self);
};

// Interface: PhysicsSystem

/** @suppress {undefinedVars, duplicate} @this{Object} */
function PhysicsSystem() {
  this.ptr = _emscripten_bind_PhysicsSystem_PhysicsSystem_0();
  getCache(PhysicsSystem)[this.ptr] = this;
};

PhysicsSystem.prototype = Object.create(WrapperObject.prototype);
PhysicsSystem.prototype.constructor = PhysicsSystem;
PhysicsSystem.prototype.__class__ = PhysicsSystem;
PhysicsSystem.__cache__ = {};
Module['PhysicsSystem'] = PhysicsSystem;

/** @suppress {undefinedVars, duplicate} @this{Object} */
PhysicsSystem.prototype['__destroy__'] = PhysicsSystem.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_PhysicsSystem___destroy___0(self);
};

// Interface: SkeletonSystem

/** @suppress {undefinedVars, duplicate} @this{Object} */
function SkeletonSystem() {
  this.ptr = _emscripten_bind_SkeletonSystem_SkeletonSystem_0();
  getCache(SkeletonSystem)[this.ptr] = this;
};

SkeletonSystem.prototype = Object.create(WrapperObject.prototype);
SkeletonSystem.prototype.constructor = SkeletonSystem;
SkeletonSystem.prototype.__class__ = SkeletonSystem;
SkeletonSystem.__cache__ = {};
Module['SkeletonSystem'] = SkeletonSystem;

/** @suppress {undefinedVars, duplicate} @this{Object} */
SkeletonSystem.prototype['__destroy__'] = SkeletonSystem.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_SkeletonSystem___destroy___0(self);
};

// Interface: TweenSystem

/** @suppress {undefinedVars, duplicate} @this{Object} */
function TweenSystem() {
  this.ptr = _emscripten_bind_TweenSystem_TweenSystem_0();
  getCache(TweenSystem)[this.ptr] = this;
};

TweenSystem.prototype = Object.create(WrapperObject.prototype);
TweenSystem.prototype.constructor = TweenSystem;
TweenSystem.prototype.__class__ = TweenSystem;
TweenSystem.__cache__ = {};
Module['TweenSystem'] = TweenSystem;

/** @suppress {undefinedVars, duplicate} @this{Object} */
TweenSystem.prototype['__destroy__'] = TweenSystem.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_TweenSystem___destroy___0(self);
};

// Interface: Truck

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Truck() {
  this.ptr = _emscripten_bind_Truck_Truck_0();
  getCache(Truck)[this.ptr] = this;
};

Truck.prototype = Object.create(WrapperObject.prototype);
Truck.prototype.constructor = Truck;
Truck.prototype.__class__ = Truck;
Truck.__cache__ = {};
Module['Truck'] = Truck;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['initFromJs'] = Truck.prototype.initFromJs = function(data, partCount) {
  var self = this.ptr;
  ensureCache.prepare();
  if (data && typeof data === 'object') data = data.ptr;
  else data = ensureString(data);
  if (partCount && typeof partCount === 'object') partCount = partCount.ptr;
  _emscripten_bind_Truck_initFromJs_2(self, data, partCount);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['setLift'] = Truck.prototype.setLift = function(distance) {
  var self = this.ptr;
  if (distance && typeof distance === 'object') distance = distance.ptr;
  _emscripten_bind_Truck_setLift_1(self, distance);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['moveForward'] = Truck.prototype.moveForward = function() {
  var self = this.ptr;
  _emscripten_bind_Truck_moveForward_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['moveBack'] = Truck.prototype.moveBack = function() {
  var self = this.ptr;
  _emscripten_bind_Truck_moveBack_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['stop'] = Truck.prototype.stop = function() {
  var self = this.ptr;
  _emscripten_bind_Truck_stop_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['setSpeed'] = Truck.prototype.setSpeed = function(speed) {
  var self = this.ptr;
  if (speed && typeof speed === 'object') speed = speed.ptr;
  _emscripten_bind_Truck_setSpeed_1(self, speed);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['setSlop'] = Truck.prototype.setSlop = function(min, max) {
  var self = this.ptr;
  if (min && typeof min === 'object') min = min.ptr;
  if (max && typeof max === 'object') max = max.ptr;
  _emscripten_bind_Truck_setSlop_2(self, min, max);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['setCollisionFilter'] = Truck.prototype.setCollisionFilter = function(categoryBits, maskBits) {
  var self = this.ptr;
  if (categoryBits && typeof categoryBits === 'object') categoryBits = categoryBits.ptr;
  if (maskBits && typeof maskBits === 'object') maskBits = maskBits.ptr;
  _emscripten_bind_Truck_setCollisionFilter_2(self, categoryBits, maskBits);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['sparge'] = Truck.prototype.sparge = function() {
  var self = this.ptr;
  _emscripten_bind_Truck_sparge_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['stopSpare'] = Truck.prototype.stopSpare = function() {
  var self = this.ptr;
  _emscripten_bind_Truck_stopSpare_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['setFluidMaterial'] = Truck.prototype.setFluidMaterial = function(id, renderType) {
  var self = this.ptr;
  if (id && typeof id === 'object') id = id.ptr;
  if (renderType && typeof renderType === 'object') renderType = renderType.ptr;
  _emscripten_bind_Truck_setFluidMaterial_2(self, id, renderType);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['setCollisionFuildFilter'] = Truck.prototype.setCollisionFuildFilter = function(group, mask) {
  var self = this.ptr;
  if (group && typeof group === 'object') group = group.ptr;
  if (mask && typeof mask === 'object') mask = mask.ptr;
  _emscripten_bind_Truck_setCollisionFuildFilter_2(self, group, mask);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['setWaterParticleAmout'] = Truck.prototype.setWaterParticleAmout = function(amout) {
  var self = this.ptr;
  if (amout && typeof amout === 'object') amout = amout.ptr;
  _emscripten_bind_Truck_setWaterParticleAmout_1(self, amout);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['setFuildViewportPos'] = Truck.prototype.setFuildViewportPos = function(x, y) {
  var self = this.ptr;
  if (x && typeof x === 'object') x = x.ptr;
  if (y && typeof y === 'object') y = y.ptr;
  _emscripten_bind_Truck_setFuildViewportPos_2(self, x, y);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['setFuildViewportSize'] = Truck.prototype.setFuildViewportSize = function(x, y) {
  var self = this.ptr;
  if (x && typeof x === 'object') x = x.ptr;
  if (y && typeof y === 'object') y = y.ptr;
  _emscripten_bind_Truck_setFuildViewportSize_2(self, x, y);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['playExplosion'] = Truck.prototype.playExplosion = function() {
  var self = this.ptr;
  _emscripten_bind_Truck_playExplosion_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['recoveryFormExposion'] = Truck.prototype.recoveryFormExposion = function() {
  var self = this.ptr;
  _emscripten_bind_Truck_recoveryFormExposion_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['active'] = Truck.prototype.active = function(active) {
  var self = this.ptr;
  if (active && typeof active === 'object') active = active.ptr;
  _emscripten_bind_Truck_active_1(self, active);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['setSpareSpeed'] = Truck.prototype.setSpareSpeed = function(speed) {
  var self = this.ptr;
  if (speed && typeof speed === 'object') speed = speed.ptr;
  _emscripten_bind_Truck_setSpareSpeed_1(self, speed);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Truck.prototype['__destroy__'] = Truck.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Truck___destroy___0(self);
};

// Interface: Level_1

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_1() {
  this.ptr = _emscripten_bind_Level_1_Level_1_0();
  getCache(Level_1)[this.ptr] = this;
};

Level_1.prototype = Object.create(WrapperObject.prototype);
Level_1.prototype.constructor = Level_1;
Level_1.prototype.__class__ = Level_1;
Level_1.__cache__ = {};
Module['Level_1'] = Level_1;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_1.prototype['init'] = Level_1.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_1_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_1.prototype['cut'] = Level_1.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_1_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_1.prototype['setParentNodeFromJs'] = Level_1.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_1_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_1.prototype['__destroy__'] = Level_1.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_1___destroy___0(self);
};

// Interface: Level_2

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_2() {
  this.ptr = _emscripten_bind_Level_2_Level_2_0();
  getCache(Level_2)[this.ptr] = this;
};

Level_2.prototype = Object.create(WrapperObject.prototype);
Level_2.prototype.constructor = Level_2;
Level_2.prototype.__class__ = Level_2;
Level_2.__cache__ = {};
Module['Level_2'] = Level_2;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_2.prototype['init'] = Level_2.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_2_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_2.prototype['cut'] = Level_2.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_2_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_2.prototype['setParentNodeFromJs'] = Level_2.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_2_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_2.prototype['__destroy__'] = Level_2.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_2___destroy___0(self);
};

// Interface: Level_3

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_3() {
  this.ptr = _emscripten_bind_Level_3_Level_3_0();
  getCache(Level_3)[this.ptr] = this;
};

Level_3.prototype = Object.create(WrapperObject.prototype);
Level_3.prototype.constructor = Level_3;
Level_3.prototype.__class__ = Level_3;
Level_3.__cache__ = {};
Module['Level_3'] = Level_3;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_3.prototype['init'] = Level_3.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_3_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_3.prototype['cut'] = Level_3.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_3_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_3.prototype['setParentNodeFromJs'] = Level_3.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_3_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_3.prototype['__destroy__'] = Level_3.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_3___destroy___0(self);
};

// Interface: Level_4

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_4() {
  this.ptr = _emscripten_bind_Level_4_Level_4_0();
  getCache(Level_4)[this.ptr] = this;
};

Level_4.prototype = Object.create(WrapperObject.prototype);
Level_4.prototype.constructor = Level_4;
Level_4.prototype.__class__ = Level_4;
Level_4.__cache__ = {};
Module['Level_4'] = Level_4;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_4.prototype['init'] = Level_4.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_4_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_4.prototype['cut'] = Level_4.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_4_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_4.prototype['setParentNodeFromJs'] = Level_4.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_4_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_4.prototype['__destroy__'] = Level_4.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_4___destroy___0(self);
};

// Interface: Level_5

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_5() {
  this.ptr = _emscripten_bind_Level_5_Level_5_0();
  getCache(Level_5)[this.ptr] = this;
};

Level_5.prototype = Object.create(WrapperObject.prototype);
Level_5.prototype.constructor = Level_5;
Level_5.prototype.__class__ = Level_5;
Level_5.__cache__ = {};
Module['Level_5'] = Level_5;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_5.prototype['init'] = Level_5.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_5_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_5.prototype['cut'] = Level_5.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_5_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_5.prototype['setParentNodeFromJs'] = Level_5.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_5_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_5.prototype['__destroy__'] = Level_5.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_5___destroy___0(self);
};

// Interface: Level_6

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_6() {
  this.ptr = _emscripten_bind_Level_6_Level_6_0();
  getCache(Level_6)[this.ptr] = this;
};

Level_6.prototype = Object.create(WrapperObject.prototype);
Level_6.prototype.constructor = Level_6;
Level_6.prototype.__class__ = Level_6;
Level_6.__cache__ = {};
Module['Level_6'] = Level_6;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_6.prototype['init'] = Level_6.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_6_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_6.prototype['cut'] = Level_6.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_6_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_6.prototype['setParentNodeFromJs'] = Level_6.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_6_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_6.prototype['__destroy__'] = Level_6.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_6___destroy___0(self);
};

// Interface: Level_7

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_7() {
  this.ptr = _emscripten_bind_Level_7_Level_7_0();
  getCache(Level_7)[this.ptr] = this;
};

Level_7.prototype = Object.create(WrapperObject.prototype);
Level_7.prototype.constructor = Level_7;
Level_7.prototype.__class__ = Level_7;
Level_7.__cache__ = {};
Module['Level_7'] = Level_7;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_7.prototype['init'] = Level_7.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_7_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_7.prototype['cut'] = Level_7.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_7_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_7.prototype['setParentNodeFromJs'] = Level_7.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_7_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_7.prototype['__destroy__'] = Level_7.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_7___destroy___0(self);
};

// Interface: Level_8

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_8() {
  this.ptr = _emscripten_bind_Level_8_Level_8_0();
  getCache(Level_8)[this.ptr] = this;
};

Level_8.prototype = Object.create(WrapperObject.prototype);
Level_8.prototype.constructor = Level_8;
Level_8.prototype.__class__ = Level_8;
Level_8.__cache__ = {};
Module['Level_8'] = Level_8;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_8.prototype['init'] = Level_8.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_8_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_8.prototype['cut'] = Level_8.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_8_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_8.prototype['setParentNodeFromJs'] = Level_8.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_8_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_8.prototype['__destroy__'] = Level_8.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_8___destroy___0(self);
};

// Interface: Level_9

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_9() {
  this.ptr = _emscripten_bind_Level_9_Level_9_0();
  getCache(Level_9)[this.ptr] = this;
};

Level_9.prototype = Object.create(WrapperObject.prototype);
Level_9.prototype.constructor = Level_9;
Level_9.prototype.__class__ = Level_9;
Level_9.__cache__ = {};
Module['Level_9'] = Level_9;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_9.prototype['init'] = Level_9.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_9_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_9.prototype['cut'] = Level_9.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_9_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_9.prototype['setParentNodeFromJs'] = Level_9.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_9_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_9.prototype['__destroy__'] = Level_9.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_9___destroy___0(self);
};

// Interface: Level_C1_10

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_10() {
  this.ptr = _emscripten_bind_Level_C1_10_Level_C1_10_0();
  getCache(Level_C1_10)[this.ptr] = this;
};

Level_C1_10.prototype = Object.create(WrapperObject.prototype);
Level_C1_10.prototype.constructor = Level_C1_10;
Level_C1_10.prototype.__class__ = Level_C1_10;
Level_C1_10.__cache__ = {};
Module['Level_C1_10'] = Level_C1_10;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_10.prototype['init'] = Level_C1_10.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_10_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_10.prototype['cut'] = Level_C1_10.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_10_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_10.prototype['setParentNodeFromJs'] = Level_C1_10.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_10_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_10.prototype['__destroy__'] = Level_C1_10.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_10___destroy___0(self);
};

// Interface: Level_C1_11

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_11() {
  this.ptr = _emscripten_bind_Level_C1_11_Level_C1_11_0();
  getCache(Level_C1_11)[this.ptr] = this;
};

Level_C1_11.prototype = Object.create(WrapperObject.prototype);
Level_C1_11.prototype.constructor = Level_C1_11;
Level_C1_11.prototype.__class__ = Level_C1_11;
Level_C1_11.__cache__ = {};
Module['Level_C1_11'] = Level_C1_11;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_11.prototype['init'] = Level_C1_11.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_11_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_11.prototype['cut'] = Level_C1_11.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_11_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_11.prototype['setParentNodeFromJs'] = Level_C1_11.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_11_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_11.prototype['__destroy__'] = Level_C1_11.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_11___destroy___0(self);
};

// Interface: Level_C1_12

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_12() {
  this.ptr = _emscripten_bind_Level_C1_12_Level_C1_12_0();
  getCache(Level_C1_12)[this.ptr] = this;
};

Level_C1_12.prototype = Object.create(WrapperObject.prototype);
Level_C1_12.prototype.constructor = Level_C1_12;
Level_C1_12.prototype.__class__ = Level_C1_12;
Level_C1_12.__cache__ = {};
Module['Level_C1_12'] = Level_C1_12;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_12.prototype['init'] = Level_C1_12.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_12_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_12.prototype['cut'] = Level_C1_12.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_12_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_12.prototype['setParentNodeFromJs'] = Level_C1_12.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_12_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_12.prototype['__destroy__'] = Level_C1_12.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_12___destroy___0(self);
};

// Interface: Level_C1_13

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_13() {
  this.ptr = _emscripten_bind_Level_C1_13_Level_C1_13_0();
  getCache(Level_C1_13)[this.ptr] = this;
};

Level_C1_13.prototype = Object.create(WrapperObject.prototype);
Level_C1_13.prototype.constructor = Level_C1_13;
Level_C1_13.prototype.__class__ = Level_C1_13;
Level_C1_13.__cache__ = {};
Module['Level_C1_13'] = Level_C1_13;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_13.prototype['init'] = Level_C1_13.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_13_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_13.prototype['cut'] = Level_C1_13.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_13_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_13.prototype['setParentNodeFromJs'] = Level_C1_13.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_13_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_13.prototype['__destroy__'] = Level_C1_13.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_13___destroy___0(self);
};

// Interface: Level_C1_14

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_14() {
  this.ptr = _emscripten_bind_Level_C1_14_Level_C1_14_0();
  getCache(Level_C1_14)[this.ptr] = this;
};

Level_C1_14.prototype = Object.create(WrapperObject.prototype);
Level_C1_14.prototype.constructor = Level_C1_14;
Level_C1_14.prototype.__class__ = Level_C1_14;
Level_C1_14.__cache__ = {};
Module['Level_C1_14'] = Level_C1_14;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_14.prototype['init'] = Level_C1_14.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_14_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_14.prototype['cut'] = Level_C1_14.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_14_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_14.prototype['setParentNodeFromJs'] = Level_C1_14.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_14_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_14.prototype['__destroy__'] = Level_C1_14.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_14___destroy___0(self);
};

// Interface: Level_C1_15

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_15() {
  this.ptr = _emscripten_bind_Level_C1_15_Level_C1_15_0();
  getCache(Level_C1_15)[this.ptr] = this;
};

Level_C1_15.prototype = Object.create(WrapperObject.prototype);
Level_C1_15.prototype.constructor = Level_C1_15;
Level_C1_15.prototype.__class__ = Level_C1_15;
Level_C1_15.__cache__ = {};
Module['Level_C1_15'] = Level_C1_15;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_15.prototype['init'] = Level_C1_15.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_15_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_15.prototype['cut'] = Level_C1_15.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_15_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_15.prototype['setParentNodeFromJs'] = Level_C1_15.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_15_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_15.prototype['__destroy__'] = Level_C1_15.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_15___destroy___0(self);
};

// Interface: Level_C1_16

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_16() {
  this.ptr = _emscripten_bind_Level_C1_16_Level_C1_16_0();
  getCache(Level_C1_16)[this.ptr] = this;
};

Level_C1_16.prototype = Object.create(WrapperObject.prototype);
Level_C1_16.prototype.constructor = Level_C1_16;
Level_C1_16.prototype.__class__ = Level_C1_16;
Level_C1_16.__cache__ = {};
Module['Level_C1_16'] = Level_C1_16;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_16.prototype['init'] = Level_C1_16.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_16_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_16.prototype['cut'] = Level_C1_16.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_16_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_16.prototype['setParentNodeFromJs'] = Level_C1_16.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_16_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_16.prototype['__destroy__'] = Level_C1_16.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_16___destroy___0(self);
};

// Interface: Level_C1_17

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_17() {
  this.ptr = _emscripten_bind_Level_C1_17_Level_C1_17_0();
  getCache(Level_C1_17)[this.ptr] = this;
};

Level_C1_17.prototype = Object.create(WrapperObject.prototype);
Level_C1_17.prototype.constructor = Level_C1_17;
Level_C1_17.prototype.__class__ = Level_C1_17;
Level_C1_17.__cache__ = {};
Module['Level_C1_17'] = Level_C1_17;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_17.prototype['init'] = Level_C1_17.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_17_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_17.prototype['cut'] = Level_C1_17.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_17_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_17.prototype['setParentNodeFromJs'] = Level_C1_17.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_17_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_17.prototype['__destroy__'] = Level_C1_17.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_17___destroy___0(self);
};

// Interface: Level_C1_18

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_18() {
  this.ptr = _emscripten_bind_Level_C1_18_Level_C1_18_0();
  getCache(Level_C1_18)[this.ptr] = this;
};

Level_C1_18.prototype = Object.create(WrapperObject.prototype);
Level_C1_18.prototype.constructor = Level_C1_18;
Level_C1_18.prototype.__class__ = Level_C1_18;
Level_C1_18.__cache__ = {};
Module['Level_C1_18'] = Level_C1_18;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_18.prototype['init'] = Level_C1_18.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_18_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_18.prototype['cut'] = Level_C1_18.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_18_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_18.prototype['setParentNodeFromJs'] = Level_C1_18.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_18_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_18.prototype['__destroy__'] = Level_C1_18.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_18___destroy___0(self);
};

// Interface: Level_C1_19

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_19() {
  this.ptr = _emscripten_bind_Level_C1_19_Level_C1_19_0();
  getCache(Level_C1_19)[this.ptr] = this;
};

Level_C1_19.prototype = Object.create(WrapperObject.prototype);
Level_C1_19.prototype.constructor = Level_C1_19;
Level_C1_19.prototype.__class__ = Level_C1_19;
Level_C1_19.__cache__ = {};
Module['Level_C1_19'] = Level_C1_19;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_19.prototype['init'] = Level_C1_19.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_19_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_19.prototype['cut'] = Level_C1_19.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_19_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_19.prototype['setParentNodeFromJs'] = Level_C1_19.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_19_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_19.prototype['__destroy__'] = Level_C1_19.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_19___destroy___0(self);
};

// Interface: Level_C1_20

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_20() {
  this.ptr = _emscripten_bind_Level_C1_20_Level_C1_20_0();
  getCache(Level_C1_20)[this.ptr] = this;
};

Level_C1_20.prototype = Object.create(WrapperObject.prototype);
Level_C1_20.prototype.constructor = Level_C1_20;
Level_C1_20.prototype.__class__ = Level_C1_20;
Level_C1_20.__cache__ = {};
Module['Level_C1_20'] = Level_C1_20;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_20.prototype['init'] = Level_C1_20.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_20_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_20.prototype['cut'] = Level_C1_20.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_20_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_20.prototype['setParentNodeFromJs'] = Level_C1_20.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_20_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_20.prototype['__destroy__'] = Level_C1_20.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_20___destroy___0(self);
};

// Interface: Level_C1_21

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_21() {
  this.ptr = _emscripten_bind_Level_C1_21_Level_C1_21_0();
  getCache(Level_C1_21)[this.ptr] = this;
};

Level_C1_21.prototype = Object.create(WrapperObject.prototype);
Level_C1_21.prototype.constructor = Level_C1_21;
Level_C1_21.prototype.__class__ = Level_C1_21;
Level_C1_21.__cache__ = {};
Module['Level_C1_21'] = Level_C1_21;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_21.prototype['init'] = Level_C1_21.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_21_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_21.prototype['cut'] = Level_C1_21.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_21_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_21.prototype['setParentNodeFromJs'] = Level_C1_21.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_21_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_21.prototype['__destroy__'] = Level_C1_21.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_21___destroy___0(self);
};

// Interface: Level_C1_22

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_22() {
  this.ptr = _emscripten_bind_Level_C1_22_Level_C1_22_0();
  getCache(Level_C1_22)[this.ptr] = this;
};

Level_C1_22.prototype = Object.create(WrapperObject.prototype);
Level_C1_22.prototype.constructor = Level_C1_22;
Level_C1_22.prototype.__class__ = Level_C1_22;
Level_C1_22.__cache__ = {};
Module['Level_C1_22'] = Level_C1_22;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_22.prototype['init'] = Level_C1_22.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_22_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_22.prototype['cut'] = Level_C1_22.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_22_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_22.prototype['setParentNodeFromJs'] = Level_C1_22.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_22_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_22.prototype['__destroy__'] = Level_C1_22.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_22___destroy___0(self);
};

// Interface: Level_C1_23

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_23() {
  this.ptr = _emscripten_bind_Level_C1_23_Level_C1_23_0();
  getCache(Level_C1_23)[this.ptr] = this;
};

Level_C1_23.prototype = Object.create(WrapperObject.prototype);
Level_C1_23.prototype.constructor = Level_C1_23;
Level_C1_23.prototype.__class__ = Level_C1_23;
Level_C1_23.__cache__ = {};
Module['Level_C1_23'] = Level_C1_23;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_23.prototype['init'] = Level_C1_23.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_23_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_23.prototype['cut'] = Level_C1_23.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_23_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_23.prototype['setParentNodeFromJs'] = Level_C1_23.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_23_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_23.prototype['__destroy__'] = Level_C1_23.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_23___destroy___0(self);
};

// Interface: Level_C1_24

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_24() {
  this.ptr = _emscripten_bind_Level_C1_24_Level_C1_24_0();
  getCache(Level_C1_24)[this.ptr] = this;
};

Level_C1_24.prototype = Object.create(WrapperObject.prototype);
Level_C1_24.prototype.constructor = Level_C1_24;
Level_C1_24.prototype.__class__ = Level_C1_24;
Level_C1_24.__cache__ = {};
Module['Level_C1_24'] = Level_C1_24;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_24.prototype['init'] = Level_C1_24.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_24_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_24.prototype['cut'] = Level_C1_24.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_24_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_24.prototype['setParentNodeFromJs'] = Level_C1_24.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_24_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_24.prototype['__destroy__'] = Level_C1_24.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_24___destroy___0(self);
};

// Interface: Level_C1_25

/** @suppress {undefinedVars, duplicate} @this{Object} */
function Level_C1_25() {
  this.ptr = _emscripten_bind_Level_C1_25_Level_C1_25_0();
  getCache(Level_C1_25)[this.ptr] = this;
};

Level_C1_25.prototype = Object.create(WrapperObject.prototype);
Level_C1_25.prototype.constructor = Level_C1_25;
Level_C1_25.prototype.__class__ = Level_C1_25;
Level_C1_25.__cache__ = {};
Module['Level_C1_25'] = Level_C1_25;
/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_25.prototype['init'] = Level_C1_25.prototype.init = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_25_init_0(self);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_25.prototype['cut'] = Level_C1_25.prototype.cut = function(x1, y1, x2, y2) {
  var self = this.ptr;
  if (x1 && typeof x1 === 'object') x1 = x1.ptr;
  if (y1 && typeof y1 === 'object') y1 = y1.ptr;
  if (x2 && typeof x2 === 'object') x2 = x2.ptr;
  if (y2 && typeof y2 === 'object') y2 = y2.ptr;
  return _emscripten_bind_Level_C1_25_cut_4(self, x1, y1, x2, y2);
};

/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_25.prototype['setParentNodeFromJs'] = Level_C1_25.prototype.setParentNodeFromJs = function(nativeId) {
  var self = this.ptr;
  if (nativeId && typeof nativeId === 'object') nativeId = nativeId.ptr;
  _emscripten_bind_Level_C1_25_setParentNodeFromJs_1(self, nativeId);
};


/** @suppress {undefinedVars, duplicate} @this{Object} */
Level_C1_25.prototype['__destroy__'] = Level_C1_25.prototype.__destroy__ = function() {
  var self = this.ptr;
  _emscripten_bind_Level_C1_25___destroy___0(self);
};
