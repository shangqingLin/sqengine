
export default class RenderPipline {
    protected native: Module.RenderPipline;

    destroy() {
        if (this.native) {
            window.Module.destroy(this.native);
        }
    }
}