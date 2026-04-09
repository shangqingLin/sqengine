import { Application } from "../framework/Application";
import System from "../framework/System";

export default class SkeletonSystem extends System
{
    private native:Module.SkeletonSystem;
    public static enabled:boolean = false;
    constructor()
    {
        super();
        this.native = new window.Module.SkeletonSystem();
        Application.ins.registerSystem(this);
        SkeletonSystem.enabled = true;
    }
}