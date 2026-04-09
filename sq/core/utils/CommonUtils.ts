export default {
    now: (!window.performance || !window.performance.now || !window.performance.timing) ? Date.now : function () {
        return window.performance.now();
    },

    isDefine(v: any) {
        if (v !== undefined && v !== null && v !== "") {
            if (typeof v === "number") return !isNaN(v);
            return true;
        }
        return false;
    }
} as {
    now(): number;
    isDefine(v: any): boolean;
}