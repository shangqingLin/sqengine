import { LabelComponent } from "../2d";
import { SpriteFrame } from "../assets";
import { Component } from "../framework";


export enum EditBoxInputFlag {
    /**
    * 表明输入的文本是保密的数据，任何时候都应该隐藏起来，它隐含了 EDIT_BOX_INPUT_FLAG_SENSITIVE。
    */
    PASSWORD = 0,
    /**
     * 表明输入的文本是敏感数据，它禁止存储到字典或表里面，也不能用来自动补全和提示用户输入。
     * 一个信用卡号码就是一个敏感数据的例子。
     */
    SENSITIVE = 1,
    /**
     * 这个标志用来指定在文本编辑的时候，是否把每一个单词的首字母大写。
     */
    INITIAL_CAPS_WORD = 2,
    /**
     * 这个标志用来指定在文本编辑是否每个句子的首字母大写。
     */
    INITIAL_CAPS_SENTENCE = 3,
    /**
     * 自动把输入的所有字符大写。
     */
    INITIAL_CAPS_ALL_CHARACTERS = 4,
    /**
     * Don't do anything with the input text.
     */
    DEFAULT = 5,
}


/**
 * 输入模式。
 * @readonly
 * @enum EditBox.InputMode
 */
export enum EditBoxInputMode {
    /**
     * 用户可以输入任何文本，包括换行符。
     */
    ANY = 0,
    /**
     * 允许用户输入一个电子邮件地址。
     */
    EMAIL_ADDR = 1,
    /**
     * 允许用户输入一个整数值。
     */
    NUMERIC = 2,
    /**
     * 允许用户输入一个电话号码。
     */
    PHONE_NUMBER = 3,
    /**
     * 允许用户输入一个 URL。
     */
    URL = 4,
    /**
     * 允许用户输入一个实数。
     */
    DECIMAL = 5,
    /**
     * 除了换行符以外，用户可以输入任何文本。
     */
    SINGLE_LINE = 6,
}



/**
 * 文本输入框组件
 */
export class EditBoxComponent extends Component {

    private _text: string;
    private _maxLength: number;
    private _placeholderLabel: LabelComponent;
    private _inputFlag: EditBoxInputFlag;
    private _backgroundImage: SpriteFrame;
    private _inputMode: EditBoxInputMode;
    private _textLabel: LabelComponent;
    private _tabIndex: number;

    /**
     * 让当前 EditBox 获得焦点。
     */
    focus() {

    }

    /**
     * 输入框的初始输入内容，如果为空则会显示占位符的文本。
     */
    get text(): string {
        return this._text;
    }

    set text(value: string) {
        if (this._maxLength >= 0 && value.length >= this._maxLength) {
            value = value.slice(0, this._maxLength);
        }

        if (this._text === value) {
            return;
        }

        this._text = value;
    }

    /**
     * 输入框占位符的文本内容。
     */
    get placeholder(): string {
        if (!this._placeholderLabel) {
            return '';
        }
        // return this._placeholderLabel.string;
    }

    set placeholder(value) {
        if (this._placeholderLabel) {
            // this._placeholderLabel.string = value;
        }
    }

    /**
     * 输入框输入文本节点上挂载的 Label 组件对象。
     */
    get textLabel(): LabelComponent | null {
        return this._textLabel;
    }

    set textLabel(oldValue) {
        if (this._textLabel !== oldValue) {
            this._textLabel = oldValue;
        }
    }

    /**
     * 输入框占位符节点上挂载的 Label 组件对象。
     */
    get placeholderLabel(): LabelComponent | null {
        return this._placeholderLabel;
    }

    set placeholderLabel(oldValue) {
        if (this._placeholderLabel !== oldValue) {
            this._placeholderLabel = oldValue;
        }
    }

    /**
     * 输入框的背景图片。
     */
    get backgroundImage(): SpriteFrame | null {
        return this._backgroundImage;
    }

    set backgroundImage(value: SpriteFrame | null) {
        if (this._backgroundImage === value) {
            return;
        }
        this._backgroundImage = value;
    }

    /**
     * 指定输入标志位，可以指定输入方式为密码或者单词首字母大写。
     */
    get inputFlag(): EditBoxInputFlag {
        return this._inputFlag;
    }

    set inputFlag(value) {
        if (this._inputFlag === value) {
            return;
        }

        this._inputFlag = value;
    }

    /**
     * 指定输入模式: ANY表示多行输入，其它都是单行输入，移动平台上还可以指定键盘样式。
     */
    get inputMode(): EditBoxInputMode {
        return this._inputMode;
    }

    set inputMode(oldValue) {
        if (this._inputMode !== oldValue) {
            this._inputMode = oldValue;
        }
    }

    /**
     * 输入框最大允许输入的字符个数。
     * - 如果值为小于 0 的值，则不会限制输入字符个数。
     * - 如果值为 0，则不允许用户进行任何输入。
     */
    get maxLength(): number {
        return this._maxLength;
    }
    set maxLength(value: number) {
        this._maxLength = value;
    }

    /**
     * 修改 DOM 输入元素的 tabIndex（这个属性只有在 Web 上面修改有意义）。
     */
    get tabIndex(): number {
        return this._tabIndex;
    }

    set tabIndex(value) {
        if (this._tabIndex !== value) {
            this._tabIndex = value;
        }
    }

}