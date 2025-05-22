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

#include "widget.h"
#include "window.h"

namespace vephor
{

struct RadioButton
{
    string name;
    string brief;
};

class RadioButtonWidget : public Widget
{
public:
    explicit RadioButtonWidget(const string& title = "");

    json serialize(vector<vector<char>>* buffers) const override;

private:
    vector<RadioButton> buttons_;
    Color pressed_background_color_;
    Color pressed_text_color_;
    Color released_background_color_;
    Color released_text_color_;
};

//
// Implementation
//

inline RadioButtonWidget::RadioButtonWidget(const string& title) : Widget(title) {}

inline json RadioButtonWidget::serialize(vector<vector<char>>* buffers) const
{
    auto ser = Widget::serialize(buffers);

    return ser;
}

} // namespace vephor
