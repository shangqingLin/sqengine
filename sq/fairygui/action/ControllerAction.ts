
import { Controller } from "../Controller";
import { ByteBuffer } from "../utils/ByteBuffer";

export class ControllerAction {
    public fromPage: Array<string>;
    public toPage: Array<string>;

    constructor() {
    }

    public run(controller: Controller, prevPage: string, curPage: string): void {
        if ((!this.fromPage || this.fromPage.length == 0 || this.fromPage.indexOf(prevPage) != -1)
            && (!this.toPage || this.toPage.length == 0 || this.toPage.indexOf(curPage) != -1))
            this.enter(controller);
        else
            this.leave(controller);
    }

    protected enter(controller: Controller): void {

    }

    protected leave(controller: Controller): void {

    }
}
