
import { GComponent } from "./GComponent";
import { EventDispatch } from "../core";
import { FUIEvent } from "./FUIEvent";
import { ControllerAction } from "./action/ControllerAction";


var _nextPageId: number = 0;

/**
 * 控制器
 */
export class Controller extends EventDispatch {

    /**
     * 当前选中的控制器索引
     */
    private _selectedIndex: number;

    /**
     * 前一次选择的控制器索引
     */
    private _previousIndex: number;
    private _pageIds: Array<string>;
    private _pageNames: Array<string>;


    private _actions?: Array<ControllerAction>;

    public name: string;

    //Controller所属的GComponet
    public parent: GComponent;
    public autoRadioGroupDepth?: boolean;
    public changing?: boolean;

    public constructor() {
        super();
        this._pageIds = [];
        this._pageNames = [];
        this._selectedIndex = -1;
        this._previousIndex = -1;
    }

    public dispose(): void {

    }


    public get selectedIndex(): number {
        return this._selectedIndex;
    }

    /**
     * 选择激活指定的控制器
     */
    public set selectedIndex(value: number) {
        if (this._selectedIndex != value) {
            if (value > this._pageIds.length - 1)
                throw new Error("index out of bounds: " + value);

            this.changing = true;
            this._previousIndex = this._selectedIndex;
            this._selectedIndex = value;
            this.parent.applyController(this);

            this.fire(FUIEvent.STATUS_CHANGED, this);

            this.changing = false;
        }
    }

    public onChanged<TFunction extends (...any: any[]) => void>(callback: TFunction, thisArg?: any): void {
        this.on(FUIEvent.STATUS_CHANGED, callback, thisArg);
    }

    public offChanged<TFunction extends (...any: any[]) => void>(callback: TFunction, thisArg?: any): void {
        this.off(FUIEvent.STATUS_CHANGED, callback, thisArg);
    }

    //功能和设置selectedIndex一样，但不会触发事件
    public setSelectedIndex(value: number): void {
        if (this._selectedIndex != value) {
            if (value > this._pageIds.length - 1)
                throw new Error("index out of bounds: " + value);

            this.changing = true;
            this._previousIndex = this._selectedIndex;
            this._selectedIndex = value;
            this.parent.applyController(this);
            this.changing = false;
        }
    }

    public get previsousIndex(): number {
        return this._previousIndex;
    }

    public get selectedPage(): string {
        if (this._selectedIndex == -1)
            return null;
        else
            return this._pageNames[this._selectedIndex];
    }

    public set selectedPage(val: string) {
        var i: number = this._pageNames.indexOf(val);
        if (i == -1)
            i = 0;
        this.selectedIndex = i;
    }

    //功能和设置selectedPage一样，但不会触发事件
    public setSelectedPage(value: string): void {
        var i: number = this._pageNames.indexOf(value);
        if (i == -1)
            i = 0;
        this.setSelectedIndex(i);
    }

    public get previousPage(): string {
        if (this._previousIndex == -1)
            return null;
        else
            return this._pageNames[this._previousIndex];
    }

    public get pageCount(): number {
        return this._pageIds.length;
    }

    public getPageName(index: number): string {
        return this._pageNames[index];
    }

    public addPage(name?: string): void {
        name = name || "";
        this.addPageAt(name, this._pageIds.length);
    }

    public addPageAt(name?: string, index?: number): void {
        name = name || "";
        var nid: string = "" + (_nextPageId++);
        if (index == null || index == this._pageIds.length) {
            this._pageIds.push(nid);
            this._pageNames.push(name);
        }
        else {
            this._pageIds.splice(index, 0, nid);
            this._pageNames.splice(index, 0, name);
        }
    }

    public removePage(name: string): void {
        var i: number = this._pageNames.indexOf(name);
        if (i != -1) {
            this._pageIds.splice(i, 1);
            this._pageNames.splice(i, 1);
            if (this._selectedIndex >= this._pageIds.length)
                this.selectedIndex = this._selectedIndex - 1;
            else
                this.parent.applyController(this);
        }
    }

    public removePageAt(index: number): void {
        this._pageIds.splice(index, 1);
        this._pageNames.splice(index, 1);
        if (this._selectedIndex >= this._pageIds.length)
            this.selectedIndex = this._selectedIndex - 1;
        else
            this.parent.applyController(this);
    }

    public clearPages(): void {
        this._pageIds.length = 0;
        this._pageNames.length = 0;
        if (this._selectedIndex != -1)
            this.selectedIndex = -1;
        else
            this.parent.applyController(this);
    }

    public hasPage(aName: string): boolean {
        return this._pageNames.indexOf(aName) != -1;
    }

    public getPageIndexById(aId: string): number {
        return this._pageIds.indexOf(aId);
    }

    public getPageIdByName(aName: string): string | null {
        var i: number = this._pageNames.indexOf(aName);
        if (i != -1)
            return this._pageIds[i];
        else
            return null;
    }

    public getPageNameById(aId: string): string | null {
        var i: number = this._pageIds.indexOf(aId);
        if (i != -1)
            return this._pageNames[i];
        else
            return null;
    }

    public getPageId(index: number): string | null {
        return this._pageIds[index];
    }

    public get selectedPageId(): string | null {
        if (this._selectedIndex == -1)
            return null;
        else
            return this._pageIds[this._selectedIndex];
    }

    public set selectedPageId(val: string | null) {
        var i: number = this._pageIds.indexOf(val);
        this.selectedIndex = i;
    }

    public set oppositePageId(val: string | null) {
        var i: number = this._pageIds.indexOf(val);
        if (i > 0)
            this.selectedIndex = 0;
        else if (this._pageIds.length > 1)
            this.selectedIndex = 1;
    }

    public get previousPageId(): string | null {
        if (this._previousIndex == -1)
            return null;
        else
            return this._pageIds[this._previousIndex];
    }

    public runActions(): void {
        if (this._actions) {
            var cnt: number = this._actions.length;
            for (var i: number = 0; i < cnt; i++) {
                this._actions[i].run(this, this.previousPageId, this.selectedPageId);
            }
        }
    }
}




