import Vec2 from "./Vec2";

export default class AABB {
    /**
      * AABB盒子的左下角
      */
    public lowerBound: Vec2 = new Vec2;

    /**
     * AABB盒子的右上角
     */
    public upperBound: Vec2 = new Vec2;

    public static TEMP: AABB = new AABB();
    public static TEMP2: AABB = new AABB();

    public copyFrom(aabb: AABB) {
        this.lowerBound.copy(aabb.lowerBound);
        this.upperBound.copy(aabb.upperBound);
    }

    public toString():string
    {
        return "lower("+this.lowerBound.x +"," + this.lowerBound.y+") upper:("+this.upperBound.x +","+this.upperBound.y+")";

    }

    public static Union(a: AABB, b: AABB, c?: AABB) : AABB {
        c = c || new AABB();
        c.lowerBound.x = Math.min(a.lowerBound.x, b.lowerBound.x);
        c.lowerBound.y = Math.min(a.lowerBound.y, b.lowerBound.y);
        c.upperBound.x = Math.max(a.upperBound.x, b.upperBound.x);
        c.upperBound.y = Math.max(a.upperBound.y, b.upperBound.y);
        return c;
    }

    public static Center(a: AABB, b?: Vec2): Vec2 {
        b = b || new Vec2;
        b.x = 0.5 * (a.lowerBound.x + a.upperBound.x);
        b.y = 0.5 * (a.lowerBound.y + a.upperBound.y);
        return b;
    }

    public static Extents(a: AABB, b?: Vec2) : Vec2 {
        b = b || new Vec2;
        b.x = 0.5 * (a.upperBound.x - a.lowerBound.x);
        b.y = 0.5 * (a.upperBound.y - a.lowerBound.y);
        return b;
    }

    public static Perimeter(a: AABB): number {
        let wx: number = a.upperBound.x - a.lowerBound.x;
        let wy: number = a.upperBound.y - a.lowerBound.y;
        return 2.0 * (wx + wy);
    }

    public static isValid(a: AABB): boolean {
        let d: Vec2 = Vec2.temp1Vec2.sub2(a.upperBound, a.lowerBound);
        let valid: boolean = d.x >= 0.0 && d.y >= 0.0;
        valid = valid && Vec2.isValid(a.lowerBound) && Vec2.isValid(a.upperBound);
        return valid;
    }

    public static enlargeAABB(a: AABB, b: AABB): boolean {
        let changed = false;
        if (b.lowerBound.x < a.lowerBound.x) {
            a.lowerBound.x = b.lowerBound.x;
            changed = true;
        }

        if (b.lowerBound.y < a.lowerBound.y) {
            a.lowerBound.y = b.lowerBound.y;
            changed = true;
        }

        if (a.upperBound.x < b.upperBound.x) {
            a.upperBound.x = b.upperBound.x;
            changed = true;
        }

        if (a.upperBound.y < b.upperBound.y) {
            a.upperBound.y = b.upperBound.y;
            changed = true;
        }
        return changed;
    }

    public static Contains(a: AABB, b: AABB): boolean {
        let s: boolean = true;
        s = s && a.lowerBound.x <= b.lowerBound.x;
        s = s && a.lowerBound.y <= b.lowerBound.y;
        s = s && b.upperBound.x <= a.upperBound.x;
        s = s && b.upperBound.y <= a.upperBound.y;
        return s;
    }

    public static Overlaps(a: AABB, b: AABB) : boolean {
        return !(b.lowerBound.x > a.upperBound.x || b.lowerBound.y > a.upperBound.y || a.lowerBound.x > b.upperBound.x ||
            a.lowerBound.y > b.upperBound.y);
    }
}