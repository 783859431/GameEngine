#include "GridRender.h"

void GridRender::draw(CommandBuffer cmd,float t)
{
    static float x=0,y = 0;
    static float zoom = 1;

    if (Input::Mouse::MouseButtonDown(Input::LeftButton))
    {
        auto currentMousePos = glm::vec2(Input::Mouse::deltaX, Input::Mouse::deltaY);
        camPos -= currentMousePos/zoom;
    }
 
    zoom*=powf(2, Input::Mouse::scrollY);
    zoom = glm::clamp(zoom,0.001f,1000.0f);
    cmd.BindPipeLine(pipeline.get());
    GridData data{ glm::vec4(camPos.x,camPos.y,zoom,0),glm::vec2(g_width,g_height),{0,0} };
    cmd.PushConst(pipeline.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, (char*)&data, sizeof(data));
    cmd.Draw();
}