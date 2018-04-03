#pragma once
#include "imgui/imgui.h"

namespace internal
{
	void imguiInit();
	void imguiDraw(ImDrawData* draw_data);
	void InitImGUI();
	void StartUI(int windowWidth, int windowHeight);
	void EndUI();
}

