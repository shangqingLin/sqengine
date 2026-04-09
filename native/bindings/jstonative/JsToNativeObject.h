#pragma once

namespace bridge
{
    class JsToNativeObject
    {
        public:
           int nativeId = -1;
           JsToNativeObject();
           void initialize(int id);
           virtual ~JsToNativeObject();
    };
}