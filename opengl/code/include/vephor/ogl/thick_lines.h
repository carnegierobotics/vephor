/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#pragma once

#include "window.h"

namespace vephor
{

class ThickLines
{
public:
    ThickLines(
        const MatXRef& line_verts,
        const MatXRef& colors = MatX(),
		const Vec4& default_color = Vec4(1,1,1,1)
    );
    ~ThickLines();
    void setLineWidth(float p_line_width){line_width = p_line_width;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
	void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
	MatX verts;
	MatX next;
	MatX prev;
    MatX colors;
	VecX dirs;
	VecXui indices;
    float line_width = 0.1f;

    GLuint program_id;
	
	GLuint vao_id;
	GLuint pos_buffer_id;
	GLuint next_buffer_id;
	GLuint prev_buffer_id;
    GLuint color_buffer_id;
	GLuint dir_buffer_id;
	GLuint ibo_id;
	
	GLuint pos_attr_loc;
	GLuint next_attr_loc;
	GLuint prev_attr_loc;
    GLuint color_attr_loc;
	GLuint dir_attr_loc;

    GLuint mvp_matrix_id;
	GLuint thickness_id;
	GLuint aspect_id;
	GLuint use_miter_id;
};

}