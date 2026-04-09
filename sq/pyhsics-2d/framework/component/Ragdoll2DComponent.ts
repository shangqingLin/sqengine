// import { ComponentType } from "../../../native_binding/index";
// import ContactListener2DComponent from "./ContactListener2DComponent";
// import type Node from "../../../scene/Node";
// import { js } from "../../../core/index";

// export enum RagdollBoneType {
//     HEAD,            // 头部
//     SPINE,           // 脊椎
//     HIP,             // 臀部
//     UPPER_LEFT_ARM,  // 左上臂
//     LOWER_LEFT_ARM,  // 左下臂
//     UPPER_RIGHT_ARM, // 右上臂
//     LOWER_RIGHT_ARM, // 右下臂
//     UPPER_LEFT_LEG,  // 左大腿
//     LOWER_LEFT_LEG,  // 左小腿
//     UPPER_RIGHT_LEG, // 右大腿
//     LOWER_RIGHT_LEG // 右小腿
// };

// export class Ragdoll2DComponent extends ContactListener2DComponent {
//     private bindMap: { [key: number]: Node } = js.createMap();
//     constructor() {
//         super();
//         this.nativeType = ComponentType.Ragdoll2DComponent;
//     }

//     createFrontView(): void {
//         this.nativeBeginOp(11);
//         this.nativeEndOp();
//     }

//     createSideView() {
//         this.nativeBeginOp(12);
//         this.nativeEndOp();
//     }

//     bind(part: RagdollBoneType, node: Node) {
//         if (this.bindMap[part] === node) return;
//         this.nativeBeginOp(13);
//         this.nativeWriteOpArg("i8", part);
//         this.nativeWriteOpArg("i32", node.getId());
//         this.nativeEndOp();
//     }
// }