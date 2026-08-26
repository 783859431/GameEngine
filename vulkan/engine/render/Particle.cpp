#include "Particle.h"
#include "../Global.h"
float ParticleSystem::__totalParticleCountFactor = 1.0f;
void ParticleSystem::update(float dt)
{
    if (_isActive && _emissionRate)
    {
        float rate = 1.0f / _emissionRate;
        int totalParticles = static_cast<int>(_totalParticles * __totalParticleCountFactor);

        //issue #1201, prevent bursts of particles, due to too high emitCounter
        if (_particleCount < totalParticles)
        {
            _emitCounter += dt;
            if (_emitCounter < 0.f)
                _emitCounter = 0.f;
        }

        int emitCount = std::min(totalParticles-_particleCount, (int)(_emitCounter / rate));
        addParticles(emitCount);
        _emitCounter -= rate * emitCount;

        _elapsed += dt;
        if (_elapsed < 0.f)
            _elapsed = 0.f;
        if (_duration != -1.0f && _duration < _elapsed)
        {
            this->stop();
        }
    }
    for (int i = 0; i < _particleCount; i++)
        particles[i].timeToLive -= dt;

    for (int i = 0; i < _particleCount; ++i)
    {

        if (particles[i].timeToLive <= 0.0f)
        {
            int j = _particleCount - 1;
            while (j > 0 && particles[j].timeToLive <= 0)
            {
                _particleCount--;
                j--;
            }
            particles[i].copy(particles[_particleCount - 1]);//用后面的粒子填补前面的空缺
            //_particleCount为当前存活的粒子数量
            --_particleCount;
        }
    }

     
        if (mode == ParticleMode::Gravity)
        {
            for (int i = 0; i < _particleCount; ++i)
            {
                glm::vec2 tmp, radial = { 0.0f, 0.0f }, tangential;

                // radial acceleration
                if (particles[i].position.x || particles[i].position.y)
                {
                    radial = glm::normalize(particles[i].position);
                }
                tangential = radial;
                radial.x *= particles[i].mode.grivatyMode.radialAccel;
                radial.y *= particles[i].mode.grivatyMode.radialAccel;

                // tangential acceleration
                std::swap(tangential.x, tangential.y);
                tangential.x *= -particles[i].mode.grivatyMode.tangentialAccel;
                tangential.y *= particles[i].mode.grivatyMode.tangentialAccel;

                // (gravity + radial + tangential) * dt
                tmp.x = radial.x + tangential.x + this->gravity.x;
                tmp.y = radial.y + tangential.y + this->gravity.y;
                tmp.x *= dt;
                tmp.y *= dt;

                particles[i].mode.grivatyMode.dirX += tmp.x;
                particles[i].mode.grivatyMode.dirY += tmp.y;

                // this is cocos2d-x v3.0
                // if (_configName.length()>0 && _yCoordFlipped != -1)

                // this is cocos2d-x v3.0
                tmp.x = particles[i].mode.grivatyMode.dirX * dt;
                tmp.y = particles[i].mode.grivatyMode.dirY * dt;

                particles[i].position.x += tmp.x;
                particles[i].position.y += tmp.y;
            }

        }
        else
        {
            for (int i = 0; i < _particleCount; ++i)
            {
                particles[i].mode.radiusMode.angle += particles[i].mode.radiusMode.degreesPerSecond * dt;
                particles[i].mode.radiusMode.radius += particles[i].mode.radiusMode.deltaRadius * dt;
                particles[i].position.x = -cosf(particles[i].mode.radiusMode.angle) * particles[i].mode.radiusMode.radius;
                particles[i].position.y = -sinf(particles[i].mode.radiusMode.angle) * particles[i].mode.radiusMode.radius;
            }
        }
        for (int i = 0; i < _particleCount; ++i)
        {
            //color r,g,b,a
            particles[i].color += particles[i].deltaColor * dt;
            //size
            particles[i].size += (particles[i].deltaSize * dt);
            particles[i].size = std::max(0.0f, particles[i].size);
            //angle
            particles[i].rotation += particles[i].deltaRotation * dt;
        }

   
}


void ParticleSystem::stop()
{
    _isActive = false;
    _elapsed = _duration;
    _emitCounter = 0;
}
void ParticleSystem::reset()
{
    _isActive = true;
    _elapsed = 0;
    for (int i = 0; i < _particleCount; ++i)
    {
        particles[i].timeToLive = 0.0f;
    }
}
void ParticleSystem::draw(CommandBuffer& cmd, int currentFrame)
{
    if (!this->_isActive) return;

    int offset = 0;
    for (int i = 0; i < _particleCount; i++)
    {

        int size = sizeof(glm::vec2);
        vtb[currentFrame].copy(&particles[i].position, size, offset);
        offset += size;

        size = sizeof(float);
        float uint = PixelToUnit(particles[i].size);
        vtb[currentFrame].copy(&uint, size, offset);
        offset += size;

        size = sizeof(float);
        vtb[currentFrame].copy(&particles[i].rotation, size, offset);
        offset += size;

        size = sizeof(glm::vec4);
        vtb[currentFrame].copy(&particles[i].color, size, offset);
        offset += size;


    }
    cmd.bindPipeLine(pipeline.get());
    cmd.bindSets(pipeline.getLayout(), &g_set0[currentFrame], 1);
    cmd.bindVertex(this->vtb[currentFrame].getBuffer());
    cmd.draw(6, _particleCount);
}

void ParticleSystem::addParticles(int count)
{
    if (count <= 0)return;
    uint32_t RANDSEED = rand();
    int start = _particleCount;
    _particleCount += count;

    for (int i = start; i < _particleCount; ++i)
    {
        //life 
        float theLife = _life + _lifeDeviation * RANDOM(&RANDSEED);
        particles[i].timeToLive = std::max(0.0f, theLife);
        //position
        particles[i].position.x = _sourcePosition.x + _posDeviation.x * RANDOM(&RANDSEED);
        particles[i].position.y = _sourcePosition.y + _posDeviation.y * RANDOM(&RANDSEED);
        //color
        particles[i].color.x = _startColor.x + _startColorDeviation.x * RANDOM(&RANDSEED);
        particles[i].color.y = _startColor.y + _startColorDeviation.y * RANDOM(&RANDSEED);
        particles[i].color.z = _startColor.z + _startColorDeviation.z * RANDOM(&RANDSEED);
        particles[i].color.w = _startColor.w + _startColorDeviation.w * RANDOM(&RANDSEED);

        glm::vec4 endColor;

        endColor.x = _endColor.x + _endColorDeviation.x * RANDOM(&RANDSEED);
        endColor.y = _endColor.y + _endColorDeviation.y * RANDOM(&RANDSEED);
        endColor.z = _endColor.z + _endColorDeviation.z * RANDOM(&RANDSEED);
        endColor.w = _endColor.w + _endColorDeviation.w * RANDOM(&RANDSEED);

        particles[i].deltaColor.x = (endColor.x - particles[i].color.x) / particles[i].timeToLive;
        particles[i].deltaColor.y = (endColor.y - particles[i].color.y) / particles[i].timeToLive;
        particles[i].deltaColor.z = (endColor.z - particles[i].color.z) / particles[i].timeToLive;
        particles[i].deltaColor.w = (endColor.w - particles[i].color.w) / particles[i].timeToLive;
        //size
        particles[i].size = _startSize + _startSizeDeviation * RANDOM(&RANDSEED);
        particles[i].size = std::max(0.0f, particles[i].size);

        float endSize = _endSize + _endSizeDeviation * RANDOM(&RANDSEED);
        endSize = std::max(0.0f, endSize);
        particles[i].deltaSize = (endSize - particles[i].size) / particles[i].timeToLive;
        //rotation
        particles[i].rotation = _startSpin + _startSpinDeviation * RANDOM(&RANDSEED);
        float endA = _endSpin + _endSpinDeviation * RANDOM(&RANDSEED);
        particles[i].deltaRotation = (endA - particles[i].rotation) / particles[i].timeToLive;
        //position
        // Mode Gravity: A
        if (mode == ParticleMode::Gravity)
        {

            // radial accel
            particles[i].mode.grivatyMode.radialAccel = radialAccel + radialAccelDeviation * RANDOM(&RANDSEED);
            // tangential accel
            particles[i].mode.grivatyMode.tangentialAccel = tangentialAccel + tangentialAccelDeviation * RANDOM(&RANDSEED);
            // rotation is dir
            float a = glm::radians(_angle + _angleDeviation * RANDOM(&RANDSEED));
            glm::vec2 v(cosf(a), sinf(a));
            float s = speed + speedDeviation * RANDOM(&RANDSEED);
            glm::vec2 dir = v * s;
            particles[i].mode.grivatyMode.dirX = dir.x;//v * s ;
            particles[i].mode.grivatyMode.dirY = dir.y;
            if (rotationIsDir)
                particles[i].rotation = -glm::degrees(glm::atan(dir.y / dir.x));

        }
        // Mode Radius: B
        else
        {
            //Need to check by Jacky
            // Set the default diameter of the particle from the source position
            particles[i].mode.radiusMode.radius = startRadius + startRadiusDeviation * RANDOM(&RANDSEED);
            particles[i].mode.radiusMode.angle = glm::radians(_angle + _angleDeviation * RANDOM(&RANDSEED));
            particles[i].mode.radiusMode.degreesPerSecond = glm::radians(rotatePerSecond + rotatePerSecondDeviation * RANDOM(&RANDSEED));

            float endRadius = this->endRadius + endRadiusDeviation * RANDOM(&RANDSEED);
            particles[i].mode.radiusMode.deltaRadius = (endRadius - particles[i].mode.radiusMode.radius) / particles[i].timeToLive;


        }



    }



}


void ParticleSystem::init(int numberOfParticles)
{
    _totalParticles = numberOfParticles;

    particles.clear();

    particles.resize(numberOfParticles);

    // default, active
    _isActive = true;

    // by default be in mode A:
    mode = ParticleMode::Gravity;

    // duration
    _duration = -1.0f;

    gravity = { 0,2 };

    // Gravity Mode: speed of particles
    speed = 1;

    speedDeviation = 1;

    // Gravity Mode: radial
    radialAccel = 0;
    radialAccelDeviation = 1;

    // Gravity mode: tangential
    tangentialAccel = 0;
    tangentialAccelDeviation = 1;

    // emitter position
    _sourcePosition = { 20,0 };
    _posDeviation = { 20,0 };


    // angle
    _angle = 0;
    _angleDeviation = 0;

    // life of particles
    _life = 10;
    _lifeDeviation =5;

    // size, in pixels
    _startSize = 5.0f;
    _startSizeDeviation = 2.5f;
    _endSize = _startSize;


    // emits per second
    _emissionRate = 500;

    // color of particles
    _startColor = { 1.0f,1.0f,1.0f,1.0f };
    _startColorDeviation = { 0.0f,0.0f,0.0f,0.0f };
    _endColor = { 1.0f,1.0f,1.0f,0.0f };
    _endColorDeviation = { 0.0f,0.0f,0.0f,0.0f };

    DoTimes([&](int i) 
        {
            vtb[i].clean();
            vtb[i].allocBuffer(numberOfParticles * Particle2D::getUpLoadSize());
        });

}
void ParticleSystem::createPipeline(VkRenderPass pass)
{
    PipelineConfig cf = PipelineConfig::basic();
    auto desp = Particle2D::getDescription();
    auto binding = Particle2D::getBinding();

    cf.vertexInputInfo.vertexAttributeDescriptionCount = desp.size();
    cf.vertexInputInfo.vertexBindingDescriptionCount = binding.size();
    cf.vertexInputInfo.pVertexAttributeDescriptions = desp.data();
    cf.vertexInputInfo.pVertexBindingDescriptions = binding.data();

    cf.renderPass = pass;
    cf.setLayouts.push_back(g_layout);
    cf.vertShader = Shader::LoadShader("shader/particleVert.spv");
    cf.fragShader = Shader::LoadShader("shader/particleFrag.spv");
    pipeline.create(cf);
}