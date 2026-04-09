import { SpriteComponent } from "../2d/index";
import type { SpriteFrame, Texture2d } from "../assets/index";
import UINode from "./UINode";

export default class UISprite extends UINode {
    protected override createUIContentComponent(): void {
        this.content = this.addComponent(SpriteComponent);
    }
    
    set spriteFrame(sprite:SpriteFrame) {
        (this.content as SpriteComponent).spriteFrame = sprite;
    }

    get spriteFrame(): Texture2d | SpriteFrame {
        return (this.content as SpriteComponent).spriteFrame;
    }

    set spriteFrameUUID(uuid: string) {
        (this.content as SpriteComponent).spriteFrameUUID = uuid;
    }

    set spriteFrameUrl(url: string) {
        (this.content as SpriteComponent).spriteFrameUrl = url;
    }
}