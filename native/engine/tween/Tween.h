#pragma once
#include "../scene/Node.h"
#include "tween-ease/define.h"
#include "../core/core.h"
#include <vector>

struct TweenDefine
{
    TweenEase ease{TweenEase::linear};
    unsigned int property = 0;
    float *propertyTargetValue = nullptr;
    Node *target = nullptr;
    unsigned char eventCall;
    // 时长，单位为毫秒
    float duration = 0.f;
    bool loop = false;
    ~TweenDefine();
};

enum class TweenPropertyFlag : unsigned int
{
    X = 1 << 1,
    Y = 1 << 2,
    SX = 1 << 3,
    SY = 1 << 4,
    RX = 1 << 5,
    RY = 1 << 6,
    ROTATION = 1 << 7
};
SQ_ENUM_CONVERSION_OPERATOR(TweenPropertyFlag);

struct TweenProperty
{
    TweenPropertyFlag propertyType;
    float start = 0.f;
    float end = 0.f;
    TweenProperty() {};
    TweenProperty(TweenPropertyFlag propertyType, float start, float end) : propertyType(propertyType), start(start), end(end) {}
};

class Tween
{
private:
    static const unsigned int STOP = 2;     // 1 << 1;
    static const unsigned int COMPLETE = 4; // 1 << 2;
    static const unsigned int ACTIVE = 8;   // 1 << 3;
    std::vector<TweenProperty> properties;

    // js端生成的唯一ID
    int id = -1;
    unsigned int state = 0;
    unsigned char eventCall = 0;
    float timeTrace = 0.f;
    TweenEase ease = TweenEase::linear;
    Node *target = nullptr;
    // 时长，单位为毫秒
    float duration = 0.f;
    bool loop = false;

    void recovery();
    void update(float dt);

public:
    friend class TweenSystem;
    Tween();
    void start();
    void stop();
    void puase();
    void resume();
    void setDefine(TweenDefine &);
    bool isComplete();
    bool isStop();
};
