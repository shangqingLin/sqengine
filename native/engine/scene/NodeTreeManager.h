#pragma once

#include "../core/core.h"
#include "../core/sqstd/Array.h"

class Node;
class NodeTreeManager
{
private:
    sqstd::Array<Node *> inhierChangeNodeArray;
    sqstd::Array<Node*> waitArray;
    bool lock = false;
    void update();
    void start();
    void updateEnable();
    void updateDisable();
    void updatePost();

    // 在Node中调用
    void addInhierChangeNode(Node *);
    void removeInhierChangeNode(Node *);

public:
    friend class Root;
    friend class Node;
    static NodeTreeManager *getInstnace();
};
