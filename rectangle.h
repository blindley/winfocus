#ifndef RECTANGLE202208231636
#define RECTANGLE202208231636

#include <optional>
#include <algorithm>
#include <vector>

struct Rectangle_t {
    int left, top, right, bottom;

    int width() const { return right - left; }
    int height() const { return bottom - top; }
};

struct BorderRectangleSet {
    std::optional<Rectangle_t> left, top, right, bottom;

    template<typename Appendable>
    void appendToContainer(Appendable& container) {
        if (left) container.push_back(*left);
        if (right) container.push_back(*right);
        if (top) container.push_back(*top);
        if (bottom) container.push_back(*bottom);
    }
};

inline BorderRectangleSet clipAgainst(Rectangle_t back, Rectangle_t front) {
    BorderRectangleSet theSet = {};

    if (back.top >= back.bottom || back.left >= back.right)
        return theSet;

    Rectangle_t trimmedRect = back;

    if (trimmedRect.top < front.top) {
        Rectangle_t top = trimmedRect;
        top.bottom = std::min(top.bottom, front.top);
        theSet.top = top;
        trimmedRect.top = front.top;
        if (trimmedRect.top >= trimmedRect.bottom)
            return theSet;
    }

    if (trimmedRect.bottom > front.bottom) {
        Rectangle_t bottom = trimmedRect;
        bottom.top = std::max(bottom.top, front.bottom);
        theSet.bottom = bottom;
        trimmedRect.bottom = front.bottom;
        if (trimmedRect.top >= trimmedRect.bottom)
            return theSet;
    }

    if (trimmedRect.left < front.left) {
        Rectangle_t left = trimmedRect;
        left.right = std::min(left.right, front.left);
        theSet.left = left;
        trimmedRect.left = front.left;
        if (trimmedRect.left >= trimmedRect.right)
            return theSet;
    }

    if (trimmedRect.right > front.right) {
        Rectangle_t right = trimmedRect;
        right.left = std::max(right.left, front.right);
        theSet.right = right;

        // redundant
        // trimmedRect.right = front.right;
        // if (trimmedRect.left >= trimmedRect.right)
        //     return theSet;
    }

    return theSet;
}

#endif
