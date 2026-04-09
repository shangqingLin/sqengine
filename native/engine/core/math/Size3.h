#ifndef _CORE_MATH_SIZE3_H_
#define _CORE_MATH_SIZE3_H_

template<typename T>
class Size3{
    private:
        T width;
        T height;
        T depth;
    public:
        Size3(T width,T height,T depth):width(width),height(height),depth(depth){};
        Size3(){};
        void set(T width,T height,T depth){
            this->width = width;
            this->height = height;
            this->depth = depth;
        }
};

#endif