#pragma once

#include "ui_sandbox_effects/figma.hpp"

#include <cstddef>

namespace ui_sandbox::effects {

/// std140 demo effects: one float + vec3 _pad (or two floats + vec2 _pad) → 16 bytes
inline constexpr std::size_t kStdDemoEffectUboBytes = sizeof(float) * 4;

/// Single source of truth for built-in post effect ordering and on-disk fragment stems.
struct BuiltinEffectSpec {
    const char* name;
    const char* frag_stem; /// `post/{frag_stem}.slang`
    bool use_post_common;
    std::size_t uniform_bytes;
};

inline constexpr BuiltinEffectSpec kBuiltinEffectSpecs[] = {
    {"grayscale", "grayscale", true, kStdDemoEffectUboBytes},
    {"sepia", "sepia", true, kStdDemoEffectUboBytes},
    {"pixelate", "pixelate", true, kStdDemoEffectUboBytes},
    {"chromatic", "chromatic", true, kStdDemoEffectUboBytes},
    {"invert", "invert", true, kStdDemoEffectUboBytes},
    {"scanlines", "scanlines", true, kStdDemoEffectUboBytes},
    {"figma_fill", "figma_fill", false, sizeof(FigmaUnifiedGradientUBO)},
};

inline constexpr std::size_t kBuiltinEffectSpecCount = sizeof(kBuiltinEffectSpecs) / sizeof(kBuiltinEffectSpecs[0]);

} // namespace ui_sandbox::effects
