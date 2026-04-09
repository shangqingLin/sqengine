import { Texture2d } from "../assets/Texture2d";
import { ImageAsset, IMemoryImageSource } from "../assets/ImageAsset";
import { js } from "../../core";
import { BuildinShapder } from "../../rendering";
import GraphicsShader from "../../2d/component/graphics/GraphicsShader";
import ParticlePhysicsShader from "../../pyhsics-2d/rendering/ParticleShader";
import SpriteShader from "../../2d/component/sprite/SpriteShader";

class BuildInAssetManager {

    private whiteColortexture: Texture2d;
    private buildinMaterialMap: { [key: string]: Function } = js.createMap();
    initialize() {
        this.buildinMaterialMap["default-clear-stencil"] = BuildinShapder.getStencilClearMaterial.bind(BuildinShapder);
        this.buildinMaterialMap["default-graphics"] = GraphicsShader.getDefaultGraphicsMaterial.bind(GraphicsShader);
        this.buildinMaterialMap["default-sprite"] = SpriteShader.getDefaultSpriteMaterial.bind(SpriteShader);
        this.buildinMaterialMap["default-physics-particle-texture"] = ParticlePhysicsShader.getTextureBlurRenderMaterial.bind(ParticlePhysicsShader);
        this.buildinMaterialMap["default-physics-particle-sdf"] = ParticlePhysicsShader.getSdfRenderMaterial.bind(ParticlePhysicsShader);
    }

    getWhiteColortexture(): Texture2d {
        if (!this.whiteColortexture) {
            this.whiteColortexture = new Texture2d();
            let imageAsset = new ImageAsset();
            let numChannels = 4;
            let size = 2 * 2; //2x2大小的纹理，4个像素    
            const whiteValueView = new Uint8Array(size * numChannels);
            let offset = 0;
            for (let i = 0; i < size; i++) {
                whiteValueView[offset] = 255;
                whiteValueView[offset + 1] = 255;
                whiteValueView[offset + 2] = 255;
                whiteValueView[offset + 3] = 255;
                offset += numChannels;
            }

            let source: IMemoryImageSource = {
                _data: whiteValueView,
                width: 2,
                height: 2
            }
            imageAsset.source = source;

            this.whiteColortexture.setMipmaps(imageAsset);
        }
        return this.whiteColortexture;
    }

    getMaterial(type: string | number) {
        if (typeof type === "number") {
            //native调用过来的
            type = window.Module.UTF8ToString(type);
        }
        return this.buildinMaterialMap[type]();
    }
}

let buildinResManager = new BuildInAssetManager();
window["buildinResManager"] = buildinResManager;
export default buildinResManager;
