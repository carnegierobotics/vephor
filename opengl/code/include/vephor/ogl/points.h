/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#pragma once

#include "window.h"

namespace vephor
{

class Points
{
public:
    Points(
        const vector<Vec3>& pts
    );
    ~Points();
	void setColor(const Vec3& p_color){color = p_color;}
    void setSize(float p_size){size = p_size;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    MatX verts;
    Vec3 color = Vec3(1,1,1);
	float size = 0.03f;

    GLuint vao_id;
    GLuint pos_buffer_id;
    GLuint program_id;

    GLuint pos_attr_loc;
	
	GLuint mvp_matrix_id;
    GLuint modelview_matrix_id;
	GLuint size_id;
    GLuint color_id;
};

}