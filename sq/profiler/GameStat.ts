import { value } from "../core";

class GameStat {

    fps: number = 0;

    drawCall: number = 0;
    instanceDrawCall: number = 0;
    graphicsAPICall: number = 0;
    unfiromCall: number = 0;
    bindTextureCall: number = 0;
    bindFrameBufferCall: number = 0;
    bindBuffer: number = 0;
    updateTexture: number = 0;
    useShader: number = 0;
    bufferUpdate: number = 0;
    nodeNum: number = 0;
    vaoBind: number = 0;
    private itemElements: any = {};

    private _timer: number = Date.now();
    loop(dt: number) {
        var timer = Date.now();
        if (!this._timer || timer - this._timer >= 200) {
            this._timer = timer;
            this.fps = Math.floor(1000 / dt);
            // if(this.fps <= 50){
            //     console.info("fsp",this.fps);
            // }
        }
        this.frameEnd();
    }


    _showItem(name: string, value: number) {
        let element: HTMLDivElement = this.itemElements[name];
        if (!element) {
            let parent = document.getElementById("profile");
            element = document.createElement("div");
            parent.append(element);
            this.itemElements[name] = element;
        }
        element.innerText = name + ": " + value.toString();
    }

    frameEnd() {

        let items = [
            {
                name: "fps",
                value: this.fps
            },
            {
                name: "drawCall",
                value: this.drawCall
            },
            {
                name: "instanceDrawCall",
                value: this.instanceDrawCall
            },
            {
                name: "graphicsCall",
                value: this.graphicsAPICall
            },
            {
                name: "uniformCall",
                value: this.unfiromCall
            },
            {
                name: "bindTextureCall",
                value: this.bindTextureCall
            },
            {
                name: "bindFrameBufferCall",
                value: this.bindFrameBufferCall
            },
            {
                name: "bindBuffer",
                value: this.bindBuffer
            },
            {
                name: "updateTexture",
                value: this.updateTexture
            },
            {
                name: "useShader",
                value: this.useShader
            },
            {
                name: "bufferUpdate",
                value: this.bufferUpdate
            },
            {
                name: "vaoBind",
                value: this.vaoBind
            },
            {
                name: "nodeNum",
                value: this.nodeNum
            },
            {
                name: "wasm",
                value: window.Module.HEAP8.byteLength / 1024 / 1024
            }
        ]

        for (let i = 0; i < items.length; ++i) {
            this._showItem(items[i].name, items[i].value);
        }
        this.drawCall = 0;
        this.graphicsAPICall = 0;
        this.unfiromCall = 0;
        this.bindTextureCall = 0;
        this.bindFrameBufferCall = 0;
        this.bindBuffer = 0;
        this.updateTexture = 0;
        this.useShader = 0;
        this.bufferUpdate = 0;
        this.vaoBind = 0;
        this.instanceDrawCall = 0;
    }
}

export default new GameStat;