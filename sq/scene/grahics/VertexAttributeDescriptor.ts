import { Format } from "../../gfx";


export interface AttributeCustomSetting
{
    offset:number;
    count:number;
    stride:number;
    type:Format;
    instanceStride:number;
};


export interface VertexAttributeDescriptor
{
    name:string;
    format:Format;
    custom?:AttributeCustomSetting;
}