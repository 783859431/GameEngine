#pragma once
#include "Collider.h"
#include <functional>
// 前向声明，用于碰撞回调
class BoxCollider2D;

// 碰撞回调函数类型：参数为自身和另一个碰撞体
using CollisionCallback = std::function<void(BoxCollider2D* other)>;
class BoxCollider2D:public Collider
{
public:
    // 构造与析构
    BoxCollider2D(b2WorldId world, const glm::vec2& position, const glm::vec2& size,
        float angle = 0.0f, bool isDynamic = true,bool isBullet = false ,bool allowRot = true);

    // 禁止拷贝（Box2D指针不可拷贝）
    BoxCollider2D(const BoxCollider2D&) = delete;
    BoxCollider2D& operator=(const BoxCollider2D&) = delete;

    // ---------- 物理属性设置 ----------

    void setSize(const glm::vec2& size);               //动态调整碰撞箱大小
   
    

    // ---------- 获取数据 ----------

    glm::vec2 getSize() const;


    // ---------- 碰撞回调 ----------
    //void setOnCollisionEnter(CollisionCallback callback);
    //void setOnCollisionExit(CollisionCallback callback);

    //// 内部接口：由碰撞监听器调用
    //void onCollisionEnter(BoxCollider2D* other);
    //void onCollisionExit(BoxCollider2D* other);

    // ---------- 更新 ----------
    void update(); // 同步物理引擎状态到逻辑层（如有需要）

private:

    glm::vec2 m_size;                   // 逻辑尺寸


};

