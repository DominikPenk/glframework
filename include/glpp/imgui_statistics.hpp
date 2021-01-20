#pragma once

#include <glpp/imgui.hpp>

struct ImStatisticCriteria {
	float median;
	float mean;
	float quartile[2];
	float range[2];
	float whisker[2];
};

namespace ImGui {

	typedef int ImGuiStatisticsFlags;

	enum ImGuiStatisticsFlags_ {
		ImGuiStatisticsFlags_None             = 0x0,
		ImGuiStatisticsFlags_OutOfMemory      = 0x1,		// Set this flag if you do not want the sample data reordered
		ImGuiStatisticsFlags_SamplesNotSorted = 0x1 << 1
	};

	ImStatisticCriteria ComputeStatisticCriteria(float* samples, size_t numSamples, ImGuiStatisticsFlags flags = 0);
	void BoxPlotEx(const char* label, float* samples, int numSamples, ImStatisticCriteria* criteria = nullptr, ImVec2 frame_size = ImVec2(0, 0), ImGuiStatisticsFlags flags = 0);
	void BoxPlot(const char* label, float* samples, int numSamples, ImVec2 frame_size = ImVec2(0, 0), ImGuiStatisticsFlags flags = 0);

}