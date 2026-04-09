import { ActionType, Item, Transition, TValue, TweenConfig } from "../../Transition";
import { GPath } from "../../tween/GPath";
import { CurveType, GPathPoint } from "../../tween/GPathPoint";
import { ByteBuffer } from "../../utils/ByteBuffer";


export default class TransitionParse {


    private static decodeValue(item: Item, buffer: ByteBuffer, value: TValue): void {
        switch (item.type) {
            case ActionType.XY:
            case ActionType.Size:
            case ActionType.Pivot:
            case ActionType.Skew:
                value.b1 = buffer.readBool();
                value.b2 = buffer.readBool();
                value.f1 = buffer.readFloat();
                value.f2 = buffer.readFloat();

                if (buffer.version >= 2 && item.type === ActionType.XY)
                    value.b3 = buffer.readBool(); //percent
                break;

            case ActionType.Alpha:
            case ActionType.Rotation:
                value.f1 = buffer.readFloat();
                break;

            case ActionType.Scale:
                value.f1 = buffer.readFloat();
                value.f2 = buffer.readFloat();
                break;

            case ActionType.Color:
                let color = buffer.readColor();
                value.f1 = (color.r << 16) + (color.g << 8) + color.b;
                break;

            case ActionType.Animation:
                value.playing = buffer.readBool();
                value.frame = buffer.readInt();
                break;

            case ActionType.Visible:
                value.visible = buffer.readBool();
                break;

            case ActionType.Sound:
                value.sound = buffer.readS();
                value.volume = buffer.readFloat();
                break;

            case ActionType.Transition:
                value.transName = buffer.readS();
                value.playTimes = buffer.readInt();
                break;

            case ActionType.Shake:
                value.amplitude = buffer.readFloat();
                value.duration = buffer.readFloat();
                break;

            case ActionType.ColorFilter:
                value.f1 = buffer.readFloat();
                value.f2 = buffer.readFloat();
                value.f3 = buffer.readFloat();
                value.f4 = buffer.readFloat();
                break;

            case ActionType.Text:
            case ActionType.Icon:
                value.text = buffer.readS();
                break;
        }
    }

    static setup(transition: Transition, buffer: ByteBuffer) {
        transition.name = buffer.readS();

        //@ts-ignore
        transition._options = buffer.readInt();

        //@ts-ignore
        transition._autoPlay = buffer.readBool();

        //@ts-ignore
        transition._autoPlayTimes = buffer.readInt();

        //@ts-ignore
        transition._autoPlayDelay = buffer.readFloat();

        var cnt: number = buffer.readShort();
        for (var i: number = 0; i < cnt; i++) {
            var dataLen: number = buffer.readShort();
            var curPos: number = buffer.position;

            buffer.seek(curPos, 0);

            var item: Item = new Item(buffer.readByte());

            //@ts-ignore
            transition._items[i] = item;

            item.time = buffer.readFloat();
            var targetId: number = buffer.readShort();
            if (targetId < 0)
                item.targetId = "";
            else
                //@ts-ignore
                item.targetId = transition._owner.getChildAt(targetId).id;
            item.label = buffer.readS();

            if (buffer.readBool()) {
                buffer.seek(curPos, 1);

                item.tweenConfig = new TweenConfig();
                item.tweenConfig.duration = buffer.readFloat();

                //@ts-ignore
                if (item.time + item.tweenConfig.duration > transition._totalDuration)
                    //@ts-ignore
                    transition._totalDuration = item.time + item.tweenConfig.duration;

                item.tweenConfig.easeType = buffer.readByte();
                item.tweenConfig.repeat = buffer.readInt();
                item.tweenConfig.yoyo = buffer.readBool();
                item.tweenConfig.endLabel = buffer.readS();

                buffer.seek(curPos, 2);

                this.decodeValue(item, buffer, item.tweenConfig.startValue);

                buffer.seek(curPos, 3);

                this.decodeValue(item, buffer, item.tweenConfig.endValue);

                if (buffer.version >= 2) {
                    var pathLen: number = buffer.readInt();
                    if (pathLen > 0) {
                        item.tweenConfig.path = new GPath();
                        var pts: Array<GPathPoint> = new Array<GPathPoint>();

                        for (var j: number = 0; j < pathLen; j++) {
                            var curveType: number = buffer.readByte();
                            switch (curveType) {
                                case CurveType.Bezier:
                                    pts.push(GPathPoint.newBezierPoint(buffer.readFloat(), buffer.readFloat(),
                                        buffer.readFloat(), buffer.readFloat()));
                                    break;

                                case CurveType.CubicBezier:
                                    pts.push(GPathPoint.newCubicBezierPoint(buffer.readFloat(), buffer.readFloat(),
                                        buffer.readFloat(), buffer.readFloat(),
                                        buffer.readFloat(), buffer.readFloat()));
                                    break;

                                default:
                                    pts.push(GPathPoint.newPoint(buffer.readFloat(), buffer.readFloat(), curveType));
                                    break;
                            }
                        }

                        item.tweenConfig.path.create(pts);
                    }
                }
            }
            else {
                //@ts-ignore
                if (item.time > transition._totalDuration)
                    //@ts-ignore
                    transition._totalDuration = item.time;

                buffer.seek(curPos, 2);

                this.decodeValue(item, buffer, item.value);
            }

            buffer.position = curPos + dataLen;
        }
    }
}