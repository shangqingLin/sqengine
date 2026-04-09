import { NodeEventType } from "../../../scene";
import { FUIEvent } from "../../FUIEvent";
import { GComboBox } from "../../GComboBox";
import { GComponent } from "../../GComponent";
import { GList } from "../../GList";
import { RelationType } from "../../Relations";
import { ByteBuffer } from "../../utils/ByteBuffer";
import { UIPackage } from "../UIPackage";
import GComponentParse from "./GComponentParse";


export default class GComboBoxParse extends GComponentParse {

    private constructExtensionImpl(g: GComponent, buffer: ByteBuffer) {
        var str: string;
        let combox: GComboBox = g as GComboBox;

        //@ts-ignore
        combox._buttonController = combox.getController("button");

        //@ts-ignore
        combox._titleObject = combox.getChild("title");

        //@ts-ignore
        combox._iconObject = combox.getChild("icon");

        str = buffer.readS();
        if (str) {
            let obj = UIPackage.createObjectFromURL(str);
            if (!(obj instanceof GComponent)) {
                console.error("下拉框必须为元件");
                return;
            }
            combox.dropdown = obj;
            combox.dropdown.name = "this.dropdown";

            //@ts-ignore
            combox._list = combox.dropdown.getChild("list", GList);

            //@ts-ignore
            if (combox._list == null) {
                //@ts-ignore
                console.error(g.resourceURL + ": 下拉框的弹出元件里必须包含名为list的列表");
                return;
            }

            //@ts-ignore
            combox._list.on(FUIEvent.CLICK_ITEM, g.onClickItem, g);

            //@ts-ignore
            combox._list.addRelation(combox.dropdown, RelationType.Width);

            //@ts-ignore
            combox._list.removeRelation(combox.dropdown, RelationType.Height);

            //@ts-ignore
            combox.dropdown.addRelation(combox._list, RelationType.Height);

            //@ts-ignore
            combox.dropdown.removeRelation(combox._list, RelationType.Width);

            //@ts-ignore
            combox.dropdown.on(FUIEvent.UNDISPLAY, combox.onPopupClosed, combox);
        }

        //@ts-ignore
        combox._node.on(NodeEventType.MOUSE_DOWN, combox, combox.onTouchBegin_1);

        //@ts-ignore
        combox._node.on(NodeEventType.MOUSE_UP, combox, combox.onTouchEnd_1);

        //@ts-ignore
        combox._node.on(NodeEventType.MOUSE_OVER, combox, combox.onRollOver_1);

        //@ts-ignore
        combox._node.on(NodeEventType.MOUSE_OUT, combox, combox.onRollOut_1);
    }

    protected override async constructExtensionAsync(g: GComponent, buffer: ByteBuffer) {
        this.constructExtensionImpl(g, buffer);
    }

    protected override constructExtension(g: GComponent, buffer: ByteBuffer): void {
        this.constructExtensionImpl(g, buffer);
    }


    private setupAfterImpl(g: GComponent, beginPos: number, buffer: ByteBuffer) {

        let combox: GComboBox = g as GComboBox;

        //表示引用这个组件处没有对此组件做任何修改
        if (!buffer.seek(beginPos, 6))
            return;


        if (buffer.readByte() !== g.packageItem.objectType)
            return;

        var i: number;
        var iv: number;
        var nextPos: number;
        var str: string;
        var itemCount: number = buffer.readShort();
        for (i = 0; i < itemCount; i++) {
            nextPos = buffer.readShort();
            nextPos += buffer.position;

            //@ts-ignore
            combox._items[i] = buffer.readS();

            //@ts-ignore
            combox._values[i] = buffer.readS();
            str = buffer.readS();
            if (str != null) {
                //@ts-ignore
                if (combox._icons == null)
                    //@ts-ignore
                    combox._icons = new Array<string>();

                //@ts-ignore
                combox._icons[i] = str;
            }

            buffer.position = nextPos;
        }

        str = buffer.readS();
        if (str != null) {
            combox.text = str;
            //@ts-ignore
            combox._selectedIndex = combox._items.indexOf(str);
        }
        //@ts-ignore
        else if (combox._items.length > 0) {

            //@ts-ignore
            combox._selectedIndex = 0;

            //@ts-ignore
            combox.text = combox._items[0];
        }
        else
            //@ts-ignore
            combox._selectedIndex = -1;

        str = buffer.readS();
        if (str != null)
            combox.icon = str;

        if (buffer.readBool())
            combox.titleColor = buffer.readColor();
        iv = buffer.readInt();
        if (iv > 0)
            //@ts-ignore
            combox._visibleItemCount = iv;

        //@ts-ignore
        tcomboxhis._popupDirection = buffer.readByte();

        iv = buffer.readShort();
        if (iv >= 0)
            //@ts-ignore
            combox._selectionController = combox.parent.getControllerAt(iv);
    }

    protected override async parseSetupAfterAsync(g: GComponent, beginPos: number, buffer: ByteBuffer) {
        return Promise.resolve()
            .then(() => {
                return super.parseSetupAfterAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupAfterImpl(g, beginPos, buffer);
            })
    }

    protected override parseSetupAfter(g: GComponent, beginPos: number, buffer: ByteBuffer): void {
        super.parseSetupAfter(g, beginPos, buffer);
        this.setupAfterImpl(g, beginPos, buffer);
    }
}