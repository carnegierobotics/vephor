/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#pragma once

#include "../window.h"

namespace vephor
{

class Lines
{
public:
    Lines(
        const MatXRef& line_verts,
        const MatXRef& colors = MatX(),
		const Color& default_color = Vec4(1,1,1,1)
    );
    ~Lines();
	void setLineStrip(bool p_is_strip){is_strip = p_is_strip;}
    void setAlpha(bool p_is_alpha){is_alpha = p_is_alpha;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    MatX verts;
    MatX colors;
    bool is_strip = true;
    bool is_alpha = false;

    GLuint vao_id;
    GLuint pos_buffer_id;
    GLuint color_buffer_id;
    GLuint program_id;

    GLuint pos_attr_loc;
    GLuint color_attr_loc;

    GLuint mvp_matrix_id;
};

}