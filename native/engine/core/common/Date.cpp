#include "Date.h"
#include <chrono>
#include <iomanip>
#include <ctime>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

long getDateNow()
{

#ifdef __EMSCRIPTEN__
    double milliseconds = emscripten_get_now();
#else
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 转换为毫秒时间戳
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
#endif
    return milliseconds;
}

Date::Date()
{
    reset();
}

long Date::reset()
{
    start = getDateNow();
    return start;
}

long Date::get()
{
    return getDateNow() - start;
}
