import { Event } from "../input/Events";

export class FUIEvent extends Event {
    public static DISPLAY: string = "fui_display";
    public static UNDISPLAY: string = "fui_undisplay";
    public static GEAR_STOP: string = "fui_gear_stop";
    public static LINK: string = "fui_text_link";
    public static Submit: string = "editing-return";
    public static TEXT_CHANGE: string = "text-changed";

    public static STATUS_CHANGED: string = "fui_status_changed";
    public static XY_CHANGED: string = "fui_xy_changed";
    public static SIZE_CHANGED: string = "fui_size_changed";
    public static SIZE_DELAY_CHANGE: string = "fui_size_delay_change";

    public static DRAG_START: string = "fui_drag_start";
    public static DRAG_MOVE: string = "fui_drag_move";
    public static DRAG_END: string = "fui_drag_end";
    public static DROP: string = "fui_drop";

    public static SCROLL: string = "fui_scroll";
    public static SCROLL_END: string = "fui_scroll_end";
    public static PULL_DOWN_RELEASE: string = "fui_pull_down_release";
    public static PULL_UP_RELEASE: string = "fui_pull_up_release";

    public static CLICK_ITEM: string = "fui_click_item";

    public get isShiftDown(): boolean {
        return false;
    }

    public get isCtrlDown(): boolean {
        return false;
    }
}
