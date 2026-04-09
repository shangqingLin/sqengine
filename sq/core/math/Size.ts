import { serializable, sqclass, SQFloat,type } from "../data/index";

@sqclass("sq.Size")
export default class Size{

    public static TEMP_SIZE = new Size();

    @type(SQFloat)
    @serializable
    public width:number = 0;
    
    @type(SQFloat)
    @serializable
    public height:number = 0;

    constructor(width?:number,height?:number){
        this.width = width || 0;
        this.height = height || 0;
    }

    public set(width:number,height:number){
        this.width = width;
        this.height = height;
        return this;
    }
}

