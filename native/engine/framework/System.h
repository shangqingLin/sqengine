#ifndef _SYSTEM_H_
#define _SYSTEM_H_

class System{
    public:
        virtual void init(){};
        virtual void startUpdate(){};
        virtual void update(float dt){};
        virtual void postUpdate(){};
};

#endif