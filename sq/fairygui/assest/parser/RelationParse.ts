import { GObject } from "../../GObject";
import { RelationItem, Relations } from "../../Relations";
import { ByteBuffer } from "../../utils/ByteBuffer";

export default class RelationParse {
    static async setupAsync(relation: Relations, buffer: ByteBuffer, parentToChild: boolean) {
        var cnt: number = buffer.readByte();
        for (var i: number = 0; i < cnt; i++) {
            this.steupOne(relation, buffer, parentToChild);
        }
    }

    static setup(relation: Relations, buffer: ByteBuffer, parentToChild: boolean) {
        var cnt: number = buffer.readByte();
        for (var i: number = 0; i < cnt; i++) {
            this.steupOne(relation, buffer, parentToChild);
        }
    }

    private static steupOne(relation: Relations, buffer: ByteBuffer, parentToChild: boolean): void {
        var target: GObject;
        var targetIndex: number = buffer.readShort();
        if (targetIndex == -1)
            //@ts-ignore
            target = relation._owner.parent;
        else if (parentToChild)
            //@ts-ignore
            target = (<GComponent>relation._owner).getChildAt(targetIndex);
        else
            //@ts-ignore
            target = relation._owner.parent.getChildAt(targetIndex);

        //@ts-ignore
        var newItem: RelationItem = new RelationItem(relation._owner);
        newItem.target = target;

        //@ts-ignore
        relation._items.push(newItem);

        var cnt2: number = buffer.readByte();
        for (var j: number = 0; j < cnt2; j++) {
            var rt: number = buffer.readByte();
            var usePercent: boolean = buffer.readBool();
            newItem.internalAdd(rt, usePercent);
        }
    }
}