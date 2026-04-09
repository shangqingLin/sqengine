import { Vec2 } from "../../../core";
import { GObject } from "../../GObject";
import { GTextField } from "../../GTextField";
import { ByteBuffer } from "../../utils/ByteBuffer";
import { BaseParse } from "./BaseParse";
import GObjectParse from "./GObjectParse";


export default class GTextFieldParse extends BaseParse {

    private setupBeforeImpl(g: GTextField, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 5);

        g.font = buffer.readS();
        g.fontSize = buffer.readShort();
        g.color = buffer.readColor();
        g.align = buffer.readByte();
        g.verticalAlign = buffer.readByte();
        g.leading = buffer.readShort();
        g.letterSpacing = buffer.readShort();

        //@ts-ignore
        g._ubbEnabled = buffer.readBool();
        g.autoSize = buffer.readByte();
        g.underline = buffer.readBool();
        g.italic = buffer.readBool();
        g.bold = buffer.readBool();
        g.singleLine = buffer.readBool();
        if (buffer.readBool()) {
            g.strokeColor = buffer.readColor();
            g.stroke = buffer.readFloat();
        }

        if (buffer.readBool()) {
            g.shadowColor = buffer.readColor();
            let f1 = buffer.readFloat();
            let f2 = buffer.readFloat();
            g.shadowOffset = new Vec2(f1, f2);
        }

        if (buffer.readBool())
            //@ts-ignore
            g._templateVars = {};
    }

    protected override async parseSetupBeforeAsync(g: GTextField, beginPos: number, buffer: ByteBuffer) {

        return Promise.resolve()
            .then(() => {
                return GObjectParse.setupBeforeAddAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupBeforeImpl(g, beginPos, buffer);
            });
    }

    protected override parseSetupBefore(g: GTextField, beginPos: number, buffer: ByteBuffer): void {
        GObjectParse.setupBeforeAdd(g, beginPos, buffer);
        this.setupBeforeImpl(g, beginPos, buffer);
    }


    private setupAfterImpl(g: GTextField, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 6);
        var str: string = buffer.readS();
        if (str != null) {
            g.text = str;
        }
        else {
            g.text = "";
        }
    }

    protected override async parseSetupAfterAsync(g: GTextField, beginPos: number, buffer: ByteBuffer) {

        return Promise.resolve()
            .then(() => {
                return GObjectParse.setupAfterAddAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupAfterImpl(g, beginPos, buffer);
            });
    }

    protected override parseSetupAfter(g: GTextField, beginPos: number, fileBufferData: ByteBuffer): void {
        GObjectParse.setupAfterAdd(g, beginPos, fileBufferData);
        this.setupAfterImpl(g, beginPos, fileBufferData);
    }
}