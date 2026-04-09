#pragma once
#include <unordered_map>
#include "JsToNativeObject.h"

namespace bridge
{
    class JsToNativeObjectManager
    {
    private:
        std::unordered_map<int,JsToNativeObject*> nodeMap;
    public:
        JsToNativeObjectManager();
        void add(JsToNativeObject*);
        void remove(JsToNativeObject*);

        template <typename T>
        T * getById(int id)
        {
            std::unordered_map<int, JsToNativeObject *>::iterator it = nodeMap.find(id);
            if(it != nodeMap.end()){
                return dynamic_cast<T*>(it->second);
            }else{
                return nullptr;
            }
        }
        static JsToNativeObjectManager* getInstance();
    };
}

