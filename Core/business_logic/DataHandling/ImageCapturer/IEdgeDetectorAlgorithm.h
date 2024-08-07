#pragma once
#include <cstdint>
#include <vector>
#include <array>
constexpr uint32_t imgWidth = 320;
constexpr uint32_t imgHeight = 240;
class IEdgeDetectorAlgorithm
{
public:
	virtual void process(const std::array<std::array<uint8_t, imgWidth>, imgHeight> &image, std::array<std::array<uint8_t, imgWidth>, imgHeight> &edges) = 0;
};
