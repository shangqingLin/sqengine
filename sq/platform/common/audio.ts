import { EventDispatch } from "../../core";


export abstract class AudioPlayer extends EventDispatch {

    /**
     * 播放这个音频
     */
    abstract play(): Promise<void>;
    abstract stop(): void;
    abstract pause(): Promise<void>;
    abstract get src(): string;

    /**
     * 就是正常速度播放一次音频所需要的时间
     */
    abstract get duration(): number

    abstract get currentTime(): number;

    abstract get sampleRate(): number;

    /**
     * 返回一个表示缓冲区中存储的 PCM 数据的长度的整数（以采样帧数为单位）。
     */
    abstract get length(): number;

    /**
     * 设置是否循环播放
     */
    abstract set loop(b: boolean);
    abstract get loop();

    /**
     * 音量大小
     */
    abstract get volume(): number;
    abstract set volume(val: number);

    abstract get state(): AudioState;

}


export enum AudioEvent {
    PLAYED = 'play',
    PAUSED = 'pause',
    STOPPED = 'stop',
    SEEKED = 'seeked',
    ENDED = 'ended',
    INTERRUPTION_BEGIN = 'interruptionBegin',
    INTERRUPTION_END = 'interruptionEnd',
    USER_GESTURE = 'on_gesture',  //只有Web平台支持
}

export enum AudioState {
    INIT,
    PLAYING,
    PAUSED,
    STOPPED,
    INTERRUPTED,
}
