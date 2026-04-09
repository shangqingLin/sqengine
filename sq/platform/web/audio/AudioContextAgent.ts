import { EventDispatch } from "../../../core";


const _contextRunningEvent = 'on-context-running';
class AudioContextAgent extends EventDispatch
{
    public readonly audioContext = new (window.AudioContext || window.webkitAudioContext || window.mozAudioContext)();

    private _isRunning:boolean = false;
    constructor()
    {
        super();

          /**
           * 我们都知道AudioContext.state属性，这个属性是记录了Context的一些状态变化
           * 如果AudioContext.state状态发生改变时，则触发该事件
           */
          this.audioContext.onstatechange = (): void => {
            if (this.audioContext.state === 'running') {
                this._isRunning = true;
                this.fire(_contextRunningEvent);
            } else {
                this._isRunning = false;
            }
        };
    }

    get isRunning (): boolean {
        return this._isRunning;
    }

    /**
     * 从 AudioContext 被创建并开始运行以来，已经过去了多少秒
     */
    get currentTime (): number {
        return this.audioContext.currentTime;
    }

    /**
     * closed:调用 audioContext.close()，并且关闭了整个Context，context 彻底不可用
     * running：表示当前Context可以用来播放音频了。
     * suspended:页面进入后台\浏览器为了省电自动暂停音频\移动端锁屏等整体停止播放就会切换到这个状态。由running进入到supended
     * interrupted：
     */
    get state()
    {
        return this.audioContext.state;
    }

    public onceRunning (cb: (...args: any[]) => void, target?: any): void {
        this.once(_contextRunningEvent, cb, target);
    }

    public offRunning (cb?: (...args: any[]) => void, target?: any): void {
        this.off(_contextRunningEvent, cb, target);
    }

    public runContext (): Promise<void> {
        return new Promise((resolve) => {
            if (this.isRunning) {
                resolve();
                return;
            }
            const context = this.audioContext;
            if (!context.resume) {
                resolve();
                return;
            }
            context.resume();
            if (context.state === 'running') {
                resolve();
                return;
            }
            // // Force running audio context if state is not 'running', may be 'suspended' or 'interrupted'.
            // const canvas = document.getElementById('GameCanvas') as HTMLCanvasElement;
            // // HACK NOTE: if the user slide after touch start, the context cannot be resumed correctly.
            // const onGesture = (): void => {
            //     context.resume().then(() => {
            //         canvas?.removeEventListener('touchend', onGesture, { capture: true });
            //         canvas?.removeEventListener('mouseup', onGesture, { capture: true });
            //         resolve();
            //     }).catch((e) => { debug.warn('onGesture resume error', e); });
            // };
            // canvas?.addEventListener('touchend', onGesture, { capture: true });
            // canvas?.addEventListener('mouseup', onGesture, { capture: true });
        });
    }


    /**
     * 创建一个BufferSource节点
     * @param audioBuffer 
     * @param loop 
     * @returns 
     */
    public createBufferSource (audioBuffer?: AudioBuffer, loop?: boolean): AudioBufferSourceNode {
        const sourceBufferNode = this.audioContext.createBufferSource();
        if (audioBuffer !== undefined) {
            sourceBufferNode.buffer = audioBuffer;
        }
        if (loop !== undefined) {
            sourceBufferNode.loop = loop;
        }
        return sourceBufferNode;
    }

    /**
     * 创建一个GainNode节点
     * @param volume 
     * @returns 
     */
    public createGain (volume = 1): GainNode {
        const gainNode = this.audioContext.createGain();
        this.setGainValue(gainNode, volume);
        return gainNode;
    }

    public setGainValue (gain: GainNode, volume: number): void {
        if (gain.gain.setTargetAtTime) {
            try {
                gain.gain.setTargetAtTime(volume, this.audioContext.currentTime, 0);
            } catch (e) {
                // Some unknown browsers may crash if timeConstant is 0
                gain.gain.setTargetAtTime(volume, this.audioContext.currentTime, 0.01);
            }
        } else {
            gain.gain.value = volume;
        }
    }

    public connectContext (audioNode: GainNode): void {
        if (!this.audioContext) {
            return;
        }
        audioNode.connect(this.audioContext.destination);
    }
}

export default new AudioContextAgent;