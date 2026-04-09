#ifndef CORE_HANDLER_H_
#define CORE_HANDLER_H_

class Handler
{
private:
     void* obj; // 存储对象指针
     void (Handler::*func)(); // 存储成员函数指针
public:
    inline void clear(){obj = nullptr;func = nullptr;}
    inline bool has(){return func != nullptr;}

    void call();

    template<typename O>
    inline bool equals(void *object, void (O::*function)(const float&))
    {
        return obj == object && func == reinterpret_cast<void(TimerHandler::*)(const float&)>(function);
    }

    template<typename O>
    inline void setFunction(O* object, void (O::*function)(const float&)) {
        obj = static_cast<void*>(object);
        func = reinterpret_cast<void(TimerHandler::*)(const float&)>(function);
    }
};

#endif