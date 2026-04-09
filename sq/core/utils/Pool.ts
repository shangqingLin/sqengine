import js from "./js";

export interface PoolObject {
    onRecovery(): void;
    onDestroy(): void;
}

class Pool {
    private poolMap: { [key: number]: Array<PoolObject> } = js.createMap();
    private static keyName: string = "____poolKey";
    private static key: number = 0;

    private getKey(cls: Constructor<PoolObject>): number {
        //@ts-ignore
        let key: number = cls[Pool.keyName];
        if (key === undefined) {
            key = ++Pool.key;
            //@ts-ignore
            cls[Pool.keyName] = key;
        }
        return key;
    }

    recovery(obj: PoolObject) {
        let cls: Constructor<PoolObject> = js.getInstanceConstructor(obj) as Constructor<PoolObject>;
        let key: number = this.getKey(cls);
        let cache = this.poolMap[key];
        if (!cache) {
            cache = [];
            this.poolMap[key] = cache;
        }
        obj.onRecovery();
        cache.push(obj);
    }

    get(cls: Constructor<PoolObject>) {
        let key: number = this.getKey(cls);
        let cache = this.poolMap[key];
        let obj;
        if (cache) {
            obj = cache.pop();
        }
        if (!obj) {
            obj = new cls();
        }
        return obj;
    }

    clear(cls: Constructor<PoolObject>) {
        let key: number = this.getKey(cls);
        let cache = this.poolMap[key];
        if (cache) {
            for (let i = 0, n = cache.length; i < n; ++i) {
                cache[i].onDestroy();
            }
            this.poolMap[key] = null;
        }
    }
}
const pool = new Pool;
export {
    pool
}