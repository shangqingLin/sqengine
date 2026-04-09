import { GMovieClip } from "../../GMovieClip";
import { GObject } from "../../GObject";
import { ByteBuffer } from "../../utils/ByteBuffer";
import { BaseParse } from "./BaseParse";
import GObjectParse from "./GObjectParse";

export default class GMovieClipParse extends BaseParse {


    public override constructFromResource(g: GMovieClip) {
        g.sourceWidth = g.packageItem.width;
        g.sourceHeight = g.packageItem.height;
        g.initWidth = g.sourceWidth;
        g.initHeight = g.sourceHeight;

        g.setSize(g.sourceWidth, g.sourceHeight);

        let contentItem = g.packageItem.getHighResolution();
        contentItem.load();

        g._content.interval = contentItem.interval;
        g._content.swing = contentItem.swing;
        g._content.repeatDelay = contentItem.repeatDelay;
        g._content.frames = contentItem.frames;
        g._content.smoothing = contentItem.smoothing;
    }

    public override async constructFromResourceAsync(g: GMovieClip) {
        this.constructFromResource(g);
    }


    private setupBeforeImpl(g: GMovieClip, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 5);
        if (buffer.readBool())
            g.color = buffer.readColor();
        buffer.readByte(); //flip
        g._content.frame = buffer.readInt();
        g._content.playing = buffer.readBool();
    }

    protected override async parseSetupBeforeAsync(g: GMovieClip, beginPos: number, buffer: ByteBuffer) {
        return Promise.resolve()
            .then(() => {
                return GObjectParse.setupBeforeAddAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupBeforeImpl(g, beginPos, buffer);
            });
    }

    protected override parseSetupBefore(g: GMovieClip, beginPos: number, buffer: ByteBuffer): void {
        GObjectParse.setupBeforeAdd(g, beginPos, buffer);
        this.setupBeforeImpl(g, beginPos, buffer);
    }

    protected override async parseSetupAfterAsync(g: GMovieClip, beginPos: number, buffer: ByteBuffer) {
        return GObjectParse.setupAfterAddAsync(g, beginPos, buffer);
    }

    protected override parseSetupAfter(g: GMovieClip, beginPos: number, buffer: ByteBuffer) {
        GObjectParse.setupAfterAdd(g, beginPos, buffer);
    }
}