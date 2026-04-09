import { AudioClip } from "../../assets";
import { math, serializable, sqclass, type } from "../../core";
import { AudioPlayer, AudioState } from "../../platform";
import Component from "../component/Component";

/**
 * 声音源组件
 */

@sqclass("sq.AudioSourceComponent")
export default class AudioSourceComponent extends Component {


    private _clip: AudioClip;
    private _loop: boolean = false;
    private _playOnAwake: boolean = false;
    private _volume: number = 1;
    private _audioPlayer: AudioPlayer;

    /**
    * 设定要播放的音频。
    */
    @type(AudioClip)
    @serializable
    set clip(val) {
        if (val === this._clip) {
            return;
        }
        this._clip = val;
    }
    get clip(): AudioClip | null {
        return this._clip;
    }

    /**
    * 是否循环播放音频？
    */
    set loop(val) {
        this._loop = val;
        if (this._audioPlayer) {
            this._audioPlayer.loop = val;
        }
    }
    get loop(): boolean {
        return this._loop;
    }

    /**
    * 是否启用自动播放。 <br>
    * 请注意，根据最新的自动播放策略，现在对大多数平台，自动播放只会在第一次收到用户输入后生效。 <br>
    * 参考：https://www.chromium.org/audio-video/autoplay
    */
    set playOnAwake(val) {
        this._playOnAwake = val;
    }
    get playOnAwake(): boolean {
        return this._playOnAwake;
    }

    /**
    * 音频的音量（大小范围为 0.0 到 1.0）。
    * 请注意，在某些平台上，音量控制可能不起效。
    */
    set volume(val) {
        if (Number.isNaN(val)) { return; }
        val = math.clamp(val, 0, 1);
        if (this._audioPlayer) {
            this._audioPlayer.volume = val;
            this._volume = this._audioPlayer.volume;
        } else {
            this._volume = val;
        }
    }
    get volume(): number {
        return this._volume;
    }


    play() {

    }

    puase() {

    }

    stop() {

    }

    /**
   * 以指定音量倍数播放一个音频一次。最终播放的音量为 `audioSource.volume * volumeScale`。 <br>
   * @param clip The audio clip to be played.
   * @param volumeScale volume scaling factor wrt. current value.
   */
    public playOneShot(clip: AudioClip, volumeScale = 1): void {
        // if (!clip._nativeAsset) {
        //     error('Invalid audio clip');
        //     return;
        // }
        // let player: OneShotAudio;
        // AudioPlayer.loadOneShotAudio(clip._nativeAsset.url, this._volume * volumeScale, {
        //     audioLoadMode: clip.loadMode,
        // }).then((oneShotAudio) => {
        //     player = oneShotAudio;
        //     audioManager.discardOnePlayingIfNeeded();
        //     oneShotAudio.onEnd = (): void => {
        //         audioManager.removePlaying(oneShotAudio);
        //     };
        //     oneShotAudio.play();
        //     audioManager.addPlaying(oneShotAudio);
        // }).catch((e): void => {
        //     if (player) {
        //         audioManager.removePlaying(player);
        //     }
        // });
    }

    /**
     * @en
     * Set current playback time, in seconds.
     * @zh
     * 以秒为单位设置当前播放时间。
     * @param num playback time to jump to.
     */
    set currentTime(num: number) {
        if (Number.isNaN(num)) { return; }
        num = math.clamp(num, 0, this.duration);
        // if (!this._isLoaded && this.clip) {
        //     this._operationsBeforeLoading.push({ op: AudioOperationType.SEEK, params: [num] });
        //     return;
        // }
        // this._cachedCurrentTime = num;
        // eslint-disable-next-line @typescript-eslint/no-empty-function
        // this._audioPlayer?.seek(this._cachedCurrentTime).catch((e): void => { });
    }

    /**
     * 以秒为单位获取当前播放时间。
     */
    get currentTime(): number {
        return this._audioPlayer.currentTime;
    }

    /**
     * 获取以秒为单位的音频总时长。
     */
    get duration(): number {
        return this._audioPlayer.duration;
    }

    /**
     * @en
     * Get current audio state.
     * @zh
     * 获取当前音频状态。
     */
    get state(): AudioState {
        return this._audioPlayer ? this._audioPlayer.state : AudioState.INIT;
    }

    /**
     * 当前音频是否正在播放？
     */
    get playing(): boolean {
        return this.state === AudioState.PLAYING;
    }

}