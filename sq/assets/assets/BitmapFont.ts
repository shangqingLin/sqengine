import { serializable, sqclass, type } from "../../core";
import Font from "./Font";
import SpriteFrame from "./SpriteFrame";


export interface IConfig {
    [key: string]: any;
}

export class FontLetterDefinition {
    public u = 0;
    public v = 0;
    public w = 0;
    public h = 0;
    public offsetX = 0;
    public offsetY = 0;
    public textureID = 0;
    public valid = false;
    public xAdvance = 0;
}

export interface ILetterDefinition {
    [key: string]: FontLetterDefinition;
}

export class FontAtlas {
    public letterDefinitions: { [key: string]: any };
    public declare texture;

    constructor(texture: any) {
        this.letterDefinitions = {};
        this.texture = texture;
    }

    public addLetterDefinitions(letter: any, letterDefinition: any): void {
        this.letterDefinitions[letter] = letterDefinition;
    }

    public cloneLetterDefinition(): ILetterDefinition {
        const copyLetterDefinitions: ILetterDefinition = {};
        for (const key of Object.keys(this.letterDefinitions)) {
            const value = new FontLetterDefinition();
            // js.mixin(value, this.letterDefinitions[key]);
            copyLetterDefinitions[key] = value;
        }
        return copyLetterDefinitions;
    }

    public getTexture(): any {
        return this.texture;
    }

    public getLetter(key: any): any {
        return this.letterDefinitions[key];
    }

    public getLetterDefinitionForChar(char: string, labelInfo?: any): any {
        const key = char.charCodeAt(0);
        const hasKey = this.letterDefinitions.hasOwnProperty(key);
        let letter;
        if (hasKey) {
            letter = this.letterDefinitions[key];
        } else {
            letter = null;
        }
        return letter;
    }

    public clear(): void {
        this.letterDefinitions = {};
    }
}

@sqclass('sq.BitmapFont')
export class BitmapFont extends Font {
    @serializable
    public fntDataStr = '';

    /**
     * @zh 位图字体所使用的精灵。
     */
    @type(SpriteFrame)
    public spriteFrame: SpriteFrame | null = null;

    /**
     * 文字尺寸。
     */
    @serializable
    public fontSize = -1;


    @serializable
    public fntConfig: IConfig | null = null;

    /**
     * @deprecated since v3.7.0, this is an engine private interface that will be removed in the future.
     */
    public declare fontDefDictionary: FontAtlas;

    override onLoad(): void {
        const spriteFrame = this.spriteFrame;
        if (!this.fontDefDictionary && spriteFrame) {
            this.fontDefDictionary = new FontAtlas(spriteFrame.texture);
        }

        const fntConfig = this.fntConfig;
        if (!fntConfig) {
            return;
        }

        const fontDict = fntConfig.fontDefDictionary;
        for (const fontDef in fontDict) {
            const letter = new FontLetterDefinition();
            const rect = fontDict[fontDef].rect;
            letter.offsetX = fontDict[fontDef].xOffset;
            letter.offsetY = fontDict[fontDef].yOffset;
            letter.w = rect.width;
            letter.h = rect.height;
            letter.u = rect.x;
            letter.v = rect.y;
            // FIXME: only one texture supported for now
            letter.textureID = 0;
            letter.valid = true;
            letter.xAdvance = fontDict[fontDef].xAdvance;

            this.fontDefDictionary.addLetterDefinitions(fontDef, letter);
        }
    }
}
