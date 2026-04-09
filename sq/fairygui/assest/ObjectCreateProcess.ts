import { GObject } from "../GObject";
import { PackageItem } from "./PackageItem";
import { BaseParse } from "./parser/BaseParse";
import { UIObjectFactory } from "./UIObjectFactory";
import { GButton } from "../GButton";
import GButtonParse from "./parser/GButtonParse";
import GComponentParse from "./parser/GComponentParse";
import { GComponent } from "../GComponent";
import { js } from "../../core";
import { GComboBox } from "../GComboBox";
import GComboBoxParse from "./parser/GComboBoxParse";
import { GGraph } from "../GGraph";
import GGraphParse from "./parser/GGraphParse";
import { GGroup } from "../GGroup";
import GGroupParse from "./parser/GGroupParse";
import { GImage } from "../GImage";
import GImageParse from "./parser/GImageParse";
import { GLabel } from "../GLabel";
import GLabelParse from "./parser/GLabelParse";
import { GList } from "../GList";
import GListParse from "./parser/GListParse";
import { GLoader } from "../GLoader";
import GLoaderParse from "./parser/GLoaderParse";
import { GMovieClip } from "../GMovieClip";
import GMovieClipParse from "./parser/GMovieClipParse";
import { GProgressBar } from "../GProgressBar";
import GProgressBarParse from "./parser/GProgressBarParse";
import { GRichTextField } from "../GRichTextField";
import GRichTextFieldParse from "./parser/GRichTextFieldParse";
import { GScrollBar } from "../GScrollBar";
import GScrollBarParse from "./parser/GScrollBarParse";
import { GSlider } from "../GSlider";
import GSliderParse from "./parser/GSliderParse";
import { GTextField } from "../GTextField";
import GTextFieldParse from "./parser/GTextFieldParse";
import { GTextInput } from "../GTextInput";
import GTextInputParse from "./parser/GTextInputParse";
import { GTree } from "../GTree";
import GTreeParase from "./parser/GTreeParse";
export default class ObjectCreateProcess {


    private static constructorParseMap: Map<Constructor<GObject>, Constructor<BaseParse>> = new Map<Constructor<GObject>, Constructor<BaseParse>>();
    private static constructorParseMap2: Map<Constructor<BaseParse>, Constructor<GObject>> = new Map<Constructor<BaseParse>, Constructor<GObject>>();
    private static parsePoolMap: Map<Constructor<GObject>, Array<BaseParse>> = new Map<Constructor<GObject>, Array<BaseParse>>();

    static initialize() {
        let map: Map<Constructor<GObject>, Constructor<BaseParse>> = ObjectCreateProcess.constructorParseMap;
        map.set(GButton, GButtonParse);
        map.set(GComponent, GComponentParse);
        map.set(GComboBox, GComboBoxParse);
        map.set(GGraph, GGraphParse);
        map.set(GGroup, GGroupParse);
        map.set(GImage, GImageParse);
        map.set(GLabel, GLabelParse);
        map.set(GList, GListParse);
        map.set(GLoader, GLoaderParse);
        map.set(GMovieClip, GMovieClipParse);
        map.set(GProgressBar, GProgressBarParse);
        map.set(GRichTextField, GRichTextFieldParse);
        map.set(GScrollBar, GScrollBarParse);
        map.set(GSlider, GSliderParse);
        map.set(GTextField, GTextFieldParse);
        map.set(GTextInput, GTextInputParse);
        map.set(GTree, GTreeParase);
        for (const [key, value] of map) {
            this.constructorParseMap2.set(value, key);
        }

    }

    static getParseByGObject(g: GObject): BaseParse {
        let c: Constructor<GObject> = js.getInstanceConstructor(g) as Constructor<GObject>;
        let parsePool: Array<BaseParse> = ObjectCreateProcess.parsePoolMap.get(c);
        if (!parsePool) {
            parsePool = [];
            ObjectCreateProcess.parsePoolMap.set(c, parsePool);
        }
        let parse: BaseParse = parsePool.pop();
        if (!parse) {
            let parseConstructor = ObjectCreateProcess.constructorParseMap.get(c);
            parse = new parseConstructor();
        }
        return parse;
    }

    static recoveryParse(parse: BaseParse) {
        let parseContructor: Constructor<BaseParse> = js.getInstanceConstructor(parse) as Constructor<BaseParse>;
        let objectContructor: Constructor<GObject> = this.constructorParseMap2.get(parseContructor);
        let parsePool: Array<BaseParse> = ObjectCreateProcess.parsePoolMap.get(objectContructor);
        parsePool.push(parse);
    }

    /**
     * 分帧异步方式创建Object
     */
    static async createAsyncObject(item: PackageItem, userClass?: new () => GObject): Promise<GObject | null> {
        var g: GObject = UIObjectFactory.newObject(item, userClass);
        if (g == null)
            return Promise.resolve(null);
        let parse: GComponentParse = ObjectCreateProcess.getParseByGObject(g) as GComponentParse;
        return parse.constructFromResourceAsync(g as GComponent).then(() => {
            ObjectCreateProcess.recoveryParse(parse);
            return g;
        });
    }

    /**
     * 同步创建Object
     * @param item 
     * @param userClass 
     */
    static createObject(item: PackageItem, userClass?: new () => GObject): GObject | null {
        var g: GObject = UIObjectFactory.newObject(item, userClass);
        if (g == null)
            return null;
        let parse: GComponentParse = ObjectCreateProcess.getParseByGObject(g) as GComponentParse;
        parse.constructFromResource(g as GComponent);
        ObjectCreateProcess.recoveryParse(parse);
        return g;
    }
}