import { math } from "../../../core";

interface IDuration {
    duration: number;
}

export default class AudioTimer {
    private _nativeAudio: IDuration;
    private _startTime = 0;
    private _startOffset = 0;
    private _isPaused = true;

    constructor (nativeAudio: IDuration) {
        this._nativeAudio = nativeAudio;
    }

    public destroy (): void {
        this._nativeAudio = undefined as any;
    }

    get duration (): number {
        return this._nativeAudio.duration;
    }

    /**
     * Get the current time of audio timer.
     */
    get currentTime (): number {
        if (this._isPaused) {
            return this._startOffset;
        } else {
            return this._calculateCurrentTime();
        }
    }

    private _now (): number {
        return performance.now() / 1000;
    }

    private _calculateCurrentTime (): number {
        const timePassed = this._now() - this._startTime;
        const currentTime = this._startOffset + timePassed;
        if (currentTime >= this.duration) {
            // timer loop
            this._startTime = this._now();
            this._startOffset = 0;
        }
        return currentTime % this.duration;
    }

    /**
     * Start the audio timer.
     * Call this method when audio is played.
     */
    start (): void {
        this._isPaused = false;
        this._startTime = this._now();
    }
    
    pause (): void {
        if (this._isPaused) {
            return;
        }
        this._isPaused = true;
        this._startOffset = this._calculateCurrentTime();
    }

    /**
     * Stop the audio timer.
     * Call this method when audio playing ended or audio is stopped.
     */
    stop (): void {
        this._isPaused = true;
        this._startOffset = 0;
    }

    /**
     * Seek the audio timer.
     * Call this method when audio is seeked.
     */
    seek (time: number): void {
        this._startTime = this._now();
        this._startOffset = math.clamp(time, 0, this.duration);
    }
}
