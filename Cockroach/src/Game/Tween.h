#pragma once
#include "Timer.h"

namespace Cockroach
{
    enum TweenFunc
    {
        Linear
    };

    template<typename T>
    class Tween
    {
    public:
        Tween() {}
        Tween(T startValue, T endValue, float duration, TweenFunc tweenFunc);

        T start = {};
        T end = {};
        Timer timer = Timer(0.0f);
        TweenFunc tweenFunc = Linear;

        float CalculateTween(float t);
        T Step(float dt);
        bool Finished();
    };

    template<typename T>
    Tween<T>::Tween(T start, T end, float duration, TweenFunc tweenFunc)
        : start(start), end(end), timer(duration), tweenFunc(tweenFunc)
    {}

    template<typename T>
    float Tween<T>::CalculateTween(float t)
    {
        switch (tweenFunc)
        {
        case Linear:
        {
            return t;
        }
        }
    }

    template<typename T>
    T Tween<T>::Step(float dt)
    {
        timer.Tick(dt);

        if (timer.Finished())
            return end;

        float t = CalculateTween(timer.Progress01());

        return lerp(start, end, t);
    }

    template<typename T>
    bool Tween<T>::Finished()
    {
        return timer.Finished();
    }
}