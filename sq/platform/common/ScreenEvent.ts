
enum ScreenEvent {
    HIDDEN, //屏幕切换到后台触发
    VISIBLE, //屏幕从后天切换回来触发
    RENDER_SIZE_CHANGE //渲染分辨率发生更改
}

export default ScreenEvent;