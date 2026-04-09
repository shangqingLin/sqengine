import { SpriteComponent } from "../2d";
import { Material } from "../assets";
import { Shader } from "../rendering";
import Node from "../scene/Node";

/**
 * 2D雾效
 */
export class Fog2D extends Node {

    private static fogMaterial:Material;

    // private static createMaterial() : Material {
    //     if(!this.fogMaterial){
    //         let shader = new Shader();
    //         shader.addAttribute();
    //     }
    // }
    constructor(){
        super();
        this.addComponent(SpriteComponent);
    }
}