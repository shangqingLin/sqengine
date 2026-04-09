
export default abstract class GraphicsBuffer {

    private data: Uint8Array;
    setData(data: Uint8Array): void {
        if (this.data == data) return;
        this.data = data;
        this.setDirty();
    }

    clearData() {
        this.data = null;
    }

    getData(): Uint8Array {
        return this.data;
    }

    abstract setDirty(): void;
}