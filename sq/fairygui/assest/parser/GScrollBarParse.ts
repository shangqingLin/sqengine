import { NodeEventType } from "../../../scene";
import { GComponent } from "../../GComponent";
import { GScrollBar } from "../../GScrollBar";
import { ByteBuffer } from "../../utils/ByteBuffer";
import GComponentParse from "./GComponentParse";


export default class GScrollBarParse extends GComponentParse {

    protected override async constructExtensionAsync(g: GScrollBar, buffer: ByteBuffer): Promise<void> {
        this.constructExtension(g, buffer);
    }

    protected override constructExtension(g: GScrollBar, buffer: ByteBuffer) {
        buffer.seek(0, 6);

        //@ts-ignore
        g._fixedGripSize = buffer.readBool();

        //@ts-ignore
        g._grip = g.getChild("grip");

        //@ts-ignore
        if (!g._grip) {
            console.error("需要定义grip");
            return;
        }

        //@ts-ignore
        g._bar = g.getChild("bar");

        //@ts-ignore
        if (!g._bar) {
            console.error("需要定义bar");
            return;
        }

        //@ts-ignore
        g._arrowButton1 = g.getChild("arrow1");

        //@ts-ignore
        g._arrowButton2 = g.getChild("arrow2");

        //@ts-ignore
        g._grip.on(NodeEventType.MOUSE_DOWN, g.onGripTouchDown, g);

        //@ts-ignore
        g._grip.on(NodeEventType.MOUSE_MOVE, g.onGripTouchMove, g);

        //@ts-ignore
        g._grip.on(NodeEventType.MOUSE_UP, g.onGripTouchEnd, g);

        //@ts-ignore
        if (g._arrowButton1)
            //@ts-ignore
            g._arrowButton1.on(NodeEventType.MOUSE_DOWN, g.onClickArrow1, g);

        //@ts-ignore
        if (g._arrowButton2)
            //@ts-ignore
            g._arrowButton2.on(NodeEventType.MOUSE_DOWN, g.onClickArrow2, g);


        //@ts-ignore
        g.on(NodeEventType.MOUSE_DOWN, g.onBarTouchBegin, g);
    }
}