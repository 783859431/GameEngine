#include "MyGame.h"
#include "engine/physic/BoxCollider2D.h"
#include <cJSON.h>
#include "./engine/utils/FileUtils.h"
//求点是否在圆内，如果在圆内就使用点坐标，否则使用点和圆心连线与圆交点的坐标
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
void MyGame::SaveScene()
{

    cJSON* root = cJSON_CreateObject();
    cJSON* arr = cJSON_CreateArray();
    for (auto bk:scene)
    {
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "mat", bk->mat);
        cJSON_AddNumberToObject(obj, "shape", bk->shape);
        cJSON_AddNumberToObject(obj, "x", bk->sp->transform.x);
        cJSON_AddNumberToObject(obj, "y", bk->sp->transform.y);
        cJSON_AddNumberToObject(obj, "w", bk->sp->w);
        cJSON_AddNumberToObject(obj, "h", bk->sp->h);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "blocks", arr);
    char* json = cJSON_Print(root);
    std::ofstream file("data.json", std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "文件打开失败\n";
    }
    file.write(json, strlen(json));
    file.close();
    free(json);
    cJSON_Delete(root);
}
void CreateCollider(MapBlock* mapbk,b2WorldId world,bool enable)
{
    if (mapbk->shape == BlockShape::RIGHT_TRIANGLE)
    {
        float w = PixelToUnit(mapbk->sp->w);
        float h = PixelToUnit(mapbk->sp->h);
        mapbk->collider = new PolyCollider(world,
            { mapbk->sp->transform.x, mapbk->sp->transform.y },
            { {-w / 2,-h / 2},{-w / 2,h / 2},{w / 2,h / 2} });
    }
    else if (mapbk->shape == BlockShape::TRIANGLE)
    {
        float w = PixelToUnit(mapbk->sp->w);
        float h = PixelToUnit(mapbk->sp->h);
        mapbk->collider = new PolyCollider(world,
            { mapbk->sp->transform.x, mapbk->sp->transform.y },
            { {-w / 2,h / 2},{w / 2,h / 2},{0,-h / 2} });
    }
    else if (mapbk->shape == BlockShape::QUAD)
    {
        float w = PixelToUnit(mapbk->sp->w);
        float h = PixelToUnit(mapbk->sp->h);
        mapbk->collider = new BoxCollider2D(world,
            { mapbk->sp->transform.x, mapbk->sp->transform.y }, { w,h });
    }
    else if (mapbk->shape == BlockShape::SBALL)
    {
        float w = PixelToUnit(mapbk->sp->w);
        mapbk->collider = new BallCollider2D(world,
            { mapbk->sp->transform.x, mapbk->sp->transform.y }, w / 2);
    }
    mapbk->collider->setEnabled(enable);
    mapbk->collider->enableHitEvent(true);
    mapbk->collider->setHitCallback(
        [mapbk](Collider* other, b2ContactHitEvent& e)
        {
            mapbk->health -= e.approachSpeed;
            // std::cout << mapbk->health<<"\n";
        });
    mapbk->collider->setBodyUserData(mapbk);
}
void MyGame::CreateBlock(BlockShape shape, BlockMat mat)
{
    MapBlock* mapbk = new MapBlock();
    mapbk->health = 100;
    mapbk->mat = mat;
    mapbk->shape = shape;
    mapbk->state = BlockState::PERFECT;
    int id = mapbk->getIndex();
    Sprite* sp = new Sprite();
    sp->transform.x = 5;
    sp->transform.y = 5;
    sp->setTexture(gameTexs["blocks"]);
    sp->setRawPos(*blocks[id]);
    sp->setWH(sp->rawPos.w,sp->rawPos.h);
    mapbk->sp = sp;
    sp->obj = mapbk;
    CreateCollider(mapbk,m_worldId,false);
    editor->Add(sp);
    scene.push_back(mapbk);
}
void MyGame::DeleteBlock(MapBlock* it) {
    editor->Del(it->sp);
    scene.erase(std::remove(scene.begin(), scene.end(), it), scene.end());
    delete it;
}
void MyGame::LoadScene() {
    std::ifstream file("data.json", std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "文件打开失败\n";
    }
    std::stringstream buffer;
    buffer<< file.rdbuf();
    std::string content = buffer.str();
    cJSON* root = cJSON_Parse(content.c_str());
    cJSON* blks = cJSON_GetObjectItem(root,"blocks");
    int sz = cJSON_GetArraySize(blks);
    for (int i = 0; i < sz; i++)
    {
        MapBlock* mapbk = new MapBlock();
        cJSON* o = cJSON_GetArrayItem(blks, i);
        cJSON* prop = 0;
        prop = cJSON_GetObjectItem(o, "shape");
        mapbk->shape = (BlockShape)prop->valueint;

        prop = cJSON_GetObjectItem(o, "mat");
        mapbk->mat = (BlockMat)prop->valueint;


        Sprite* sp = new Sprite();
        sp->setTexture(gameTexs["blocks"]);
        sp->setRawPos(*blocks[mapbk->getIndex()]);
        mapbk->sp = sp;
        sp->obj = mapbk;
        prop = cJSON_GetObjectItem(o, "x");
        mapbk->sp->transform.x = prop->valuedouble;

        prop = cJSON_GetObjectItem(o, "y");
        mapbk->sp->transform.y = prop->valuedouble;

        prop = cJSON_GetObjectItem(o, "w");
        mapbk->sp->w = prop->valuedouble;

        prop = cJSON_GetObjectItem(o, "h");
        mapbk->sp->h = prop->valuedouble;

        editor->Add(sp);
        scene.push_back(mapbk);
        CreateCollider(mapbk, m_worldId, true);
    }
    cJSON_Delete(root);
    file.close();
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
            force.x *= 9;
            force.y *= 3;
            ball->setLinearVelocity(force);
            ball->setAngularVelocity(5);
        }



    }




}
struct ExplodeData {
    glm::vec2 center;
    float rad;
    float  maxImpulse;
};
bool OverlapCallback(b2ShapeId shapeId, void* context)
{

    Collider* collider = (Collider*)b2Shape_GetUserData(shapeId);
    
    MapBlock* bk = (MapBlock*)collider->getBodyUserData();
    ExplodeData* data = (ExplodeData*)context;

    glm::vec2 pos = collider->getPosition();
    
    glm::vec2 offset =  pos- data->center;
    
    float dist = glm::length(offset);
    glm::vec2 dir = glm::normalize(offset);

    float strength = 1.0f - dist / data->rad;
    strength = std::clamp(strength,0.0f,1.0f);
    glm::vec2 impulse = dir * (data->maxImpulse * strength);
    if(bk)
        bk->health -= data->maxImpulse * strength;
    collider->applyLinearImplush(impulse);

    return true;
}

std::vector<std::string> split(const std::string& arr, char s)
{
    std::stringstream ss(arr);
    std::string token;
    std::vector<std::string> res;
    while (std::getline(ss, token, s)) {
        res.push_back(token);
    }
    return res;
}

void MyGame::LoadJson() {
    std::vector<char> fileBuffer;
    FileUtils::GetInst()->Read("texture/spts.json", fileBuffer);
    cJSON* root = cJSON_Parse(fileBuffer.data());
    cJSON* sptList = cJSON_GetObjectItem(root, "sprites");
    int size = cJSON_GetArraySize(sptList);
    for (int i = 0; i < size; i++)
    {
        cJSON* item = cJSON_GetArrayItem(sptList, i);
        cJSON* filename = cJSON_GetObjectItem(item, "filename");
        cJSON* region = cJSON_GetObjectItem(item, "region");
        cJSON* x = cJSON_GetObjectItem(region, "x");
        cJSON* y = cJSON_GetObjectItem(region, "y");
        cJSON* w = cJSON_GetObjectItem(region, "w");
        cJSON* h = cJSON_GetObjectItem(region, "h");
        std::string name = filename->valuestring;
        name = name.substr(0, name.find_last_of('.'));
        auto ll = split(name, '_');
        if (ll.size() == 3)
        {
            auto mat = MapBlock::str2mat(ll[0]);
            auto shape = MapBlock::str2shape(ll[1]);
            auto state = MapBlock::str2state(ll[2]);
            blocks[MapBlock::toIndex(shape, mat, state)] = new Rect(x->valueint, y->valueint, w->valueint, h->valueint);
        }

    }
    cJSON_Delete(root);
}
//模拟爆炸产生的冲击
void MyGame::Explode(const glm::vec2& center, float rad, float maxImpulse) 
{
    particleSys._sourcePosition = center;
    particleSys.reset();
    b2AABB box = { {center.x-rad,center.y-rad},{center.x+rad,center.y+rad} };
    ExplodeData data;
    data.center = center;
    data.maxImpulse = maxImpulse;
    data.rad = rad;
    b2World_OverlapAABB(m_worldId, box, b2DefaultQueryFilter(),OverlapCallback,& data);

}
//++++//
enum Category
{
    CBall = 1 << 0,
    CWall = 1 << 1,
    CBlock = 1 << 2
};
//++++//

void MyGame::UserUpdate(CommandBuffer& cmd,int currentFrame)
{
    b2World_Step(m_worldId, 1 / 60.0f, 4);
    camera.update();
    handleKey();
    changeTransform();
    for (auto it : scene) {
        it->Update();
        it->sp->setRawPos(*blocks[it->getIndex()]);
        spRender.drawSprite(it->sp);
        if (it->destroy)
        {
            if (it->mat == TNT)
            {
                Explode({ it->sp->transform.x,it->sp->transform.y },10,100);
            }

            removeList.push_back(it);
        }
    }
    DispatchEvent(m_worldId);//先处理碰撞事件，再考虑删除碰撞体，否则有的碰撞体已经被删了，碰撞事件还在
    for (auto it : removeList)
    {
        DeleteBlock(it);
    }
    removeList.clear();
    editor->MoveSel(this->camera,dt);
    editor->ControlPoints(this->polyRender);
    particleSys.update(dt);
    particleSys.draw(cmd, currentFrame);



   // spRender.drawSprite(mbk.sp);
    //spRender.drawSprite(&spGround);
    //spRender.drawSprite(&spSky);
    DrawColliders(&this->polyRender, m_worldId);
   // std::u16string nhsj = u"你好a\"bc\"\\d'1ab1'哈";
   // textRender.drawText(nhsj, 5, 5, {1,0,0,1});
    polyRender.drawCircle({ 5,15,1,0,0 }, 5, { 0,1,0,1 });
    //spRender.drawSprite(&bird);
    DrawGUI(cmd);
    if (state == 1)
    {
       glm::vec2 pos = ball->getPosition();
       glm::vec2 c = { 5,15 };
       glm::vec2 force = (c - pos);
       force.x *= 9;
       force.y *= 3;
       GenPath(this->pts, pos, force,5, 0.5, 20);
       for (auto& it : pts)
           polyRender.drawCircle({ it.x,it.y,1,0 }, 0.5, { 1,0,0,1 });
       polyRender.drawLine({ bird.transform.x,bird.transform.y }, { 5,15 }, { 0,1,0,1 });
    }
    textRender.flush(cmd, currentFrame);
    polyRender.flushCircles(cmd, currentFrame);
    polyRender.flushPloys(cmd, currentFrame);
    polyRender.flushLines(cmd, currentFrame);
    spRender.flush(cmd, currentFrame);
  


}

void MyGame::UserInit()
{
    editor = new SpEditor();
    int all =  BlockMat::MAT_COUNT * BlockShape::SHAPE_COUNT * (BlockState::STATE_COUNT);
    //mbk.health = 100;
    //mbk.shape = BlockShape::RIGHT_TRIANGLE;
    //mbk.state = BlockState::PERFECT;
    //mbk.mat = BlockMat::GLASS;
    //mbk.sp = new Sprite();
    //editor->Add(mbk.sp);
    blocks.resize(all);
    textRender.init("C:/Windows/Fonts/msyh.ttc", 16);
    auto tex = new Texture();
    tex->loadRes("texture/BIRDS_1.png");
    gameTexs["birds"] = tex;
    tex = new Texture();
    tex->loadRes("texture/111.png");
    gameTexs["blocks"] = tex;

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = { 0,5 };
    m_worldId = b2CreateWorld(&worldDef);
    //ground.loadRes("texture/gd.png");
    //sky.loadRes("texture/sky.png");
    //spGround.setTexture(&ground);
    //spSky.setTexture(&sky);
    //spGround.setRawPos({0.0,0.0,(float)ground.width,(float)ground.height});
    //spGround.setWH(800,ground.height);
    //spSky.setWH(800, sky.height+150);
    //spGround.transform.x = 20;
    //spGround.transform.y = 25;
    //spSky.transform.x = 20;
    //spSky.transform.y = 10;
    //blk.sampler = SamplerManager::getSamper(SamplerType::LinearClamp);
    //mbk.sp->setTexture(&blk);
    //mbk.sp->transform.x = 5;
    //mbk.sp->transform.y = 5;
   
    LoadJson();
    //Rect* re = blocks[mbk.getIndex()];
    //mbk.sp->setRawPos(*re);
    //mbk.sp->setWH(re->w, re->h);
    polyRender.init(this->swapchain.renderPass);
    spRender.init(this->swapchain.renderPass);

    particleSys.init(1000);
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


    bird.setTexture(gameTexs["birds"]);
    bird.setRawPos({ 850.0f, 880.0f, 50.0f, 50.0f });
    bird.setWH(50.0f, 50.0f);
    bird.transform.x = 3;
    bird.transform.y = 15;

    ball = new BallCollider2D(m_worldId, { 3,15 }, 1.0f, 0);
    
    b2MassData ms = ball->getMassData();
    ms.mass = 100;
    ball->setMassData(ms);

    ball->setEnabled(false);
    //ball->setColliderFilter(CBall, CWall);
    ball->enableHitEvent(true);
    BoxCollider2D* bb = new BoxCollider2D(m_worldId, { 30,30 }, { 100,1 }, 0, false);
  //  bb->setColliderFilter(CWall, CBall);
    ball->enableHitEvent(true);
    ball->setHitCallback(
        [&](Collider* other, b2ContactHitEvent& e) 
        {
            //Explode(ball->getPosition(),10,100);
            state = 1;
            ball->setTransform(glm::vec2{3,15});
            ball->setEnabled(false);
        });
    LoadScene();


}

void MyGame::UserClean()
{
    for (auto& pa : gameTexs) {
        delete pa.second;
    }
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
    ImGui::NewFrame();
    {
      
        ImGui::Begin("Camera");      // Create a window called "Hello, world!" and append into it.
        ImGui::SliderFloat("x", &camPos.x, -50.0f, 50.0f);
        ImGui::SliderFloat("y", &camPos.y, -50.0f, 50.0f);
        ImGui::SliderFloat("z", &camPos.z, -500.0f, 500.0f);
        camera.translate(camPos - this->camera.getPosition());
        ImGui::End();
  
    }
    //{
    // 
    //    ImGui::Begin("ParticleSystem");      // Create a window called "Hello, world!" and append into it.
    //    ImGui::Checkbox("Active", &particleSys._isActive);
    //    ImGui::InputFloat("speed", &particleSys.speed);
    //    ImGui::InputFloat("speedOffset", &particleSys.speedDeviation);
    //    ImGui::InputFloat("angle", &particleSys._angle);
    //    ImGui::InputFloat("angleOffset", &particleSys._angleDeviation);
    //    ImGui::InputFloat2("position", (float*) & particleSys._sourcePosition);
    //    ImGui::InputFloat2("posOffset", (float*) & particleSys._posDeviation);
    //    ImGui::InputFloat("life", &particleSys._life);
    //    ImGui::InputFloat("lifeOffset", &particleSys._lifeDeviation);
    //    ImGui::InputFloat("startSize", &particleSys._startSize);
    //    ImGui::InputFloat("startSizeOffset", &particleSys._startSizeDeviation);
    //    ImGui::InputFloat("endSize", &particleSys._endSize);
    //    ImGui::InputFloat("endSizeOffset", &particleSys._endSizeDeviation);
    //    ImGui::InputFloat("emissionRate", &particleSys._emissionRate);
    //    ImGui::ColorEdit4("startColor", (float*) & particleSys._startColor);
    //    ImGui::ColorEdit4("startColorOffset", (float*) & particleSys._startColorDeviation);
    //    ImGui::ColorEdit4("endColor", (float*)&particleSys._endColor);
    //    ImGui::ColorEdit4("endColorOffset", (float*)&particleSys._endColorDeviation);
    //    ImGui::End();

    //}
    if (editor->sel)
    {

        ImGui::Begin("SpriteInfo");      // Create a window called "Hello, world!" and append into it.
        ImGui::InputFloat("spW", &editor->sel->w);
        ImGui::InputFloat("spH", &editor->sel->h);
        ImGui::InputFloat("spX", &editor->sel->transform.x);
        ImGui::InputFloat("spY", &editor->sel->transform.y);
        ImGui::InputFloat("health", &(((MapBlock*)editor->sel->obj)->health));
        
        if (ImGui::Button("delete"))
        {
            DeleteBlock((MapBlock*)editor->sel->obj);
            editor->sel = 0;
    
        }
        Collider* colli=0;
        if(editor->sel)
            colli = ((MapBlock*)editor->sel->obj)->collider;
        if (colli)
        {
            bool isEn = colli->isEnabled();
            if (ImGui::Checkbox("phx",&isEn))
            {
                colli->setEnabled(isEn);
            }
            if (!isEn)
            {
                colli->setTransform(editor->sel->transform);
            }

        }
        ImGui::End();
    }

    {
        static const char* shapes[] = {"rt","tri","qud","ball"};
        static const char* mats[] = {"wood","glass","rock","tnt"};
        static int selShape = 0;
        static int selMat = 0;
        ImGui::Begin("SpriteSpan");      // Create a window called "Hello, world!" and append into it.
        ImGui::Combo("shape",&selShape,shapes,IM_ARRAYSIZE(shapes));
        ImGui::Combo("mat", &selMat, mats, IM_ARRAYSIZE(mats));
        if (ImGui::Button("create"))
        {
            CreateBlock((BlockShape)selShape,(BlockMat)selMat);
        }
        if (ImGui::Button("save"))
        {
            SaveScene();
        }
        ImGui::End();

    }     
    ImGui::EndFrame();
    counter += this->dt;

    if (counter > 1)
    {
        counter = 0;
        n = sprintf(txt, "FPS:%d", (int)(1 / dt));
    }
    int len = sprintf(mousePos, "screen x:%.2f,y:%.2f", Input::Mouse::x, Input::Mouse::y);
    auto draw = ImGui::GetForegroundDrawList();
    draw->AddText(ImVec2(700, 50),IM_COL32(0, 255, 0, 255),txt,txt + n);
    draw->AddText(ImVec2(500, 50),IM_COL32(0, 255, 0, 255),mousePos,mousePos + len);
    glm::vec3 world;
    ScreenToWorld(this->camera, { Input::Mouse::x ,Input::Mouse::y }, world);
    ////boxs[0]->setTransform({world.x,25});
    //int row = std::round(world.x);
    //int col = std::round(world.y);
    //static Sprite* selected = 0;
    len = sprintf(mousePos, "world x:%.2f,y:%.2f", world.x, world.y);
    if (state == 1)
    {
        glm::vec2 pt = GetOverlapPoint(center, 5, world);
        ball->setTransform(glm::vec2{ pt.x,pt.y });

    }

   
    draw->AddText(ImVec2(500, 70),IM_COL32(0, 255, 0, 255),mousePos,mousePos + len);
    // console->Draw("console");
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmd.m_command);
}
