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
#include "material.h"

namespace vephor
{
namespace ogl
{

class InstancedPoints
{
public:
    InstancedPoints(const MatXRef& pts,
                    const MatXRef& colors = MatX(),
                    const VecXRef& sizes = VecX(),
                    const Vec4& default_color = Vec4(1, 1, 1, 1),
                    float default_size = 0.03F);

    // Kept for backwards compatibility.
    InstancedPoints(const MatXRef& pts, const MatXRef& colors, const Vec4& default_color);

    ~InstancedPoints();

    void setTexture(const shared_ptr<Texture>& p_tex);
    void setSize(float p_size);
    void setSizes(const MatXRef& p_sizes);
    void setOpacity(const float& p_opacity);
    void setScreenSpaceMode(bool p_ss_mode);

    void renderOGL(Window* window, const TransformSim3& world_from_body);

    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
    void onRemoveFromWindow(Window* window);

private:
    void generateMaterial();
    void setupVAO();

    Window* curr_window = nullptr;

    MatX verts;
    MatX uvs;
    MatX offsets;
    MatX colors;
    RVecX sizes;
    float size = 0.03f;
    bool ss_mode = false;

	shared_ptr<MaterialProgram> material;
	MaterialState material_state;

    GLuint vao_id = std::numeric_limits<GLuint>::max();

    GLuint pos_buffer_id = std::numeric_limits<GLuint>::max();
    GLuint uv_buffer_id = std::numeric_limits<GLuint>::max();
    GLuint offset_buffer_id = std::numeric_limits<GLuint>::max();
    GLuint color_buffer_id = std::numeric_limits<GLuint>::max();
    GLuint size_buffer_id = std::numeric_limits<GLuint>::max();
};

} // namespace ogl
} // namespace vephor