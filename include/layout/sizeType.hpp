#pragma once 

#include <cstddef>

enum class SizeType {
    Content,
    Fixed,
    Percentage,
    Flex,
};

struct SizeSpec {
    SizeType type{SizeType::Content};
    std::size_t value{};
};
