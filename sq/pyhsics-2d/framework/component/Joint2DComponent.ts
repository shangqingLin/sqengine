import { sqclass, Vec2, type, serializable, SQBoolean } from "../../../core";
import Component from "../../../framework/component/Component"
import Node from "../../../scene/Node";
import Collision2DComponent from "./Collision2DComponent";
import { RigidBody2DComponent } from "./RigidBody2DComponent";

/**
 * 2D关节组件基类
 */
@sqclass("sq.Joint2DComponent")
export class Joint2DComponent extends Component {
    private _connectNode: Node;
    private _collideConnected: boolean = false;
    private localAnchorA?: Vec2;
    private localAnchorB?: Vec2;

    @type(Node)
    @serializable
    set connectNode(node: Node) {
        if (this._connectNode !== node) {
            /*debug:start */
            if (!(node.hasComponent(RigidBody2DComponent) || node.hasComponentInherit(Collision2DComponent))) {
                throw new Error("connect must has RigidBody2DComponent or Collision2DComponent");
            }
            /* debug:end */
            this._connectNode = node;
            this.nativeBeginOp(1);
            this.nativeWriteOpArg("i32", node.id);
            this.nativeEndOp();
        }
    }

    get connectNode() {
        return this._connectNode
    }


    @type(SQBoolean)
    @serializable   
    set collideConnected(collide: boolean) {
        if (this._collideConnected !== collide) {
            this._collideConnected = collide;
            this.nativeBeginOp(2);
            this.nativeWriteOpArg("i8", collide ? 1 : 0);
            this.nativeEndOp();
        }
    }

    get collideConnected() {
        return this._collideConnected;
    }


    @type(Vec2)
    @serializable
    set connectNodeLocalAnchor(vec: Vec2) {
        this.nativeBeginOp(3);
        this.nativeWriteOpArg("f32", vec.x);
        this.nativeWriteOpArg("f32", vec.y);
        this.nativeEndOp();

        /* editor:start */
        if (!this.localAnchorB) {
            this.localAnchorB = new Vec2;
        }
        this.localAnchorB.copy(vec);
        /* editor:end */
    }

    get connectNodeLocalAnchor() {
        return this.localAnchorB;
    }

    @type(Vec2)
    @serializable
    set ownerLocalAnchor(vec: Vec2) {
        this.nativeBeginOp(4);
        this.nativeWriteOpArg("f32", vec.x);
        this.nativeWriteOpArg("f32", vec.y);
        this.nativeEndOp();
        if (!this.localAnchorA) {
            this.localAnchorA = new Vec2;
        }
        this.localAnchorA.copy(vec);
    }

    get ownerLocalAnchor() {
        if (!this.localAnchorA) {
            this.localAnchorA = new Vec2;
        }
        return this.localAnchorA;
    }

    set connectNodeRotate(rotate: number) {
        this.nativeBeginOp(5);
        this.nativeWriteOpArg("f32", rotate);
        this.nativeEndOp();
    }

    set ownerNodeRotate(rotate: number) {
        this.nativeBeginOp(6);
        this.nativeWriteOpArg("f32", rotate);
        this.nativeEndOp();
    }

    set constraintHertz(herz: number) {
        this.nativeBeginOp(7);
        this.nativeWriteOpArg("f32", herz);
        this.nativeEndOp();
    }

    set constraintDampingRatio(damping: number) {
        this.nativeBeginOp(8);
        this.nativeWriteOpArg("f32", damping);
        this.nativeEndOp();
    }

    set forceThreshold(force: number) {
        this.nativeBeginOp(9);
        this.nativeWriteOpArg("f32", force);
        this.nativeEndOp();
    }

    set torqueThreshold(torque: number) {
        this.nativeBeginOp(10);
        this.nativeWriteOpArg("f32", torque);
        this.nativeEndOp();
    }
}