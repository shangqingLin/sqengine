import { BlueprintData } from "../assets/index";
import { ParameterData, ParameterDataType } from "./node/data/ParameterData";
import type Node from "../scene/Node";
import BuildinParameter from "./BuildinParameter";
import { js } from "../core/index";
import type { Component } from "../framework/index";
export default class BPScript {

    private parameters: { [key: string]: any } = {};
    private tempParameters: { [key: string]: any } = Object.create(null);
    private assetParamterMap: { [key: number]: ParameterData };
    private _data: BlueprintData;
    private dynmaicParamterId: number;

    /**
     * 蓝图类实例
     */
    public readonly node: Node;

    set data(data: BlueprintData) {
        this._data = data;
        this.setParameterDatas(data.parameterDatas)
    }

    get data() {
        return this._data;
    }

    private setParameterDatas(parameters: Array<ParameterData> | null) {
        this.assetParamterMap = Object.create(null);
        if (parameters) {
            for (let i = 0, n = parameters.length; i < n; ++i) {
                this.assetParamterMap[parameters[i].id] = parameters[i];
            }
        }
    }

    defineParameter(value?: any, parameterId?: number): number {
        if (!this.dynmaicParamterId == undefined) {
            this.dynmaicParamterId = 0;
        }
        if (parameterId === undefined) {
            while (true) {
                parameterId = ++this.dynmaicParamterId;
                if (!this.parameters[parameterId]) {
                    break;
                }
            }
        }
        this.parameters[parameterId] = value;
        return parameterId;
    }

    setParemeter(parameterId: number, value: any) {
        if (!this.parameters.hasOwnProperty(parameterId)) {
            throw new Error();
        }
        this.parameters[parameterId] = value;
    }

    getParameter(parameterId: number): any {
        if (this.parameters[parameterId] !== undefined) {
            return this.parameters[parameterId];
        }
        let parameterData: ParameterData = this.assetParamterMap[parameterId];

        if (parameterData.parameterType === ParameterDataType.BUILDIN) {
            switch (parameterData.value) {
                case BuildinParameter.SELF:
                    return this.node;
                case BuildinParameter.COMPONENT:
                    {
                        let clsComponent: Constructor<Component> = js.getClassByName(parameterData.clsName) as Constructor<Component>;
                        return this.node.getComponent(clsComponent);
                    }
                default:
                    return null;
            }
        }
        return parameterData.value;
    }

    setTmpParameter(nodeId: number, pinId: number, value: any) {
        let key = nodeId + "_" + pinId;
        this.tempParameters[key] = value;
    }

    getTmpParameter(nodeId: number, pinId: number): any {
        let key = nodeId + "_" + pinId;
        let value = this.tempParameters[key];
        delete this.tempParameters[key];
        return value;
    }

    play() {

    }
}