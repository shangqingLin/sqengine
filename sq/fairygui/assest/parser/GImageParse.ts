import { SpriteType } from "../../../2d";
import { GImage } from "../../GImage";
import { BaseParse } from "./BaseParse";
import GObjectParse from "./GObjectParse";
import { SpriteFrame } from "../../../assets";
import { ByteBuffer } from "../../utils/ByteBuffer";
import { GObject } from "../../GObject";
import { PackageItem } from "../PackageItem";

export default class GImageParse extends BaseParse {

    public override constructFromResource(g: GImage) {

        let fileContentItem: PackageItem = g.packageItem.getBranch();
        let fileBufferData: ByteBuffer = fileContentItem.rawData;

        g.sourceWidth = g.packageItem.width;
        g.sourceHeight = g.packageItem.height;
        g.initWidth = g.sourceWidth;
        g.initHeight = g.sourceHeight;
        g.setSize(g.sourceWidth, g.sourceHeight);

        let contentItem = g.packageItem.getHighResolution();
        contentItem.load();

        if (contentItem.scale9Grid)
            g._content.type = SpriteType.SLICED;
        else if (contentItem.scaleByTile)
            g._content.type = SpriteType.TILED;
        g._content.spriteFrame = <SpriteFrame>contentItem.asset;
    }

    public override async constructFromResourceAsync(g: GImage): Promise<void> {
        this.constructFromResource(g);
    };

    private setupBeforeImpl(g: GImage, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 5);
        if (buffer.readBool())
            g.color = buffer.readColor();
        g._content.flip = buffer.readByte();
        g._content.fillMethod = buffer.readByte();
        if (g._content.fillMethod != 0) {
            g._content.fillOrigin = buffer.readByte();
            g._content.fillClockwise = buffer.readBool();
            g._content.fillAmount = buffer.readFloat();
        }
    }

    protected override async parseSetupBeforeAsync(g: GImage, beginPos: number, buffer: ByteBuffer) {
        return Promise.resolve()
            .then(() => {
                return GObjectParse.setupBeforeAddAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupBeforeImpl(g, beginPos, buffer);
            });
    }

    protected override parseSetupBefore(g: GImage, beginPos: number, buffer: ByteBuffer): void {
        GObjectParse.setupBeforeAdd(g, beginPos, buffer);
        this.setupBeforeImpl(g, beginPos, buffer);
    }

    public override async parseSetupAfterAsync(g: GImage, beginPos: number, buffer: ByteBuffer) {
        return GObjectParse.setupAfterAddAsync(g, beginPos, buffer);
    }

    protected override parseSetupAfter(g: GObject, beginPos: number, buffer: ByteBuffer): void {
        GObjectParse.setupAfterAdd(g, beginPos, buffer);
    }
}