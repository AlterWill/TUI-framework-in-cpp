#pragma once

namespace dividerStyle {

namespace horizontal {
inline constexpr char32_t light = U'─';
inline constexpr char32_t heavy = U'━';
inline constexpr char32_t doubleBorder = U'═';
inline constexpr char32_t dashed = U'╍';
inline constexpr char32_t block = U'█';
inline constexpr char32_t ascii = U'-';
inline constexpr char32_t none = U' ';
}  // namespace horizontal

namespace vertical {
inline constexpr char32_t light = U'│';
inline constexpr char32_t heavy = U'┃';
inline constexpr char32_t doubleBorder = U'║';
inline constexpr char32_t dashed = U'╎';
inline constexpr char32_t block = U'█';
inline constexpr char32_t ascii = U'|';
inline constexpr char32_t none = U' ';
}  // namespace vertical

}  // namespace dividerStyle
