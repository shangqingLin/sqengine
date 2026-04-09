# SQENGINE
一个2D H5游戏引引擎，完全使用C++开发，TypeScript作为前端调用接口，使用Emscriptent工具编译成WebAssembly(Wasm)运行的游戏引擎，相比使用纯JavaScript开发的游戏引擎最少快3倍。
使用ECS（Entity-Component-System） 实体-组件结构。
---
支持：
- 基于FreeType的文本渲染
- 精灵Sprite
- 2D光照系统
- 完全基于GPU端计算的粒子系统
- 基于Box2D 3.x版本进行深度修改的物理引擎
- 物理流体引擎
- 资源管理
