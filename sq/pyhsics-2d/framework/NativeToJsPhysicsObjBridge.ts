import { NativeToJsObjectBridge, dispatch } from "../../native_binding";
import { ContactManifold } from "./define";
import Node from "../../scene/Node";
import { ParticlePhysics2DComponent } from "./component/ParticlePhysics2DComponent";
import Collision2DComponent from "./component/Collision2DComponent";
import { Byte, js } from "../../core";
import WallSlide2DComponent from "./component/WallSlide2DComponent";

export default class NativeToJsPhysicsObjBridge extends NativeToJsObjectBridge {
    private static _ins: NativeToJsPhysicsObjBridge;

    private contactPointCache: Array<ContactManifold> = [];

    static getInstance(): NativeToJsPhysicsObjBridge {
        if (!this._ins) {
            this._ins = new NativeToJsPhysicsObjBridge;
        }
        return this._ins;
    }


    private getOrCreate(dataAdress: number): ContactManifold {
        let contact: ContactManifold = this.contactPointCache.pop();
        if (!contact) {
            contact = new ContactManifold();
        }
        //@ts-ignore
        contact.dataAdress = dataAdress;
        return contact;
    }

    private recovery(contact: ContactManifold) {
        contact.clear();
        this.contactPointCache.push(contact);
    }
    public override dispatch(byte: Byte): void {
        let op = byte.readByte();
        switch (op) {
            case 1:
                this.processCollisionBeginContact(byte);
                break;
            case 2:
                this.processCollisionEndContact(byte);
                break;
            case 3:
                this.processCollisionSensorBegin(byte);
                break
            case 4:
                this.processCollisionSensorEnd(byte);
                break;
            case 5:
                this.processParticleComponentBeginContactBody(byte);
                break;
            case 6:
                this.processParticleComponentEndContactBody(byte);
                break;
            case 7:
                this.processParitcleComponentBeginParticle(byte);
                break;
            case 8:
                this.processParticleComponentEndParticle(byte);
                break;
            case 9:
                this.processParitcleBeginToCollisionComponent(byte);
                break;
            case 10:
                this.processParitcleEndToCollisionComponent(byte);
                break;
            default:
                console.error("NativeToJsPhysicsObjBridge::dispatch invalid op code:", op);
                break;
        }
    }

    /**
     * 
     * 这里需要注意一个事实：就是对于物理引擎来说Begin和End是一定成对出现的，但对于JS端来说不一定
     *     因为C++底层是否通知到JS端还会判断是否注册了Begin或End事件，如果只注册了End Handler，但
     *     没有注册Begin Handler则C++端只会通知End Handler到JS端。
     *    
     *    虽然在注册Begin Handler或End Handler同时告诉C++端激活End和Begin的通知，但还会出现只激活一个的情况
     *   比如说：A和B Body，在某个时刻A和B发生了碰撞，然后在C++触发Begin,但这时A和B并没有注册End 和 Begin Handler
     *        所以对于JS端来说不知道A和B发生了碰撞，突然在此期间B注册End Handler，然后后续A和B不发生碰撞了，这时会通知
     *        B Body End了，但B Body从头到尾都还没有接收到Begin
     * 
     */

    private processCollisionBeginContact(byte: Byte) {
        let nodeNativeIdA = byte.readInt32();
        let nodeNativeIdB = byte.readInt32();
        let contactId = byte.readInt32();
        let dataAdress = byte.readDataAddress();
        let nodeA: Node = dispatch.getJsToNativeObject(nodeNativeIdA).getCustomData() as Node;
        let nodeB: Node = dispatch.getJsToNativeObject(nodeNativeIdB).getCustomData() as Node;
        let component: Collision2DComponent = nodeA.getCompoentInherit(Collision2DComponent);
        let contact: ContactManifold = this.getOrCreate(dataAdress);
        //@ts-ignore
        contact.id = contactId;
        //@ts-ignore
        contact.collisoinNode = nodeB;

        // console.info("js begin", contact.id);

        //@ts-ignore
        component.onTriggerBeginContact(contact);
        this.recovery(contact);
    }

    private processCollisionEndContact(byte: Byte) {
        let nodeNativeId = byte.readInt32();
        let collisionNodeNativeId = byte.readInt32();
        let contactId = byte.readInt32();
        let node: Node = dispatch.getJsToNativeObject(nodeNativeId).getCustomData() as Node;
        let component: Collision2DComponent = node.getCompoentInherit(Collision2DComponent);
        let collisoinNode: Node = dispatch.getJsToNativeObject(collisionNodeNativeId).getCustomData() as Node;
        // console.info("js end", contact.id);
        //@ts-ignore
        component.onTriggerEndContact(collisoinNode, contactId);
    }


    private processCollisionSensorBegin(byte: Byte) {
        let ownerNodeId: number = byte.readInt32();
        let sensorNodeId: number = byte.readInt32();
        let componentKey: number = byte.readInt32();
        let ownerNode: Node = dispatch.getJsToNativeObject(ownerNodeId).getCustomData() as Node;
        let sensorNode: Node = dispatch.getJsToNativeObject(sensorNodeId).getCustomData() as Node;
        let collisionComponent = ownerNode.getComponentByKey<Collision2DComponent>(componentKey);
        //@ts-ignore
        collisionComponent.onTriggerSensorBegin(sensorNode);
    }

    private processCollisionSensorEnd(byte: Byte) {
        let ownerNodeId: number = byte.readInt32();
        let sensorNodeId: number = byte.readInt32();
        let componentKey: number = byte.readInt32();
        let ownerNode: Node = dispatch.getJsToNativeObject(ownerNodeId).getCustomData() as Node;
        let sensorNode: Node = dispatch.getJsToNativeObject(sensorNodeId).getCustomData() as Node;
        let collisionComponent = ownerNode.getComponentByKey<Collision2DComponent>(componentKey);
        //@ts-ignore
        collisionComponent.onTriggerSensorEnd(sensorNode);
    }

    private processParticleComponentBeginContactBody(byte: Byte) {
        // let dataAdress = byte.readDataAddress();
        // let contact: ContactManifold = this.getOrCreate(dataAdress);
        // let node: Node = contact.nodeA;
        // let component: ParticlePhysics2DComponent = node.getComponent(ParticlePhysics2DComponent);
        // //@ts-ignore
        // if (component._onBeginContactBodyCallback) component._onBeginContactBodyCallback(contact);
    }

    private processParticleComponentEndContactBody(byte: Byte) {
        let dataAdress = byte.readDataAddress();
        let id = window.Module.getValue(dataAdress, "i32");
        // console.info("end ",contact.data);

        // let component: ParticlePhysics2DComponent = contact.nodeA.getComponent(ParticlePhysics2DComponent);
        // //@ts-ignore
        // if (component._onEndContactBodyCallback) component._onEndContactBodyCallback(contact);
        // this.recovery(contact);
    }

    private processParitcleComponentBeginParticle(byte: Byte) {
        let nodeId: number = byte.readInt32();
        let node: Node = dispatch.getJsToNativeObject(nodeId).getCustomData() as Node;
        let component: ParticlePhysics2DComponent = node.getComponent(ParticlePhysics2DComponent);
        let dataAdress = byte.readDataAddress();
        //@ts-ignore
        if (component._onBeginContactParticleCallback) component._onBeginContactParticleCallback();
    }

    private processParticleComponentEndParticle(byte: Byte) {
        let nodeId: number = byte.readInt32();
        let node: Node = dispatch.getJsToNativeObject(nodeId).getCustomData() as Node;
        let component: ParticlePhysics2DComponent = node.getComponent(ParticlePhysics2DComponent);
        let particleIdA: number = byte.readInt32();
        let particleIdB: number = byte.readInt32();

        //@ts-ignore
        if (component._onEndContactParticleCallback) component._onEndContactParticleCallback(particleIdA, particleIdB);
    }

    private processUpdateContact(byte: Byte) {
        let nodeNativeId = byte.readInt32();
        let dataAdress = byte.readDataAddress();
        let node: Node = dispatch.getJsToNativeObject(nodeNativeId).getCustomData() as Node;
        let component: Collision2DComponent = node.getCompoentInherit(Collision2DComponent);
        let id = window.Module.getValue(dataAdress, "i32");
        // let contact: ContactManifold = this.contactPointUse[id];
        //@ts-ignore
        // component.onTriggerUpdateContact(contact);
    }



    private processWallSideComponent(byte: Byte) {
        let nodeNativeId = byte.readInt32();
        let node: Node = dispatch.getJsToNativeObject(nodeNativeId).getCustomData() as Node;
        let component = node.getComponent(WallSlide2DComponent);
        //@ts-ignore
        let callback = component._onSlopStopHandler;
        callback();
    }

    private processParitcleBeginToCollisionComponent(byte: Byte) {
        let ownerNodeId: number = byte.readInt32();
        let contactNodeId: number = byte.readInt32(); //哪个节点添加的粒子组件
        let particleIndex: number = byte.readInt32();
        let componentKey: number = byte.readInt32();
        let ownerNode: Node = dispatch.getJsToNativeObject(ownerNodeId).getCustomData() as Node;
        let contactNode: Node = dispatch.getJsToNativeObject(contactNodeId).getCustomData() as Node;
        let collisionComponent = ownerNode.getComponentByKey<Collision2DComponent>(componentKey);        
        //@ts-ignore
        if (collisionComponent.particleBeginContactHandler) collisionComponent.particleBeginContactHandler(contactNode, particleIndex);
    }

    private processParitcleEndToCollisionComponent(byte: Byte) {
        let ownerNodeId: number = byte.readInt32();
        let contactNodeId: number = byte.readInt32();
        let particleIndex: number = byte.readInt32();
        let componentKey: number = byte.readInt32();
        let ownerNode: Node = dispatch.getJsToNativeObject(ownerNodeId).getCustomData() as Node;
        let contactNode: Node = dispatch.getJsToNativeObject(contactNodeId).getCustomData() as Node;
        let collisionComponent = ownerNode.getComponentByKey<Collision2DComponent>(componentKey);
        //@ts-ignore
        if (collisionComponent.particleEndContactHandler) collisionComponent.particleEndContactHandler(contactNode, particleIndex);
    }
}
