import { GearBase, GearTweenConfig } from "../../gear/GearBase";
import { ByteBuffer } from "../../utils/ByteBuffer";


export class GearBaseParse {
    static setup(g: GearBase, buffer: ByteBuffer) {

        //@ts-ignore
        g._controller = g._owner.parent.getControllerAt(buffer.readShort());

        //@ts-ignore
        g.init();

        var i: number;
        var page: string;
        var cnt: number = buffer.readShort();

        if ("pages" in g) {
            (<any>g).pages = buffer.readSArray(cnt);
        }
        else {
            for (i = 0; i < cnt; i++) {
                page = buffer.readS();
                if (page == null)
                    continue;

                //@ts-ignore
                g.addStatus(page, buffer);
            }

            if (buffer.readBool())
                //@ts-ignore
                g.addStatus(null, buffer);
        }

        if (buffer.readBool()) {
            //@ts-ignore
            g._tweenConfig = new GearTweenConfig();
            //@ts-ignore
            g._tweenConfig.easeType = buffer.readByte();
            //@ts-ignore
            g._tweenConfig.duration = buffer.readFloat();
            //@ts-ignore
            g._tweenConfig.delay = buffer.readFloat();
        }

        if (buffer.version >= 2) {
            if ("positionsInPercent" in g) {
                if (buffer.readBool()) {
                    (<any>g).positionsInPercent = true;
                    for (i = 0; i < cnt; i++) {
                        page = buffer.readS();
                        if (page === null)
                            continue;

                        (<any>g).addExtStatus(page, buffer);
                    }

                    if (buffer.readBool())
                        (<any>g).addExtStatus(null, buffer);
                }
            }
            else if ("condition" in g)
                (<any>g).condition = buffer.readByte();
        }
    }
}
