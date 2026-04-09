import { serializable, sqclass, SQFloat, Vec2, type, Size, Rect } from "../../core";
import { Component } from "../../framework";
import Node from "../../scene/Node";
import { SpriteComponent } from "./sprite/SpriteComponent";
import { RenderTexutre, Texture2d } from "../../assets";

/**
 * 1、视差滚动
 * 2、无限重复
 */
@sqclass("sq.Parallax2DComponent")
export default class Parallax2DComponent extends Component {
    /**
     * 视差滚动
     */
    private _scrollScaleX: number = 0.0;
    private _scrollScaleY: number = 0.0;
    private _mapPos: Vec2 = new Vec2();

    /**
     * 下面是Repeat一张图片逻辑
     */
    @serializable
    @type(Size)
    private repeatNodePool: Array<Node>;
    private repeatNodeActive: Array<Node>;
    private repeatTexture: Texture2d | RenderTexutre;
    private scaleOffsetScrollPos: Vec2;
    private repeatViewport: Rect;
    private afterDeserialize() {

    }

    private createRepeatNode(): Node {
        let node: Node = this.repeatNodePool.pop();
        if (!node) {
            node = new Node();
            let sprite = node.addComponent(SpriteComponent);

            //相对于左下角进行定位
            sprite.anchorX = 0;
            sprite.anchorY = 0;
            sprite.texture = this.repeatTexture;
            this.node.addChild(node);
            // console.info("repeat node",node.getId());
        }
        node.visible = true;
        this.repeatNodeActive.push(node);
        return node;
    }


    /**
     * 视差是以屏幕第一象限左下角为原点，你可以设置你的原点
     * setRepeatViewport就是设置原点
     * @param viewport 
     */
    public setRepeatViewport(viewport: Rect) {
        this.repeatViewport = viewport;
        if (this.repeatTexture) {
            this.updateRepeatLayout();
        }
    }

    public setRepeatTexture(texture: Texture2d | RenderTexutre) {
        this.repeatTexture = texture;
        this.repeatNodePool = [];
        this.repeatNodeActive = [];
        this.scaleOffsetScrollPos = new Vec2();
        if (this.repeatViewport) {
            this.updateRepeatLayout();
        }
    }

    private updateRepeatLayout() {
        for (let i = 0; i < this.repeatNodeActive.length; ++i) {
            let node = this.repeatNodeActive[i];
            node.visible = false;
            this.repeatNodePool.push(node);
        }

        this.repeatNodeActive.length = 0;

        //逻辑看上去有点难懂：
        //其实就是将背景看做静态的铺好的地图，从坐标原点开始平铺：
        // 从左往右： 第一张为（0，y）,第二张为(this.repeatSize.width,y)，第三为(this.repeatSize.width * 2,y)……
        // 从右往左：第一张为（-this.repeatSize.width），第二张为（-this.repeatSize.width * 2，y）……
        // 等上面平铺好了，再用照相机在上面左右挪动的逻辑

        //构建一个可视区域Bound。min就是当前的位置repeatScrollPos，max就是当前位置加上可视区域大小
        let minX = this._mapPos.x - this.scaleOffsetScrollPos.x;
        let minY = this._mapPos.y - this.scaleOffsetScrollPos.y;
        let maxX = minX + this.repeatViewport.width;
        let maxY = minY + this.repeatViewport.height;

        //然后看看这个可视区域内都是哪些节点

        //横坐标落在哪张index上
        let beginIndex = minX < 0 ? Math.ceil(minX / this.repeatTexture.width) : Math.floor(minX / this.repeatTexture.width);
        let lastIndex = maxX < 0 ? Math.ceil(maxX / this.repeatTexture.width) : Math.floor(maxX / this.repeatTexture.width);

        // console.info(minX, maxX, this.repeatTexture.width, beginIndex, lastIndex);

        // console.info("====================");
        for (let i = beginIndex; i <= lastIndex; ++i) {
            let node = this.createRepeatNode();

            //this.repeatViewport.x + i * this.repeatTexture.width 这部分这个计算正常的图片位置
            //然后再添加this.scaleOffsetScrollPos.x减少照相机对节点的位移,模拟视差
            node.transform.x = this.repeatViewport.x + i * this.repeatTexture.width + this.scaleOffsetScrollPos.x;
            node.transform.y = this.repeatViewport.y + this.scaleOffsetScrollPos.y;
        }
    }

    /**
     * 设置为0为完全停止，设置为1表示与Camera一样的移动速度
     */
    @serializable
    @type(SQFloat)
    public set scrollScaleX(s: number) {
        this._scrollScaleX = 1 - s;
    }

    @serializable
    @type(SQFloat)
    public set scrollScaleY(s: number) {
        this._scrollScaleY = 1 - s;
    }


    public moveTo(mapX: number, mapY: number) {
        let offsetX = mapX - this._mapPos.x;
        let offsetY = mapY - this._mapPos.y;
        this._mapPos.set(mapX, mapY);
        let scrollOffsetX = offsetX * this._scrollScaleX;
        let scrollOffsetY = offsetY * this._scrollScaleY;
        if (this.repeatTexture) {
            this.scaleOffsetScrollPos.x += scrollOffsetX;
            this.scaleOffsetScrollPos.y += scrollOffsetY;
            this.updateRepeatLayout();
            // console.info(mapX, mapY, offsetX, offsetY, this._scrollScaleX, this._scrollScaleY);
        } else {
            this.node.transform.setWorldTranslate(scrollOffsetX, scrollOffsetY);
        }
    }
}