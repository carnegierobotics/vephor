/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor/ogl/draw/gaussian_splat.h"

namespace vephor
{
namespace ogl
{

GaussianSplat::GaussianSplat()
{

}

void GaussianSplat::onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
{
    glGenFramebuffers(1, &oit_fbo);


	glGenTextures(1, &accum_id);
	glBindTexture(GL_TEXTURE_2D, accum_id);
	glTexImage2D(GL_TEXTURE_2D, 0, buffer_type, 
				size[0], size[1], 0, 
				buffer_type, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_2D, accum_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border_color[] = {1.0, 1.0, 1.0, 1.0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    



	glGenTextures(1, &revealage_id);
	glBindTexture(GL_TEXTURE_2D, revealage_id);
	glTexImage2D(GL_TEXTURE_2D, 0, buffer_type, 
				size[0], size[1], 0, 
				buffer_type, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_2D, revealage_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border_color[] = {1.0, 1.0, 1.0, 1.0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);



    glBindFramebuffer(GL_FRAMEBUFFER, oit_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accum_id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, revealage_id, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GaussianSplat::onRemoveFromWindow(Window*)
{
}

void GaussianSplat::renderOGL(Window* window, const TransformSim3& world_from_body)
{
    // Render to our accumulation buffer and revealage buffer

    glBindFramebuffer(GL_FRAMEBUFFER, oit_fbo);

    // Color accumulation buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accum_tex, 0);

    // Revealage buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, reveal_tex, 0);

    GLenum bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, bufs);

    // IMPORTANT: both buffers must start cleared:
    float clear_accum[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    glClearBufferfv(GL_COLOR, 0, clear_accum);

    float one = 1.0f;
    glClearBufferfv(GL_COLOR, 1, &one); // revealage starts at 1.0




    // Run the composite pass

	material_state.size = size * world_from_body.scale;
	material->activate(window, world_from_body, material_state);
	

	glBindVertexArray(vao_id);

    GLboolean gl_depth_test;

    if (!depth_test)
    {
        glGetBooleanv(GL_DEPTH_TEST, &gl_depth_test);
        glDisable(GL_DEPTH_TEST);
    }

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glDrawArraysInstanced(GL_TRIANGLES, 0, verts.cols(), offsets.cols());
	
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

    if (!depth_test)
    {
        if (gl_depth_test)
            glEnable(GL_DEPTH_TEST);
    }

	glBindVertexArray(0);

	material->deactivate();
}

} // namespace ogl
} // namespace vephor