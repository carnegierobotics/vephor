///
/// Copyright 2025
/// Carnegie Robotics, LLC
/// 4501 Hatfield Street, Pittsburgh, PA 15201
/// https://www.carnegierobotics.com
///
/// This code is provided under the terms of the Master Services Agreement (the Agreement).
/// This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
///

#pragma once

#include "window.h"

namespace vephor
{

class Widget
{
public:
    explicit Widget(const string& title = "");

    virtual ~Widget() = default;

    virtual json serialize(vector<vector<char>>* buffers) const;

    //
    // Mutators
    //

    void setBackgroundColor(const Color& color);

    void setTextColor(const Color& color);

    void setAnchor(float x, float y);

    void setAnchor(const Vec2& anchor);

    void setAnchorBottomLeft();

    void setAnchorLeft();

    void setAnchorTopLeft();

    void setAnchorBottom();

    void setAnchorCentered();

    void setAnchorTop();

    void setAnchorBottomRight();

    void setAnchorRight();

    void setAnchorTopRight();

private:
    string title_;
    Vec2 anchor_{Vec2::Zero()};
    Color background_color_{1.0F, 1.0F, 1.0F, 1.0F};
    Color text_color_{0.0F, 0.0F, 0.0F, 1.0F};
};

//
// Implementation
//

inline Widget::Widget(const string& title) : title_(title) {}

inline json Widget::serialize(vector<vector<char>>* /* buffers */) const
{
    json ser = {{"type", "widget"},
                {"configuration",
                 {{"title", title_},
                  {"anchor", toJson(anchor_)},
                  {"background_color", toJson(background_color_.getRGBA())},
                  {"text_color", toJson(text_color_.getRGBA())}}}};

    return ser;
}

inline void Widget::setBackgroundColor(const Color& color) { background_color_ = color; }

inline void Widget::setTextColor(const Color& color) { text_color_ = color; }

inline void Widget::setAnchor(const Vec2& anchor) { anchor_ = anchor; }

inline void Widget::setAnchor(const float x, const float y) { setAnchor({x, y}); }

inline void Widget::setAnchorBottomLeft() { setAnchor({0, 0}); }

inline void Widget::setAnchorLeft() { setAnchor(0, 0.5); }

inline void Widget::setAnchorTopLeft() { setAnchor(0, 1); }

inline void Widget::setAnchorBottom() { setAnchor(0.5, 0); }

inline void Widget::setAnchorCentered() { setAnchor(0.5, 0.5); }

inline void Widget::setAnchorTop() { setAnchor(0.5, 1); }

inline void Widget::setAnchorBottomRight() { setAnchor(1, 0); }

inline void Widget::setAnchorRight() { setAnchor(1, 0.5); }

inline void Widget::setAnchorTopRight() { setAnchor(1, 1); }

} // namespace vephor
