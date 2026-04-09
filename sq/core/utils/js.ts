

let _nameToClass: Record<string, Constructor> = Object.create(null);

export default {

    /**
     * 在JS中，class和Function都同属于同一种类型，通过这个方法可以判断是class还是function
     * 如果是Class，则toString输出会以class开头，比如 : class Test{}; Test.toString() = "class Test{}"，通过此来区分是Function还是class
     * 而function toString输出是以function开头的
     * @param fn 
     * @returns {Boolean}
     */
    isFunction(fn: any): boolean {
        return typeof fn === 'function' && (/^function.*/.test(fn.toString()));
    },
    isClass(fn: any): boolean {
        return typeof fn === 'function' && (/^class.*/.test(fn.toString()));
    },

    /**
     * 获取constructor类的父类的构造函数
     * @param constructor 
     * @returns 
     */
    getSuper(constructor: Function): any {
        //由原型链继承可以知道，子类的原型是父类的原型的一个实例
        /**
         * function A{}
         * 
         * function B{}
         * 
         * var a = new A;
         * B.prototype = a;
         * B的原型为一个A的实例
         * 所以，我需要找到a实例的原型，通过getPrototypeOf可以找到，
         * 刚好a实例的原型为A类的原型，这样就找到了父类的原型了
         */

        //这一句相当于获取实例a
        const proto = constructor.prototype;

        //这一句从实例a中得到a的类的原型。从而得到构造函数
        const superProto = proto && Object.getPrototypeOf(proto);
        return superProto && superProto.constructor;
    },

    getSuperByInstance(obj: any): any {
        let constructor = this.getInstanceConstructor(obj);
        return this.getSuper(constructor);
    },

    getPrototypeByInstance(obj: any) {
        return obj.__proto__ || Object.getPrototypeOf(obj);
    },

    getPrototypeByCls(cls: Constructor | Function) {
        return cls.prototype || cls.constructor.prototype;
    },
    getInstanceConstructor(obj: any): Constructor {
        return this.getPrototypeByInstance(obj).constructor;
    },
    setClassByName(name: string, cls: Constructor) {

        /* debug:start */
        if (_nameToClass[name]) throw Error("类名重复注册：" + name);
        /* debug:end */

        _nameToClass[name] = cls;
    },
    getClassByName(name: string): Constructor {
        return _nameToClass[name];
    },
    getAllClasses(): Array<Constructor> {
        let keys = Object.keys(_nameToClass);
        let classes = new Array(keys.length);
        for (let i = 0, n = keys.length; i < n; ++i) {
            classes[i] = _nameToClass[keys[i]];
        }
        return classes;
    },


    /**
     * 这种对象相对于new Object、{}来说是一个非常轻量级的对象，他不会继承Object，
     * 所以没有Object上的任何函数，也没有原型链，所以搜索起来效率高。
     * @returns 
     */
    createMap() {
        return Object.create(null);
    },

    /**
     * 判断一个对象是一个普通的Object、{}还是由Class实例化得来的
     * @param obj 
     * @returns 
     */
    isPlainObject(obj: any): Boolean {
        return Object.getPrototypeOf(obj) === Object.prototype;
    }
} as {
    createMap: () => any,
    getAllClasses: () => Array<Constructor>,
    getClassByName: (name: string) => Constructor,
    setClassByName: (name: string, cls: Constructor) => void,
    getPrototypeByInstance: (obj: any) => any,
    getInstanceConstructor: (obj: any) => Constructor,
    getPrototypeByCls: (cls: Constructor | Function) => any,
    getSuperByInstance: (obj: any) => any,
    getSuper: (constructor: Function) => any,
    isPlainObject(obj: any): Boolean,
    isFunction(fn: any): boolean,
    isClass(fn: any): boolean
}