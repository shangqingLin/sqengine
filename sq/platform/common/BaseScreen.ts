import { EventDispatch } from "../../core";

export default class BaseScreen extends EventDispatch {
    protected nativeScreen: Module.Screen;
    public initialize(options?: any) {
        this.nativeScreen = new window.Module.Screen();
    }

    public setDesignSize(width: number, height: number) {
        this.nativeScreen.setDesignSize(width, height);
    }

    public setRenderSize(width: number, height: number): void {
        this.nativeScreen.setRenderSize(width, height);
    }
}