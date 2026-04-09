import Keyboard from "./wx/input/Keyboard";
import Mouse from "./wx/input/Mouse";
import Touch from "./wx/input/Touch";
import Pacer from "./wx/Pacer";
export * from "./wx/Screen";
export * from "./wx/env";
export * from "./wx/assets/donwloader";
export * from "./wx/assets/asset-manager";
export * from "./common/index";
import PlatformWx from "./wx/PlatformWx";
import cacheManager from "./wx/assets/CacheManager";
import { systemInfo } from "./wx/SystemInfo";
export {
    Keyboard,
    Mouse,
    Touch,
    Pacer,
    cacheManager,
    systemInfo,
    PlatformWx as Platform
}