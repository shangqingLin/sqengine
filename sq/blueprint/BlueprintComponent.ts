import { BlueprintAsset, BlueprintData } from "../assets/index";
import { js, serializable, sqclass, type } from "../core/index";
import Component from "../framework/component/Component";
import type { BPEventNode } from "./node/CommonNode";
import NodeManager from "./node/NodeManager";
import BPScript from "./BPScript";
import { EventNodeData } from "./node/data/EventNodeData";

@sqclass("sq.BlueprintComponent"
    /* editor:start */
    , {
        componentInspector: false
    }
    /* editor:end */
)
export default class BlueprintComponent extends Component {
    private currentAsset: BlueprintAsset;
    private eventMapData: { [key: string]: EventNodeData };
    readonly script: BPScript;
    constructor() {
        super();
        this.script = new BPScript();
    }

    protected override onInitialize(): void {
        //@ts-ignore
        this.script.node = this.node;
        
        //@ts-ignore
        // this.node.onBlueprintEventHandler = this.onNodeEventFireHandler.bind(this);
    }

    private onNodeEventFireHandler(eventName: string, ...arg: any) {
        this.playEvent(eventName, ...arg);
    }

    @type(function () {
        return BlueprintAsset
    })
    @serializable
    set asset(asset: BlueprintAsset) {
        if (this.currentAsset === asset) return;
        this.currentAsset = asset;
        this.blueprintData = asset.nodeData;
    }

    get asset() {
        return this.currentAsset;
    }

    set blueprintData(datas: BlueprintData) {
        this.script.data = datas;
        let eventMapData = this.script.data.eventMapData;
        if (eventMapData) {
            this.eventMapData = js.createMap();
            for (let i = 0, n = eventMapData.length; i < n; ++i) {
                if (eventMapData[i] instanceof EventNodeData) {
                    this.eventMapData[(eventMapData[i] as EventNodeData).eventName] = eventMapData[i] as EventNodeData;
                }
            }
        }
    }


    playEvent(eventName: string | number, arg?: any) {
        if (!this.eventMapData) return;
        let data = this.eventMapData[eventName];
        if (data) {
            let node: BPEventNode = NodeManager.getInstance().getNode(data, this.script);

            //@ts-ignore
            node.eventTriggerData = arg;
            node.entryIn();
        }

        /* debug:start */
        else {
            throw new Error(eventName + " 不存在");
        }
        /* debug:end */
    }
}