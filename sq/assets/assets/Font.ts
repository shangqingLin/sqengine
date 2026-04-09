import { sqclass } from "../../core/index";
import Asset from "./Asset";

@sqclass("sq.Font")
export default class Font extends Asset
{

    constructor()
    {
        super();
        this.native = new window.Module.FreeTypeFont(this.id);
    }
}