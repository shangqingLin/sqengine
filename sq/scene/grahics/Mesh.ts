import { Byte } from "../../core";
import { dispatch, JsToNativeObjectBridge, ObjectType } from "../../native_binding";
import IndexBuffer from "./IndexBuffer";
import type { VertexAttributeDescriptor } from "./VertexAttributeDescriptor";
import VertexBuffer from "./VertexBuffer";

export class Mesh {

    private vertexBuffers: Array<VertexBuffer> = [];
    private indexBuffer: IndexBuffer;
    private nativeObject: JsToNativeObjectBridge;
    constructor() {
        this.nativeObject = dispatch.createJsToNativeObject(ObjectType.Mesh, this);
        this.nativeObject.beginOp(1);
        this.nativeObject.endOp();
    }

    getIndexBuffer(): Readonly<IndexBuffer> {
        return this.indexBuffer;
    }
    
    setIndexBuffer(index: IndexBuffer): void {
        if (this.indexBuffer == index) return;
        this.indexBuffer = index;
        this.nativeObject.beginOp(4);
        //@ts-ignore
        this.nativeObject.writeOpArg("i32", index.nativeObject.getId());
        this.nativeObject.endOp();
    }

    addVertexBuffer(vertexBuffer: VertexBuffer, attribute?: VertexAttributeDescriptor | Array<VertexAttributeDescriptor>): void {
        if (this.vertexBuffers.indexOf(vertexBuffer) !== -1) return;
        this.vertexBuffers.push(vertexBuffer);

        this.nativeObject.beginOp(3);
        //@ts-ignore
        this.nativeObject.writeOpArg("i32", vertexBuffer.nativeObject.getId());
        let attributes: Array<VertexAttributeDescriptor> = attribute as Array<VertexAttributeDescriptor>;
        if (!Array.isArray(attribute)) {
            attributes = [attribute];
        }

        let num = attributes.length;
        this.nativeObject.writeOpArg("i32", num);
        for (let i = 0; i < num; ++i) {
            let a: VertexAttributeDescriptor = attributes[i];
            this.nativeObject.writeOpArg("str", a.name);
            this.nativeObject.writeOpArg("i32", a.format);
            if (a.custom) {
                this.nativeObject.writeOpArg("i8", 1);
                this.nativeObject.writeOpArg("i32", a.custom.offset || 0);
                this.nativeObject.writeOpArg("i32", a.custom.count || 0);
                this.nativeObject.writeOpArg("i32", a.custom.stride || 0);
                this.nativeObject.writeOpArg("i32", a.custom.type || 0);
                this.nativeObject.writeOpArg("i32", a.custom.instanceStride || 0);
            } else {
                this.nativeObject.writeOpArg("i8", 0);
            }
        }
        this.nativeObject.endOp();
    }

    /**
     * 如果Mesh不使用的话必须手动调用destroy
     */
    destroy() {
        this.nativeObject.beginOp(2);
        this.nativeObject.endOp();
        this.nativeObject.destroy();
        this.nativeObject = null;
    }
}