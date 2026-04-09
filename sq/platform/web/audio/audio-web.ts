import { http, ResponseType } from "../../../net/http";
import AudioContextAgent from "./AudioContextAgent";
import { AudioEvent, AudioPlayer, AudioState } from "../../common/audio";
import AudioTimer from "./audio-timer";
import { math } from "../../../core";

let AudioBufferCache = {
    _audioBufferDataMap: {},
    addCache(url: string, audioBuffer: AudioSourceWeb): void {
        const audioBufferData = this._audioBufferDataMap[url];
        if (audioBufferData) {
            /* debug:start */
            console.warn(`Audio buffer ${url} has been cached`);
            /* debug:end */
            return;
        }
        this._audioBufferDataMap[url] = {
            usedCount: 1,
            audioBuffer,
        };
    },

    retainCache(url: string): void {
        const audioBufferData = this._audioBufferDataMap[url];
        if (!audioBufferData) {
            /* debug:start */
            console.warn(`Audio buffer cache ${url} has not been added.`);
            /* deubg:end */
            return;
        }
        audioBufferData.usedCount++;
    },

    getCache(url: string): AudioSourceWeb | null | undefined {
        const audioBufferData = this._audioBufferDataMap[url];
        return audioBufferData?.audioBuffer;
    },

    tryReleasingCache(url: string): void {
        const audioBufferData = this._audioBufferDataMap[url];
        if (!audioBufferData) {
            /* debug:start */
            console.warn(`Audio buffer cache ${url} has not been added.`);
            /* debug:end */
            return;
        }
        if (--audioBufferData.usedCount <= 0) {
            delete this._audioBufferDataMap[url];
        }
    }
};


export class AudioSourceWeb extends AudioPlayer {
    private _buffer: AudioBuffer;
    private _url: string;
    private _runningCallback: any;
    private _sourceNode: AudioBufferSourceNode;
    private _loop: boolean = false;
    private _gainNode: GainNode;
    private _state: AudioState;
    private _audioTimer: AudioTimer;
    private _currentTimer: number;
    private _volume: number = 1.0;
    constructor(audioBuffer: AudioBuffer, url: string) {
        super();
        this._buffer = audioBuffer;
        this._url = url;
        this._gainNode = AudioContextAgent!.createGain();
        AudioContextAgent.connectContext(this._gainNode);

    }

    get buffer(): AudioBuffer { return this._buffer; }

    get src(): string { return this._url; }

    /**
     * 返回一个双精度数，表示缓冲区中存储的 PCM 数据的持续时间（以秒为单位）。
     * 就是正常速度播放一次音频所需要的时间
     */
    get duration(): number { return this._buffer.duration; }

    /**
     * 返回一个以浮点数表示的采样率。该采样率是存储在缓冲区的 PCM 数据每秒钟的采样。
     */
    get sampleRate(): number { return this._buffer.sampleRate; }

    get currentTime(): number { return this._audioTimer.currentTime; };

    /**
     * 返回一个表示缓冲区中存储的 PCM 数据的长度的整数（以采样帧数为单位）。
     * 
     */
    get length(): number { return this._buffer.length; }

    /**
     * 设置是否循环播放
     */
    set loop(b: boolean) {
        this._loop = b;
    }
    get loop() {
        return this._loop;
    }

    get volume(): number {
        return this._volume;
    }
    set volume(val: number) {
        val = math.clamp01(val);
        this._volume = val;
        AudioContextAgent.setGainValue(this._gainNode, val);
    }
    get state(): AudioState {
        return this._state;
    }

    private offRunning(): void {
        if (this._runningCallback) {
            AudioContextAgent!.offRunning(this._runningCallback);
            this._runningCallback = undefined;
        }
    }

    override play(): Promise<void> {
        return this._doPlay();
    }

    private _doPlay(): Promise<void> {
        return new Promise((resolve) => {
            if (AudioContextAgent.isRunning) {
                this._startSourceNode();
                resolve();
            } else {
                this.offRunning();
                this._runningCallback = (): void => {
                    this._startSourceNode();
                    resolve();
                };
                AudioContextAgent.onceRunning(this._runningCallback);
                AudioContextAgent.runContext();
            }
        });
    }


    private _startSourceNode(): void {
        this._stopSourceNode();
        this._sourceNode = AudioContextAgent.createBufferSource(this._buffer, this.loop);
        this._sourceNode.connect(this._gainNode);
        this._sourceNode.loop = this._loop;

        /**
         * start(when?, offset?, duration?) 函数用于播放Source Node中的音频数据
         * 1、when：表示什么时候才开播放，即你就算调用了start，但系统还没有到when指定的时间，则不会播放。
         *     when指定的是一个时间戳，但这个时间戳不是Date，而是audioContext的时间系统
         *   比如：source.start(audioContext.currentTime + 0.5); 表示 0.5 秒后播放
         *  
         *  如果指定的为0，或小于 audioContext.currentTime 则表示立刻播放
         * 
         * 2、offset
         *  从音频的哪一秒开始播。相对于 AudioBuffer 的起始位置。
         *  
         * 3、duration
         *   从 offset 开始，播放的长度
         */
        this._sourceNode.start(0, this._audioTimer.currentTime);
        this._state = AudioState.PLAYING;
        this._audioTimer.start();

        /**
         * 用于Web API 提供的End事件不是所有平台都支持。
         * 所以这里我们自己去计算音频播放时长然后去触发END事件。所以创建了AudioTimer类来处理这种需求
         */
        const checkEnded = (): void => {
            if (this.loop) {
                this._currentTimer = window.setTimeout(checkEnded, this.buffer.duration * 1000);
            } else {  // do ended
                this._audioTimer.stop();
                this.fire(AudioEvent.ENDED);
                this._state = AudioState.INIT;
            }
        };
        window.clearTimeout(this._currentTimer);
        this._currentTimer = window.setTimeout(checkEnded, (this.buffer.duration - this._audioTimer.currentTime) * 1000);
    }

    private _stopSourceNode(): void {
        if (this._sourceNode) {
            this._sourceNode.stop();
            this._sourceNode.disconnect();
            this._sourceNode.buffer = null;
            this._sourceNode = undefined;
        }
    }


    override pause(): Promise<void> {
        this.offRunning();
        if (this._state !== AudioState.PLAYING || !this._sourceNode) {
            return Promise.resolve();
        }
        this._audioTimer.pause();
        this._state = AudioState.PAUSED;
        window.clearTimeout(this._currentTimer);
        this._stopSourceNode();
        return Promise.resolve();
    }

    stop(): Promise<void> {
        this.offRunning();
        if (!this._sourceNode) {
            this._audioTimer.stop();
            this._state = AudioState.STOPPED;
            return Promise.resolve();
        }
        this._audioTimer.stop();
        this._state = AudioState.STOPPED;
        window.clearTimeout(this._currentTimer);
        this._stopSourceNode();
        return Promise.resolve();
    }

    onInterruptionBegin(cb: () => void): void { this.on(AudioEvent.INTERRUPTION_BEGIN, this, cb); }
    offInterruptionBegin(cb?: () => void): void { this.off(AudioEvent.INTERRUPTION_BEGIN, this, cb); }
    onInterruptionEnd(cb: () => void): void { this.on(AudioEvent.INTERRUPTION_END, this, cb); }
    offInterruptionEnd(cb?: () => void): void { this.off(AudioEvent.INTERRUPTION_END, this, cb); }
    onEnded(cb: () => void): void { this.on(AudioEvent.ENDED, this, cb); }
    offEnded(cb?: () => void): void { this.off(AudioEvent.ENDED, this, cb); }
}


export function downloaderAudioWeb(url: string, callback: (error: number, audioBuffer: AudioSourceWeb | null) => void) {
    const cachedAudioBuffer = AudioBufferCache.getCache(url);

    if (cachedAudioBuffer) {
        AudioBufferCache.retainCache(url);
        return;
    }

    http.get(url, {
        responseType: ResponseType.ARRAY_BUFFER
    }, (status: number, data: ArrayBuffer) => {
        AudioContextAgent.audioContext.decodeAudioData(data)
            .then((audioBuffer: AudioBuffer) => {
                let webSource = new AudioSourceWeb(audioBuffer, url);
                AudioBufferCache.addCache(url, webSource);
                callback(status, webSource);
            })

            /* debug:start */
            .catch((e: any) => {
                console.warn('loadNative error', url, e);
            });
        /*debug:end*/
    });
}
