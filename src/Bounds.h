#pragma once

#include <InteractiveToolkit/ITKCommon/ITKCommon.h>
#include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>

class Bounds
{
public:
    MathCore::vec2i pos;
    MathCore::vec2f min, max;
    float area;

    Bounds()
    {
    }

    Bounds(float area)
    {
        this->area = area;
    }

    Bounds(const MathCore::vec2i &pos, const MathCore::vec2f &pixel_size)
    {
        this->pos = pos;

        min = (MathCore::vec2f)pos;
        max = min + 1.0f;

        min *= pixel_size;
        max *= pixel_size;

        area = pixel_size.x * pixel_size.y;
    }

    void print()
    {
        printf("   [%i, %i] -> min(%f, %f), max(%f, %f)\n",
               pos.x, pos.y,
               min.x, min.y,
               max.x, max.y);
    }

    static Bounds Intersect(const Bounds &a,const Bounds &b) {
        using namespace MathCore;
        if (a.max.x < b.min.x || a.min.x > b.max.x)
            return Bounds(0);
        if (a.max.y < b.min.y || a.min.y > b.max.y)
            return Bounds(0);

        Bounds result;

        result.min = OP<vec2f>::maximum( a.min, b.min);
        result.max = OP<vec2f>::minimum( a.max, b.max);

        auto size = result.max - result.min;
        result.area = size.x * size.y;

        return result;
    }
};
