#pragma once
#include "glm/glm.hpp"
#include "Pipeline.h"
#include "VertexBuffer.h"
#include "CommandBuffer.h"
#include "../Global.h"
#define MAX_PARTICLES 10000
extern VkDescriptorSet g_set0[];
extern VkDescriptorSetLayout g_layout;
/**
 A more effect random number getter function, get from ejoy2d.
 */
inline static float RANDOM(unsigned int* seed) {
    *seed = *seed * 134775813 + 1;
    union {
        uint32_t d;
        float f;
    } u;
    u.d = (((uint32_t)(*seed) & 0x7fff) << 8) | 0x40000000;
    return u.f - 3.0f;
}

enum ParticleMode {
    Gravity,
    Radius
};
class Particle2D
{
public:
	glm::vec2 position;
    float size;
    float rotation;
	glm::vec4 color;

    glm::vec4 deltaColor;
	float timeToLive;
    float deltaSize;
    float deltaRotation;
    union Mode
    {
        struct GrivatyMode{
            float dirX;
            float dirY;
            float radialAccel;
            float tangentialAccel;
        } grivatyMode;
        struct RadiusMode {
            float angle;
            float degreesPerSecond;
            float radius;
            float deltaRadius;
        } radiusMode;

    } mode;

    void copy(Particle2D& other)
    {
        memcpy(this,&other,sizeof(Particle2D));
    }
    static int getUpLoadSize()
    {
        return sizeof(position) + sizeof(color) + sizeof(size) + sizeof(rotation);
    }
    static std::vector<VkVertexInputBindingDescription> getBinding()
    {

        VkVertexInputBindingDescription vibd;
        vibd.binding = 0;
        vibd.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
        vibd.stride = getUpLoadSize();
        return std::vector{ vibd };
    }


    static  std::vector<VkVertexInputAttributeDescription> getDescription()
    {
        VkVertexInputAttributeDescription attr;
        std::vector<VkVertexInputAttributeDescription> attrs;
        attrs.reserve(4);

        attr.binding = 0;
        attr.location = 0;
        attr.format = VK_FORMAT_R32G32_SFLOAT;
        attr.offset = offsetof(Particle2D, position);
        attrs.push_back(attr);

        attr.binding = 0;
        attr.location = 1;
        attr.format = VK_FORMAT_R32_SFLOAT;
        attr.offset = offsetof(Particle2D, size);
        attrs.push_back(attr);

        attr.binding = 0;
        attr.location = 2;
        attr.format = VK_FORMAT_R32_SFLOAT;
        attr.offset = offsetof(Particle2D, rotation);
        attrs.push_back(attr);

        attr.binding = 0;
        attr.location = 3;
        attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attr.offset = offsetof(Particle2D, color);
        attrs.push_back(attr);
        return attrs;
    }

};
       
class ParticleSystem {

public:
	Pipeline pipeline;
	VertexBuffer vtb[MAX_FRAMES_IN_FLIGHT];
	std::vector<Particle2D> particles;
    void update(float dt);
    void stop();
    void reset();
    void draw(CommandBuffer& cmd, int currentFrame);
    void addParticles(int count);
    void init(int numberOfParticles);
    void createPipeline(VkRenderPass pass);
    /** Gravity value. Only available in 'Gravity' mode. */
    glm::vec2 gravity = {0.0f,0.0f};
    /** speed of each particle. Only available in 'Gravity' mode.  */
    float speed = 0.0f;
    /** speed variance of each particle. Only available in 'Gravity' mode. */
    float speedDeviation = 0.0f;
    /** tangential acceleration of each particle. Only available in 'Gravity' mode. */
    float tangentialAccel = 0.0f;
    /** tangential acceleration variance of each particle. Only available in 'Gravity' mode. */
    float tangentialAccelDeviation = 0.0f;
    /** radial acceleration of each particle. Only available in 'Gravity' mode. */
    float radialAccel = 0.0f;
    /** radial acceleration variance of each particle. Only available in 'Gravity' mode. */
    float radialAccelDeviation = 0.0f;
    /** set the rotation of each particle to its direction Only available in 'Gravity' mode. */
    bool rotationIsDir = 0.0f;



    /** The starting radius of the particles. Only available in 'Radius' mode. */
    float startRadius = 0.0f;
    /** The starting radius variance of the particles. Only available in 'Radius' mode. */
    float startRadiusDeviation = 0.0f;
    /** The ending radius of the particles. Only available in 'Radius' mode. */
    float endRadius = 0.0f;
    /** The ending radius variance of the particles. Only available in 'Radius' mode. */
    float endRadiusDeviation = 0.0f;
    /** Number of degrees to rotate a particle around the source pos per second. Only available in 'Radius' mode. */
    float rotatePerSecond = 0.0f;
    /** Variance in degrees for rotatePerSecond. Only available in 'Radius' mode. */
    float rotatePerSecondDeviation = 0.0f;




    /** How many seconds the emitter will run. -1 means 'forever' */
    float _duration = 0.0f;
    /** sourcePosition of the emitter */
    glm::vec2 _sourcePosition = {0.0f,0.0f};
    /** Position variance of the emitter */
    glm::vec2 _posDeviation = { 0.0f,0.0f };
    /** life, and life variation of each particle */
    float _life = 0.0f;
    /** life variance of each particle */
    float _lifeDeviation = 0.0f;
    /** angle and angle variation of each particle */
    float _angle = 0.0f;
    /** angle variance of each particle */
    float _angleDeviation = 0.0f;

    /** start size in pixels of each particle */
    float _startSize = 0.0f;
    /** size variance in pixels of each particle */
    float _startSizeDeviation = 0.0f;
    /** end size in pixels of each particle */
    float _endSize = 0.0f;
    /** end size variance in pixels of each particle */
    float _endSizeDeviation=0.0f;
    /** start color of each particle */
    glm::vec4 _startColor;
    /** start color variance of each particle */
    glm::vec4 _startColorDeviation;
    /** end color and end color variation of each particle */
    glm::vec4 _endColor;
    /** end color variance of each particle */
    glm::vec4 _endColorDeviation;
    //* initial angle of each particle
    float _startSpin = 0.0f;
    //* initial angle of each particle
    float _startSpinDeviation = 0.0f;
    //* initial angle of each particle
    float _endSpin = 0.0f;
    //* initial angle of each particle
    float _endSpinDeviation = 0.0f;
    /** emission rate of the particles */
    float _emissionRate = 0.0f;
    /** maximum particles of the system */
    int _totalParticles = 0;
    int _particleCount = 0;
    static float __totalParticleCountFactor;
    ParticleMode mode;
    bool _isActive = 0;
    float _emitCounter = 0.0f;
    float _elapsed = 0.0f;
};