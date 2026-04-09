import { NodeEventType } from "../../../scene";
import { ButtonMode } from "../../common/FieldTypes";
import { GButton } from "../../GButton";
import { ByteBuffer } from "../../utils/ByteBuffer";
import GComponentParse from "./GComponentParse";

export default class GButtonParse extends GComponentParse {

    private constructExtensionImpl(g: GButton, buffer: ByteBuffer) {
        buffer.seek(0, 6);

        //@ts-ignore
        g._mode = buffer.readByte();
        var str: string = buffer.readS();
        if (str)
            //@ts-ignore
            g._sound = str;

        //@ts-ignore
        g._soundVolumeScale = buffer.readFloat();

        //@ts-ignore
        g._downEffect = buffer.readByte();

        //@ts-ignore
        g._downEffectValue = buffer.readFloat();

        //@ts-ignore
        if (g._downEffect == 2)
            g.setPivot(0.5, 0.5, g.pivotAsAnchor);

        //@ts-ignore
        g._buttonController = g.getController("button");

        //@ts-ignore
        g._titleObject = g.getChild("title");

        //@ts-ignore
        g._iconObject = g.getChild("icon");

        //@ts-ignore
        if (g._titleObject)
            //@ts-ignore
            g._title = g._titleObject.text;

        //@ts-ignore
        if (g._iconObject)
            //@ts-ignore
            g._icon = g._iconObject.icon;

        //@ts-ignore
        if (g._mode === ButtonMode.Common)
            //@ts-ignore
            g.setState(GButton.UP);

        //@ts-ignore
        g._node.on(NodeEventType.MOUSE_DOWN, g, g.onTouchBegin_1);

        //@ts-ignore
        g._node.on(NodeEventType.MOUSE_UP, g, g.onTouchEnd_1);

        //@ts-ignore
        g._node.on(NodeEventType.MOUSE_OVER, g, g.onRollOver_1);

        //@ts-ignore
        g._node.on(NodeEventType.MOUSE_OUT, g, g.onRollOut_1);

        //@ts-ignore
        g._node.on(NodeEventType.MOUSE_CLICK, g, g.onClick_1);
    }

    protected override async constructExtensionAsync(g: GButton, buffer: ByteBuffer): Promise<void> {
        this.constructExtensionImpl(g, buffer);
    }

    protected override constructExtension(g: GButton, buffer: ByteBuffer): void {
        this.constructExtensionImpl(g, buffer);
    }


    private parseSetupAfterImpl(g: GButton, beginPos: number, buffer: ByteBuffer) {
        if (!buffer.seek(beginPos, 6))
            return;

        if (buffer.readByte() != g.packageItem.objectType)
            return;

        var str: string;
        var iv: number;

        str = buffer.readS();
        if (str != null)
            g.title = str;
        str = buffer.readS();
        if (str != null)
            g.selectedTitle = str;
        str = buffer.readS();
        if (str != null)
            g.icon = str;
        str = buffer.readS();
        if (str != null)
            g.selectedIcon = str;
        if (buffer.readBool())
            g.titleColor = buffer.readColor();
        iv = buffer.readInt();
        if (iv != 0)
            g.titleFontSize = iv;
        iv = buffer.readShort();
        if (iv >= 0)
            //@ts-ignore
            g._relatedController = g.parent.getControllerAt(iv);

        //@ts-ignore
        g._relatedPageId = buffer.readS();

        str = buffer.readS();
        if (str != null)
            //@ts-ignore
            g._sound = str;

        if (buffer.readBool())
            //@ts-ignore
            g._soundVolumeScale = buffer.readFloat();

        g.selected = buffer.readBool();
    }

    protected override async parseSetupAfterAsync(g: GButton, beginPos: number, buffer: ByteBuffer) {

        return Promise.resolve()
            .then(() => {
                return super.parseSetupAfterAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.parseSetupAfterImpl(g, beginPos, buffer);
            })
    }

    protected override parseSetupAfter(g: GButton, beginPos: number, buffer: ByteBuffer): void {
        super.parseSetupAfter(g, beginPos, buffer);
        this.parseSetupAfterImpl(g, beginPos, buffer);
    }
}