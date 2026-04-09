import { MaskComponent } from "../../../2d";
import { ObjectType, OverflowType, ScrollBarDisplayType, ScrollType } from "../../common/FieldTypes";
import { GComponent } from "../../GComponent";
import { GObject } from "../../GObject";
import { ScrollPane } from "../../ScrollPane";
import { TranslationHelper } from "../../TranslationHelper";
import { UIConfig } from "../../UIConfig";
import { PackageItem } from "../PackageItem";
import { BaseParse } from "./BaseParse";
import { UIPackage } from "../UIPackage";
import { Controller } from "../../Controller";
import { ControllerAction } from "../../action/ControllerAction";
import { PlayTransitionAction } from "../../action/PlayTransitionAction";
import { ChangePageAction } from "../../action/ChangePageAction";
import { UIObjectFactory } from "../UIObjectFactory";
import ObjectCreateProcess from "../ObjectCreateProcess";
import GObjectParse from "./GObjectParse";
import { Relations } from "../../Relations";
import RelationParse from "./RelationParse";
import { ByteBuffer } from "../../utils/ByteBuffer";
import { Transition } from "../../Transition";
import TransitionParse from "./TransitionParse";
import { Application } from "../../../framework/Application";
import { ChildHitArea, PixelHitTest } from "../../event/HitTest";
import { NodeEventType } from "../../../scene";

export default class GComponentParse extends BaseParse {

    private parseFunctionSequence: Array<(g: GObject, fileBufferData: ByteBuffer) => Promise<void>>;
    private parseConstructorIndex = 0;
    private runConstructorCompleteCallback: Function = null;
    constructor() {
        super();
        this.parseFunctionSequence = [
            this.parseTranslation,
            this.parseSize,
            this.parseLayoutProprety,
            this.parseOverflow,
            this.parseController,
            this.parseChildrenAsync,
            this.parseComponentRelationsAsync,
            this.parseChildRelationAsync,
            this.parseChildSetupAfterAsync,
            this.parseMask,
            this.parseHit,
            this.parseTransitionAsync,
            this.parseCompleteAsync
        ];
    }

    private runContructorBuildAsync(g: GObject, fileBufferData: ByteBuffer) {
        if (!this.parseFunctionSequence || this.parseFunctionSequence.length === 0) return this.onRunConsructorComplete();
        let func: (g: GObject, fileBufferData: ByteBuffer) => Promise<void> = this.parseFunctionSequence[this.parseConstructorIndex++];
        if (!func) {
            return this.onRunConsructorComplete();
        }
        func.apply(this, [g, fileBufferData])
            .then(() => {
                if (Application.ins.getFromFrameStartTime() >= 5) {
                    Application.ins.timer.nextTick(this, this.runContructorBuildAsync, [g, fileBufferData]);
                    return;
                }
                this.runContructorBuildAsync(g, fileBufferData);
            });
    }

    protected onRunConsructorComplete() {
        this.runConstructorCompleteCallback();
        this.runConstructorCompleteCallback = null;
    };

    public override async constructFromResourceAsync(g: GComponent): Promise<void> {
        this.parseConstructorIndex = 0;
        return new Promise<void>((resolve, reject) => {
            this.runConstructorCompleteCallback = resolve;
            let contentItem: PackageItem = g.packageItem.getBranch();
            let fileBufferData: ByteBuffer = contentItem.rawData;
            this.runContructorBuildAsync(g, fileBufferData);
        });
    }

    public override constructFromResource(g: GComponent): void {
        let contentItem: PackageItem = g.packageItem.getBranch();
        let fileBufferData: ByteBuffer = contentItem.rawData;
        this.parseTranslation(g, fileBufferData);
        this.parseSize(g, fileBufferData);
        this.parseLayoutProprety(g, fileBufferData);
        this.parseOverflow(g, fileBufferData);
        this.parseController(g, fileBufferData);
        this.parseChildren(g, fileBufferData);
        this.parseComponentRelations(g, fileBufferData);
        this.parseChildRelation(g, fileBufferData);
        this.parseChildSetupAfter(g, fileBufferData);
        this.parseMask(g, fileBufferData);
        this.parseHit(g, fileBufferData);
        this.parseTransition(g, fileBufferData);
        this.parseComplete(g, fileBufferData);
    }

    private async parseTranslation(g: GObject, buffer: ByteBuffer) {
        var contentItem: PackageItem = g.packageItem.getBranch();
        if (!contentItem.decoded) {
            contentItem.decoded = true;
            TranslationHelper.translateComponent(contentItem);
        }
    }

    private async parseSize(g: GObject, buffer: ByteBuffer) {
        buffer.seek(0, 0);
        g.sourceWidth = buffer.readInt();
        g.sourceHeight = buffer.readInt();
        g.initWidth = g.sourceWidth;
        g.initHeight = g.sourceHeight;
        g.setSize(g.sourceWidth, g.sourceHeight);
    }

    private async parseLayoutProprety(g: GObject, buffer: ByteBuffer) {
        if (buffer.readBool()) {
            g.minWidth = buffer.readInt();
            g.maxWidth = buffer.readInt();
            g.minHeight = buffer.readInt();
            g.maxHeight = buffer.readInt();
        }

        if (buffer.readBool()) {
            let f1 = buffer.readFloat();
            let f2 = buffer.readFloat();
            g.setPivot(f1, f2, buffer.readBool());
        }

        if (buffer.readBool()) {
            //@ts-ignore
            g._margin.top = buffer.readInt();

            //@ts-ignore
            g._margin.bottom = buffer.readInt();

            //@ts-ignore
            g._margin.left = buffer.readInt();

            //@ts-ignore
            g._margin.right = buffer.readInt();
        }
    }

    protected async parseOverflow(g: GObject, buffer: ByteBuffer) {

        let cg: GComponent = g as GComponent;

        //溢出设置
        var overflow: number = buffer.readByte();
        if (overflow == OverflowType.Scroll) {
            var savedPos: number = buffer.position;
            buffer.seek(0, 7);
            cg.setOverflow(OverflowType.Scroll);
            this.parseSteupScrollPanel(cg, buffer);
            buffer.position = savedPos;
        }
        else
            cg.setOverflow(overflow);
    }

    protected parseSteupScrollPanel(g: GComponent, buffer: ByteBuffer) {
        //组件中的“溢出处理”设置,如果选择的是溢出是可以滚动的
        //这里解析组件中溢出的设置（即在“溢出处理”右边的齿轮按钮面板里面的设置）

        let scrollPanel: ScrollPane = g.scrollPane;

        //@ts-ignore
        scrollPanel._scrollType = buffer.readByte();
        var scrollBarDisplay: ScrollBarDisplayType = buffer.readByte();
        var flags: number = buffer.readInt();


        if (buffer.readBool()) {
            //定位设置

            //@ts-ignore
            scrollPanel._scrollBarMargin.top = buffer.readInt();
            //@ts-ignore
            scrollPanel._scrollBarMargin.bottom = buffer.readInt();
            //@ts-ignore
            scrollPanel._scrollBarMargin.left = buffer.readInt();
            //@ts-ignore
            scrollPanel._scrollBarMargin.right = buffer.readInt();
        }

        var vtScrollBarRes: string = buffer.readS();
        var hzScrollBarRes: string = buffer.readS();
        var headerRes: string = buffer.readS();
        var footerRes: string = buffer.readS();

        //@ts-ignore
        if ((flags & 1) != 0) scrollPanel._displayOnLeft = true;

        //@ts-ignore
        if ((flags & 2) != 0) scrollPanel._snapToItem = true;
        if ((flags & 4) != 0) scrollPanel._displayInDemand = true;

        //@ts-ignore
        if ((flags & 8) != 0) scrollPanel._pageMode = true;


        if (flags & 16)
            //@ts-ignore
            scrollPanel._touchEffect = true;
        else if (flags & 32)
            //@ts-ignore
            scrollPanel._touchEffect = false;
        else
            //@ts-ignore
            scrollPanel._touchEffect = UIConfig.defaultScrollTouchEffect;
        if (flags & 64)
            //@ts-ignore
            scrollPanel._bouncebackEffect = true;
        else if (flags & 128)
            //@ts-ignore
            scrollPanel._bouncebackEffect = false;
        else
            //@ts-ignore
            scrollPanel._bouncebackEffect = UIConfig.defaultScrollBounceEffect;

        //@ts-ignore
        if ((flags & 256) != 0) scrollPanel._inertiaDisabled = true;

        //@ts-ignore
        if ((flags & 512) != 0) scrollPanel._dontClip = true;

        //@ts-ignore
        if ((flags & 1024) != 0) scrollPanel._floating = true;

        //@ts-ignore
        if ((flags & 2048) != 0) scrollPanel._dontClipMargin = true;

        //@ts-ignore
        if (!scrollPanel._dontClip) {
            //@ts-ignore
            let mask = scrollPanel._maskContainer.addComponent(MaskComponent);
            //@ts-ignore
            scrollPanel._maskContainer.on(MaskComponent.MASK_TYPE_CHANGE, scrollPanel, scrollPanel.onMaskTypeChange);

            if (mask.subComp) {
                //@ts-ignore
                scrollPanel._maskContainerUITrans = mask.subComp;
                //@ts-ignore
                scrollPanel._maskContainerUITrans.setAnchorPoint(0, 1);
            }

        }

        if (scrollBarDisplay === ScrollBarDisplayType.Default)
            scrollBarDisplay = UIConfig.defaultScrollBarDisplay;

        if (scrollBarDisplay !== ScrollBarDisplayType.Hidden) {

            //@ts-ignore
            if (scrollPanel._scrollType === ScrollType.Both || scrollPanel._scrollType === ScrollType.Vertical) {
                var res: string = vtScrollBarRes ? vtScrollBarRes : UIConfig.verticalScrollBar;
                if (res) {
                    //@ts-ignore
                    scrollPanel._vtScrollBar = <GScrollBar><any>(UIPackage.createObjectFromURL(res));
                    //@ts-ignore
                    if (!scrollPanel._vtScrollBar)
                        throw new Error("cannot create scrollbar from " + res);

                    //@ts-ignore
                    scrollPanel._vtScrollBar.setScrollPane(scrollPanel, true);

                    //scrollPanel._vtScrollBar.node.parent = scrollPanel.owner.node;
                    //@ts-ignore
                    scrollPanel.owner.node.addChild(scrollPanel._vtScrollBar.node);
                }
            }

            //水平滚动条
            //@ts-ignore
            if (scrollPanel._scrollType === ScrollType.Both || scrollPanel._scrollType === ScrollType.Horizontal) {
                var res: string = hzScrollBarRes ? hzScrollBarRes : UIConfig.horizontalScrollBar;
                if (res) {

                    //创建水平方向的ScrollBar样式
                    //@ts-ignore
                    scrollPanel._hzScrollBar = <GScrollBar><any>(UIPackage.createObjectFromURL(res));

                    //@ts-ignore
                    if (!scrollPanel._hzScrollBar)
                        throw new Error("cannot create scrollbar from " + res);

                    //@ts-ignore
                    scrollPanel._hzScrollBar.setScrollPane(scrollPanel, false);

                    // scrollPanel._hzScrollBar.node.parent = o.node;
                    //@ts-ignore
                    scrollPanel.owner.node.addChild(scrollPanel._hzScrollBar.node);
                }
            }

            if (scrollBarDisplay == ScrollBarDisplayType.Auto)
                //@ts-ignore
                scrollPanel._scrollBarDisplayAuto = true;

            //@ts-ignore
            if (scrollPanel._scrollBarDisplayAuto) {

                //@ts-ignore
                if (scrollPanel._vtScrollBar)
                    //@ts-ignore
                    scrollPanel._vtScrollBar.node.visible = false;

                //@ts-ignore
                if (scrollPanel._hzScrollBar)
                    //@ts-ignore
                    scrollPanel._hzScrollBar.node.visible = false;

                //@ts-ignore
                scrollPanel.owner.on(NodeEventType.MOUSE_OVER, scrollPanel.onRollOver, scrollPanel);

                //@ts-ignore
                scrollPanel.owner.on(NodeEventType.MOUSE_OUT, scrollPanel.onRollOut, scrollPanel);
            }
        }

        if (headerRes) {
            //@ts-ignore
            scrollPanel._header = <GComponent>(UIPackage.createObjectFromURL(headerRes));

            //@ts-ignore
            if (scrollPanel._header == null)
                throw new Error("cannot create scrollPane header from " + headerRes);
            else
                //@ts-ignore
                scrollPanel._maskContainer.insertChild(scrollPanel._header.node, 0);
        }

        if (footerRes) {
            //@ts-ignore
            scrollPanel._footer = <GComponent><any>(UIPackage.createObjectFromURL(footerRes));
            //@ts-ignore
            if (scrollPanel._footer == null)
                throw new Error("cannot create scrollPane footer from " + footerRes);
            else
                //@ts-ignore
                scrollPanel._maskContainer.insertChild(scrollPanel._footer.node, 0);
        }

        //@ts-ignore
        scrollPanel._refreshBarAxis = (scrollPanel._scrollType === ScrollType.Both || scrollPanel._scrollType === ScrollType.Vertical) ? "y" : "x";

        scrollPanel.setSize(scrollPanel.owner.width, scrollPanel.owner.height);
    }

    private async parseController(g: GComponent, buffer: ByteBuffer) {
        if (buffer.readBool())
            buffer.skip(8);

        g._buildingDisplayList = true;

        buffer.seek(0, 1);

        var nextPos: number;
        var controllerCount: number = buffer.readShort();
        for (let i = 0; i < controllerCount; i++) {
            nextPos = buffer.readShort();
            nextPos += buffer.position;
            this.parseSteupController(g, buffer);
            buffer.position = nextPos;
        }
    }

    private parseSteupController(g: GComponent, buffer: ByteBuffer) {

        var controller: Controller = new Controller();
        //@ts-ignore
        g._controllers.push(controller);
        controller.parent = g;

        var beginPos: number = buffer.position;
        buffer.seek(beginPos, 0);

        controller.name = buffer.readS();
        if (buffer.readBool())
            controller.autoRadioGroupDepth = true;

        buffer.seek(beginPos, 1);

        var i: number;
        var nextPos: number;
        var cnt: number = buffer.readShort();

        for (i = 0; i < cnt; i++) {
            //@ts-ignore
            controller._pageIds.push(buffer.readS());

            //@ts-ignore
            controller._pageNames.push(buffer.readS());
        }

        var homePageIndex: number = 0;
        if (buffer.version >= 2) {
            var homePageType: number = buffer.readByte();
            switch (homePageType) {
                case 1:
                    homePageIndex = buffer.readShort();
                    break;

                case 2:
                    //@ts-ignore
                    homePageIndex = controller._pageNames.indexOf(UIPackage.branch);
                    if (homePageIndex == -1)
                        homePageIndex = 0;
                    break;

                case 3:
                    //@ts-ignore
                    homePageIndex = controller._pageNames.indexOf(UIPackage.getVar(buffer.readS()));
                    if (homePageIndex == -1)
                        homePageIndex = 0;
                    break;
            }
        }

        buffer.seek(beginPos, 2);

        cnt = buffer.readShort();
        if (cnt > 0) {
            //@ts-ignore
            if (!controller._actions)
                //@ts-ignore
                controller._actions = new Array<ControllerAction>();

            for (i = 0; i < cnt; i++) {
                nextPos = buffer.readShort();
                nextPos += buffer.position;

                var action: ControllerAction = createAction(buffer.readByte());
                this.parseSetupControllerAction(action, buffer);

                //@ts-ignore
                controller._actions.push(action);

                buffer.position = nextPos;
            }
        }

        //@ts-ignore
        if (controller.parent && controller._pageIds.length > 0)
            //@ts-ignore
            controller._selectedIndex = homePageIndex;
        else
            //@ts-ignore
            controller._selectedIndex = -1;
    }

    private parseSetupControllerAction(action: ControllerAction, buffer: ByteBuffer): void {
        var cnt: number;
        var i: number;

        //ControllerAction
        cnt = buffer.readShort();
        action.fromPage = [];
        for (i = 0; i < cnt; i++)
            action.fromPage[i] = buffer.readS();

        cnt = buffer.readShort();
        action.toPage = [];
        for (i = 0; i < cnt; i++)
            action.toPage[i] = buffer.readS();

        if (action instanceof PlayTransitionAction) {
            (action as PlayTransitionAction).transitionName = buffer.readS();
            (action as PlayTransitionAction).playTimes = buffer.readInt();
            (action as PlayTransitionAction).delay = buffer.readFloat();
            (action as PlayTransitionAction).stopOnExit = buffer.readBool();
        } else {
            //ChangePageAction

            (action as ChangePageAction).objectId = buffer.readS();
            (action as ChangePageAction).controllerName = buffer.readS();
            (action as ChangePageAction).targetPage = buffer.readS();
        }
    }

    /**
     * 异步方式创建子对象
     * @param g 
     * @param buffer 
     * @returns 
     */
    private async parseChildrenAsync(g: GComponent, buffer: ByteBuffer) {
        buffer.seek(0, 2);
        var childCount: number = buffer.readShort();
        if (childCount === 0) return Promise.resolve();

        return new Promise<void>((resove, reject) => {
            let current = 0;
            let parseNext = () => {
                current++;
                if (current > childCount) {
                    resove();
                    return;
                }

                this.parseOneChildAsnyc(g, buffer).then(() => {
                    parseNext();
                })
            }
            parseNext();
        });
    }

    private async parseOneChildAsnyc(g: GComponent, buffer: ByteBuffer) {
        var child: GObject;
        let dataLen = buffer.readShort();
        let curPos = buffer.position;
        buffer.seek(curPos, 0);

        var type: ObjectType = buffer.readByte();
        var src: string = buffer.readS();
        var pkgId: string = buffer.readS();

        var pi: PackageItem = null;
        if (src !== null) {
            var pkg: UIPackage;
            if (pkgId !== null)
                pkg = UIPackage.getById(pkgId);
            else
                pkg = g.packageItem.owner;
            pi = pkg ? pkg.getItemById(src) : null;
        }

        if (pi) {
            //如果有pi，表示是内嵌到当前组件中的其他组件(xml文件、图片等资源都可能是)
            child = UIObjectFactory.newObject(pi);
        }
        else {
            //表示在组件中就是一个普通的GObject控件
            child = UIObjectFactory.newObject(type);
        }

        let childParse = ObjectCreateProcess.getParseByGObject(child);
        return Promise.resolve()
            .then(() => {
                if (pi) {
                    return (childParse as GComponentParse).constructFromResourceAsync(child as GComponent);
                }
            }).then(() => {
                child._underConstruct = true;
                return new Promise<void>((resolve, reject) => {
                    //@ts-ignore
                    childParse.parseSetupBeforeAsync(child, curPos, buffer)
                        .then(() => {
                            if (Application.ins.getFromFrameStartTime() >= 5) {
                                Application.ins.timer.nextTick(this, resolve);
                            } else {
                                resolve();
                            }
                        })
                })
            }).then(() => {

                child._parent = g;
                //@ts-ignore
                g._container.addChild(child.node);

                //@ts-ignore
                g._children.push(child);
                buffer.position = curPos + dataLen;
                ObjectCreateProcess.recoveryParse(childParse);
            });
    }

    /**
     * 同步方式创建子对象
     * @param g 
     * @param buffer 
     */
    protected parseChildren(g: GComponent, buffer: ByteBuffer) {
        var child: GObject;
        var childCount: number = buffer.readShort();
        for (let i = 0; i < childCount; i++) {
            let dataLen = buffer.readShort();
            let curPos = buffer.position;
            buffer.seek(curPos, 0);

            var type: ObjectType = buffer.readByte();
            var src: string = buffer.readS();
            var pkgId: string = buffer.readS();

            var pi: PackageItem = null;
            if (src != null) {
                var pkg: UIPackage;
                if (pkgId != null)
                    pkg = UIPackage.getById(pkgId);
                else
                    pkg = g.packageItem.owner;

                pi = pkg ? pkg.getItemById(src) : null;
            }

            if (pi) {
                child = UIObjectFactory.newObject(pi);
            }
            else
                child = UIObjectFactory.newObject(type);


            let childParse = ObjectCreateProcess.getParseByGObject(child);
            if (pi) {
                (childParse as GComponentParse).constructFromResource(child as GComponent);
            }

            child._underConstruct = true;

            //@ts-ignore
            childParse.parseSetupBefore(child, curPos, buffer);

            //@ts-ignore
            g._container.addChild(child.node);
            //@ts-ignore
            g._children.push(child);
            buffer.position = curPos + dataLen;
            ObjectCreateProcess.recoveryParse(childParse);
        }
    }

    protected override async parseSetupBeforeAsync(g: GComponent, beginPos: number, buffer: ByteBuffer) {
        return GObjectParse.setupBeforeAddAsync(g, beginPos, buffer);
    }

    protected override parseSetupBefore(g: GComponent, beginPos: number, buffer: ByteBuffer) {
        GObjectParse.setupBeforeAdd(g, beginPos, buffer);
    }

    private async parseComponentRelationsAsync(g: GComponent, buffer: ByteBuffer) {
        buffer.seek(0, 3);
        let relation: Relations = g.relations;
        return RelationParse.setupAsync(relation, buffer, true);
    }

    private async parseComponentRelations(g: GComponent, buffer: ByteBuffer) {
        buffer.seek(0, 3);
        let relation: Relations = g.relations;
        return RelationParse.setup(relation, buffer, true);
    }

    private async parseChildRelationAsync(g: GComponent, buffer: ByteBuffer) {
        buffer.seek(0, 2);
        buffer.skip(2);
        let childCount: number = g.numChildren;
        if (childCount === 0) return Promise.resolve();

        return new Promise<void>((resolve, reject) => {
            var nextPos: number;
            let current: number = -1;
            let next = () => {
                ++current;
                if (current === childCount) {
                    return resolve();
                }
                nextPos = buffer.readShort();
                nextPos += buffer.position;
                buffer.seek(buffer.position, 3);

                //@ts-ignore
                RelationParse.setupAsync(g._children[current].relations, buffer, false)
                    .then(() => {
                        buffer.position = nextPos;
                        next();
                    })
            }
            next();
        });
    }

    private parseChildRelation(g: GComponent, buffer: ByteBuffer) {
        buffer.seek(0, 2);
        buffer.skip(2);
        var nextPos: number;
        for (let i = 0; i < g.numChildren; i++) {
            nextPos = buffer.readShort();
            nextPos += buffer.position;
            buffer.seek(buffer.position, 3);
            //@ts-ignore
            RelationParse.setup(g._children[i].relations, buffer, false);
            buffer.position = nextPos;
        }
    }

    private async parseChildSetupAfterAsync(g: GComponent, buffer: ByteBuffer) {
        buffer.seek(0, 2);
        buffer.skip(2);
        let childCount: number = g.numChildren;
        if (childCount === 0) return Promise.resolve();

        //执行parseSetupAfterAsync
        return new Promise<void>((resolve, reject) => {
            let current: number = -1;
            let next = () => {
                ++current;
                if (current === childCount) {
                    return resolve();
                }

                let nextPos = buffer.readShort();
                nextPos += buffer.position;

                //@ts-ignore
                let child = g._children[current];
                let childParse = ObjectCreateProcess.getParseByGObject(child);

                //@ts-ignore
                childParse.parseSetupAfterAsync(child, buffer.position, buffer)
                    .then(() => {
                        child._underConstruct = false;
                        buffer.position = nextPos;
                        ObjectCreateProcess.recoveryParse(childParse);
                        if (Application.ins.getFromFrameStartTime() >= 5) {
                            Application.ins.timer.nextTick(this, next);
                        } else {
                            next();
                        }
                    });
            }
            next();
        })
    }

    protected override async parseSetupAfterAsync(g: GComponent, beginPos: number, buffer: ByteBuffer) {

        return Promise.resolve()
            .then(() => {
                return GObjectParse.setupAfterAddAsync(g, beginPos, buffer);
            }).
            then(() => {
                buffer.seek(beginPos, 4);

                var pageController: number = buffer.readShort();
                if (pageController != -1 && g._scrollPane)
                    g._scrollPane.pageController = g._parent.getControllerAt(pageController);

                var cnt: number = buffer.readShort();
                for (var i: number = 0; i < cnt; i++) {
                    var cc: Controller = g.getController(buffer.readS());
                    var pageId: string = buffer.readS();
                    if (cc)
                        cc.selectedPageId = pageId;
                }

                if (buffer.version >= 2) {
                    cnt = buffer.readShort();
                    for (i = 0; i < cnt; i++) {
                        var target: string = buffer.readS();
                        var propertyId: number = buffer.readShort();
                        var value: String = buffer.readS();
                        var obj: GObject = g.getChildByPath(target);
                        if (obj)
                            obj.setProp(propertyId, value);
                    }
                }
            })
    }

    private parseChildSetupAfter(g: GComponent, buffer: ByteBuffer) {
        buffer.seek(0, 2);
        buffer.skip(2);
        let childCount: number = g.numChildren;
        for (let i = 0; i < childCount; i++) {
            var nextPos = buffer.readShort();
            nextPos += buffer.position;
            //@ts-ignore
            let child = g._children[i];
            let childParse = ObjectCreateProcess.getParseByGObject(child);
            //@ts-ignore
            childParse.parseSetupAfter(child, buffer.position, buffer);
            child._underConstruct = false;
            buffer.position = nextPos;
            ObjectCreateProcess.recoveryParse(childParse);
        }
    }

    protected override parseSetupAfter(g: GComponent, beginPos: number, buffer: ByteBuffer) {

        GObjectParse.setupAfterAdd(g, beginPos, buffer);

        buffer.seek(beginPos, 4);

        var pageController: number = buffer.readShort();
        if (pageController != -1 && g._scrollPane)
            g._scrollPane.pageController = g._parent.getControllerAt(pageController);

        var cnt: number = buffer.readShort();
        for (var i: number = 0; i < cnt; i++) {
            var cc: Controller = g.getController(buffer.readS());
            var pageId: string = buffer.readS();
            if (cc)
                cc.selectedPageId = pageId;
        }

        if (buffer.version >= 2) {
            cnt = buffer.readShort();
            for (i = 0; i < cnt; i++) {
                var target: string = buffer.readS();
                var propertyId: number = buffer.readShort();
                var value: String = buffer.readS();
                var obj: GObject = g.getChildByPath(target);
                if (obj)
                    obj.setProp(propertyId, value);
            }
        }
    }


    private async parseMask(g: GComponent, buffer: ByteBuffer) {
        buffer.seek(0, 4);
        buffer.skip(2); //customData
        g.opaque = buffer.readBool();
        var maskId: number = buffer.readShort();
        if (maskId != -1) {
            g.setMask(g.getChildAt(maskId), buffer.readBool());
        }
    }

    private async parseHit(g: GComponent, buffer: ByteBuffer) {
        var hitTestId: string = buffer.readS();
        var i1 = buffer.readInt();
        var i2 = buffer.readInt();

        let pi;
        if (hitTestId != null) {
            pi = g.packageItem.owner.getItemById(hitTestId);
            if (pi && pi.hitTestData)
                g.hitArea = new PixelHitTest(pi.hitTestData, i1, i2);
        }
        else if (i1 != 0 && i2 != -1) {
            g.hitArea = new ChildHitArea(g.getChildAt(i2));
        }
    }

    private async parseTransitionAsync(g: GComponent, buffer: ByteBuffer) {
        buffer.seek(0, 5);
        var transitionCount: number = buffer.readShort();

        if (transitionCount === 0) return Promise.resolve();

        return new Promise<void>((resolve, reject) => {
            let current: number = -1;
            let next = () => {
                ++current;
                if (current === transitionCount) {
                    return resolve();
                }

                let nextPos = buffer.readShort();
                nextPos += buffer.position;

                var trans: Transition = new Transition(g);

                //@ts-ignore
                g._transitions.push(trans);

                TransitionParse.setup(trans, buffer);

                buffer.position = nextPos;
                if (Application.ins.getFromFrameStartTime() >= 5) {
                    Application.ins.timer.nextTick(this, next);
                    return;
                }
                next();
            }
            next();
        })

    }

    private parseTransition(g: GComponent, buffer: ByteBuffer) {
        buffer.seek(0, 5);
        var transitionCount: number = buffer.readShort();
        for (let i = 0; i < transitionCount; i++) {
            var nextPos = buffer.readShort();
            nextPos += buffer.position;
            var trans: Transition = new Transition(g);
            TransitionParse.setup(trans, buffer);

            //@ts-ignore
            g._transitions.push(trans);

            buffer.position = nextPos;
        }
    }

    private async parseCompleteAsync(g: GComponent, buffer: ByteBuffer) {
        g.applyAllControllers();

        g._buildingDisplayList = false;
        g._underConstruct = false;

        //@ts-ignore
        g.buildNativeDisplayList();
        g.setBoundsChangedFlag();

        return Promise.resolve()
            .then(() => {
                if (g.packageItem.objectType !== ObjectType.Component)
                    return this.constructExtensionAsync(g, buffer);
            })
            .then(() => {
                //@ts-ignore
                g.onConstruct();
            });
    }

    private parseComplete(g: GComponent, buffer: ByteBuffer) {

        g.applyAllControllers();

        g._buildingDisplayList = false;
        g._underConstruct = false;

        //@ts-ignore
        g.buildNativeDisplayList();
        g.setBoundsChangedFlag();
        if (g.packageItem.objectType !== ObjectType.Component)
            this.constructExtension(g, buffer);

        //@ts-ignore
        g.onConstruct();
    }

    protected constructExtension(g: GComponent, buffer: ByteBuffer) { }
    protected async constructExtensionAsync(g: GComponent, buffer: ByteBuffer) { }
}

function createAction(type: number): ControllerAction {
    switch (type) {
        case 0:
            return new PlayTransitionAction();

        case 1:
            return new ChangePageAction();
    }
    return null;
}