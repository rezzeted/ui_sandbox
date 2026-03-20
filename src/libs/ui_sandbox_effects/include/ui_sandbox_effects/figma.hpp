#pragma once

#include <GL/gl3w.h>

namespace ui_sandbox::effects {

/// Must match `figma_fill_effect_slang()` GradientParams (std140).
struct FigmaUnifiedGradientUBO
{
    float linearAx, linearAy, linearBx, linearBy;
    float centerX, centerY, ellipseX, ellipseY;
    float gtype;
    float replaceMix;
    float angleStart;
    float paletteWidth;
    float spreadMode;
    float ditherStrength;
    float gradM0, gradM1, gradM2, gradM3, gradM4, gradM5;
    float paletteLinear;
    float _pad0, _pad1, _pad2;
};

inline constexpr int kFigmaMaxStops = 16;
inline constexpr int kFigmaPaletteTexWidth = 1024;

[[nodiscard]] bool figma_probe_rgba16f_texture();

/// UV affine: gradient space = T(0.5)*R(deg)*S(sx,sy)*T(-0.5) * T(tx,ty) applied to ImGui UV.
void figma_build_gradient_affine(float rot_deg, float scale_x, float scale_y, float tx, float ty, float* m6_out);

void rebuild_figma_palette_texture(GLuint tex, int tex_width, int n_stops, const float colors[][4],
                                   const float* positions_raw, bool store_linear_rgba16f);

} // namespace ui_sandbox::effects
