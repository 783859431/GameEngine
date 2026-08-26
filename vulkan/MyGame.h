#pragma once
#include "engine/EngineTestMain.h"
#include "engine/physic/BallCollider2D.h"
#include "engine/render/font/TextRender.h"
class MyGame :public VulkanEngine
{
    b2WorldId m_worldId;
    SpriteRender spRender;
    PolygonRender polyRender;
    TextRender textRender;
    Sprite bird;
    int state = 1;
    BallCollider2D* ball;
    Texture birdTex;
    glm::vec2 center{ 5,15 };
    float speed = 100.0f;
    std::vector<Point> pts;
    ParticleSystem particleSys;
    void changeTransform();
    void handleKey();
    void UserUpdate(CommandBuffer& cmd, int currentFrame) override;
    void UserInit() override;
    void UserClean() override;
    void DrawGUI(CommandBuffer& cmd);

};

