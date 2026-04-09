import { NodeEventType } from "../../../scene";
import { GComponent } from "../../GComponent";
import { GSlider } from "../../GSlider";
import { ByteBuffer } from "../../utils/ByteBuffer";
import GComponentParse from "./GComponentParse";


export default class GSliderParse extends GComponentParse {

    protected override async constructExtensionAsync(g: GSlider, buffer: ByteBuffer): Promise<void> {
        this.constructExtension(g,buffer);
    }

    protected override constructExtension(g: GSlider,buffer:ByteBuffer) {
        buffer.seek(0, 6);

        //@ts-ignore
        g._titleType = buffer.readByte();

        //@ts-ignore
        g._reverse = buffer.readBool();

        //@ts-ignore
        if (buffer.version >= 2) {

            //@ts-ignore
            g._wholeNumbers = buffer.readBool();
            g.changeOnClick = buffer.readBool();
        }

        //@ts-ignore
        g._titleObject = g.getChild("title");

        //@ts-ignore
        g._barObjectH = g.getChild("bar");

        //@ts-ignore
        g._barObjectV = g.getChild("bar_v");

        //@ts-ignore
        g._gripObject = g.getChild("grip");

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

        //@ts-ignore
        if (g._gripObject) {

            //@ts-ignore
            g._gripObject.on(NodeEventType.MOUSE_DOWN, g.onGripTouchBegin, g);

            //@ts-ignore
            g._gripObject.on(NodeEventType.MOUSE_MOVE, g.onGripTouchMove, g);
        }
        
        //@ts-ignore
        g._node.on(NodeEventType.MOUSE_DOWN, g, g.onBarTouchBegin);
    }
}