import { ObjectPropID } from "../common/FieldTypes";
import { GearBase } from "./GearBase";

interface GearAnimationValue {
    playing?: boolean;
    frame?: number;
}

export class GearAnimation extends GearBase {
    private _storage: { [index: string]: GearAnimationValue };
    private _default: GearAnimationValue;

    protected override init(): void {
        this._default = {
            playing: this._owner.getProp(ObjectPropID.Playing),
            frame: this._owner.getProp(ObjectPropID.Frame)
        };
        this._storage = {};
    }

    protected override addStatus(pageId: string, ...param:any): void {
        var gv: GearAnimationValue;
        if (!pageId)
            gv = this._default;
        else {
            gv = {};
            this._storage[pageId] = gv;
        }
        gv.playing = param[0] as boolean;
        gv.frame = param[1] as number;
    }

    public override apply(): void {
        this._owner._gearLocked = true;

        var gv: GearAnimationValue = this._storage[this._controller.selectedPageId] || this._default;
        this._owner.setProp(ObjectPropID.Playing, gv.playing);
        this._owner.setProp(ObjectPropID.Frame, gv.frame);

        this._owner._gearLocked = false;
    }

    public override updateState(): void {
        var gv: GearAnimationValue = this._storage[this._controller.selectedPageId];
        if (!gv) {
            gv = {};
            this._storage[this._controller.selectedPageId] = gv;
        }
        gv.playing = this._owner.getProp(ObjectPropID.Playing);
        gv.frame = this._owner.getProp(ObjectPropID.Frame);
    }
}

