import { GGroup } from "../../GGroup";
import { GObject } from "../../GObject";
import { ByteBuffer } from "../../utils/ByteBuffer";
import { BaseParse } from "./BaseParse";
import GObjectParse from "./GObjectParse";

export default class GGroupParse extends BaseParse {


    private setupBeforeImpl(g: GGroup, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 5);

        //@ts-ignore
        g._layout = buffer.readByte();

        //@ts-ignore
        g._lineGap = buffer.readInt();

        //@ts-ignore
        g._columnGap = buffer.readInt();
        if (buffer.version >= 2) {

            //@ts-ignore
            g._excludeInvisibles = buffer.readBool();

            //@ts-ignore
            g._autoSizeDisabled = buffer.readBool();

            //@ts-ignore
            g._mainGridIndex = buffer.readShort();
        }
    }

    protected override async parseSetupBeforeAsync(g: GGroup, beginPos: number, buffer: ByteBuffer) {

        return Promise.resolve()
            .then(() => {
                return GObjectParse.setupBeforeAddAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupBeforeImpl(g, beginPos, buffer);
            })
    }

    protected override parseSetupBefore(g: GGroup, beginPos: number, buffer: ByteBuffer): void {
        GObjectParse.setupBeforeAdd(g, beginPos, buffer);
        this.setupBeforeImpl(g, beginPos, buffer);
    }

    protected override async parseSetupAfterAsync(g: GGroup, beginPos: number, buffer: ByteBuffer) {
        return Promise.resolve()
            .then(() => {
                return GObjectParse.setupAfterAddAsync(g, beginPos, buffer);
            })
            .then(() => {
                if (!g.visible)
                    //@ts-ignore
                    g.handleVisibleChanged();
            });
    }

    protected override parseSetupAfter(g: GGroup, beginPos: number, buffer: ByteBuffer) {
        GObjectParse.setupAfterAdd(g, beginPos, buffer);
        if (!g.visible)
            //@ts-ignore
            g.handleVisibleChanged();
    }
}