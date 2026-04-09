import { GGraph } from "../../GGraph";
import { GObject } from "../../GObject";
import { ByteBuffer } from "../../utils/ByteBuffer";
import { BaseParse } from "./BaseParse";
import GObjectParse from "./GObjectParse";


export default class GGraphParse extends BaseParse {


    private setupBeforeImpl(g: GGraph, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 5);

        //@ts-ignore
        g._type = buffer.readByte();

        //@ts-ignore
        if (g._type != 0) {
            var i: number;
            var cnt: number;

            //@ts-ignore
            g._lineSize = buffer.readInt();

            //@ts-ignore
            g._lineColor.setFormColor(buffer.readColor(true));

            //@ts-ignore
            g._fillColor.setFormColor(buffer.readColor(true));
            if (buffer.readBool()) {

                //@ts-ignore
                g._cornerRadius = new Array<number>(4);
                for (i = 0; i < 4; i++)
                    //@ts-ignore
                    g._cornerRadius[i] = buffer.readFloat();
            }

            //@ts-ignore
            if (g._type === 3) {
                cnt = buffer.readShort();

                //@ts-ignore
                g._polygonPoints = [];

                //@ts-ignore
                g._polygonPoints.length = cnt;
                for (i = 0; i < cnt; i++)
                    //@ts-ignore
                    g._polygonPoints[i] = buffer.readFloat();
            }

            //@ts-ignore
            else if (g._type === 4) {

                //@ts-ignore
                g._sides = buffer.readShort();

                //@ts-ignore
                g._startAngle = buffer.readFloat();
                cnt = buffer.readShort();
                if (cnt > 0) {

                    //@ts-ignore
                    g._distances = [];
                    for (i = 0; i < cnt; i++)
                        //@ts-ignore
                        g._distances[i] = buffer.readFloat();
                }
            }

            //@ts-ignore
            g.updateGraph();
        }
    }
    protected override async parseSetupBeforeAsync(g: GGraph, beginPos: number, buffer: ByteBuffer) {

        return Promise.resolve()
            .then(() => {
                return GObjectParse.setupBeforeAddAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupBeforeImpl(g, beginPos, buffer);
            });
    }

    protected override parseSetupBefore(g: GGraph, beginPos: number, buffer: ByteBuffer): void {
        GObjectParse.setupBeforeAdd(g, beginPos, buffer);
        this.setupBeforeImpl(g, beginPos, buffer);
    }

    public override async parseSetupAfterAsync(g: GGraph, beginPos: number, buffer: ByteBuffer) {
        return GObjectParse.setupAfterAddAsync(g, beginPos, buffer);
    }

    public override parseSetupAfter(g: GGraph, beginPos: number, buffer: ByteBuffer) {
        GObjectParse.setupAfterAdd(g, beginPos, buffer);
    }
}