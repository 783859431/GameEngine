#pragma once
#include "engine/EngineTestMain.h"
#include "engine/physic/BallCollider2D.h"
#include "engine/physic/PolyCollider.h"
#include "engine/physic/BoxCollider2D.h"
#include "engine/render/font/TextRender.h"
#include "engine/edit/SpEditor.h"
#define SHAPE_TABLE(X) \
        X(RIGHT_TRIANGLE,"rt") \
        X(TRIANGLE,"tri") \
        X(QUAD,"qud") \
        X(SBALL,"ball")

#define MAT_TABLE(X) \
        X(WOOD,"wood") \
        X(GLASS,"glass") \
        X(ROCK,"rock") \
        X(TNT,"tnt")

#define STATE_TABLE(X) \
        X(PERFECT,"1") \
        X(SLIGHT_HURT, "2") \
        X(MODRATE_HURT, "3") \
        X(SERIOUS_HURT, "4") 
enum BlockShape{
#define X(name,str) name,
    SHAPE_TABLE(X)
#undef X
     SHAPE_COUNT
};
enum BlockMat {
#define X(name,str) name,
    MAT_TABLE(X)
#undef X
    MAT_COUNT
};
enum BlockState {
#define X(name,str) name,
    STATE_TABLE(X)
#undef X
    STATE_COUNT
};




struct MapBlock {
    BlockShape shape;
    BlockMat  mat;
    BlockState state = BlockState::PERFECT;
    float health =100;
    Sprite* sp =0; 
    Collider* collider =0;
    bool destroy = false;
    BlockState getState() {
        if (health >= 90) return BlockState::PERFECT;
        else if (health >= 70) return BlockState::SLIGHT_HURT;
        else if (health >= 40) return BlockState::MODRATE_HURT;
        else {
            if (health < 0)
                destroy = true;
            return BlockState::SERIOUS_HURT;
        }
            

    }
    void Destroy()
    {
        if(sp)
            delete sp;
        if (collider)
            delete collider;

           
    }
    ~MapBlock() {
        Destroy();
    }
    void Update() {
        
        if (collider&& collider->isEnabled()&& !destroy) {

            sp->transform = collider->getTransform();

        }
        if(mat!=BlockMat::TNT)
            this->state = getState();
        else {
            if (health < 90)
                destroy = true;
        }
    }
    static BlockMat str2mat(const std::string& mat)
    {
#define X(name,str) if(mat==str) return BlockMat::name;
        MAT_TABLE(X)
#undef X
            throw std::runtime_error("uknow_mat:" + mat);
    }
    static BlockShape str2shape(const std::string& shape)
    {
#define X(name,str) if(shape==str) return BlockShape::name;
        SHAPE_TABLE(X)
            throw std::runtime_error("uknow_shape:" + shape);
#undef X
    }
    static BlockState str2state(const std::string& state) {
#define X(name,str) if(state==str) return BlockState::name;
        STATE_TABLE(X)
            throw std::runtime_error("uknow_state:" + state);
#undef X
    }

    static std::string mat2str(BlockMat mat)
    {
        switch (mat)
        {
#define X(name,str) case BlockMat::name: return str;
            MAT_TABLE(X)
#undef X
        default:
            throw std::runtime_error("uknow_mat:" + mat);
        }
            
    }
    static std::string shape2str(BlockShape shape)
    {
        switch (shape)
        {
#define X(name,str) case BlockShape::name: return str;
            SHAPE_TABLE(X)
#undef X
        default:
            throw std::runtime_error("uknow_shape:" + shape);
        }
    }

    static std::string state2str(BlockState state) {
        switch (state)
        {
#define X(name,str) case BlockState::name: return str;
            STATE_TABLE(X)
#undef X
        default:
            throw std::runtime_error("uknow_state:" + state);
        }
    }

     std::string getSpriteName() {
         return mat2str(mat) + "_" + shape2str(shape) + "_" + state2str(state);
    }

    std::string getResName()
    {
        return mat2str(this->mat) + "_" + shape2str(this->shape) + "_" + state2str(this->state);
        
    }
    static int  toIndex(BlockShape shape, BlockMat mat, BlockState state)
    {
        return mat * BlockShape::SHAPE_COUNT * BlockState::STATE_COUNT + shape * BlockState::STATE_COUNT + state;
    }
    int getIndex() {
        return toIndex(shape, mat, this->state);
    }
};

class MyGame :public VulkanEngine
{
    b2WorldId m_worldId;
    SpriteRender spRender;
    PolygonRender polyRender;
    TextRender textRender;
    Sprite bird;
    //Texture blk;
    //Texture ground;
    //Texture sky;
    //Sprite spGround;
    //Sprite spSky;
    SpEditor* editor;
    //MapBlock mbk;
    std::vector<Rect*> blocks;
    int state = 1;
    BallCollider2D* ball;
    
    //PolyCollider* cpoly;
    //Texture birdTex;
    glm::vec2 center{ 5,15 };
    float speed = 100.0f;
    std::vector<Point> pts;
    std::vector<MapBlock*> scene;
    std::vector<MapBlock*> removeList;
    std::unordered_map<std::string, Texture*> gameTexs;
    ParticleSystem particleSys;

    void SaveScene();
    void CreateBlock(BlockShape shape, BlockMat mat);
    void DeleteBlock(MapBlock* it);
    void LoadScene();
    void changeTransform();
    void handleKey();
    void Explode(const glm::vec2& center, float range, float maxImpulse);
    void UserUpdate(CommandBuffer& cmd, int currentFrame) override;
    void LoadJson();
    void UserInit() override;
    void UserClean() override;
    void DrawGUI(CommandBuffer& cmd);

};

