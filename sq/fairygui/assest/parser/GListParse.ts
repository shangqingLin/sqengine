import { Application } from "../../../framework/Application";
import { OverflowType } from "../../common/FieldTypes";
import { Controller } from "../../Controller";
import { GButton } from "../../GButton";
import { GComponent } from "../../GComponent";
import { GList } from "../../GList";
import { GObject } from "../../GObject";
import { ByteBuffer } from "../../utils/ByteBuffer";
import GComponentParse from "./GComponentParse";

export default class GListParse extends GComponentParse {

    private setupBeforeImpl(g: GList, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 5);


        //@ts-ignore
        g._layout = buffer.readByte();

        //@ts-ignore
        g._selectionMode = buffer.readByte();

        //@ts-ignore
        g._align = buffer.readByte();

        //@ts-ignore
        g._verticalAlign = buffer.readByte();

        //@ts-ignore
        g._lineGap = buffer.readShort();

        //@ts-ignore
        g._columnGap = buffer.readShort();

        //@ts-ignore
        g._lineCount = buffer.readShort();

        //@ts-ignore
        g._columnCount = buffer.readShort();

        //@ts-ignore
        g._autoResizeItem = buffer.readBool();

        //@ts-ignore
        g._childrenRenderOrder = buffer.readByte();

        //@ts-ignore
        g._apexIndex = buffer.readShort();

        if (buffer.readBool()) {

            //@ts-ignore
            g._margin.top = buffer.readInt();

            //@ts-ignore
            g._margin.bottom = buffer.readInt();
            //@ts-ignore
            g._margin.left = buffer.readInt();
            //@ts-ignore
            g._margin.right = buffer.readInt();
        }

        var overflow: number = buffer.readByte();
        if (overflow == OverflowType.Scroll) {
            var savedPos: number = buffer.position;
            buffer.seek(beginPos, 7);
            g.setOverflow(OverflowType.Scroll);
            this.parseSteupScrollPanel(g, buffer);
            buffer.position = savedPos;
        }
        else
            g.setOverflow(overflow);

        if (buffer.readBool()) //clipSoftness
            buffer.skip(8);

        if (buffer.version >= 2) {
            g.scrollItemToViewOnClick = buffer.readBool();
            g.foldInvisibleItems = buffer.readBool();
        }

        buffer.seek(beginPos, 8);

        //@ts-ignore
        g._defaultItem = buffer.readS();
    }

    protected override async parseSetupBeforeAsync(g: GList, beginPos: number, buffer: ByteBuffer) {

        return Promise.resolve()
            .then(() => {
                return super.parseSetupBeforeAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupBeforeImpl(g, beginPos, buffer);
                if (Application.ins.getFromFrameStartTime() >= 5) {
                    return new Promise<void>((resolve, reject) => {
                        Application.ins.timer.nextTick(this, resolve);
                    })
                }
            })
            .then(() => {
                return this.readItemsAsync(g, buffer);
            })
    }

    protected async readItemsAsync(g: GList, buffer: ByteBuffer) {
        var cnt: number = buffer.readShort();

        //在编辑器中创建的Item的个数（“编辑列表数据” 的配置）
        if (cnt === 0) return Promise.resolve();

        return new Promise<void>((resolve, reject) => {
            let current = -1;
            let next = () => {
                ++current;
                if (current === cnt) {
                    resolve();
                    return;
                }

                let nextPos = buffer.readShort();
                nextPos += buffer.position;

                //在“编辑列表数据”的表格中，我们还可以为不同的单元格设置不同的组件，显示不用的样式
                //这个str就是这个单元格显示的组件的
                var str = buffer.readS();
                if (str === null) {

                    //如果没有就使用默认的（“项目资源”处的设置）
                    //@ts-ignore
                    str = g._defaultItem;
                    if (!str) {
                        buffer.position = nextPos;
                        next();
                        return;
                    }
                }

                var obj: GObject = g.getFromPool(str);
                if (obj) {
                    g.addChild(obj);
                    this.setupItem(obj, buffer);
                }
                buffer.position = nextPos;

                if (Application.ins.getFromFrameStartTime() >= 5) {
                    Application.ins.timer.nextTick(this, next);
                    return;
                }
                next();
            };
            next();
        });
    }

    protected override parseSetupBefore(g: GList, beginPos: number, buffer: ByteBuffer): void {
        super.parseSetupBefore(g, beginPos, buffer);
        this.setupBeforeImpl(g, beginPos, buffer);
        this.readItems(g, buffer);
    }

    protected readItems(g: GList, buffer: ByteBuffer) {
        var cnt: number;
        var i: number;
        var nextPos: number;
        var str: string;

        cnt = buffer.readShort();
        for (i = 0; i < cnt; i++) {
            nextPos = buffer.readShort();
            nextPos += buffer.position;

            str = buffer.readS();
            if (str == null) {
                //@ts-ignore
                str = g._defaultItem;
                if (!str) {
                    buffer.position = nextPos;
                    continue;
                }
            }

            var obj: GObject = g.getFromPool(str);
            if (obj) {
                g.addChild(obj);
                this.setupItem(obj, buffer);
            }

            buffer.position = nextPos;
        }
    }

    protected setupItem(obj: GObject, buffer: ByteBuffer): void {
        var str: string;

        str = buffer.readS();
        if (str != null)
            obj.text = str;
        str = buffer.readS();
        if (str != null && (obj instanceof GButton))
            obj.selectedTitle = str;
        str = buffer.readS();
        if (str != null)
            obj.icon = str;
        str = buffer.readS();
        if (str != null && (obj instanceof GButton))
            obj.selectedIcon = str;
        str = buffer.readS();
        if (str != null)
            obj.name = str;

        var cnt: number;
        var i: number;

        if (obj instanceof GComponent) {
            cnt = buffer.readShort();
            for (i = 0; i < cnt; i++) {
                var cc: Controller = obj.getController(buffer.readS());
                str = buffer.readS();
                if (cc)
                    cc.selectedPageId = str;
            }

            if (buffer.version >= 2) {
                cnt = buffer.readShort();
                for (i = 0; i < cnt; i++) {
                    var target: string = buffer.readS();
                    var propertyId: number = buffer.readShort();
                    var value: String = buffer.readS();
                    var obj2: GObject = obj.getChildByPath(target);
                    if (obj2)
                        obj2.setProp(propertyId, value);
                }
            }
        }
    }


    private setupAfterImpl(g: GList, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 6);
        var i: number = buffer.readShort();
        if (i != -1)
            //@ts-ignore
            g._selectionController = g.parent.getControllerAt(i);
    }

    protected override async parseSetupAfterAsync(g: GList, beginPos: number, buffer: ByteBuffer) {
        return Promise.resolve()
            .then(() => {
                return super.parseSetupAfterAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupAfterImpl(g, beginPos, buffer);
            })
    }

    protected override parseSetupAfter(g: GList, beginPos: number, buffer: ByteBuffer): void {
        super.parseSetupAfter(g, beginPos, buffer);
        this.setupAfterImpl(g, beginPos, buffer);
    }
}