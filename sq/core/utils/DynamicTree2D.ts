import { Vec2 } from "../math";
import AABB from "../math/AABB"

const NULL_INDEX = -1;

enum RotateType {
    sq_rotateNone,
    sq_rotateBF,
    sq_rotateBG,
    sq_rotateCD,
    sq_rotateCE
};

enum TreeNodeFlags {
    sq_allocatedNode = 0x0001,
    sq_enlargedNode = 0x0002,
    sq_leafNode = 0x0004,
};

class RebuildItem {
    nodeIndex: number;
    childCount: number;

    // Leaf indices
    startIndex: number;
    splitIndex: number;
    endIndex: number;
};

export interface TreeRayCastInput {
    origin: Vec2;
    translation: Vec2;
    maxFraction: number;
};


function SQ_ASSERT(b: boolean) {
    if (!b) throw new Error();
}

interface TreeBin {
    aabb: AABB;
    count: number;
};

interface TreePlane {
    leftAABB: AABB;
    rightAABB: AABB;
    leftCount: number;
    rightCount: number;
};

/*
function SqPartitionSAH(indices: Array<number>, boxes: Array<AABB>): number {
    SQ_ASSERT(boxes.length > 0);

    b2TreeBin bins[B2_BIN_COUNT];
    b2TreePlane planes[B2_BIN_COUNT - 1];

    let center: Vec2 = AABB.Center(boxes[0]);
    let centroidAABB: AABB = new AABB();
    centroidAABB.lowerBound.copy(center);
    centroidAABB.upperBound.copy(center);

    for (let i = 1; i < boxes.length; ++i) {
        AABB.Center(boxes[i], center);
        Vec2.min(centroidAABB.lowerBound, center, centroidAABB.lowerBound);
        Vec2.max(centroidAABB.upperBound, center, centroidAABB.upperBound);
    }

    let d: Vec2 = center.sub2(centroidAABB.upperBound, centroidAABB.lowerBound);

    // Find longest axis
    let axisIndex: number;
    let invD: number;
    if (d.x > d.y) {
        axisIndex = 0;
        invD = d.x;
    }
    else {
        axisIndex = 1;
        invD = d.y;
    }

    invD = invD > 0.0 ? 1.0 / invD : 0.0;

    // Initialize bin bounds and count
    for (let i = 0; i < B2_BIN_COUNT; ++i) {
        bins[i].aabb.lowerBound = (SqVec2){ FLT_MAX, FLT_MAX };
        bins[i].aabb.upperBound = (SqVec2){ -FLT_MAX, -FLT_MAX };
        bins[i].count = 0;
    }

    // Assign boxes to bins and compute bin boxes
    // TODO_ERIN optimize
    float binCount = B2_BIN_COUNT;
    float lowerBoundArray[2] = { centroidAABB.lowerBound.x, centroidAABB.lowerBound.y };
    float minC = lowerBoundArray[axisIndex];
    for (int i = 0; i < count; ++i)
    {
        SqVec2 c = AABB.Center(boxes[i]);
        float cArray[2] = { c.x, c.y };
        int binIndex = (int)(binCount * (cArray[axisIndex] - minC) * invD);
        binIndex = clampInt(binIndex, 0, B2_BIN_COUNT - 1);
        binIndices[i] = binIndex;
        bins[binIndex].count += 1;
        bins[binIndex].aabb = AABB:: Union(bins[binIndex].aabb, boxes[i]);
    }

    let planeCount: number = B2_BIN_COUNT - 1;

    // Prepare all the left planes, candidates for left child
    planes[0].leftCount = bins[0].count;
    planes[0].leftAABB = bins[0].aabb;
    for (int i = 1; i < planeCount; ++i)
    {
        planes[i].leftCount = planes[i - 1].leftCount + bins[i].count;
        planes[i].leftAABB = AABB.Union(planes[i - 1].leftAABB, bins[i].aabb);
    }

    // Prepare all the right planes, candidates for right child
    planes[planeCount - 1].rightCount = bins[planeCount].count;
    planes[planeCount - 1].rightAABB = bins[planeCount].aabb;
    for (int i = planeCount - 2; i >= 0; --i)
    {
        planes[i].rightCount = planes[i + 1].rightCount + bins[i + 1].count;
        planes[i].rightAABB = AABB.Union(planes[i + 1].rightAABB, bins[i + 1].aabb);
    }

    // Find best split to minimize SAH
    float minCost = FLT_MAX;
    let bestPlane:number = 0;
    for (let i = 0; i < planeCount; ++i)
    {
        float leftArea = AABB.Perimeter(planes[i].leftAABB);
        float rightArea = AABB.Perimeter(planes[i].rightAABB);
        int leftCount = planes[i].leftCount;
        int rightCount = planes[i].rightCount;

        float cost = leftCount * leftArea + rightCount * rightArea;
        if (cost < minCost) {
            bestPlane = i;
            minCost = cost;
        }
    }

    // Partition node indices and boxes using the Hoare partition scheme
    // https://en.wikipedia.org/wiki/Quicksort
    // https://nicholasvadivelu.com/2021/01/11/array-partition/
    let i1: number = 0, i2: number = count;
    while (i1 < i2) {
        while (i1 < i2 && binIndices[i1] < bestPlane) {
            i1 += 1;
        };

        while (i1 < i2 && binIndices[i2 - 1] >= bestPlane) {
            i2 -= 1;
        };

        if (i1 < i2) {
            // Swap indices
            {
                int temp = indices[i1];
                indices[i1] = indices[i2 - 1];
                indices[i2 - 1] = temp;
            }

            // Swap boxes
            {
                let temp: AABB = boxes[i1];
                boxes[i1] = boxes[i2 - 1];
                boxes[i2 - 1] = temp;
            }

            i1 += 1;
            i2 -= 1;
        }
    }

    SQ_ASSERT(i1 == i2);

    if (i1 > 0 && i1 < count) {
        return i1;
    }
    else {
        return count / 2;
    }
}
*/

class DynamicTreeNode {

    public aabb: AABB = new AABB();
    public height: number = 0;
    public child1: number = NULL_INDEX;
    public child2: number = NULL_INDEX;
    public parent: number = NULL_INDEX;
    public next: number = NULL_INDEX;
    public flags: number = TreeNodeFlags.sq_allocatedNode;
    public userData: any;

    reset() {
        this.height = 0;
        this.flags = 0;
        this.child1 = NULL_INDEX;
        this.child2 = NULL_INDEX;
        this.parent = NULL_INDEX;
        this.flags = TreeNodeFlags.sq_allocatedNode;
    }

    isLeaf(): boolean {
        return (this.flags & TreeNodeFlags.sq_leafNode) !== 0;
    }

    isAllocated(): boolean {
        return (this.flags & TreeNodeFlags.sq_allocatedNode) !== 0;
    }

}

export class DynamicTree {

    private proxyCount: number = 0;
    private root: number = NULL_INDEX;
    private nodeCapacity = 16;
    private nodeCount = 0;
    private nodes: Array<DynamicTreeNode>;
    private freeList: number = 0;
    private rebuildCapacity = 0;

    constructor() {
        this.nodes = new Array(this.nodeCapacity);
        for (let i = 0; i < this.nodeCapacity; ++i) {
            this.nodes[i] = new DynamicTreeNode();
        }
        this.buildFreeList(0);
    }

    private buildFreeList(freeBeginIndex: number) {
        for (let i = freeBeginIndex; i < this.nodeCapacity - 1; ++i) {
            this.nodes[i].next = i + 1;
        }
        this.nodes[this.nodeCapacity - 1].next = NULL_INDEX;
        this.freeList = freeBeginIndex;
    }

    private createNode(): number {
        if (this.freeList === NULL_INDEX) {
            if (this.nodeCount !== this.nodeCapacity) throw new Error();
            let oldCapacity: number = this.nodeCapacity;
            this.nodeCapacity += oldCapacity >> 1;
            for (let i = oldCapacity; i < this.nodeCapacity; ++i) {
                this.nodes.push(new DynamicTreeNode());
            }
            this.buildFreeList(this.nodeCount);
        }
        let nodeIndex = this.freeList;
        let node: DynamicTreeNode = this.nodes[nodeIndex];
        this.freeList = node.next;
        node.reset();
        ++this.nodeCount;
        return nodeIndex;
    }

    private freeNode(nodeId: number) {
        if (!(0 <= nodeId && nodeId < this.nodeCapacity)) throw new Error();
        if (!(0 < this.nodeCount)) throw new Error();
        this.nodes[nodeId].next = this.freeList;
        this.nodes[nodeId].flags = 0;
        this.freeList = nodeId;
        --this.nodeCount;
    }

    public createProxy(aabb: AABB, userData: any): number {

        let proxyId = this.createNode();
        let node = this.nodes[proxyId];

        // printf("createProxy %d userData %p\n", proxyId, userData);

        node.aabb.copyFrom(aabb);
        node.userData = userData;
        node.height = 0;
        node.flags = TreeNodeFlags.sq_allocatedNode | TreeNodeFlags.sq_leafNode;

        let shouldRotate = true;
        this.insertLeaf(proxyId, shouldRotate);
        ++this.proxyCount;
        return proxyId;
    }

    public destroyProxy(proxyId: number): void {
        if (this.nodes[proxyId].isLeaf()) throw new Error();
        this.removeLeaf(proxyId);
        this.freeNode(proxyId);
        --this.proxyCount;
    }

    private removeLeaf(leaf: number): void {
        if (leaf === this.root) {
            this.root = NULL_INDEX;
            return;
        }

        let parentIndex: number = this.nodes[leaf].parent;
        let parent: DynamicTreeNode = this.nodes[parentIndex];
        let grandParentIndex: number = parent.parent;
        let grandParent: DynamicTreeNode = this.nodes[grandParentIndex];
        let sibling: number;
        if (parent.child1 === leaf) {
            sibling = parent.child2;
        }
        else {
            sibling = parent.child1;
        }

        if (grandParent) {

            if (grandParent.child1 == parentIndex) {
                grandParent.child1 = sibling;
            }
            else {
                grandParent.child2 = sibling;
            }

            this.nodes[sibling].parent = grandParentIndex;
            this.freeNode(parentIndex);

            let index: number = grandParentIndex;
            while (index !== NULL_INDEX) {
                let node: DynamicTreeNode = this.nodes[index];
                let child1: DynamicTreeNode = this.nodes[node.child1];
                let child2: DynamicTreeNode = this.nodes[node.child2];
                AABB.Union(child1.aabb, child2.aabb, node.aabb);
                node.height = 1 + Math.max(child1.height, child2.height);
                index = node.parent;
            }
        }
        else {
            this.root = sibling;
            this.nodes[sibling].parent = NULL_INDEX;
            this.freeNode(parentIndex);
        }
    }

    private insertLeaf(leaf: number, shouldRotate: boolean): void {
        if (this.root == NULL_INDEX) {
            this.root = leaf;
            this.nodes[this.root].parent = NULL_INDEX;
            return;
        }
        let nodes: Array<DynamicTreeNode> = this.nodes;
        let leafAABB: AABB = nodes[leaf].aabb;
        let sibling = this.findBestSibling(leafAABB);
        let oldParent = nodes[sibling].parent;
        let newParent = this.createNode();
        let newNode = nodes[newParent];

        newNode.parent = oldParent;
        newNode.userData = null;
        AABB.Union(leafAABB, nodes[sibling].aabb, newNode.aabb);

        newNode.height = nodes[sibling].height + 1;

        if (oldParent !== NULL_INDEX) {
            // The sibling was not the root.
            if (nodes[oldParent].child1 == sibling) {
                nodes[oldParent].child1 = newParent;
            }
            else {
                nodes[oldParent].child2 = newParent;
            }

            newNode.child1 = sibling;
            newNode.child2 = leaf;
            nodes[sibling].parent = newParent;
            nodes[leaf].parent = newParent;
        }
        else {
            // The sibling was the root.
            newNode.child1 = sibling;
            newNode.child2 = leaf;
            nodes[sibling].parent = newParent;
            nodes[leaf].parent = newParent;
            this.root = newParent;
        }

        let index: number = nodes[leaf].parent;
        while (index !== NULL_INDEX) {
            let child1: number = nodes[index].child1;
            let child2: number = nodes[index].child2;

            if (child1 === NULL_INDEX || child2 === NULL_INDEX) throw new Error();

            AABB.Union(nodes[child1].aabb, nodes[child2].aabb, nodes[index].aabb);
            nodes[index].height = 1 + Math.max(nodes[child1].height, nodes[child2].height);
            nodes[index].flags |= (nodes[child1].flags | nodes[child2].flags) & TreeNodeFlags.sq_enlargedNode;

            if (shouldRotate) {
                this.rotateNodes(index);
            }

            index = nodes[index].parent;
        }
    }

    private findBestSibling(boxD: AABB): number {
        let centerD: Vec2 = AABB.Center(boxD);
        let areaD: number = AABB.Perimeter(boxD);

        let nodes: Array<DynamicTreeNode> = this.nodes;
        let rootIndex: number = this.root;
        let rootBox: AABB = nodes[rootIndex].aabb;

        let areaBase: number = AABB.Perimeter(rootBox);
        let directCost: number = AABB.Perimeter(AABB.Union(rootBox, boxD, AABB.TEMP));

        let inheritedCost: number = 0.0;
        let bestSibling: number = rootIndex;
        let bestCost: number = directCost;

        let index: number = rootIndex;
        while (nodes[index].height > 0) {

            let child1: number = nodes[index].child1;
            let child2: number = nodes[index].child2;

            SQ_ASSERT(child1 !== NULL_INDEX);

            // Cost of creating a new parent for this node and the new leaf
            let cost: number = directCost + inheritedCost;

            // Sometimes there are multiple identical costs within tolerance.
            // This breaks the ties using the centroid distance.
            if (cost < bestCost) {
                bestSibling = index;
                bestCost = cost;
            }

            // Inheritance cost seen by children
            inheritedCost += directCost - areaBase;

            let leaf1: boolean = nodes[child1].height === 0;
            let leaf2: boolean = nodes[child2].height === 0;

            // Cost of descending into child 1
            let lowerCost1: number = Number.MAX_VALUE;
            let box1: AABB = nodes[child1].aabb;
            let directCost1: number = AABB.Perimeter(AABB.Union(box1, boxD, AABB.TEMP));
            let area1: number = 0.0;
            if (leaf1) {
                // Child 1 is a leaf
                // Cost of creating new node and increasing area of node P
                let cost1: number = directCost1 + inheritedCost;

                // Need this here due to while condition above
                if (cost1 < bestCost) {
                    bestSibling = child1;
                    bestCost = cost1;
                }
            }
            else {
                // Child 1 is an internal node
                area1 = AABB.Perimeter(box1);

                // Lower bound cost of inserting under child 1. The minimum accounts for two possibilities:
                // 1. Child1 could be the sibling with cost1 = inheritedCost + directCost1
                // 2. A descendent of child1 could be the sibling with the lower bound cost of
                //       cost1 = inheritedCost + (directCost1 - area1) + areaD
                // This minimum here leads to the minimum of these two costs.
                lowerCost1 = inheritedCost + directCost1 + Math.min(areaD - area1, 0.0);
            }

            // Cost of descending into child 2
            let lowerCost2: number = Number.MAX_VALUE;
            let box2: AABB = nodes[child2].aabb;
            let directCost2: number = AABB.Perimeter(AABB.Union(box2, boxD, AABB.TEMP));
            let area2: number = 0.0;
            if (leaf2) {
                let cost2 = directCost2 + inheritedCost;
                if (cost2 < bestCost) {
                    bestSibling = child2;
                    bestCost = cost2;
                }
            }
            else {
                area2 = AABB.Perimeter(box2);
                lowerCost2 = inheritedCost + directCost2 + Math.min(areaD - area2, 0.0);
            }

            if (leaf1 && leaf2) {
                break;
            }

            // Can the cost possibly be decreased?
            if (bestCost <= lowerCost1 && bestCost <= lowerCost2) {
                break;
            }

            if (lowerCost1 === lowerCost2 && leaf1 === false) {
                if (lowerCost1 >= Number.MAX_VALUE || lowerCost2 >= Number.MAX_VALUE) throw new Error();

                // No clear choice based on lower bound surface area. This can happen when both
                // children fully contain D. Fall back to node distance.
                let d1: Vec2 = Vec2.temp1Vec2.sub2(AABB.Center(box1, Vec2.temp1Vec2), centerD);
                let d2: Vec2 = Vec2.temp2Vec2.sub2(AABB.Center(box2, Vec2.temp2Vec2), centerD);
                lowerCost1 = d1.lengthSq();
                lowerCost2 = d2.lengthSq();
            }

            // 哪个子节点的cost越小，则就取哪个
            if (lowerCost1 < lowerCost2 && leaf1 === false) {
                index = child1;
                areaBase = area1;
                directCost = directCost1;
            }
            else {
                index = child2;
                areaBase = area2;
                directCost = directCost2;
            }

            SQ_ASSERT(nodes[index].height > 0);
        }

        return bestSibling;
    }

    private rotateNodes(iA: number): void {
        if (iA === NULL_INDEX) throw new Error();

        let nodes: Array<DynamicTreeNode> = this.nodes;
        let A: DynamicTreeNode = nodes[iA];
        if (A.height < 2) {
            return;
        }

        let iB: number = A.child1;
        let iC: number = A.child2;
        // SQ_ASSERT(0 <= iB && iB < nodeCapacity);
        // SQ_ASSERT(0 <= iC && iC < nodeCapacity);

        let B: DynamicTreeNode = nodes[iB];
        let C: DynamicTreeNode = nodes[iC];

        if (B.height === 0) {
            // B is a leaf and C is internal
            // SQ_ASSERT(C.height > 0);

            let iF: number = C.child1;
            let iG: number = C.child2;
            let F: DynamicTreeNode = nodes[iF];
            let G: DynamicTreeNode = nodes[iG];
            // SQ_ASSERT(0 <= iF && iF < nodeCapacity);
            // SQ_ASSERT(0 <= iG && iG < nodeCapacity);

            // Base cost
            let costBase: number = AABB.Perimeter(C.aabb);

            // Cost of swapping B and F
            let aabbBG: AABB = AABB.Union(B.aabb, G.aabb, AABB.TEMP);
            let costBF: number = AABB.Perimeter(aabbBG);

            // Cost of swapping B and G
            let aabbBF: AABB = AABB.Union(B.aabb, F.aabb, AABB.TEMP2);
            let costBG: number = AABB.Perimeter(aabbBF);

            if (costBase < costBF && costBase < costBG) {
                // Rotation does not improve cost
                return;
            }

            if (costBF < costBG) {
                // Swap B and F
                A.child1 = iF;
                C.child1 = iB;

                B.parent = iC;
                F.parent = iA;

                C.aabb.copyFrom(aabbBG);

                C.height = 1 + Math.max(B.height, G.height);
                A.height = 1 + Math.max(C.height, F.height);
                C.flags |= (B.flags | G.flags) & TreeNodeFlags.sq_enlargedNode;
                A.flags |= (C.flags | F.flags) & TreeNodeFlags.sq_enlargedNode;
            }
            else {
                // Swap B and G
                A.child1 = iG;
                C.child2 = iB;

                B.parent = iC;
                G.parent = iA;

                C.aabb.copyFrom(aabbBF);

                C.height = 1 + Math.max(B.height, F.height);
                A.height = 1 + Math.max(C.height, G.height);
                C.flags |= (B.flags | F.flags) & TreeNodeFlags.sq_enlargedNode;
                A.flags |= (C.flags | G.flags) & TreeNodeFlags.sq_enlargedNode;
            }
        }
        else if (C.height === 0) {
            // C is a leaf and B is internal
            // SQ_ASSERT(B.height > 0);

            let iD: number = B.child1;
            let iE: number = B.child2;
            let D: DynamicTreeNode = nodes[iD];
            let E: DynamicTreeNode = nodes[iE];
            // SQ_ASSERT(0 <= iD && iD < nodeCapacity);
            // SQ_ASSERT(0 <= iE && iE < nodeCapacity);

            // Base cost
            let costBase: number = AABB.Perimeter(B.aabb);

            // Cost of swapping C and D
            let aabbCE: AABB = AABB.Union(C.aabb, E.aabb, AABB.TEMP);
            let costCD: number = AABB.Perimeter(aabbCE);

            // Cost of swapping C and E
            let aabbCD: AABB = AABB.Union(C.aabb, D.aabb, AABB.TEMP2);
            let costCE: number = AABB.Perimeter(aabbCD);

            if (costBase < costCD && costBase < costCE) {
                // Rotation does not improve cost
                return;
            }

            if (costCD < costCE) {
                // Swap C and D
                A.child2 = iD;
                B.child1 = iC;

                C.parent = iB;
                D.parent = iA;

                B.aabb.copyFrom(aabbCE);

                B.height = 1 + Math.max(C.height, E.height);
                A.height = 1 + Math.max(B.height, D.height);
                B.flags |= (C.flags | E.flags) & TreeNodeFlags.sq_enlargedNode;
                A.flags |= (B.flags | D.flags) & TreeNodeFlags.sq_enlargedNode;
            }
            else {
                // Swap C and E
                A.child2 = iE;
                B.child2 = iC;

                C.parent = iB;
                E.parent = iA;

                B.aabb.copyFrom(aabbCD);
                B.height = 1 + Math.max(C.height, D.height);
                A.height = 1 + Math.max(B.height, E.height);
                B.flags |= (C.flags | D.flags) & TreeNodeFlags.sq_enlargedNode;
                A.flags |= (B.flags | E.flags) & TreeNodeFlags.sq_enlargedNode;
            }
        }
        else {
            let iD: number = B.child1;
            let iE: number = B.child2;
            let iF: number = C.child1;
            let iG: number = C.child2;

            let D: DynamicTreeNode = nodes[iD];
            let E: DynamicTreeNode = nodes[iE];
            let F: DynamicTreeNode = nodes[iF];
            let G: DynamicTreeNode = nodes[iG];

            // SQ_ASSERT(0 <= iD && iD < nodeCapacity);
            // SQ_ASSERT(0 <= iE && iE < nodeCapacity);
            // SQ_ASSERT(0 <= iF && iF < nodeCapacity);
            // SQ_ASSERT(0 <= iG && iG < nodeCapacity);

            // Base cost
            let areaB: number = AABB.Perimeter(B.aabb);
            let areaC: number = AABB.Perimeter(C.aabb);
            let costBase = areaB + areaC;
            let bestRotation: RotateType = RotateType.sq_rotateNone;
            let bestCost: number = costBase;

            // Cost of swapping B and F
            let aabbBG: AABB = AABB.Union(B.aabb, G.aabb, AABB.TEMP);
            let costBF: number = areaB + AABB.Perimeter(aabbBG);
            if (costBF < bestCost) {
                bestRotation = RotateType.sq_rotateBF;
                bestCost = costBF;
            }

            // Cost of swapping B and G
            let aabbBF: AABB = AABB.Union(B.aabb, F.aabb, AABB.TEMP2);
            let costBG: number = areaB + AABB.Perimeter(aabbBF);
            if (costBG < bestCost) {
                bestRotation = RotateType.sq_rotateBG;
                bestCost = costBG;
            }

            // Cost of swapping C and D
            let aabbCE = AABB.Union(C.aabb, E.aabb);
            let costCD = areaC + AABB.Perimeter(aabbCE);
            if (costCD < bestCost) {
                bestRotation = RotateType.sq_rotateCD;
                bestCost = costCD;
            }

            // Cost of swapping C and E
            let aabbCD: AABB = AABB.Union(C.aabb, D.aabb);
            let costCE: number = areaC + AABB.Perimeter(aabbCD);
            if (costCE < bestCost) {
                bestRotation = RotateType.sq_rotateCE;
                // bestCost = costCE;
            }

            switch (bestRotation) {
                case RotateType.sq_rotateNone:
                    break;

                case RotateType.sq_rotateBF:
                    A.child1 = iF;
                    C.child1 = iB;

                    B.parent = iC;
                    F.parent = iA;

                    C.aabb.copyFrom(aabbBG);
                    C.height = 1 + Math.max(B.height, G.height);
                    A.height = 1 + Math.max(C.height, F.height);
                    C.flags |= (B.flags | G.flags) & TreeNodeFlags.sq_enlargedNode;
                    A.flags |= (C.flags | F.flags) & TreeNodeFlags.sq_enlargedNode;
                    break;

                case RotateType.sq_rotateBG:
                    A.child1 = iG;
                    C.child2 = iB;

                    B.parent = iC;
                    G.parent = iA;

                    C.aabb.copyFrom(aabbBF);
                    C.height = 1 + Math.max(B.height, F.height);
                    A.height = 1 + Math.max(C.height, G.height);
                    C.flags |= (B.flags | F.flags) & TreeNodeFlags.sq_enlargedNode;
                    A.flags |= (C.flags | G.flags) & TreeNodeFlags.sq_enlargedNode;
                    break;

                case RotateType.sq_rotateCD:
                    A.child2 = iD;
                    B.child1 = iC;

                    C.parent = iB;
                    D.parent = iA;

                    B.aabb = aabbCE;
                    B.height = 1 + Math.max(C.height, E.height);
                    A.height = 1 + Math.max(B.height, D.height);
                    B.flags |= (C.flags | E.flags) & TreeNodeFlags.sq_enlargedNode;
                    A.flags |= (B.flags | D.flags) & TreeNodeFlags.sq_enlargedNode;
                    break;

                case RotateType.sq_rotateCE:
                    A.child2 = iE;
                    B.child2 = iC;

                    C.parent = iB;
                    E.parent = iA;

                    B.aabb = aabbCD;
                    B.height = 1 + Math.max(C.height, D.height);
                    A.height = 1 + Math.max(B.height, E.height);
                    B.flags |= (C.flags | D.flags) & TreeNodeFlags.sq_enlargedNode;
                    A.flags |= (B.flags | E.flags) & TreeNodeFlags.sq_enlargedNode;
                    break;

                default:
                    throw new Error();
                    break;
            }
        }
    }


    public moveProxy(proxyId: number, aabb: AABB): void {
        this.removeLeaf(proxyId);
        this.nodes[proxyId].aabb.copyFrom(aabb);
        this.insertLeaf(proxyId, false);
    }


    public enlargeProxy(proxyId: number, aabb: AABB): void {

        let nodes: Array<DynamicTreeNode> = this.nodes;
        nodes[proxyId].aabb.copyFrom(aabb);

        //更新父节点的AABB
        let parentIndex: number = nodes[proxyId].parent;
        while (parentIndex !== NULL_INDEX) {
            let changed: boolean = AABB.enlargeAABB(nodes[parentIndex].aabb, aabb);
            nodes[parentIndex].flags |= TreeNodeFlags.sq_enlargedNode;
            parentIndex = nodes[parentIndex].parent;
            if (changed === false) {
                break;
            }
        }
        while (parentIndex !== NULL_INDEX) {
            if (nodes[parentIndex].flags & TreeNodeFlags.sq_enlargedNode) {
                // early out because this ancestor was previously ascended and marked as enlarged
                break;
            }

            nodes[parentIndex].flags |= TreeNodeFlags.sq_enlargedNode;
            parentIndex = nodes[parentIndex].parent;
        }
    }

    getHeight(): number {
        if (this.root === NULL_INDEX) {
            return 0;
        }
        return this.nodes[this.root].height;
    }

    getAreaRatio(): number {
        if (this.root === NULL_INDEX) {
            return 0.0;
        }

        let nodes: Array<DynamicTreeNode> = this.nodes;
        let root: DynamicTreeNode = nodes[this.root];
        let rootArea: number = AABB.Perimeter(root.aabb);

        let totalArea = 0.0;
        for (let i = 0; i < this.nodeCapacity; ++i) {
            let node: DynamicTreeNode = nodes[i];
            if (node.isAllocated() === false || node.isLeaf() || i === this.root) {
                continue;
            }
            totalArea += AABB.Perimeter(node.aabb);
        }

        return totalArea / rootArea;
    }

    getRootBounds(): AABB {
        if (this.root != NULL_INDEX) {
            return this.nodes[this.root].aabb;
        }
        return null;
    }

    /**
     * 根据参数aabb查询在aabb区域内的叶节点，如果查询到则执行callback回调通知外部
     * @param context 外部自定义的数据，作为callbak的参数传递
     */
    public Query(aabb: AABB, callback: (nodeId: number, userData: any) => boolean) {
        if (this.nodeCount == 0) {
            return;
        }

        let SQ_TREE_STACK_SIZE = 1024;
        let nodes: Array<DynamicTreeNode> = this.nodes;
        let stack = new Array(SQ_TREE_STACK_SIZE);
        let stackCount = 0;
        stack[stackCount++] = this.root;

        while (stackCount > 0) {
            let nodeId = stack[--stackCount];

            SQ_ASSERT(nodeId !== NULL_INDEX);

            let node: DynamicTreeNode = nodes[nodeId];
            if (AABB.Overlaps(node.aabb, aabb)) {
                if (node.isLeaf()) {
                    let proceed: boolean = callback(nodeId, node.userData);
                    if (proceed == false) {
                        return;
                    }
                }
                else {
                    if (stackCount < SQ_TREE_STACK_SIZE - 1) {
                        stack[stackCount++] = node.child1;
                        stack[stackCount++] = node.child2;
                    }
                    else {
                        // SQ_ASSERT(stackCount < SQ_TREE_STACK_SIZE - 1);
                    }
                }
            }
        }
    }


    public RayCast(input: TreeRayCastInput, callback: (output: TreeRayCastInput, nodeId: number, userData: any) => number) {
        if (this.nodeCount == 0) {
            return;
        }

        let p1: Vec2 = input.origin;
        let d: Vec2 = input.translation;

        let v: Vec2 = new Vec2();
        let abs_v: Vec2 = new Vec2();

        v.copy(d);
        v.normalize();
        v.crossSV(1.0);

        abs_v.copy(v);
        abs_v.abs();

        // Separating axis for segment (Gino, p80).
        // |dot(v, p1 - c)| > dot(|v|, h)

        let maxFraction: number = input.maxFraction;

        let p2: Vec2 = new Vec2();
        Vec2.mulAdd(p1, maxFraction, d, p2);

        // Build a bounding box for the segment.
        let segmentAABB: AABB = AABB.TEMP;
        Vec2.min(p1, p2, segmentAABB.lowerBound);
        Vec2.max(p1, p2, segmentAABB.upperBound);

        let SQ_TREE_STACK_SIZE = 1024;
        let stack: Array<number> = new Array(SQ_TREE_STACK_SIZE);
        let stackCount = 0;
        stack[stackCount++] = this.root;

        let subInput: TreeRayCastInput = {
            origin: new Vec2(),
            translation: new Vec2()
        } as TreeRayCastInput;
        subInput.maxFraction = input.maxFraction;
        subInput.origin.copy(input.origin);
        subInput.translation.copy(input.translation);

        while (stackCount > 0) {
            let nodeId = stack[--stackCount];
            if (nodeId === NULL_INDEX) {
                // todo is this possible?
                // SQ_ASSERT(false);
                continue;
            }

            let node: DynamicTreeNode = this.nodes[nodeId];
            let nodeAABB = node.aabb;
            if (AABB.Overlaps(nodeAABB, segmentAABB) === false) {
                continue;
            }

            // Separating axis for segment (Gino, p80).
            // |dot(v, p1 - c)| > dot(|v|, h)
            // radius extension is added to the node in this case
            let c: Vec2 = AABB.Center(nodeAABB, Vec2.temp1Vec2);
            let h: Vec2 = AABB.Extents(nodeAABB, Vec2.temp2Vec2);
            let term1: number = Math.abs(v.dot(Vec2.temp1Vec2.sub2(p1, c)));
            let term2: number = abs_v.dot(h);
            if (term2 < term1) {
                continue;
            }

            if (node.isLeaf()) {
                subInput.maxFraction = maxFraction;

                let value = callback(subInput, nodeId, node.userData);

                if (value == 0) {
                    return;
                }

                if (0.0 < value && value <= maxFraction) {
                    // Update segment bounding box.
                    maxFraction = value;
                    p2 = Vec2.mulAdd(p1, maxFraction, d);
                    Vec2.min(p1, p2, segmentAABB.lowerBound);
                    Vec2.max(p1, p2, segmentAABB.upperBound);
                }
            }
            else {
                if (stackCount < SQ_TREE_STACK_SIZE - 1) {
                    let c1: Vec2 = AABB.Center(this.nodes[node.child1].aabb);
                    let c2: Vec2 = AABB.Center(this.nodes[node.child2].aabb);
                    if (Vec2.distanceSquared(c1, p1) < Vec2.distanceSquared(c2, p1)) {
                        stack[stackCount++] = node.child2;
                        stack[stackCount++] = node.child1;
                    }
                    else {
                        stack[stackCount++] = node.child1;
                        stack[stackCount++] = node.child2;
                    }
                }
                else {
                    //SQ_ASSERT(stackCount < SQ_TREE_STACK_SIZE - 1);
                }
            }
        }
    }

    /*
    rebuild(fullBuild: boolean): number {
        if (this.proxyCount == 0) {
            return;
        }

        // Ensure capacity for rebuild space
        if (this.proxyCount > this.rebuildCapacity) {
            let newCapacity: number = this.proxyCount + this.proxyCount / 2;

            SqFree(leafIndices, rebuildCapacity * sizeof(int));

            leafIndices = (int *)SqAlloc(newCapacity * sizeof(int));


            SqFree(leafBoxes, rebuildCapacity * sizeof(AABB));
            leafBoxes = SqAlloc(newCapacity * sizeof(AABB));
            SqFree(binIndices, rebuildCapacity * sizeof(int));
            binIndices = SqAlloc(newCapacity * sizeof(int));
            #endif
            rebuildCapacity = newCapacity;
        }

        let nodes: Array<DynamicTreeNode> = this.nodes;
        let SQ_TREE_STACK_SIZE = 1024;
        let leafCount: number = 0;
        let stack = new Array(SQ_TREE_STACK_SIZE);
        let stackCount: number = 0;

        let nodeIndex: number = this.root;
        let node = nodes[nodeIndex];

        // These are the nodes that get sorted to rebuild the tree.
        // I'm using indices because the node pool may grow during the build.
        int * leafIndices = this.leafIndices;

        #if B2_TREE_HEURISTIC == 0
        SqVec2 * leafCenters = this.leafCenters;
        #else
        AABB * leafBoxes = this.leafBoxes;
        #endif

        // Gather all proxy nodes that have grown and all internal nodes that haven't grown. Both are
        // considered leaves in the tree rebuild.
        // Free all internal nodes that have grown.
        // todo use a node growth metric instead of simply enlarged to reduce rebuild size and frequency
        // this should be weighed against B2_AABB_MARGIN
        while (true) {
            if (node.height === 0 || ((node.flags & TreeNodeFlags.sq_enlargedNode) === 0 && fullBuild === false)) {
                leafIndices[leafCount] = nodeIndex;
                leafBoxes[leafCount] = node.aabb;
                leafCount += 1;

                // Detach
                node.parent = NULL_INDEX;
            }
            else {
                let doomedNodeIndex: number = nodeIndex;

                // Handle children
                nodeIndex = node.child1;

                if (stackCount < SQ_TREE_STACK_SIZE) {
                    stack[stackCount++] = node.child2;
                }
                else {
                    // SQ_ASSERT(stackCount < SQ_TREE_STACK_SIZE);
                }

                node = nodes[nodeIndex];

                // Remove doomed node
                this.freeNode(doomedNodeIndex);

                continue;
            }

            if (stackCount === 0) {
                break;
            }

            nodeIndex = stack[--stackCount];
            node = nodes[nodeIndex];
        }

        this.root = this.buildTree(leafCount);
        // Validate();
        return leafCount;
    }

    private buildTree(leafCount: number): number {
        if (leafCount == 1) {
            this.nodes[leafIndices[0]].parent = NULL_INDEX;
            return leafIndices[0];
        }

        #if B2_TREE_HEURISTIC == 0
        SqVec2 * leafCenters = this.leafCenters;
        #else
        AABB * leafBoxes = this.leafBoxes;
        int * binIndices = this.binIndices;
        #endif


        let nodes: Array<DynamicTreeNode> = this.nodes;
        let stack: Array<RebuildItem> = new Array(SQ_TREE_STACK_SIZE);
        let top: number = 0;
        stack[0].nodeIndex = this.createNode();
        stack[0].childCount = -1;
        stack[0].startIndex = 0;
        stack[0].endIndex = leafCount;
        stack[0].splitIndex = SqPartitionSAH(leafIndices, binIndices, leafBoxes, leafCount);
        while (true) {
            let item: RebuildItem = stack + top;

            item.childCount += 1;

            if (item.childCount == 2) {
                // This internal node has both children established

                if (top == 0) {
                    // all done
                    break;
                }

                let parentItem: RebuildItem = stack + (top - 1);
                let parentNode: DynamicTreeNode = nodes[parentItem.nodeIndex];

                if (parentItem.childCount == 0) {
                    SQ_ASSERT(parentNode.child1 == NULL_INDEX);
                    parentNode.child1 = item.nodeIndex;
                }
                else {
                    SQ_ASSERT(parentItem.childCount === 1);
                    SQ_ASSERT(parentNode.child2 === NULL_INDEX);
                    parentNode.child2 = item.nodeIndex;
                }

                let node: DynamicTreeNode = nodes[item.nodeIndex];

                SQ_ASSERT(node.parent === NULL_INDEX);
                node.parent = parentItem.nodeIndex;

                SQ_ASSERT(node.child1 != NULL_INDEX);
                SQ_ASSERT(node.child2 != NULL_INDEX);
                let child1: DynamicTreeNode = nodes[node.child1];
                let child2: DynamicTreeNode = nodes[node.child2];

                AABB.Union(child1.aabb, child2.aabb, node.aabb);
                node.height = 1 + Math.max(child1.height, child2.height);
                node.categoryBits = child1.categoryBits | child2.categoryBits;

                // Pop stack
                top -= 1;
            }
            else {
                let startIndex: number, endIndex: number;
                if (item.childCount == 0) {
                    startIndex = item.startIndex;
                    endIndex = item.splitIndex;
                }
                else {
                    SQ_ASSERT(item.childCount === 1);
                    startIndex = item.splitIndex;
                    endIndex = item.endIndex;
                }

                let count = endIndex - startIndex;
                if (count == 1) {
                    let childIndex: number = leafIndices[startIndex];
                    let node: DynamicTreeNode = nodes[item.nodeIndex];

                    if (item.childCount === 0) {
                        SQ_ASSERT(node.child1 === NULL_INDEX);
                        node.child1 = childIndex;
                    }
                    else {
                        SQ_ASSERT(item.childCount === 1);
                        SQ_ASSERT(node.child2 == NULL_INDEX);
                        node.child2 = childIndex;
                    }

                    let childNode = nodes + childIndex;
                    SQ_ASSERT(childNode.parent === NULL_INDEX);
                    childNode.parent = item.nodeIndex;
                }
                else {
                    SQ_ASSERT(count > 0);
                    SQ_ASSERT(top < SQ_TREE_STACK_SIZE);

                    top += 1;
                    let newItem: RebuildItem = stack + top;
                    newItem.nodeIndex = this.createNode();
                    newItem.childCount = -1;
                    newItem.startIndex = startIndex;
                    newItem.endIndex = endIndex;
                    newItem.splitIndex = SqPartitionSAH(leafIndices + startIndex, binIndices + startIndex, leafBoxes + startIndex, count);
                    newItem.splitIndex += startIndex;
                }
            }
        }

        let rootNode: DynamicTreeNode = nodes[stack[0].nodeIndex];
        // SQ_ASSERT(rootNode.parent === NULL_INDEX);
        // SQ_ASSERT(rootNode.children.child1 !== NULL_INDEX);
        // SQ_ASSERT(rootNode.children.child2 !== NULL_INDEX);

        let child1: DynamicTreeNode = nodes[rootNode.child1];
        let child2: DynamicTreeNode = nodes[rootNode.child2];

        AABB.Union(child1.aabb, child2.aabb, rootNode.aabb);
        rootNode.height = 1 + Math.max(child1.height, child2.height);
        rootNode.categoryBits = child1.categoryBits | child2.categoryBits;

        return stack[0].nodeIndex;
    }
    */

    getAABB(proxyId: number): AABB {
        return this.nodes[proxyId].aabb;
    }

    getUserData(proxyId: number): any {
        return this.nodes[proxyId].userData;
    }

    /* debug:start */
    print() {

        if (this.root === NULL_INDEX) return console.log("tree empty");

        let layers = [];
        layers.push([this.nodes[this.root]]);

        let index = 0;
        while (true) {
            let layerItems = layers[index];
            if (!layerItems) break;

            let ll: Array<any> = [];
            for (let i = 0; i < layerItems.length; ++i) {
                let itemNode: DynamicTreeNode = layerItems[i];

                if (itemNode.child1 !== NULL_INDEX) {
                    ll.push(this.nodes[itemNode.child1]);
                }

                if (itemNode.child2 !== NULL_INDEX) {
                    ll.push(this.nodes[itemNode.child2]);
                }
            }

            if (ll.length > 0) {
                layers.push(ll);
            }
            ++index;
        }

        let layerCount = layers.length;

        // 二叉树每一层节点的个数：2^(n-1) 第一层n=1，第二层n=2
        //计算最后一层的节点个数，从而得到整条树的长度
        let lastLayerNumNode = Math.pow(2, layerCount - 1);
        let w = lastLayerNumNode;

        let str = "";
        for (let i = 0; i < layerCount; ++i) {
            let layer = layers[i];
            let needW = Math.pow(2, i);
            let begin = (w - needW) / 2;

            for (let bb = 0; bb < begin; ++bb) {
                str += " ";
            }

            for (let cc = 0; cc < layer.length; ++cc) {
                let node = layer[cc];
                if (node.isLeaf()) {
                    let p = node.userData && node.userData.t ? node.userData.t : "*";
                    str += p;
                } else {
                    str += "●";
                }
                // str += " ";
            }
            str += "\n";
        }

        console.info(this);
        console.info(layers)
        console.info(str);
    }
    /* debug:end */
}

