#include <glpp/imgui_statistics.hpp>
#include <glpp/imgui.hpp>
#include <algorithm>
#include <numeric>

ImStatisticCriteria ImGui::ComputeStatisticCriteria(float* samples, size_t numSamples, ImGuiStatisticsFlags flags)
{
    float* data;
    const bool samplesUnsorted = flags & ImGuiStatisticsFlags_SamplesNotSorted;
    const bool copySamples     = (flags & ImGuiStatisticsFlags_OutOfMemory) && samplesUnsorted;
    if (copySamples) {
        data = (float*)std::malloc(sizeof(float) * numSamples);
        std::memcpy(data, samples, sizeof(float) * numSamples);
    }
    else
    {
        data = samples;
    }

    ImStatisticCriteria criteria;

    if (samplesUnsorted) {
        std::sort(data, data + numSamples);
    }
    auto p_quartile = [=](float p) {
        int i = (int)std::floor((float)numSamples * p);
        return numSamples % 2 == 0
            ? 0.5f * (data[std::max(i - 1, 0)] + data[i])
            : data[i];
    };

    criteria.range[0] = data[0];
    criteria.range[1] = data[numSamples - 1];
    criteria.median = p_quartile(0.5f);
    criteria.quartile[0] = p_quartile(0.25f);
    criteria.quartile[1] = p_quartile(0.75f);

    criteria.mean = std::accumulate(data, data + numSamples, .0f) / numSamples;

    // Compute whiser min and max values (defined by distance of maximum 1.5 distance to quartile)
    int wIdx[2] = { -1, -1 };
    const float iqr = criteria.quartile[1] - criteria.quartile[0];
    for (size_t i = 0; i < numSamples; ++i) {
        float lowSample = data[i];
        float highSample = data[numSamples - i - 1];
        if (wIdx[0] == -1 && criteria.quartile[0] - lowSample <= iqr) { wIdx[0] = i; }
        if (wIdx[1] == -1 && highSample - criteria.quartile[1] <= iqr) { wIdx[1] = numSamples - i - 1; }
    }
    criteria.whisker[0] = data[wIdx[0]];
    criteria.whisker[1] = data[wIdx[1]];

    if (copySamples) {
        std::free(data);
    }
    return criteria;
}

void ImGui::BoxPlotEx(const char* label, float* samples, int numSamples, ImStatisticCriteria* _criteria, ImVec2 frame_size, ImGuiStatisticsFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    if (frame_size.x == 0.0f)
        frame_size.x = CalcItemWidth();
    if (frame_size.y == 0.0f) 
        frame_size.y = frame_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0, &frame_bb))
        return;
    const bool hovered = ItemHoverable(frame_bb, id) && inner_bb.Contains(g.IO.MousePos);

    ImStatisticCriteria criteria = _criteria == nullptr
        ? ComputeStatisticCriteria(samples, numSamples, flags)
        : *_criteria;

    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    const ImU32 col = GetColorU32(ImGuiCol_PlotLines);
    const ImU32 col_hovered = GetColorU32(ImGuiCol_PlotLinesHovered);
    const float span = criteria.range[1] - criteria.range[0];
    
    char toolTip[3 * 1024 + 1] = { 0 };
    int  toolTipLength = 0;
    
    // Draw box
    const ImVec2 tl((criteria.quartile[0] - criteria.range[0]) / span, 0.f);
    const ImVec2 br((criteria.quartile[1] - criteria.range[0]) / span, 1.f);
    const ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tl);
    const ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, br);
    window->DrawList->AddRect(pos0, pos1, col);

    if (hovered&& std::abs(g.IO.MousePos.x - pos0.x) < 5) {
        toolTipLength += ImFormatString(toolTip + toolTipLength, IM_ARRAYSIZE(toolTip) - toolTipLength, "0.25 Quartile: %8.4f\n", criteria.quartile[0]);
        window->DrawList->AddLine(pos0, ImVec2(pos0.x, pos1.y), col_hovered, 3.f);
    }
    if (hovered && std::abs(g.IO.MousePos.x - pos1.x) < 5) {
        toolTipLength += ImFormatString(toolTip + toolTipLength, IM_ARRAYSIZE(toolTip) - toolTipLength, "0.75 Quartile: %8.4f\n", criteria.quartile[1]);
        window->DrawList->AddLine(ImVec2(pos1.x, pos0.y), pos1, col_hovered, 3.f);
    }

    // Draw Median
    const float tm = (criteria.median - criteria.range[0]) / span;
    const float mx = inner_bb.Min.x + tm * (inner_bb.Max.x - inner_bb.Min.x);
    window->DrawList->AddLine(ImVec2(mx, inner_bb.Min.y), ImVec2(mx, inner_bb.Max.y), col);
    if (hovered && std::abs(g.IO.MousePos.x - mx) < 5) {
        toolTipLength += ImFormatString(toolTip + toolTipLength, IM_ARRAYSIZE(toolTip) - toolTipLength, "Median:        %8.4f\n", criteria.median);
        window->DrawList->AddLine(ImVec2(mx, pos0.y), ImVec2(mx, pos1.y), col_hovered, 3.f);
    }

    // Draw whisker
    const float w0 = inner_bb.Min.x + (criteria.whisker[0] - criteria.range[0]) / span * (inner_bb.Max.x - inner_bb.Min.x);
    const float w1 = inner_bb.Min.x + (criteria.whisker[1] - criteria.range[0]) / span * (inner_bb.Max.x - inner_bb.Min.x);
    const float y0 = inner_bb.Min.y + 0.25f * (inner_bb.Max.y - inner_bb.Min.y);
    const float y1 = inner_bb.Min.y + 0.75f * (inner_bb.Max.y - inner_bb.Min.y);
    const float y3 = 0.5f * (inner_bb.Min.y + inner_bb.Max.y);
    window->DrawList->AddLine(ImVec2(w0, y0), ImVec2(w0, y1), col);
    window->DrawList->AddLine(ImVec2(w1, y0), ImVec2(w1, y1), col);
    window->DrawList->AddLine(ImVec2(w0, y3), ImVec2(pos0.x, y3), col);
    window->DrawList->AddLine(ImVec2(w1, y3), ImVec2(pos1.x, y3), col);
    if (hovered && std::abs(w0 - g.IO.MousePos.x) < 5) {
        toolTipLength += ImFormatString(toolTip + toolTipLength, IM_ARRAYSIZE(toolTip) - toolTipLength, "Left Whisker:  %8.4f\n", criteria.whisker[0]);
        window->DrawList->AddLine(ImVec2(w0, y0), ImVec2(w0, y1), col_hovered, 3.f);
    }
    if (hovered && std::abs(w1 - g.IO.MousePos.x) < 5) {
        toolTipLength += ImFormatString(toolTip + toolTipLength, IM_ARRAYSIZE(toolTip) - toolTipLength, "Right Whisker: %8.4f\n", criteria.whisker[1]);
        window->DrawList->AddLine(ImVec2(w1, y0), ImVec2(w1, y1), col_hovered, 3.f);
    }

    // Draw outliers
    ImDrawListSplitter splitter;
    splitter.Split(window->DrawList, 2);
    const float radius = std::min(0.2f * inner_bb.GetHeight(), 5.f);
    for (size_t i = 0; i < numSamples; ++i) {
        const float s = samples[i];
        if (s < criteria.whisker[0] || s > criteria.whisker[1]) {
            const float sx = inner_bb.Min.x + (s - criteria.range[0]) / span * (inner_bb.Max.x - inner_bb.Min.x);
            float dist = std::abs(sx - g.IO.MousePos.x);
            bool active = hovered && dist < radius;
            if (active) {
                float d = std::max(std::abs(s - criteria.whisker[0]), std::abs(s - criteria.whisker[1]));
                toolTipLength += ImFormatString(toolTip + toolTipLength, IM_ARRAYSIZE(toolTip) - toolTipLength, "Outlier:       %8.4f [Distance: %3.3f]\n", s, d);
            }
            splitter.SetCurrentChannel(window->DrawList, active ? 1 : 0);
            window->DrawList->AddCircle(ImVec2(sx, y3), radius, active ? col_hovered : col);
        }
    }
    splitter.Merge(window->DrawList);
    if (toolTipLength != 0) {
        SetTooltip("%s", toolTip);
    }

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
}

void ImGui::BoxPlot(const char* label, float* samples, int numSamples, ImVec2 frame_size, ImGuiStatisticsFlags flags) {
    ImGui::BoxPlotEx(label, samples, numSamples, nullptr, frame_size, flags);
}