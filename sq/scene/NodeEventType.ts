
enum NodeEventType {

    //TOUCH事件也被统一抽象成Mouse了
    MOUSE_DOWN = 1, // 1  touchstart
    MOUSE_MOVE = 2, // 10 touchmove
    MOUSE_UP = 4, //100 tocuhcancel/touchend
    MOUSE_OUT = 8, //1000 touchmove
    MOUSE_OVER = 16,//10000 touchmove
    MOUSE_WHEEL = 32, //100000
    MOUSE_RIGHT_DOWN = 64, //
    MOUSE_RIGHT_UP = 128, //10000000
    MOUSE_CLICK = 256, //tocuhcancel/touchend


    TRASNFORM_CHANGE = 1 << 9,
    ADD_COMPONENT,
    REMOVE_COMPONENT,
    NODE_ADD_CHILD,
    NODE_ADD_PARENT,
    NODE_REMOVE_PARENT,
    KEY_DOWN,
    KEY_UP,
    RESIZE,
    LAYER_CHANGE,
    ANCHOR_CHANGE,
    DESTROY
}
export default NodeEventType;