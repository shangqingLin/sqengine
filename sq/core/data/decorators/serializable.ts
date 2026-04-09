import { getOrCreateProperty, PropertyStash, PropertyStashFlag } from "./define";

export const serializable: (target: any, propertyKey: string) => void = function (target: any, propertyKey: string): void {
    let prop: PropertyStash = getOrCreateProperty(target, propertyKey);
    prop.flag |= PropertyStashFlag.SERIALIZABLE;
};