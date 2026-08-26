#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include "../Texture.h"
#include "../Pipeline.h"
#include "../UniformBuffer.h"
#include "../VertexBuffer.h"
#include "../SpriteRender.h"


//字体图片在图集中的位置
struct Metric
{
    //字符BitMap在Atlas中的位置
    float x;
    float y;
    //字符BitMap的宽度和高度
    float w;
    float h;
    //
    float top;
    float left;
    float advanceX;//间距
    float advanceY;

};

struct  AtlasBitmapInfo
{
    float width;
    float height;
    std::unordered_map<char32_t,Metric> metrics;

};
struct CharInst
{
    glm::vec4 chRect;//位置和大小
    glm::vec4 chSampleRect;//采样矩形
    glm::vec4 color;

    static std::vector<VkVertexInputBindingDescription> getBinding()
    {

        VkVertexInputBindingDescription vibd;
        vibd.binding = 0;
        vibd.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
        vibd.stride = sizeof(CharInst);
        return std::vector{ vibd };
    }


    static  std::vector<VkVertexInputAttributeDescription> getDescription()
    {
        VkVertexInputAttributeDescription attr;
        std::vector<VkVertexInputAttributeDescription> attrs;
        attrs.reserve(3);

        attr.binding = 0;
        attr.location = 0;
        attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attr.offset = offsetof(CharInst, chRect);
        attrs.push_back(attr);

        attr.binding = 0;
        attr.location = 1;
        attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attr.offset = offsetof(CharInst, chSampleRect);
        attrs.push_back(attr);

        attr.binding = 0;
        attr.location = 2;
        attr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attr.offset = offsetof(CharInst, color);
        attrs.push_back(attr);

        return attrs;
    }

};

//它的功能很简单就是产生一张字体的bitmap
class TextAtlas
{
public:
    bool init(const std::string& fontPath, int fontSize);
    void clean();
    void genAtlasBitmap(std::vector<char>& altasBitmap, AtlasBitmapInfo& atlasData);

    FT_Library m_ft = 0;
    FT_Face m_face = 0;
    FT_Encoding m_encoding;
    static bool m_initialized;
    int m_fontSize = 0;
    std::string m_fontPath;
};

class TextRender
{
   
public:
    int maxTextChar = 1000;
    Pipeline textPipe;
    UniformBuffer texInfo;
    DSetLayout setLayout;
    VertexBuffer textVert[MAX_FRAMES_IN_FLIGHT];
    VkDescriptorSet sets[MAX_FRAMES_IN_FLIGHT];
    VkDescriptorSet setsTempBuffer[2];
    std::vector<CharInst> chars;

    void init(const std::string& fontPath,int fontSize);

    void drawText(const std::u16string text, float x, float y, const glm::vec4& color);

    void flush(CommandBuffer& cmd, int currentFrame);

    void clean();

    ~TextRender();
    //这里的char32_t 实际上是用utf-16编码的字符
    AtlasBitmapInfo info;
    Texture charAtlasImg;


};

