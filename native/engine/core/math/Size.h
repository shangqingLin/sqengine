#pragma once

template <typename T>
class Size
{
public:
    T width;
    T height;
    Size(T width, T height) : width(width), height(height) {};
    Size() {};
    void set(T width, T height)
    {
        this->width = width;
        this->height = height;
    }
};

