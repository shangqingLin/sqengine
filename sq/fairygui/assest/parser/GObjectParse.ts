import { Application } from "../../../framework/Application";
import { GObject } from "../../GObject";
import { ByteBuffer } from "../../utils/ByteBuffer";
import { GGroup } from "../../GGroup";
import { GearBase } from "../../gear/GearBase";
import { GearBaseParse } from "./GearParse";


export default class GObjectParse {

    static async setupBeforeAddAsync(g: GObject, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 0);
        buffer.skip(5);

        //@ts-ignore
        g._id = buffer.readS();
        g.name = buffer.readS();

        let nextTick = () => {
            if (Application.ins.getFromFrameStartTime() >= 5) {
                return new Promise<void>((resolve, reject) => {
                    Application.ins.timer.nextTick(this, () => {
                        resolve();
                    });
                })
            }
        }

        return Promise.resolve()
            .then(() => {
                var f1: number = buffer.readInt();
                var f2: number = buffer.readInt();
                g.setPosition(f1, f2);
                return nextTick();
            })
            .then(() => {
                if (buffer.readBool()) {
                    g.initWidth = buffer.readInt();
                    g.initHeight = buffer.readInt();
                    g.setSize(g.initWidth, g.initHeight, true);
                }
                return nextTick();
            })
            .then(() => {
                if (buffer.readBool()) {
                    g.minWidth = buffer.readInt();
                    g.maxWidth = buffer.readInt();
                    g.minHeight = buffer.readInt();
                    g.maxHeight = buffer.readInt();
                }
                return nextTick();
            })
            .then(() => {
                if (buffer.readBool()) {
                    var f1: number = buffer.readFloat();
                    var f2: number = buffer.readFloat();
                    g.setScale(f1, f2);
                }
                return nextTick();
            })
            .then(() => {
                if (buffer.readBool()) {
                    var f1 = buffer.readFloat();
                    var f2 = buffer.readFloat();
                    //this.setSkew(f1, f2);
                }
                return nextTick();
            })
            .then(() => {
                if (buffer.readBool()) {
                    var f1: number = buffer.readFloat();
                    var f2: number = buffer.readFloat();
                    g.setPivot(f1, f2, buffer.readBool());
                }
                return nextTick();
            })
            .then(() => {
                var f1: number = buffer.readFloat();
                if (f1 !== 1)
                    g.alpha = f1;

                f1 = buffer.readFloat();
                if (f1 != 0)
                    g.rotation = f1;

                if (!buffer.readBool())
                    g.visible = false;
                if (!buffer.readBool())
                    g.touchable = false;
                if (buffer.readBool())
                    g.grayed = true;
                g.blendMode = buffer.readByte();

                var filter: number = buffer.readByte();
                if (filter == 1) {
                    //TODO: filter support
                }

                var str: string = buffer.readS();
                if (str != null)
                    g.data = str;
            });
    }


    static setupBeforeAdd(g: GObject, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 0);
        buffer.skip(5);

        //@ts-ignore
        g._id = buffer.readS();
        g.name = buffer.readS();

        var f1: number = buffer.readInt();
        var f2: number = buffer.readInt();
        g.setPosition(f1, f2);

        if (buffer.readBool()) {
            g.initWidth = buffer.readInt();
            g.initHeight = buffer.readInt();
            g.setSize(g.initWidth, g.initHeight, true);
        }

        if (buffer.readBool()) {
            g.minWidth = buffer.readInt();
            g.maxWidth = buffer.readInt();
            g.minHeight = buffer.readInt();
            g.maxHeight = buffer.readInt();
        }

        if (buffer.readBool()) {
            var f1: number = buffer.readFloat();
            var f2: number = buffer.readFloat();
            g.setScale(f1, f2);
        }

        if (buffer.readBool()) {
            var f1 = buffer.readFloat();
            var f2 = buffer.readFloat();
            //this.setSkew(f1, f2);
        }

        if (buffer.readBool()) {
            var f1: number = buffer.readFloat();
            var f2: number = buffer.readFloat();
            g.setPivot(f1, f2, buffer.readBool());
        }

        var f1: number = buffer.readFloat();
        if (f1 !== 1)
            g.alpha = f1;

        f1 = buffer.readFloat();
        if (f1 != 0)
            g.rotation = f1;

        if (!buffer.readBool())
            g.visible = false;
        if (!buffer.readBool())
            g.touchable = false;
        if (buffer.readBool())
            g.grayed = true;
        g.blendMode = buffer.readByte();

        var filter: number = buffer.readByte();
        if (filter == 1) {
            //TODO: filter support
        }

        var str: string = buffer.readS();
        if (str != null)
            g.data = str;

    }


    static async setupAfterAddAsync(g: GObject, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 1);

        var str: string = buffer.readS();
        // if (str != null)
        // g.tooltips = str;

        var groupId: number = buffer.readShort();
        if (groupId >= 0)
            g.group = <GGroup>g.parent.getChildAt(groupId);

        buffer.seek(beginPos, 2);

        var cnt: number = buffer.readShort();

        if (cnt === 0) return Promise.resolve();

        return new Promise<void>((resolve, reject) => {
            let current = 0;
            let next = () => {
                ++current;
                if (current > cnt) {
                    resolve();
                    return;
                }

                var nextPos: number = buffer.readShort();
                nextPos += buffer.position;
                var gear: GearBase = g.getGear(buffer.readByte());
                GearBaseParse.setup(gear, buffer);
                buffer.position = nextPos;
                if (Application.ins.getFromFrameStartTime() >= 5) {
                    Application.ins.timer.nextTick(this, next);
                    return;
                }
                next();
            }
            next();
        })
    }

    static setupAfterAdd(g: GObject, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 1);

        var str: string = buffer.readS();
        // if (str != null)
        //     this.tooltips = str;

        var groupId: number = buffer.readShort();
        if (groupId >= 0)
            g.group = <GGroup>g.parent.getChildAt(groupId);

        buffer.seek(beginPos, 2);

        var cnt: number = buffer.readShort();
        for (var i: number = 0; i < cnt; i++) {
            var nextPos: number = buffer.readShort();
            nextPos += buffer.position;

            var gear: GearBase = g.getGear(buffer.readByte());
            GearBaseParse.setup(gear, buffer);

            buffer.position = nextPos;
        }
    }
}
