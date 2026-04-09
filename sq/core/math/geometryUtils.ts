import Vec2 from "./Vec2";

export default {
      /*
	 * 根据入射线和表面法线，计算反射线
	 */
    //   computeReflectionDirection(direction,normal,result){
    //     result = result || new Vec3();
    //     result.scale2(normal,2 * direction.dot(normal));
    //     result.sub2(direction,result);
    //     return result;
    // },


    /*
	 * 返回平行与normal，但长度不等于normal的向量
	 * 这个向量的长度是根据normal和direction的点积来计算，也就是夹角越小，返回的向量越长
	 */
    // parallelComponent(direction,normal,result){
        // result = result || new Vec3();
        // var magnitude = direction.dot(normal);
        // result.scale2(normal,magnitude);
        // return result;
    // },

    /*
     * 返回与normal垂直的向量，其实就是求法线垂直于平面的垂足处的切线
     * direction是用来计算这个切线的长度
     * Returns the portion of 'direction' that is perpindicular to 'normal'
     */
    // perpindicularComponent(direction,normal,result){
        // result = result || new Vec3();
        // var p = Vec3.pool.get();
        // result.sub2(direction,this.parallelComponent(direction, normal,p));
        // Vec3.pool.put(p);
        // return result;
    // }
}