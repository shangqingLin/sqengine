import { js, serializable, sqclass, SQJSON, type } from "../../core/index";
import AABB from "../../core/math/AABB";
import { DynamicTree } from "../../core/utils/DynamicTree2D";
import { Deserialize, deserialize, DeserializeContext, SerializedObjectReference, SerializedTypedObject, SerializedValue } from "../../serialization";
import { NodeBound } from "./data/TIledMapLayerData";
import TiledMapBaseLayer from "./TiledMapBaseLayer";
import type Node from "../../scene/Node";
import { PrefadInstnace } from "../../scene/prefab";


interface NodeInfo {
    prefabData: any;
    parentData: NodeInfo;
    children: Array<NodeInfo>;
    childrenIndex: number;
    node: Node | PrefadInstnace;
}

@sqclass("sq.TiledMapObjectLayer")
export default class TiledMapObjectLayer extends TiledMapBaseLayer {


    @serializable
    @type(SQJSON)
    private prefabJson: SerializedValue;

    @serializable
    @type(SQJSON)
    private bvhTreeNodeBounds: Array<NodeBound>;

    private fileIdToNodeMap: { [key: string]: NodeInfo };
    // private deserializeObj = new Deserialize();

    constructor() {
        super();
        // this.deserializeObj.context = {
        //     checkExportProperty: this.checkExportProperty
        // } as DeserializeContext;
    }

    private checkExportProperty(obj: any, propertyName: string) {
        return propertyName !== "_children";
    }

    protected override onInitMap(map: TiledMapBaseLayer): void {
        super.onInitMap(map);

        if (this.prefabJson) {

            //构建节点树

            let count: number = this.prefabJson.length;
            let indexToDataMap = js.createMap();
            this.fileIdToNodeMap = js.createMap();
            let createNode = (prefabData: any, index: number) => {
                let info = {
                    prefabData: prefabData,
                    childrenIndex: index
                } as NodeInfo;

                if (prefabData.fileId)
                    this.fileIdToNodeMap[prefabData.fileId] = info;

                if (prefabData._children && prefabData._children.length > 0) {
                    info.children = [];
                    for (let i = 0; i < prefabData._children.length; ++i) {
                        let d: SerializedObjectReference = prefabData._children[i];
                        let dd = indexToDataMap[d.__id__];
                        if (dd) {
                            info.children[i] = dd;
                        } else {
                            dd = createNode(this.prefabJson[d.__id__], i);
                            dd.parentData = info;
                            info.children[i] = dd;
                            indexToDataMap[d.__id__] = dd;
                        }
                    }
                }
                return info;
            }

            let childIndex = 0;
            for (let b = 0; b < count; ++b) {
                let prefabData: SerializedTypedObject = this.prefabJson[b];
                if (prefabData.fileId) { //表示为Node类，其他类没有fileId
                    if (!indexToDataMap[b]) {
                        indexToDataMap[b] = createNode(prefabData, childIndex);
                        ++childIndex;
                    }
                }
            }
        }

        if (this.bvhTreeNodeBounds) {
            this.setbvHBound(this.bvhTreeNodeBounds);
            this.bvhTreeNodeBounds = null;
        }
    }

    override showAll(): void {
        let childrens: Object | Array<Object> = deserialize(this.prefabJson).objects;
        if (Array.isArray(childrens)) {
            for (let i = 0; i < childrens.length; ++i) {
                this.addChild(childrens[i] as Node);
            }
        } else {
            this.addChild(childrens as Node);
        }

    }

    override showArea(x: number, y: number, width: number, height: number): void { }

    public setbvHBound(bounds: Array<NodeBound>) {
        //@ts-ignore
        let objectTree: DynamicTree = this.map.objectTree;
        for (let i = 0, n = bounds.length; i < n; ++i) {
            let bound = bounds[i];
            AABB.TEMP.lowerBound.x = bound.x;
            AABB.TEMP.lowerBound.y = bound.y;
            AABB.TEMP.upperBound.x = bound.x + bound.width;
            AABB.TEMP.upperBound.y = bound.y + bound.height;
            let userData = js.createMap();
            userData.layer = this;
            userData.fileId = bound.fileId;
            objectTree.createProxy(AABB.TEMP, userData);
        }
    }

    public hideItem(fileId: string) {
        let objConfig: NodeInfo = this.fileIdToNodeMap[fileId];
        if (objConfig.node) {
            // console.info("remove", objConfig.prefabData.fileId);
            objConfig.node.destroy();
            objConfig.node = null;
        }
    }

    public showItem(fileId: string) {
                
        // console.info("showItem", fileId);

        let needToCreate = [];
        let rootParent: NodeInfo;
        let hasObjToCreate: boolean = false;
        {
            //确保fileId节点的父节点全部都会被创建

            let objConfig: NodeInfo = this.fileIdToNodeMap[fileId];

            /* debug:start */
            if (!objConfig) console.error("Layer层:" + this.name + "不存在GameObject的定义。FileId为：" + fileId);
            /* debug:end */

            needToCreate.push(objConfig);

            //判断此节点是否被创建了，创建了的节点的会存储node实例
            rootParent = objConfig;
            
            if (!hasObjToCreate) hasObjToCreate = !rootParent.node;
            while (true) {
                if (rootParent.parentData) {
                    rootParent = rootParent.parentData;
                    needToCreate.push(rootParent);
                    if (!hasObjToCreate) hasObjToCreate = !rootParent.node;
                }
                else break;
            }
        }

        if (!hasObjToCreate) return;

        let deserializeObj = new Deserialize();
        deserializeObj.context = {
            checkExportProperty: this.checkExportProperty
        } as DeserializeContext;
        deserializeObj.serializedData = this.prefabJson;

        let create = (parentNode: Node, nodeInfo: NodeInfo, childIndex: number) => {
            if (!nodeInfo.node) {

                if (nodeInfo.prefabData.__type__ === "sq.PrefadInstnace") {
                    let prefabInstnace: PrefadInstnace = deserializeObj.deserializeObject(nodeInfo.prefabData);
                    nodeInfo.node = prefabInstnace;
                    prefabInstnace.setRoot(parentNode, childIndex);
                } else {
                    nodeInfo.node = deserializeObj.deserializeObject(nodeInfo.prefabData);
                    parentNode.addChildAt(nodeInfo.node as Node, childIndex);
                }
                // console.info("fuck you ", nodeInfo.prefabData);
                // console.info(nodeInfo.node,childIndex);
            }

            let children = nodeInfo.children;
            if (children) {
                for (let i = 0; i < children.length; ++i) {
                    if (needToCreate.indexOf(children[i]) !== -1) {
                        create(nodeInfo.node as Node, children[i], i);
                        break;
                    }
                }
            }
        }
        create(this, rootParent, rootParent.childrenIndex);
        // console.info("show parent ", rootParent);

    }


    /*
    public setBound(bounds: Array<NodeBound>) {

        //@ts-ignore
        let objectTree: DynamicTree = this.map.objectTree;

        bounds = [];
        bounds.push({
            x: 10,
            y: 10,
            width: 10,
            height: 10
        } as NodeBound);

        bounds.push({
            x: 100,
            y: 10,
            width: 10,
            height: 10
        } as NodeBound);

        bounds.push({
            x: 10,
            y: 100,
            width: 10,
            height: 10
        } as NodeBound);

        bounds.push({
            x: 100,
            y: 100,
            width: 10,
            height: 10
        } as NodeBound);

        bounds.push({
            x: 20,
            y: 20,
            width: 10,
            height: 10,
            userData: { t: "a" }
        } as NodeBound);
        
        for (let i = 0, n = bounds.length; i < n; ++i) {
            let bound = bounds[i];
            AABB.TEMP.lowerBound.x = bound.x;
            AABB.TEMP.lowerBound.y = bound.y;
            AABB.TEMP.upperBound.x = bound.x + bound.width;
            AABB.TEMP.upperBound.y = bound.y + bound.height;
            objectTree.createProxy(AABB.TEMP, bound.userData);
        }

        objectTree.print();
    }*/

}