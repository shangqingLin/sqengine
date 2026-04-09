import { HorizontalTextAlignment, VerticalTextAlignment } from "../2d";
import { Color } from "../core";
import { EventMouse } from "../input/Events";
import { NodeEventType } from "../scene";
import { EditBoxComponent, EditBoxInputFlag, EditBoxInputMode } from "../ui/EditBoxComponent";
import { AutoSizeType } from "./common/FieldTypes";
import { FUIEvent } from "./FUIEvent";
import { GTextField } from "./GTextField";
import { defaultParser } from "./utils/UBBParser";

export class GTextInput extends GTextField {
    public _editBox: EditBoxComponent;

    private _promptText: string;

    public constructor() {
        super();
        this._node.name = "GTextInput";
        this._touchDisabled = false;
    }

    protected override onCreateUIContent() {
        super.onCreateUIContent();
        
        this._editBox = this._node.addComponent(MyEditBox);
        this._editBox.maxLength = -1;
        this.on(NodeEventType.MOUSE_UP, this.onTouchEnd1, this);

        this.autoSize = AutoSizeType.None;
    }

    public set editable(val: boolean) {
        // this._editBox.enabled = val;
    }

    public get editable(): boolean {
        // return this._editBox.enabled;
        return false;
    }

    public set maxLength(val: number) {
        if (val == 0)
            val = -1;
        this._editBox.maxLength = val;
    }

    public get maxLength(): number {
        return this._editBox.maxLength;
    }

    public set promptText(val: string | null) {
        this._promptText = val;
        let newCreate: boolean = !this._editBox.placeholderLabel;
        if (newCreate)
            this.assignFont(this._editBox.placeholderLabel, this._realFont);
        this._editBox.placeholderLabel.text = defaultParser.parse(this._promptText, true);

        if (defaultParser.lastColor) {
            let c = this._editBox.placeholderLabel.getColor();
            if (!c)
                c = new Color();
            // c.fromHEX(defaultParser.lastColor);
            this.assignFontColor(this._editBox.placeholderLabel, c);
        }
        else
            this.assignFontColor(this._editBox.placeholderLabel, this._color);

        if (defaultParser.lastSize)
            this._editBox.placeholderLabel.fontSize = parseInt(defaultParser.lastSize);
        else
            this._editBox.placeholderLabel.fontSize = this._fontSize;
    }

    public get promptText(): string | null {
        return this._promptText;
    }

    public set restrict(value: string | null) {
        //not supported
    }

    public get restrict(): string | null {
        return "";
    }

    public get password(): boolean {
        return this._editBox.inputFlag === EditBoxInputFlag.PASSWORD;;
    }

    public set password(val: boolean) {
        this._editBox.inputFlag = val ? EditBoxInputFlag.PASSWORD : EditBoxInputFlag.DEFAULT;
    }

    public override get align(): HorizontalTextAlignment {
        return this._editBox.textLabel.horizontalAlign;
    }

    public override set align(value: HorizontalTextAlignment) {
        this._editBox.textLabel.horizontalAlign = value;
        if (this._editBox.placeholderLabel) {
            this._editBox.placeholderLabel.horizontalAlign = value;
        }
    }

    public override get verticalAlign(): VerticalTextAlignment {
        return this._editBox.textLabel.verticalAlign;
    }

    public override set verticalAlign(value: VerticalTextAlignment) {
        this._editBox.textLabel.verticalAlign = value;
        if (this._editBox.placeholderLabel) {
            this._editBox.placeholderLabel.verticalAlign = value;
        }
    }

    public override get singleLine(): boolean {
        return this._editBox.inputMode != EditBoxInputMode.ANY;
    }

    public override set singleLine(value: boolean) {
        this._editBox.inputMode = value ? EditBoxInputMode.SINGLE_LINE : EditBoxInputMode.ANY;
    }

    public requestFocus(): void {
        this._editBox.focus();
    }

    protected override markSizeChanged(): void {
        //不支持自动大小，所以这里空
    }

    protected override updateText(): void {
        var text2: string = this._text;

        if (this._templateVars)
            text2 = this.parseTemplate(text2);

        if (this._ubbEnabled) //不支持同一个文本不同样式
            text2 = defaultParser.parse(text2, true);

        this._editBox.text = text2;
    }

    protected override updateFont() {
        this.assignFont(this._editBox.textLabel, this._realFont);
        if (this._editBox.placeholderLabel)
            this.assignFont(this._editBox.placeholderLabel, this._realFont);
    }

    protected override updateFontColor() {
        this.assignFontColor(this._editBox.textLabel, this._color);
    }

    protected override updateFontSize() {
        this._editBox.textLabel.fontSize = this._fontSize;
        this._editBox.textLabel.lineHeight = this._fontSize + this._leading;
        if (this._editBox.placeholderLabel)
            this._editBox.placeholderLabel.fontSize = this._editBox.textLabel.fontSize;
    }

    protected override updateOverflow() {
        //not supported
    }

    private onTextChanged() {
        this._text = this._editBox.text;
    }

    private onTouchEnd1(evt: EventMouse) {
        // (<MyEditBox>this._editBox).openKeyboard();
        // evt.propagationStopped = true
    }
}

class MyEditBox extends EditBoxComponent {
    // protected _init(): void {
    //     super._init();

    //     this.placeholderLabel.getComponent(UITransform).setAnchorPoint(0, 1);
    //     this.textLabel.getComponent(UITransform).setAnchorPoint(0, 1);
    //     this.placeholderLabel.overflow = Overflow.CLAMP;
    //     this.textLabel.overflow = Overflow.CLAMP;
    // }

    protected _registerEvent() {
        //取消掉原来的事件处理
    }

    public openKeyboard() {
        // let impl = this["_impl"];
        // if (impl) {
        //     impl.beginEditing();
        // }
    }
}