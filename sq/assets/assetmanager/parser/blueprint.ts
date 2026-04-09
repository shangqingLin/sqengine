import { AssetInfo } from "../config";
import { NodeData, NodePinData, BaseNode, BPNodeClassStash, BPPinData, PinType, ParameterData } from "../../../blueprint/index";
import { findClassSlashByClass, js } from "../../../core/index"
import { BlueprintData, BlueprintAsset } from "../../assets/BlueprintAsset";
import type Asset from "../../assets/Asset";
import { SerializedValue } from "script/engine/sq/serialization";

export interface PinSaveConfig {
    type: PinType;
    pinId : number;

    linkNodeIndex: number;

    /**
     * bpNodeClassMeta中设置的id
     */
    linkPinId: number;

    linkParameterId?: any;
}

export interface NodeSaveData {
    posX?: number;
    posY?: number;
    node: string;
    entryIn?: PinSaveConfig;
    entryOut?: PinSaveConfig;
    inputs?: Array<PinSaveConfig>;
    outputs?: Array<PinSaveConfig>;
    config?: { [key: string]: any };
}

export interface GraphMapSaveData {
    nodes: Array<NodeSaveData>;
}

export interface ParameterSaveData {
    id: number;
    name?: string;
    value: any;
    dataType?: string;
    paremterType?: number;
    clsName?: string;
}

export interface BlueprintMapSaveData {
    node: SerializedValue;
    parameterIdGen: number;
    eventMapData: GraphMapSaveData;
    funcDatas: Array<GraphMapSaveData>;
    macroDatas: Array<GraphMapSaveData>;
    parameterDatas: Array<ParameterSaveData>;
}


function parseGraphMapData(graphMapData: GraphMapSaveData): Array<NodeData> {

    let nodes: Array<NodeSaveData> = graphMapData.nodes;
    let result: Array<NodeData> = new Array(nodes.length);

    let createPinData = (ownerNodeData:NodeData,config: PinSaveConfig, linkPinType: number) => {
        let pin: NodePinData = new NodePinData();
        pin.type = config.type;
        pin.id = config.pinId;
        pin.ownerNodeData = ownerNodeData;
        if (config.linkNodeIndex !== undefined) {
            pin.linkNodeData = createNodeData(config.linkNodeIndex);
            pin.linkPinData = pin.linkNodeData.findPinById(linkPinType, config.linkPinId);
        } else if (config.linkParameterId !== undefined) {
            pin.linkParameterId = config.linkParameterId;
        }
        return pin;
    }

    function createNodeData(index: number): NodeData {

        if (result[index]) {
            return result[index];
        }

        let nodeConfig = nodes[index];
        let nodeCls = js.getClassByName(nodeConfig.node) as Constructor<BaseNode>;
        let classStash: BPNodeClassStash = findClassSlashByClass(nodeCls) as BPNodeClassStash;

        let nodeData;
        if (classStash.meta && classStash.meta.dataCls) {
            nodeData = new classStash.meta.dataCls();
        } else {
            nodeData = new NodeData();
        }

        if (nodeConfig.config) {
            Object.assign(nodeData, nodeConfig.config);
        }

        result[index] = nodeData;
        nodeData.nodeCls = nodeCls;
        if (nodeConfig.entryIn) {
            nodeData.entryIn = createPinData( nodeData,nodeConfig.entryIn, 4);
        }

        if (nodeConfig.entryOut) {
            nodeData.entryOut = createPinData(nodeData, nodeConfig.entryOut, 3);
        }

        if (nodeConfig.inputs && nodeConfig.inputs.length > 0) {
            let n: number = nodeConfig.inputs.length;
            nodeData.inputMap = js.createMap();
            for (let i = 0; i < n; ++i) {
                let pinConfig: PinSaveConfig = nodeConfig.inputs[i];
                let pinData = createPinData(nodeData, pinConfig, 2);
                nodeData.inputMap[pinData.id] = pinData;
            }
        }

        if (nodeConfig.outputs && nodeConfig.outputs.length > 0) {
            let n: number = nodeConfig.outputs.length;
            nodeData.outputMap = js.createMap();
            for (let i = 0; i < n; ++i) {
                let pinConfig: PinSaveConfig = nodeConfig.outputs[i];
                let pinData = createPinData(nodeData,pinConfig, 1);
                nodeData.outputMap[pinData.id] = pinData;
            }
        }

        /* editor:start */
        nodeData.saveData = nodeConfig;
        /* editor:end */

        return nodeData;
    }

    for (let i = 0, n = nodes.length; i < n; ++i) {
        createNodeData(i);
    }
    return result;
}

export function parseBlueprint(url: string, configAssetInfo: AssetInfo, data: BlueprintMapSaveData, onComplete: (error: Error | null, asset: Asset) => void): void {
    let blueprintAsset = new BlueprintAsset;
    let assetData: BlueprintData = Object.create(null);
    assetData.node = data.node;

    if (data.eventMapData)
        assetData.eventMapData = parseGraphMapData(data.eventMapData);

    if (data.funcDatas) {
        assetData.funcDatas = new Array(data.funcDatas.length);
        for (let i = 0, n = data.funcDatas.length; i < n; ++i) {
            assetData.funcDatas[i] = parseGraphMapData(data.funcDatas[i]);
        }
    }
    if (data.macroDatas) {
        assetData.macroDatas = new Array(data.macroDatas.length);
        for (let i = 0, n = data.macroDatas.length; i < n; ++i) {
            assetData.macroDatas[i] = parseGraphMapData(data.macroDatas[i]);
        }
    }

    if (data.parameterDatas) {
        let n: number = data.parameterDatas.length;
        assetData.parameterDatas = new Array(n);
        for (let i = 0; i < n; ++i) {
            let parameter = new ParameterData();
            let saveData = data.parameterDatas[i];
            parameter.value = saveData.value;
            parameter.id = saveData.id;
            parameter.parameterType = saveData.paremterType;
            if (saveData.clsName) {
                parameter.clsName = saveData.clsName;
            }
            assetData.parameterDatas[i] = parameter;

            /* editor:start */
            parameter.saveData = saveData;
            /* editor:end */
        }
    }

    /* editor:start */
    assetData.saveData = data;
    /* editor:end */

    blueprintAsset.nodeData = assetData;
    onComplete(null, blueprintAsset);
}
