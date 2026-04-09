import { sqenum } from "../core/index";

enum BuildinParameter
{
    /**
     * 当蓝图类的类实例
     */
    SELF,

    /**
     * 表示从当前蓝图类上查找对应的组件实例
     */
    COMPONENT,

}
sqenum(BuildinParameter);
export default BuildinParameter;