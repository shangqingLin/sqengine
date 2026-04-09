import { GList } from "../../GList";
import { GTreeNode } from "../../GTreeNode";
import { ByteBuffer } from "../../utils/ByteBuffer";
import GListParse from "./GListParse";

export default class GTreeParase extends GListParse {
    private treeSetupBeforeImpl(g: GList, beginPos: number, buffer: ByteBuffer) {
        buffer.seek(beginPos, 9);

        //@ts-ignore
        g._indent = buffer.readInt();

        //@ts-ignore
        g._clickToExpand = buffer.readByte();
    }

    public override async parseSetupBeforeAsync(g: GList, beginPos: number, buffer: ByteBuffer) {
        return Promise.resolve()
            .then(() => {
                return super.parseSetupBeforeAsync(g, beginPos, buffer);
            })
            .then(() => {
                this.treeSetupBeforeImpl(g, beginPos, buffer);
            })
    }

    protected override async readItemsAsync(g: GList, buffer: ByteBuffer) {
        var cnt: number;
        var i: number;
        var nextPos: number;
        var str: string;
        var isFolder: boolean;
        var lastNode: GTreeNode;
        var level: number;
        var prevLevel: number = 0;

        cnt = buffer.readShort();
        for (i = 0; i < cnt; i++) {
            nextPos = buffer.readShort();
            nextPos += buffer.position;

            str = buffer.readS();
            if (str == null) {
                str = g.defaultItem;
                if (!str) {
                    buffer.position = nextPos;
                    continue;
                }
            }

            isFolder = buffer.readBool();
            level = buffer.readByte();

            var node: GTreeNode = new GTreeNode(isFolder, str);
            node.expanded = true;
            if (i == 0)
                //@ts-ignore
                g._rootNode.addChild(node);
            else {
                if (level > prevLevel)
                    lastNode.addChild(node);
                else if (level < prevLevel) {
                    for (var j: number = level; j <= prevLevel; j++)
                        lastNode = lastNode.parent;
                    lastNode.addChild(node);
                }
                else
                    lastNode.parent.addChild(node);
            }
            lastNode = node;
            prevLevel = level;
            this.setupItem(node.cell, buffer);
            buffer.position = nextPos;
        }
    }

    protected override parseSetupBefore(g: GList, beginPos: number, buffer: ByteBuffer): void {
        super.parseSetupBefore(g, beginPos, buffer);
        this.treeSetupBeforeImpl(g, beginPos, buffer);
    }
}