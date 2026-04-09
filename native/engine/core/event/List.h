
#ifndef _CORE_EVENT_LIST_H_
#define _CORE_EVENT_LIST_H_

namespace event
{

    template <typename ListNode>
    bool listAppend(ListNode **head, ListNode *newNode)
    {
        if (newNode->next != nullptr || newNode->prev != nullptr)
        {
            return false;
        }
        if (*head == nullptr)
        {
            newNode->next = newNode;
            newNode->prev = newNode;
            *head = newNode;
        }
        else
        {
            auto *first = *head;
            auto *last = (*head)->prev;
            newNode->prev = last;
            newNode->next = first;
            first->prev = newNode;
            last->next = newNode;
        }
        return true;
    }

    template <typename ListNode>
    bool detachFromList(ListNode **head, ListNode *node)
    {
        if (*head == nullptr || node->prev == nullptr || node->next == nullptr)
        {
            return false;
        }
        if (node->prev == node && node->next == node)
        {                             // the only node
            *head = nullptr;
        }
        else
        {
            auto *nextNode = node->next;
            auto *prevNode = node->prev;
            nextNode->prev = prevNode;
            prevNode->next = nextNode;
            if (node == *head)
            {
                *head = nextNode;
            }
        }
        node->prev = nullptr;
        node->next = nullptr;
        return true;
    }

}

#define EVENT_LIST_LOOP_BEGIN(tempVar, list) \
    if (list)                                \
    {                                        \
        auto *tempVar = list;                \
        do                                   \
        {                                    \
            auto *nextCopy = tempVar->next;

#define EVENT_LIST_LOOP_END(tempVar, list) \
    tempVar = nextCopy;                    \
    }                                      \
    while (tempVar != list)                \
        ;                                  \
    }

#define EVENT_LIST_LOOP_REV_BEGIN(tempVar, list) \
    if (list)                                    \
    {                                            \
        auto *tempVar = list->prev;              \
        bool isLastListNode = false;             \
        do                                       \
        {                                        \
            auto *nextCopy = tempVar->prev;      \
            isLastListNode = tempVar == list;

#define EVENT_LIST_LOOP_REV_END(tempVar, list) \
    tempVar = nextCopy;                        \
    }                                          \
    while (!isLastListNode)                    \
        ;                                      \
    }

#endif