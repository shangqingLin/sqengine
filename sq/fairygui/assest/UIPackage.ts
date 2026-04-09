import { PackageItem } from "./PackageItem";
import { Asset, AssetManager, AudioClip, BitmapFont, ImageAsset, SpriteAtlas, SpriteFrame, Texture2d } from "../../assets";
import { AssetLoadType } from "../../assets/assetmanager/AssetManager";
import { ByteBuffer } from "../utils/ByteBuffer";
import { Rect, Size, Vec2 } from "../../core";
import { GObject } from "../GObject";
import { ObjectType, PackageItemType } from "../common/FieldTypes";
import { TranslationHelper } from "../TranslationHelper";
import { Frame } from "../display/MovieClip";
import PathUtils from "../../core/utils/path";
import ObjectCreateProcess from "./ObjectCreateProcess";
import { UIObjectFactory } from "./UIObjectFactory";

type PackageDependency = { id: string, name: string };


/**
 * 一个Packge就是一个UIPackage实例
 */
export class UIPackage {
    private _id: string;
    private _name: string;
    private _path: string;
    private _items: Array<PackageItem>;
    private _itemsById: { [index: string]: PackageItem };
    private _itemsByName: { [index: string]: PackageItem };
    private _sprites: { [index: string]: AtlasSprite };
    private _dependencies: Array<PackageDependency>;
    private _branches: Array<string>;
    public _branchIndex: number;

    public constructor() {
        this._items = [];
        this._itemsById = {};
        this._itemsByName = {};
        this._sprites = {};
        this._dependencies = [];
        this._branches = [];
        this._branchIndex = -1;
    }

    public static get branch(): string | null {
        return _branch;
    }

    public static set branch(value: string | null) {
        _branch = value;
        for (var pkgId in _instById) {
            var pkg: UIPackage = _instById[pkgId];
            if (pkg._branches) {
                pkg._branchIndex = pkg._branches.indexOf(value);
            }
        }
    }

    public static getVar(key: string): string | null {
        return _vars[key];
    }

    public static setVar(key: string, value: string | null) {
        _vars[key] = value;
    }

    public static getById(id: string): UIPackage {
        return _instById[id];
    }

    public static getByName(name: string): UIPackage {
        return _instByName[name];
    }

    /**
     * 加载一个fgui文件。并创建对应的UIPackage实例
     * @param path 
     * @param onComplete 
     */
    public static loadPackage(path: string, onComplete?: (error: any, pkg: UIPackage) => void) {
        AssetManager.getInstance().load(path, AssetLoadType.BUFFER, (err: Error | null, asset: Asset) => {
            if (err) {
                if (onComplete != null)
                    onComplete(err, null);
                return;
            }

            let pkg: UIPackage = new UIPackage();
            let buffer: ArrayBuffer = asset.file as ArrayBuffer;
            asset.destroy();
            pkg.loadPackage(new ByteBuffer(buffer), path);
            let cnt: number = pkg._items.length;
            let urls: Array<string> = [];
            for (var i: number = 0; i < cnt; i++) {
                var pi: PackageItem = pkg._items[i];
                if (pi.type === PackageItemType.Atlas || pi.type === PackageItemType.Sound) {
                    urls.push(pi.file);
                }
            }

            //加载依赖的资源
            let total = urls.length;
            let lastErr: Error;
            let taskComplete = (err: Error | null, asset: Asset) => {
                total--;
                if (err)
                    lastErr = err;

                if (total <= 0) {
                    _instById[pkg.id] = pkg;
                    _instByName[pkg.name] = pkg;
                    if (pkg._path)
                        _instById[pkg._path] = pkg;

                    if (onComplete != null)
                        onComplete(lastErr, pkg);
                }
            }
            if (total > 0) {

                AssetManager.getInstance().load(urls, taskComplete);
            }
            else
                taskComplete(null, null);
        });
    }

    public static removePackage(packageIdOrName: string): void {
        var pkg: UIPackage = _instById[packageIdOrName];
        if (!pkg)
            pkg = _instByName[packageIdOrName];
        if (!pkg)
            throw new Error("No package found: " + packageIdOrName);
        pkg.dispose();
        delete _instById[pkg.id];
        delete _instByName[pkg.name];
        if (pkg._path)
            delete _instById[pkg._path];
    }

    public static createObject(pkgName: string, resName: string, userClass?: new () => GObject): GObject {
        var pkg: UIPackage = UIPackage.getByName(pkgName);
        if (pkg)
            return pkg.createObject(resName, userClass);
        else
            return null;
    }

    public static createObjectFromURL(url: string, userClass?: new () => GObject): GObject {
        var pi: PackageItem = UIPackage.getItemByURL(url);
        if (pi)
            return pi.owner.internalCreateObject(pi, userClass);
        else
            return null;
    }

    public static getItemURL(pkgName: string, resName: string): string {
        var pkg: UIPackage = UIPackage.getByName(pkgName);
        if (!pkg)
            return null;

        var pi: PackageItem = pkg._itemsByName[resName];
        if (!pi)
            return null;

        return "ui://" + pkg.id + pi.id;
    }

    public static getItemByURL(url: string): PackageItem {
        var pos1: number = url.indexOf("//");
        if (pos1 == -1)
            return null;

        var pos2: number = url.indexOf("/", pos1 + 2);
        if (pos2 == -1) {
            if (url.length > 13) {
                var pkgId: string = url.substring(5, 13);
                var pkg: UIPackage = UIPackage.getById(pkgId);
                if (pkg != null) {
                    var srcId: string = url.substring(13);
                    return pkg.getItemById(srcId);
                }
            }
        }
        else {
            var pkgName: string = url.substring(pos1 + 2, pos2);
            pkg = UIPackage.getByName(pkgName);
            if (pkg != null) {
                var srcName: string = url.substring(pos2 + 1);
                return pkg.getItemByName(srcName);
            }
        }

        return null;
    }

    public static normalizeURL(url: string): string {
        if (url == null)
            return null;

        var pos1: number = url.indexOf("//");
        if (pos1 == -1)
            return null;

        var pos2: number = url.indexOf("/", pos1 + 2);
        if (pos2 == -1)
            return url;

        var pkgName: string = url.substring(pos1 + 2, pos2);
        var srcName: string = url.substring(pos2 + 1);
        return UIPackage.getItemURL(pkgName, srcName);
    }

    public static setStringsSource(source: string): void {
        TranslationHelper.loadFromXML(source);
    }

    /**
     * 从二进制文件中解析fgui文件
     * @param buffer 
     * @param path 
     */
    private loadPackage(buffer: ByteBuffer, path: string): void {

        //检查文件的fgui版本是否与当前库对齐
        if (buffer.readUint() != 0x46475549)
            throw new Error("FairyGUI: old package format found in '" + path + "'");

        this._path = path;
        buffer.version = buffer.readInt();
        var ver2: boolean = buffer.version >= 2;
        var compressed: boolean = buffer.readBool();
        this._id = buffer.readString();
        this._name = buffer.readString();
        buffer.skip(20);

        var indexTablePos: number = buffer.position;
        var cnt: number;
        var i: number;
        var nextPos: number;
        var str: string;
        var branchIncluded: boolean;

        buffer.seek(indexTablePos, 4);

        cnt = buffer.readInt();
        var stringTable: Array<string> = new Array<string>(cnt);
        buffer.stringTable = stringTable;

        for (i = 0; i < cnt; i++)
            stringTable[i] = buffer.readString();

        if (buffer.seek(indexTablePos, 5)) {
            cnt = buffer.readInt();
            for (i = 0; i < cnt; i++) {
                let index = buffer.readUshort();
                let len = buffer.readInt();
                stringTable[index] = buffer.readString(len);
            }
        }

        buffer.seek(indexTablePos, 0);
        cnt = buffer.readShort();
        for (i = 0; i < cnt; i++)
            this._dependencies.push({ id: buffer.readS(), name: buffer.readS() });

        if (ver2) {
            cnt = buffer.readShort();
            if (cnt > 0) {
                this._branches = buffer.readSArray(cnt);
                if (_branch)
                    this._branchIndex = this._branches.indexOf(_branch);
            }

            branchIncluded = cnt > 0;
        }

        buffer.seek(indexTablePos, 1);

        var pi: PackageItem;
        let pos = path.lastIndexOf('/');
        let shortPath = pos === -1 ? "" : path.substring(0, pos + 1);
        path = path + "_";

        //包中共有多少个文件（xml、每一张图片作为一个文件等）
        cnt = buffer.readShort();
        for (i = 0; i < cnt; i++) {
            nextPos = buffer.readInt();
            nextPos += buffer.position;

            pi = new PackageItem();
            pi.owner = this;
            pi.type = buffer.readByte();
            pi.id = buffer.readS();
            pi.name = buffer.readS();
            buffer.readS(); //path
            pi.file = buffer.readS();
            buffer.readBool();//exported
            pi.width = buffer.readInt();
            pi.height = buffer.readInt();

            // console.info(pi);

            switch (pi.type) {
                case PackageItemType.Image:
                    {
                        //表示为单张图片
                        pi.objectType = ObjectType.Image;
                        var scaleOption: number = buffer.readByte();
                        if (scaleOption == 1) {
                            pi.scale9Grid = new Rect();
                            pi.scale9Grid.x = buffer.readInt();
                            pi.scale9Grid.y = buffer.readInt();
                            pi.scale9Grid.width = buffer.readInt();
                            pi.scale9Grid.height = buffer.readInt();
                            pi.tileGridIndice = buffer.readInt();
                        }
                        else if (scaleOption == 2)
                            pi.scaleByTile = true;

                        pi.smoothing = buffer.readBool();
                        break;
                    }

                case PackageItemType.MovieClip:
                    {
                        //动画文件
                        pi.smoothing = buffer.readBool();
                        pi.objectType = ObjectType.MovieClip;
                        pi.rawData = buffer.readBuffer();
                        break;
                    }

                case PackageItemType.Font:
                    {
                        //字体文件
                        pi.rawData = buffer.readBuffer();
                        break;
                    }

                case PackageItemType.Component:
                    {
                        //xml文件
                        var extension: number = buffer.readByte();
                        if (extension > 0)
                            pi.objectType = extension;
                        else
                            pi.objectType = ObjectType.Component;
                        pi.rawData = buffer.readBuffer();

                        UIObjectFactory.resolveExtension(pi);
                        break;
                    }

                case PackageItemType.Atlas:
                case PackageItemType.Sound:
                case PackageItemType.Misc:
                    {
                        //图集、声音和其他一些杂项文件
                        pi.file = path + PathUtils.getBasename(pi.file);
                        break;
                    }

                case PackageItemType.Spine:
                case PackageItemType.DragonBones:
                    {
                        pi.file = shortPath + PathUtils.getBasename(pi.file);
                        pi.skeletonAnchor = new Vec2();
                        pi.skeletonAnchor.x = buffer.readFloat();
                        pi.skeletonAnchor.y = buffer.readFloat();
                        break;
                    }
            }

            if (ver2) {
                str = buffer.readS();//branch
                if (str)
                    pi.name = str + "/" + pi.name;

                var branchCnt: number = buffer.readByte();
                if (branchCnt > 0) {
                    if (branchIncluded)
                        pi.branches = buffer.readSArray(branchCnt);
                    else
                        this._itemsById[buffer.readS()] = pi;
                }

                var highResCnt: number = buffer.readByte();
                if (highResCnt > 0)
                    pi.highResolution = buffer.readSArray(highResCnt);
            }

            this._items.push(pi);
            this._itemsById[pi.id] = pi;
            if (pi.name != null)
                this._itemsByName[pi.name] = pi;

            buffer.position = nextPos;
        }

        buffer.seek(indexTablePos, 2);

        //创建图集
        cnt = buffer.readShort();
        for (i = 0; i < cnt; i++) {
            nextPos = buffer.readShort();
            nextPos += buffer.position;

            var itemId: string = buffer.readS();
            pi = this._itemsById[buffer.readS()];

            let rect: Rect = new Rect();
            rect.x = buffer.readInt();
            rect.y = buffer.readInt();
            rect.width = buffer.readInt();
            rect.height = buffer.readInt();
            var sprite: AtlasSprite = { atlas: pi, rect: rect, offset: new Vec2(), originalSize: new Size(0, 0) };
            sprite.rotated = buffer.readBool();
            if (ver2 && buffer.readBool()) {
                sprite.offset.x = buffer.readInt();
                sprite.offset.y = buffer.readInt();
                sprite.originalSize.width = buffer.readInt();
                sprite.originalSize.height = buffer.readInt();
            }
            else {
                sprite.originalSize.width = sprite.rect.width;
                sprite.originalSize.height = sprite.rect.height;
            }

            //存储这个包中所有的图集
            this._sprites[itemId] = sprite;

            buffer.position = nextPos;
        }


        if (buffer.seek(indexTablePos, 3)) {
            cnt = buffer.readShort();
            for (i = 0; i < cnt; i++) {
                nextPos = buffer.readInt();
                nextPos += buffer.position;

                pi = this._itemsById[buffer.readS()];
                // if (pi && pi.type == PackageItemType.Image)
                //pi.hitTestData = new PixelHitTestData(buffer);

                buffer.position = nextPos;
            }
        }
    }

    public dispose(): void {
        var cnt: number = this._items.length;
        for (var i: number = 0; i < cnt; i++) {
            var pi: PackageItem = this._items[i];
            if (pi.asset) pi.asset.destroy();
        }
    }

    public get id(): string {
        return this._id;
    }

    public get name(): string {
        return this._name;
    }

    public get path(): string {
        return this._path;
    }

    public get dependencies(): Array<PackageDependency> {
        return this._dependencies;
    }

    public createObject(resName: string, userClass?: new () => GObject): GObject {
        var pi: PackageItem = this._itemsByName[resName];
        if (pi)
            return this.internalCreateObject(pi, userClass);
        else
            return null;
    }

    public internalCreateObject(item: PackageItem, userClass?: new () => GObject): GObject {
        return ObjectCreateProcess.createObject(item, userClass);
    }

    public getItemById(itemId: string): PackageItem {
        return this._itemsById[itemId];
    }

    public getItemByName(resName: string): PackageItem {
        return this._itemsByName[resName];
    }

    public getItemAssetByName(resName: string): Asset {
        var pi: PackageItem = this._itemsByName[resName];
        if (pi == null) {
            throw new Error("Resource not found -" + resName);
        }

        return this.getItemAsset(pi);
    }

    public getItemAsset(item: PackageItem): Asset {
        switch (item.type) {
            case PackageItemType.Image:
                if (!item.decoded) {
                    item.decoded = true;

                    //从图集中取得一个精灵。item.id表示是图集的ID
                    var sprite: AtlasSprite = this._sprites[item.id];
                    if (sprite) {
                        let atlasTexture: Texture2d = <Texture2d>this.getItemAsset(sprite.atlas);
                        if (atlasTexture) {
                            let sf = new SpriteFrame();
                            sf.texture = atlasTexture;
                            sf.rect = sprite.rect;
                            sf.rotated = sprite.rotated;

                            // sf.offset = new Vec2(sprite.offset.x - (sprite.originalSize.width - sprite.rect.width) / 2, 
                            //     -(sprite.offset.y - (sprite.originalSize.height - sprite.rect.height) / 2));

                            //@ts-ignore
                            sf.originalSize = sprite.originalSize;

                            if (item.scale9Grid) {
                                sf.grid9 = [
                                    item.scale9Grid.x,
                                    item.width - item.scale9Grid.xMax,
                                    item.scale9Grid.y,
                                    item.height - item.scale9Grid.yMax
                                ];
                            }
                            item.asset = sf;
                        }
                    }
                }
                break;

            case PackageItemType.Atlas:
                if (!item.decoded) {
                    item.decoded = true;
                    let texture2d: Texture2d = AssetManager.getInstance().getAssetByGameUrl(item.file) as Texture2d;
                    let spriteAtlas = new SpriteAtlas();
                    spriteAtlas.texture = texture2d;
                    item.asset = spriteAtlas;
                    if (!item.asset)
                        console.log("Resource '" + item.file + "' not found");
                }
                break;
            case PackageItemType.Sound:
                if (!item.decoded) {
                    item.decoded = true;
                    item.asset = AssetManager.getInstance().getAssetByGameUrl(item.file);
                    if (!item.asset)
                        console.log("Resource '" + item.file + "' not found");
                }
                break;

            case PackageItemType.Font:
                if (!item.decoded) {
                    item.decoded = true;
                    this.loadFont(item);
                }
                break;

            case PackageItemType.MovieClip:
                if (!item.decoded) {
                    item.decoded = true;
                    this.loadMovieClip(item);
                }
                break;

            default:
                break;
        }

        return item.asset;
    }

    public getItemAssetAsync(item: PackageItem, onComplete?: (err: Error, item: PackageItem) => void): void {
        if (item.decoded) {
            onComplete(null, item);
            return;
        }

        if (item.loading) {
            item.loading.push(onComplete);
            return;
        }

        switch (item.type) {
            case PackageItemType.Spine:
                // item.loading = [onComplete];
                // this.loadSpine(item);
                break;

            case PackageItemType.DragonBones:
                // item.loading = [onComplete];
                // this.loadDragonBones(item);
                break;

            default:
                this.getItemAsset(item);
                onComplete(null, item);
                break;
        }
    }

    public loadAllAssets(): void {
        var cnt: number = this._items.length;
        for (var i: number = 0; i < cnt; i++) {
            var pi: PackageItem = this._items[i];
            this.getItemAsset(pi);
        }
    }

    private loadMovieClip(item: PackageItem): void {
        var buffer: ByteBuffer = item.rawData;

        buffer.seek(0, 0);

        item.interval = buffer.readInt() / 1000;
        item.swing = buffer.readBool();
        item.repeatDelay = buffer.readInt() / 1000;

        buffer.seek(0, 1);

        var frameCount: number = buffer.readShort();
        item.frames = Array<Frame>(frameCount);

        var spriteId: string;
        var sprite: AtlasSprite;

        for (var i: number = 0; i < frameCount; i++) {
            var nextPos: number = buffer.readShort();
            nextPos += buffer.position;

            let rect: Rect = new Rect();
            rect.x = buffer.readInt();
            rect.y = buffer.readInt();
            rect.width = buffer.readInt();
            rect.height = buffer.readInt();
            let addDelay = buffer.readInt() / 1000;
            let frame: Frame = { rect: rect, addDelay: addDelay, texture: null };
            spriteId = buffer.readS();

            if (spriteId != null && (sprite = this._sprites[spriteId]) != null) {
                let atlasTexture: Texture2d = <Texture2d>this.getItemAsset(sprite.atlas);
                if (atlasTexture) {
                    let sx: number = item.width / frame.rect.width;
                    let sf = new SpriteFrame();

                    sf.texture = atlasTexture;
                    sf.rect = sprite.rect;
                    sf.rotated = sprite.rotated;
                    // sf.offset = new Vec2(frame.rect.x - (item.width - frame.rect.width) / 2, -(frame.rect.y - (item.height - frame.rect.height) / 2))

                    //@ts-ignore
                    sf.originalSize = new Size(item.width, item.height);
                    frame.texture = sf;
                }
            }
            item.frames[i] = frame;

            buffer.position = nextPos;
        }
    }

    private loadFont(item: PackageItem): void {
        var font: BitmapFont = new BitmapFont();
        item.asset = font;

        font.fntConfig = {
            commonHeight: 0,
            fontSize: 0,
            kerningDict: {},
            fontDefDictionary: {}
        };
        let dict = font.fntConfig.fontDefDictionary;

        var buffer: ByteBuffer = item.rawData;

        buffer.seek(0, 0);

        let ttf = buffer.readBool();
        let canTint = buffer.readBool();
        let resizable = buffer.readBool();
        buffer.readBool(); //has channel
        let fontSize = buffer.readInt();
        var xadvance: number = buffer.readInt();
        var lineHeight: number = buffer.readInt();

        let mainTexture: Texture2d;
        var mainSprite: AtlasSprite = this._sprites[item.id];
        if (mainSprite)
            mainTexture = <Texture2d>(this.getItemAsset(mainSprite.atlas));

        buffer.seek(0, 1);

        var bg: any;
        var cnt: number = buffer.readInt();
        for (var i: number = 0; i < cnt; i++) {
            var nextPos: number = buffer.readShort();
            nextPos += buffer.position;

            bg = {};
            var ch: number = buffer.readUshort();
            dict[ch] = bg;

            let rect: Rect = new Rect();
            bg.rect = rect;

            var img: string = buffer.readS();
            rect.x = buffer.readInt();
            rect.y = buffer.readInt();
            bg.xOffset = buffer.readInt();
            bg.yOffset = buffer.readInt();
            rect.width = buffer.readInt();
            rect.height = buffer.readInt();
            bg.xAdvance = buffer.readInt();
            bg.channel = buffer.readByte();
            if (bg.channel == 1)
                bg.channel = 3;
            else if (bg.channel == 2)
                bg.channel = 2;
            else if (bg.channel == 3)
                bg.channel = 1;

            if (ttf) {
                rect.x += mainSprite.rect.x;
                rect.y += mainSprite.rect.y;
            }
            else {
                let sprite: AtlasSprite = this._sprites[img];
                if (sprite) {
                    rect.copyFrom(sprite.rect);
                    bg.xOffset += sprite.offset.x;
                    bg.yOffset += sprite.offset.y;
                    if (fontSize == 0)
                        fontSize = sprite.originalSize.height;
                    if (!mainTexture) {
                        sprite.atlas.load();
                        mainTexture = <Texture2d>sprite.atlas.asset;
                    }
                }

                if (bg.xAdvance == 0) {
                    if (xadvance == 0)
                        bg.xAdvance = bg.xOffset + bg.rect.width;
                    else
                        bg.xAdvance = xadvance;
                }
            }

            buffer.position = nextPos;
        }


        font.fontSize = fontSize;
        font.fntConfig.fontSize = fontSize;
        font.fntConfig.commonHeight = lineHeight == 0 ? fontSize : lineHeight;
        font.fntConfig.resizable = resizable;
        font.fntConfig.canTint = canTint;

        let spriteFrame = new SpriteFrame();
        spriteFrame.texture = mainTexture;
        font.spriteFrame = spriteFrame;
        font.onLoad();
    }

    // private loadSpine(item: PackageItem): void {
    //     this._bundle.load(item.file, sp.SkeletonData, (err: Error | null, asset: sp.SkeletonData) => {
    //         item.decoded = true;
    //         item.asset = asset;
    //         let arr = item.loading;
    //         delete item.loading;
    //         arr.forEach(e => e(err, item));
    //     });
    // }

    // private loadDragonBones(item: PackageItem): void {
    //     this._bundle.load(item.file, dragonBones.DragonBonesAsset, (err: Error | null, asset: dragonBones.DragonBonesAsset) => {
    //         if (err) {
    //             item.decoded = true;
    //             let arr = item.loading;
    //             delete item.loading;
    //             arr.forEach(e => e(err, item));
    //             return;
    //         }

    //         item.asset = asset;
    //         let atlasFile = item.file.replace("_ske", "_tex");
    //         let pos = atlasFile.lastIndexOf('.');
    //         if (pos != -1)
    //             atlasFile = atlasFile.substring(0, pos + 1) + "json";
    //         this._bundle.load(atlasFile, dragonBones.DragonBonesAtlasAsset, (err: Error | null, asset: dragonBones.DragonBonesAtlasAsset) => {
    //             item.decoded = true;
    //             item.atlasAsset = asset;

    //             let arr = item.loading;
    //             delete item.loading;
    //             arr.forEach(e => e(err, item));
    //         });
    //     });
    // }
}

interface AtlasSprite {
    atlas: PackageItem;
    rect: Rect;
    offset: Vec2;
    originalSize: Size;
    rotated?: boolean;
}


var _instById: { [index: string]: UIPackage } = {};
var _instByName: { [index: string]: UIPackage } = {};
var _branch: string = "";
var _vars: { [index: string]: string } = {};

export interface IObjectFactoryType {
    resolveExtension(pi: PackageItem): void;
    newObject(type: number | PackageItem, userClass?: new () => GObject): GObject;
}
