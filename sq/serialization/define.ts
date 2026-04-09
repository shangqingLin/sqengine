
type SerializedFieldValue = string | number | boolean | null | SerializedObjectReference;

export type SerializedTypedObject = {
    __type__: string;
    fileId?: string;
} & Record<string, SerializedFieldValue>

export type SerializedObjectReference = {
    __type__?: string;
    __id__?: number,
    __uuid__?: string
}

export type SerializedValue = SerializedTypedObject | SerializedObjectReference | SerializedValue[] | string | number | boolean | null | any;



// export type SerializedData = SerializedValue | Array<SerializedValue>;


export interface DeserializeDependInfo {
    uuid: string;
    obj: any | Array<any>;
    name: string | number;
}

export interface DeserializeResult {
    objects: Array<Object> | Object;
    depend?: Array<DeserializeDependInfo>;
}