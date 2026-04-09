import { GTextInput } from "../../GTextInput";
import { ByteBuffer } from "../../utils/ByteBuffer";
import GTextFieldParse from "./GTextFieldParse";


export default class GTextInputParse extends GTextFieldParse {

    private setupBeforeInputImpl(g: GTextInput, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 4);

        var str: string = buffer.readS();
        if (str != null)
            g.promptText = str;
        else if (g._editBox.placeholderLabel)
            g._editBox.placeholderLabel.text = "";

        str = buffer.readS();
        if (str != null)
            g.restrict = str;

        var iv: number = buffer.readInt();
        if (iv != 0)
            g.maxLength = iv;
        iv = buffer.readInt();
        if (iv != 0) {//keyboardType
        }
        if (buffer.readBool())
            g.password = true;

        //同步一下对齐方式

        if (g._editBox.placeholderLabel) {
            let hAlign = g._editBox.textLabel.horizontalAlign;
            g._editBox.placeholderLabel.horizontalAlign = hAlign;

            let vAlign = g._editBox.textLabel.verticalAlign;
            g._editBox.placeholderLabel.verticalAlign = vAlign;
        }
    }

    protected override async parseSetupBeforeAsync(g: GTextInput, beginPos: number, buffer: ByteBuffer) {
        return Promise.resolve()
            .then(() => {
                return super.parseSetupBeforeAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.setupBeforeInputImpl(g, beginPos, buffer);
            })
    }

    protected override parseSetupBefore(g: GTextInput, beginPos: number, buffer: ByteBuffer): void {
        super.parseSetupBefore(g, beginPos, buffer);
        this.setupBeforeInputImpl(g, beginPos, buffer);
    }
}