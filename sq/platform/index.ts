import Keyboard from "./web/input/Keyboard";
import Mouse from "./web/input/Mouse";
import Touch from "./web/input/Touch";
import Pacer from "./web/Pacer";
export * from "./web/Screen";
export * from "./web/env";
export * from "./web/assets/donwloader";
export * from "./web/assets/asset-manager";
export * from "./common/index";
import PlatformWeb from "./web/PlatformWeb";
import cacheManager from "./web/assets/CacheManager";
import { systemInfo } from "./web/SystemInfo";
export {
    Keyboard,
    Mouse,
    Touch,
    Pacer,
    cacheManager,
    systemInfo,
    PlatformWeb as Platform
}