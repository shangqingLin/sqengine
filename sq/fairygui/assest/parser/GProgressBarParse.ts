import { GComponent } from "../../GComponent";
import { GProgressBar } from "../../GProgressBar";
import { ByteBuffer } from "../../utils/ByteBuffer";
import GComponentParse from "./GComponentParse";

export default class GProgressBarParse extends GComponentParse {


    protected override async constructExtensionAsync(g: GProgressBar, buffer: ByteBuffer) {
        this.constructExtension(g, buffer);
    }

    protected override constructExtension(g: GProgressBar, buffer: ByteBuffer) {
        buffer.seek(0, 6);


        //@ts-ignore
        g._titleType = buffer.readByte();

        //@ts-ignore
        g._reverse = buffer.readBool();


        //@ts-ignore
        g._titleObject = g.getChild("title");

        //@ts-ignore
        g._barObjectH = g.getChild("bar");

        //@ts-ignore
        g._barObjectV = g.getChild("bar_v");

        //@ts-ignore
        g._aniObject = g.getChild("ani");

        //@ts-ignore
        if (g._barObjectH) {

            //@ts-ignore
            g._barMaxWidth = g._barObjectH.width;

            //@ts-ignore
            g._barMaxWidthDelta = g.width - g._barMaxWidth;

            //@ts-ignore
            g._barStartX = g._barObjectH.x;
        }

        //@ts-ignore
        if (g._barObjectV) {

            //@ts-ignore
            g._barMaxHeight = g._barObjectV.height;

            //@ts-ignore
            g._barMaxHeightDelta = g.height - g._barMaxHeight;

            //@ts-ignore
            g._barStartY = g._barObjectV.y;
        }
    }

    private setupAfterImpl(g: GProgressBar, beginPos: number, buffer: ByteBuffer) {
        if (!buffer.seek(beginPos, 6)) {
            //@ts-ignore
            g.update(g._value);
            return;
        }

        //@ts-ignore
        if (buffer.readByte() != g.packageItem.objectType) {
            //@ts-ignore
            g.update(g._value);
            return;
        }

        //@ts-ignore
        g._value = buffer.readInt();

        //@ts-ignore
        g._max = buffer.readInt();
        if (buffer.version >= 2)
            //@ts-ignore
            g._min = buffer.readInt();

        //@ts-ignore
        g.update(g._value);
    }

    protected override async parseSetupAfterAsync(g: GProgressBar, beginPos: number, buffer: ByteBuffer) {
        return Promise.resolve()
            .then(() => {
                return super.parseSetupAfterAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupAfterImpl(g, beginPos, buffer);
            })
    }

    protected override parseSetupAfter(g: GProgressBar, beginPos: number, buffer: ByteBuffer): void {
        super.parseSetupAfter(g, beginPos, buffer);
        this.setupAfterImpl(g, beginPos, buffer);
    }
}