#include "ui_sandbox_effects/figma.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <vector>
#include <cstdint>

#ifndef GL_RGBA16F
#define GL_RGBA16F 0x881A
#endif

namespace ui_sandbox::effects {

static_assert(sizeof(FigmaUnifiedGradientUBO) == 96, "Figma UBO size mismatch (std140 vs Slang)");

static float figma_srgb_channel_to_linear(float c)
{
    c = std::clamp(c, 0.f, 1.f);
    return (c <= 0.04045f) ? (c / 12.92f) : std::pow((c + 0.055f) / 1.055f, 2.4f);
}

static float figma_linear_channel_to_srgb(float l)
{
    l = std::clamp(l, 0.f, 1.f);
    return (l <= 0.0031308f) ? (l * 12.92f) : (1.055f * std::pow(l, 1.f / 2.4f) - 0.055f);
}

bool figma_probe_rgba16f_texture()
{
    GLuint tid = 0;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 1, 0, GL_RGBA, GL_FLOAT, nullptr);
    const GLenum err = glGetError();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &tid);
    return err == GL_NO_ERROR;
}

void figma_build_gradient_affine(float rot_deg, float scale_x, float scale_y, float tx, float ty, float* m6_out)
{
    const float rad = rot_deg * (3.14159265358979323846f / 180.f);
    const float c = std::cos(rad);
    const float s = std::sin(rad);
    const float ox = 0.5f;
    const float oy = 0.5f;
    const float m00 = c * scale_x;
    const float m01 = -s * scale_y;
    const float m10 = s * scale_x;
    const float m11 = c * scale_y;
    m6_out[0] = m00;
    m6_out[1] = m01;
    m6_out[2] = ox - m00 * ox - m01 * oy + tx;
    m6_out[3] = m10;
    m6_out[4] = m11;
    m6_out[5] = oy - m10 * ox - m11 * oy + ty;
}

void rebuild_figma_palette_texture(GLuint tex, int tex_width, int n_stops, const float colors[][4],
                                   const float* positions_raw, bool store_linear_rgba16f)
{
    if (tex == 0 || n_stops < 2 || tex_width < 2)
        return;

    std::vector<int> order(static_cast<size_t>(n_stops));
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(),
              [&](int a, int b) { return positions_raw[a] < positions_raw[b]; });

    std::vector<float> pos(static_cast<size_t>(n_stops));
    std::vector<std::array<float, 4>> cols_lin(static_cast<size_t>(n_stops));
    for (int i = 0; i < n_stops; ++i) {
        pos[static_cast<size_t>(i)] = std::clamp(positions_raw[order[static_cast<size_t>(i)]], 0.f, 1.f);
        const float* src = colors[order[static_cast<size_t>(i)]];
        cols_lin[static_cast<size_t>(i)][0] = figma_srgb_channel_to_linear(src[0]);
        cols_lin[static_cast<size_t>(i)][1] = figma_srgb_channel_to_linear(src[1]);
        cols_lin[static_cast<size_t>(i)][2] = figma_srgb_channel_to_linear(src[2]);
        cols_lin[static_cast<size_t>(i)][3] = std::clamp(src[3], 0.f, 1.f);
    }

    auto sample_stops_linear = [&](float t) -> std::array<float, 4> {
        t = std::clamp(t, 0.f, 1.f);
        if (t <= pos.front())
            return cols_lin[0];
        if (t >= pos.back())
            return cols_lin.back();
        for (int i = 0; i < n_stops - 1; ++i) {
            if (t <= pos[static_cast<size_t>(i + 1)]) {
                const float t0 = pos[static_cast<size_t>(i)];
                const float t1 = pos[static_cast<size_t>(i + 1)];
                const float u = (t - t0) / (std::max)(t1 - t0, 1e-8f);
                const auto& a = cols_lin[static_cast<size_t>(i)];
                const auto& b = cols_lin[static_cast<size_t>(i + 1)];
                return { a[0] + (b[0] - a[0]) * u, a[1] + (b[1] - a[1]) * u, a[2] + (b[2] - a[2]) * u,
                         a[3] + (b[3] - a[3]) * u };
            }
        }
        return cols_lin.back();
    };

    glBindTexture(GL_TEXTURE_2D, tex);
    if (store_linear_rgba16f) {
        std::vector<float> pix(static_cast<size_t>(tex_width) * 4u);
        for (int x = 0; x < tex_width; ++x) {
            const float t =
                (tex_width <= 1) ? 0.f : static_cast<float>(x) / static_cast<float>(tex_width - 1);
            const auto p = sample_stops_linear(t);
            pix[static_cast<size_t>(x) * 4u + 0u] = p[0];
            pix[static_cast<size_t>(x) * 4u + 1u] = p[1];
            pix[static_cast<size_t>(x) * 4u + 2u] = p[2];
            pix[static_cast<size_t>(x) * 4u + 3u] = p[3];
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, tex_width, 1, 0, GL_RGBA, GL_FLOAT, pix.data());
    } else {
        std::vector<uint8_t> pix(static_cast<size_t>(tex_width) * 4u);
        for (int x = 0; x < tex_width; ++x) {
            const float t =
                (tex_width <= 1) ? 0.f : static_cast<float>(x) / static_cast<float>(tex_width - 1);
            const auto lin = sample_stops_linear(t);
            pix[static_cast<size_t>(x) * 4u + 0u] =
                static_cast<uint8_t>(std::clamp(figma_linear_channel_to_srgb(lin[0]) * 255.f, 0.f, 255.f));
            pix[static_cast<size_t>(x) * 4u + 1u] =
                static_cast<uint8_t>(std::clamp(figma_linear_channel_to_srgb(lin[1]) * 255.f, 0.f, 255.f));
            pix[static_cast<size_t>(x) * 4u + 2u] =
                static_cast<uint8_t>(std::clamp(figma_linear_channel_to_srgb(lin[2]) * 255.f, 0.f, 255.f));
            pix[static_cast<size_t>(x) * 4u + 3u] =
                static_cast<uint8_t>(std::clamp(lin[3] * 255.f, 0.f, 255.f));
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_width, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix.data());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace ui_sandbox::effects
