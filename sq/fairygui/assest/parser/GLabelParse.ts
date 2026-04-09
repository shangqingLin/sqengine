import { NodeEventType } from "../../../scene";
import { GComponent } from "../../GComponent";
import { GLabel } from "../../GLabel";
import { GTextField } from "../../GTextField";
import { GTextInput } from "../../GTextInput";
import { ByteBuffer } from "../../utils/ByteBuffer";
import GComponentParse from "./GComponentParse";

export default class GLabelParse extends GComponentParse {
    protected override async constructExtensionAsync(g: GLabel) {
        this.constructExtension(g);
    }

    protected override constructExtension(g: GLabel) {

        //@ts-ignore
        g._titleObject = g.getChild("title");

        //@ts-ignore
        g._iconObject = g.getChild("icon");
    }


    private setupAfterImpl(g: GLabel, beginPos: number, buffer: ByteBuffer) {
        //表示引用这个组件处没有对此组件做任何修改
        if (!buffer.seek(beginPos, 6))
            return;


        if (buffer.readByte() !== g.packageItem.objectType)
            return;

        var str: string;
        str = buffer.readS();
        if (str != null)
            g.title = str;
        str = buffer.readS();
        if (str != null)
            g.icon = str;
        if (buffer.readBool())
            g.titleColor = buffer.readColor();
        var iv: number = buffer.readInt();
        if (iv != 0)
            g.titleFontSize = iv;

        if (buffer.readBool()) {
            var input: GTextField = g.getTextField();
            if (input instanceof GTextInput) {
                str = buffer.readS();
                if (str != null)
                    input.promptText = str;

                str = buffer.readS();
                if (str != null)
                    input.restrict = str;

                iv = buffer.readInt();
                if (iv != 0)
                    input.maxLength = iv;
                iv = buffer.readInt();
                if (iv != 0) {
                    //keyboardType
                }
                if (buffer.readBool())
                    input.password = true;
            }
            else
                buffer.skip(13);
        }
        str = buffer.readS();
        if (str != null) {
            //@ts-ignore
            g._sound = str;
            if (buffer.readBool()) {
                //@ts-ignore
                g._soundVolumeScale = buffer.readFloat();
            }

            //@ts-ignore
            g._node.on(NodeEventType.MOUSE_CLICK, g, g.onClick_1);
        }
    }

    public override async parseSetupAfterAsync(g: GLabel, beginPos: number, buffer: ByteBuffer) {
        return Promise.resolve()
            .then(() => {
                return super.parseSetupAfterAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupAfterImpl(g, beginPos, buffer);
            });
    }

    protected override parseSetupAfter(g: GLabel, beginPos: number, buffer: ByteBuffer): void {
        super.parseSetupAfter(g, beginPos, buffer);
        this.setupAfterImpl(g, beginPos, buffer);
    }
}