
export function isNumber (object: any): boolean {
    return typeof object === 'number' || object instanceof Number;
}


export function isString (object: any): boolean {
    return typeof object === 'string' || object instanceof String;
}


export function isEmptyObject (obj: any): boolean {
    for (const key in obj) {
        return false;
    }
    return true;
}

export const value = ((): (object: Record<string | number, any>, propertyName: string, value_: any, writable?: boolean, enumerable?: boolean) => void => {
    const descriptor: PropertyDescriptor = {
        value: undefined,
        enumerable: false,
        writable: false,
        configurable: true,
    };
    return (object: Record<string | number, any>, propertyName: string, value_: any, writable?: boolean, enumerable?: boolean): void => {
        descriptor.value = value_;
        descriptor.writable = writable;
        descriptor.enumerable = enumerable;
        Object.defineProperty(object, propertyName, descriptor);
        descriptor.value = undefined;
    };
})();
