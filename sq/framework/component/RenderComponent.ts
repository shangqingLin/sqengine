
import Component from "./Component";
import { Material } from "../../assets/assets/Material";
import { AssetManager } from "../../assets/assetmanager/AssetManager";
import { ComponentType } from "../../native_binding";
import { BuildInMeshType } from "../../rendering";


export interface RenderDrawPrimitiveMesh {
    offset?: number;
    count: number;
    instance?: Number;
};


/**
 */
export class RenderComponent extends Component {

    protected sharedMaterial: Material;
    protected materialIns: Material;
    private materialUrl: string;

    constructor(nativeType?: number) {
        super(nativeType || ComponentType.Renderer);
    }

    setShaderMaterial(mat: Material) {
        if (this.sharedMaterial === mat) return;
        this.sharedMaterial = mat;

        this.nativeBeginOp(2);
        this.nativeWriteOpArg("i32", this.sharedMaterial ? this.sharedMaterial.getId() : -1);
        this.nativeEndOp();

        this._onMaterialModified();
    }

    setShaderMaterialFromRes(url: string, callback?: Function): void {
        if (this.materialUrl == url) {
            return;
        }
        this.materialUrl = url;
        AssetManager.getInstance().load(url, (error, mat: Material) => {
            if (error) return callback && callback(error);
            this.setShaderMaterial(mat);
            callback && callback();
        });
    }

    get material(): Material {
        return this.sharedMaterial;
    }

    setDrawInfo(mesh: RenderDrawPrimitiveMesh) {
        this.nativeBeginOp(1);
        this.nativeWriteOpArg("i32", mesh.count);
        this.nativeWriteOpArg("i32", mesh.offset || 0);
        this.nativeWriteOpArg("i32", mesh.instance || 0);
        this.nativeEndOp();
    }

    useBuildInMesh(type: BuildInMeshType) {
        this.nativeBeginOp(3);
        this.nativeWriteOpArg("i8", type);
        this.nativeEndOp();
    }

    setRenderSortOrder(order: number) {
        this.nativeBeginOp(4);
        this.nativeWriteOpArg("i32", order);
        this.nativeEndOp();
    }

    protected _onMaterialModified() { };
}