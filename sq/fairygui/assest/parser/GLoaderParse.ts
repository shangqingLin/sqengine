import { GLoader } from "../../GLoader";
import { GObject } from "../../GObject";
import { ByteBuffer } from "../../utils/ByteBuffer";
import { BaseParse } from "./BaseParse";
import GObjectParse from "./GObjectParse";


export default class GLoaderParse extends BaseParse {

    private setupBeforeImpl(g: GLoader, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 5);

        //@ts-ignore
        g._url = buffer.readS();

        //@ts-ignore
        g._align = buffer.readByte();

        //@ts-ignore
        g._verticalAlign = buffer.readByte();

        //@ts-ignore
        g._fill = buffer.readByte();

        //@ts-ignore
        g._shrinkOnly = buffer.readBool();

        //@ts-ignore
        g._autoSize = buffer.readBool();

        //@ts-ignore
        g._showErrorSign = buffer.readBool();

        //@ts-ignore
        g._playing = buffer.readBool();

        //@ts-ignore
        g._frame = buffer.readInt();

        if (buffer.readBool())
            g.color = buffer.readColor();
        g._content.fillMethod = buffer.readByte();
        if (g._content.fillMethod != 0) {

            g._content.fillOrigin = buffer.readByte();
            g._content.fillClockwise = buffer.readBool();
            g._content.fillAmount = buffer.readFloat();
        }

        //@ts-ignore
        if (g._url)
            //@ts-ignore
            g.loadContent();
    }

    protected override async parseSetupBeforeAsync(g: GLoader, beginPos: number, buffer: ByteBuffer) {
        return Promise.resolve()
            .then(() => {
                return GObjectParse.setupBeforeAddAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupBeforeImpl(g, beginPos, buffer);
            });
    }

    protected override parseSetupBefore(g: GLoader, beginPos: number, buffer: ByteBuffer): void {
        GObjectParse.setupBeforeAdd(g, beginPos, buffer);
        this.setupBeforeImpl(g, beginPos, buffer);
    }

    public override async parseSetupAfterAsync(g: GLoader, beginPos: number, buffer: ByteBuffer) {
        return GObjectParse.setupAfterAddAsync(g, beginPos, buffer);
    }

    protected override parseSetupAfter(g: GObject, beginPos: number, buffer: ByteBuffer): void {
        GObjectParse.setupAfterAdd(g, beginPos, buffer);
    }
}