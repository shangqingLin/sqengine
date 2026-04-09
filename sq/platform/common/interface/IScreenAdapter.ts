import type { Size } from "../../../core/index";
import type { WindowType } from "../../index";

export default interface IScreenAdapter
{
    get devicePixelRatio(): number;
    set windowType(type:WindowType);
    getScreenSize(): Readonly<Size>;
    setRenderSize(width: number, height: number) : void;
    getRenderSize(): Readonly<Size>;
}