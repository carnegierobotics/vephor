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

struct ButtonStyle
{
    Vec2 center{0, 0}; //!< Center pixel.
    Vec2 lengths{15, 15}; //!< Half-lengths along each axis.

    Color released_background_color{0.3F, 0.3F, 0.3F, 1.0F}; //!< Background color when not pressed.
    Color released_text_color{1.0F, 1.0F, 1.0F, 1.0F}; //!< Text color when not pressed.
    Color pressed_background_color{0.3F, 0.3F, 0.3F, 1.0F}; //!< Background color when pressed.
    Color pressed_text_color{1.0F, 1.0F, 0.0F, 1.0F}; //!< Text color when pressed.
};

//! Button element in a fixed path interface.
template <class Data>
struct Button
{
    //
    // Configuration
    //

    std::string label{""}; //!< Label text.
    ButtonStyle style; //!<

    std::function<void(Data&)> on_press{[](Data&) -> void {}}; //!< Callback invoked on button press.
    std::function<void(Data&)> on_release{[](Data&) -> void {}}; //!< Callback invoked on button release.

    //
    // State
    //

    bool is_pressed{false}; //!< Whether the button is currently pressed.

    //
    // Methods
    //

    //! Press the button if it is released.  Otherwise, do nothing.  Returns whether the button state has changed.
    bool press(Data& data)
    {
        if (!is_pressed)
        {
            on_press(data);
            is_pressed = true;
            return true;
        }

        return false;
    }

    //! Release the button if it is pressed.  Otherwise, do nothing.  Returns whether the button state has changed.
    bool release(Data& data)
    {
        if (is_pressed)
        {
            on_release(data);
            is_pressed = false;
            return true;
        }

        return false;
    }

    //! Toggle the button state.  Returns whether the button state has changed.  Toggling will always change the button
    //! state.
    bool toggle(Data& data)
    {
        if (is_pressed)
        {
            return release(data);
        }

        return press(data);
    }
};

} // namespace vephor
