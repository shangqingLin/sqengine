import { ObjectPropID } from "../common/FieldTypes";
import { ByteBuffer } from "../utils/ByteBuffer";
import { GearBase } from "./GearBase";

export class GearFontSize extends GearBase {
    private _storage: { [index: string]: number };
    private _default: number = 0;

    protected override init(): void {
        this._default = this._owner.getProp(ObjectPropID.FontSize);
        this._storage = {};
    }

    protected override addStatus(pageId: string, buffer: ByteBuffer): void {
        if (!pageId)
            this._default = buffer.readInt();
        else
            this._storage[pageId] = buffer.readInt();
    }

    public override apply(): void {
        this._owner._gearLocked = true;

        var data: any = this._storage[this._controller.selectedPageId];
        if (data !== undefined)
            this._owner.setProp(ObjectPropID.FontSize, data);
        else
            this._owner.setProp(ObjectPropID.FontSize, this._default);

        this._owner._gearLocked = false;
    }

    public override updateState(): void {
        this._storage[this._controller.selectedPageId] = this._owner.getProp(ObjectPropID.FontSize);
    }
}
