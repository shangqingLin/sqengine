
export default class IDGen{
   private pool:Array<number> = [];
   private id:number = 1;
   public get():number{
    if(this.pool.length > 0){
        return this.pool.pop();
    }
    return this.id++;
   }
   
   public recovery(id:number){
     this.pool.push(id);
   }
}