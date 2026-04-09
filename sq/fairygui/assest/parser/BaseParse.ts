import { GObject } from "../../GObject";
import { ByteBuffer } from "../../utils/ByteBuffer";
export abstract class BaseParse {

    public async constructFromResourceAsync(g: GObject): Promise<void> { return Promise.resolve(); };
    public constructFromResource(g: GObject): void { };
    protected async parseSetupBeforeAsync(g: GObject, beginPos: number, buffer: ByteBuffer) { }
    protected parseSetupBefore(g: GObject, beginPos: number, buffer: ByteBuffer): void { }

    protected async parseSetupAfterAsync(g: GObject, beginPos: number, fileBufferData: ByteBuffer): Promise<void> { }
    protected parseSetupAfter(g: GObject, beginPos: number, fileBufferData: ByteBuffer): void { }

}