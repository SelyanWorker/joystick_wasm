#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "SimpleLogger.h"

#include <string>
#include <vector>


struct JoystickState
{
    std::vector<float> axes;
    std::vector<uint8_t> buttons;
    std::string name;
    int joystick_id;
};

class JoystickWidget
{
public:
    void draw()
    {
        ImGui::SetNextWindowPos(ImVec2(300, 20));
        ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin("Joystick widget");

        draw_main_axis_plot(m_state.axes[0], m_state.axes[1]);

        ImGui::End();
        m_logger.Draw("Joystick widget");
    }

    void update(int axes_count, const float* axes,
                int button_count, const uint8_t* buttons,
                const std::string& name, int joystick_id)
    {
        m_state.axes.resize(axes_count);
        //m_state.axes_count = axes_count;

        m_state.joystick_id = joystick_id;
        m_state.name = name;

        for (int i = 0; i < m_state.axes.size(); ++i) {
            if (i == 8) continue;

            if (m_state.axes[i] != axes[i] && i != 0 && i != 1)
                m_logger.AddLog("(%d %s) axis %d = %f\n", m_state.joystick_id, m_state.name.c_str(), i, m_state.axes[i]);

            m_state.axes[i] = axes[i];
        }

        static ImVec2 prevPoint{ 0, 0 };
        m_drawingPoints = false;

        //m_state.button_count = button_count;
        m_state.buttons.resize(button_count);
        for (int i = 0; i < m_state.buttons.size(); ++i)
        {
            if (m_state.buttons[i] != buttons[i])
            {
                m_logger.AddLog("(%d %s) button %d = %d\n", m_state.joystick_id, m_state.name.c_str(), i,
                                m_state.buttons[i]);
            }

            m_state.buttons[i] = buttons[i];

            if (i == 13 && m_state.buttons[i] == 1)
            {
                ImVec2 currentPoint{m_state.axes[0], m_state.axes[1]};
                m_points.push_back(currentPoint);
                prevPoint = currentPoint;
                m_drawingPoints = true;

                continue;
            }

            if (i == 17 && m_state.buttons[i] == 1)
            {
                m_points.clear();
                continue;
            }
        }
    }

private:
    void draw_main_axis_plot(float x, float y)
    {
        if (ImPlot::BeginPlot("Scatter Plot", "X", "Y"))
        {
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);

            if (!m_drawingPoints)
            {
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 4, ImVec4(0, 1, 0, 0.5f), IMPLOT_AUTO,
                                           ImVec4(0, 1, 0, 1));
                ImPlot::PlotScatter("Main axis", &m_state.axes[0], &m_state.axes[1], 1);
            }

            for(auto point : m_points)
            {
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 4, ImVec4(1, 0, 0, 0.5f), IMPLOT_AUTO,
                                           ImVec4(1, 0, 0, 1));
                ImPlot::PlotScatter("Printed", &point.x, &point.y, 1);
            }
            ImPlot::PopStyleVar();
        }
        ImPlot::EndPlot();
    }

private:
    std::vector<ImVec2> m_points;

    bool m_drawingPoints = false;

    JoystickState m_state;
    ExampleAppLog m_logger;
};