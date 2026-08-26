#include "MyGame.h"
#include "engine/physic/BoxCollider2D.h"
glm::vec2 GetOverlapPoint(const glm::vec2& center, float r, const glm::vec2& testPoint)
{
    glm::vec2 line = center - testPoint;
    float m = glm::length(line);
    if (m <= r) return testPoint;
    return center - glm::normalize(line) * r;
}
float  GetPointDistance(glm::vec2& point, glm::vec2& linePoint1, glm::vec2& linePoint2)
{
    glm::vec2 line = linePoint2 - linePoint1;

    glm::vec2 line2 = linePoint2 - point;

    glm::vec2 h = line2 - glm::clamp((glm::dot(line, line2) / glm::dot(line, line)), 0.0f, 1.0f) * line;

    return glm::length(h);

}
void GenPath(std::vector<Point>& points, glm::vec2& startPos, glm::vec2& v, float g, float interval, float count)
{
    points.clear();
    for (int i = 1; i < count + 1; i++)
    {
        float t = interval * i;
        float x = startPos.x + v.x * t;
        float y = startPos.y + v.y * t + 0.5 * g * t * t;
        points.push_back({ x,y });
    }
}
bool HitTest(const glm::vec2 pos, const Rect& rect)
{
    return pos.x > rect.x && pos.x < (rect.x + rect.w) && pos.y>rect.y && pos.y < (rect.y + rect.h);

}
void DispatchEvent(b2WorldId world)
{

    b2ContactEvents contactEvents = b2World_GetContactEvents(world);

    for (int i = 0; i < contactEvents.beginCount; i++)
    {
        b2ContactBeginTouchEvent sevent = contactEvents.beginEvents[i];
        Collider* colliderA = (Collider*)b2Shape_GetUserData(sevent.shapeIdA);
        Collider* colliderB = (Collider*)b2Shape_GetUserData(sevent.shapeIdB);
        colliderA->onContactEventBegain(colliderB);
        colliderB->onContactEventBegain(colliderA);
    }

    for (int i = 0; i < contactEvents.endCount; i++)
    {
        b2ContactEndTouchEvent sevent = contactEvents.endEvents[i];
        Collider* colliderA = (Collider*)b2Shape_GetUserData(sevent.shapeIdA);
        Collider* colliderB = (Collider*)b2Shape_GetUserData(sevent.shapeIdB);
        colliderA->onContactEventEnd(colliderB);
        colliderB->onContactEventEnd(colliderA);
    }
    b2SensorEvents sensorEvents = b2World_GetSensorEvents(world);
    for (int i = 0; i < sensorEvents.beginCount; i++)
    {
        b2SensorBeginTouchEvent sevent = sensorEvents.beginEvents[i];
        Collider* colliderA = (Collider*)b2Shape_GetUserData(sevent.sensorShapeId);
        Collider* colliderB = (Collider*)b2Shape_GetUserData(sevent.visitorShapeId);
        colliderA->onSensorEventBegain(colliderB);
        colliderB->onSensorEventBegain(colliderA);


    }
    for (int i = 0; i < sensorEvents.endCount; i++)
    {
        b2SensorEndTouchEvent sevent = sensorEvents.endEvents[i];
        Collider* colliderA = (Collider*)b2Shape_GetUserData(sevent.sensorShapeId);
        Collider* colliderB = (Collider*)b2Shape_GetUserData(sevent.visitorShapeId);
        colliderA->onSensorEventEnd(colliderB);
        colliderB->onSensorEventEnd(colliderA);


    }
    for (int j = 0; j < contactEvents.hitCount; j++)
    {

        b2ContactHitEvent hitEvent = contactEvents.hitEvents[j];
        Collider* colliderA = (Collider*)b2Shape_GetUserData(hitEvent.shapeIdA);
        Collider* colliderB = (Collider*)b2Shape_GetUserData(hitEvent.shapeIdB);

        colliderA->onHitEvent(colliderB, hitEvent);
        colliderB->onHitEvent(colliderA, hitEvent);

    }
}
void MyGame::changeTransform()
{
    //t += dt;
    //float a = 100 * sinf(3 * t);
    //float b = 100 * cosf(3 * t);
    //b2Vec2 v = b2Body_GetPosition(m_bodyIds[0]);
    //obj.transform.setPosition({v.x,v.y,0.0f});
    //b2Rot rot = b2Body_GetRotation(m_bodyIds[0]);
    //float angle = b2Rot_GetAngle(rot);
    //obj.transform.setRotation(0,0,angle);
    auto rot = ball->getRot();
    auto pos = ball->getPosition();
    bird.transform.x = pos.x;
    bird.transform.y = pos.y;
    bird.transform.c = rot.x;
    bird.transform.s = rot.y;
}
void MyGame::handleKey()
{
    camera.update();
    float delta = dt * speed;
    if (Input::Keyboard::KeyHold(GLFW_KEY_A))
    {
        camera.translate({ -delta,0.0,0.0 });

    }
    else if (Input::Keyboard::KeyHold(GLFW_KEY_D)) {
        camera.translate({ delta,0.0,0.0 });
    }
    else if (Input::Keyboard::KeyHold(GLFW_KEY_W))
    {
        camera.translate({ 0.0,-delta,0.0 });
    }
    else if (Input::Keyboard::KeyHold(GLFW_KEY_S))
    {
        camera.translate({ 0.0,delta,0.0 });
    }
    else if (Input::Keyboard::KeyHold(GLFW_KEY_X))
    {
        if (state == 1)
        {
            state = 2;
            ball->setEnabled(true);
            glm::vec2 pos = ball->getPosition();
            glm::vec2 c = { 5,15 };
            glm::vec2 force = (c - pos);
            force.x *= 3;
            force.y *= 3;
            ball->setLinearVelocity(force);
            ball->setAngularVelocity(5);
        }



    }

    if (state == 1)
    {
        glm::vec2 pos = ball->getPosition();
        glm::vec2 c = { 5,15 };
        glm::vec2 force = (c - pos);
        force.x *= 3;
        force.y *= 3;

        GenPath(this->pts, pos, force, 5, 0.5, 20);


    }
    for (auto& it : pts)
        polyRender.drawCircle({ it.x,it.y,1,0 }, 0.5, { 1,0,0,1 });

}
enum Category
{
    Ball = 1 << 0,
    Wall = 1 << 1,
    Block = 1 << 2
};
void MyGame::UserUpdate(CommandBuffer& cmd,int currentFrame)
{
    b2World_Step(m_worldId, 1 / 60.0f, 4);
    handleKey();
    changeTransform();
    particleSys.update(dt);
    particleSys.draw(cmd, currentFrame);

    DispatchEvent(m_worldId);
    DrawColliders(&this->polyRender, m_worldId);
    std::u16string nhsj = u"你好a\"bc\"\\d'1ab1'哈";
    textRender.drawText(nhsj, 5, 5, {1,0,0,1});
    textRender.flush(cmd, currentFrame);
    polyRender.drawCircle({ 5,15,1,0,0 }, 5, { 0,1,0,1 });
    if (state == 1)
        polyRender.drawLine({ bird.transform.x,bird.transform.y }, { 5,15 }, { 0,1,0,1 }); 
    polyRender.flushCircles(cmd, currentFrame);
    polyRender.flushPloys(cmd, currentFrame);
    polyRender.flushLines(cmd, currentFrame);
    spRender.drawSprite(&bird);
    spRender.flush(cmd, currentFrame);
    DrawGUI(cmd);


}

void MyGame::UserInit()
{

    textRender.init("C:/Windows/Fonts/msyh.ttc", 64);
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = { 0,5 };
    m_worldId = b2CreateWorld(&worldDef);

    birdTex.loadRes("texture/BIRDS_1.png");

    polyRender.init(this->swapchain.renderPass);
    spRender.init(this->swapchain.renderPass);

    particleSys.init(10000);
    particleSys.createPipeline(this->swapchain.renderPass);


    Imgui_init(
        glWindow::getInstance().getGLFWwindow(),
        Device::getInstance().ins,
        Device::getInstance().gpu,
        Device::getInstance().device,
        Device::getInstance().indices.graphicsFamily.value(),
        Device::getInstance().graphicsQueue,
        NULL,
        PoolManager::inst().pools[0]->setsPool,
        swapchain.getImageCount(),
        swapchain.getImageCount(),
        swapchain.renderPass);


    bird.setTexture(&birdTex);
    bird.setRawPos({ 850.0f, 880.0f, 50.0f, 50.0f });
    bird.setWH(50.0f, 50.0f);
    bird.transform.x = 3;
    bird.transform.y = 15;

    ball = new BallCollider2D(m_worldId, { 3,15 }, 1.0f, 0);
    ball->setEnabled(false);
    ball->setColliderFilter(Ball, Wall);
    ball->enableHitEvent(true);
    BoxCollider2D* bb = new BoxCollider2D(m_worldId, { 30,20 }, { 5,1 }, 0, false);
    bb->setColliderFilter(Wall, Ball);
    bb->enableHitEvent(true);
    bb->setHitCallback(
        [](Collider* other, b2ContactHitEvent& e) 
        {
        std::cout << "speed:" << e.approachSpeed << "\n";
        });



}

void MyGame::UserClean()
{
    Imgui_clean();
}

char txt[64] = { 0 };
char mousePos[64] = { 0 };
float counter = 2;
int n;
void MyGame::DrawGUI(CommandBuffer& cmd)
{

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    glm::vec3 camPos = this->camera.getPosition();

    {
        ImGui::NewFrame();
        ImGui::Begin("Camera");      // Create a window called "Hello, world!" and append into it.
        ImGui::SliderFloat("x", &camPos.x, -50.0f, 50.0f);
        ImGui::SliderFloat("y", &camPos.y, -50.0f, 50.0f);
        ImGui::SliderFloat("z", &camPos.z, -500.0f, 500.0f);
        camera.translate(camPos - this->camera.getPosition());
        ImGui::End();
        ImGui::EndFrame();
    }

    counter += this->dt;

    if (counter > 1)
    {
        counter = 0;
        n = sprintf(txt, "FPS:%d", (int)(1 / dt));

    }
    int len = sprintf(mousePos, "screen x:%.2f,y:%.2f", Input::Mouse::x, Input::Mouse::y);
    auto draw = ImGui::GetForegroundDrawList();
    draw->AddText(
        ImVec2(700, 50),
        IM_COL32(0, 255, 0, 255),
        txt,
        txt + n

    );

    draw->AddText(
        ImVec2(500, 50),
        IM_COL32(0, 255, 0, 255),
        mousePos,
        mousePos + len

    );
    glm::vec3 world;
    ScreenToWorld(this->camera, { Input::Mouse::x ,Input::Mouse::y }, world);
    //boxs[0]->setTransform({world.x,25});
    int row = std::round(world.x);

    int col = std::round(world.y);
    static Sprite* selected = 0;


    len = sprintf(mousePos, "world x:%.2f,y:%.2f", world.x, world.y);

    if (state == 1)
    {
        glm::vec2 pt = GetOverlapPoint(center, 5, world);
        ball->setTransform({ pt.x,pt.y });

    }


    //求点是否在圆内，如果在圆内就使用点坐标，否则使用点和圆心连线与圆交点的坐标


    draw->AddText(
        ImVec2(500, 70),
        IM_COL32(0, 255, 0, 255),
        mousePos,
        mousePos + len

    );
    // console->Draw("console");
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmd.m_command);
}
