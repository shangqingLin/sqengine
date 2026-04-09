
export default interface BaseGPUPerformance {
    beginQuery(timestamp: boolean): void;
    endQuery(): void;
    delete(): void;
}