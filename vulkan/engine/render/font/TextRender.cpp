#include "TextRender.h"
#include "../../logger/Logger.h"
#include "../../utils/FileUtils.h"

extern VkDescriptorSet g_set0[MAX_FRAMES_IN_FLIGHT];
extern VkDescriptorSetLayout g_layout;
extern VkRenderPass g_renderPass;

//const std::string _glyphASCII = "\"!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþ ";

std::u16string s = u"?";
std::u16string s2 = u"你好世界\"!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
void TextRender::init(const std::string& ttfFont,int fontSize)
{

    texInfo.allocBuffer(8);
    //创建字体图集
  
    TextAtlas txAlt;
    if (!txAlt.init(ttfFont, fontSize))
        return;
    std::vector<char> bmp;
    txAlt.genAtlasBitmap(bmp, info);
    txAlt.clean();
    charAtlasImg.loadFromMemory(bmp.data(), info.width, info.height, VK_FORMAT_R8_UNORM);
    texInfo.updateData(&info,8);

    //创建set
    DescriptorSetLayoutBuilder builder2;
    setLayout = builder2
        .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1)
        .AddBinding(1,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT,1)
        .build();
    std::vector<DSetLayout*> lays;

    DoTimes([&](int i) {
        lays.push_back(&setLayout);
        });

    PoolManager::inst().allocSets(lays.data(), (int)lays.size(), sets);

    DoTimes([&](int i) {
        //创建vertbuffer
        textVert[i].allocBuffer(maxTextChar * sizeof(CharInst));
        auto bufferInfo =  texInfo.getDescriptorInfo();
        WriteSetHelper helper;
        auto imgInfo = charAtlasImg.getDescriptorImageInfo();
        helper.AddWriteBuffer(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, sets[i], &bufferInfo)
            .AddWriteImage(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,sets[i],&imgInfo,1).Update();
        });


    PipelineConfig cf = PipelineConfig::basic();

    auto desp = CharInst::getDescription();
    auto binding = CharInst::getBinding();

    cf.vertexInputInfo.vertexAttributeDescriptionCount = desp.size();
    cf.vertexInputInfo.vertexBindingDescriptionCount = binding.size();
    cf.vertexInputInfo.pVertexAttributeDescriptions = desp.data();
    cf.vertexInputInfo.pVertexBindingDescriptions = binding.data();

    cf.renderPass = g_renderPass;
    cf.setLayouts.push_back(g_layout);
    cf.setLayouts.push_back(setLayout.layout);

    cf.fragShader = Shader::LoadShader("shader/fontFrag.spv");
    cf.vertShader = Shader::LoadShader("shader/fontVert.spv");

    textPipe.create(cf);



}


void TextRender::drawText(const std::u16string text, float x, float y,const glm::vec4& color)
{
     
    float chX = 0;
    float chY = 0;
    chY = y;//字符在屏幕上的位置
    chX = x;

    // pen_x += slot->advance.x >> 6;
    for (int i = 0; i < text.size(); i++)
    {

        char32_t ch = text[i];
        auto it = info.metrics.find(ch);
        if (it == info.metrics.end())
        {
            ch = s[0];
        }
        Metric& mtc = info.metrics[ch];
        CharInst chInst;
        float offsetX = mtc.left / PIXEL_PER_UNIT;
        float offsetY = -mtc.top / PIXEL_PER_UNIT;
        chInst.chRect = {chX + offsetX ,chY + offsetY,mtc.w / PIXEL_PER_UNIT,mtc.h / PIXEL_PER_UNIT};
        chInst.chSampleRect = { mtc.x,mtc.y,mtc.w,mtc.h };
        chInst.color = color;
        chars.push_back(chInst);
        chX += (( mtc.advanceX) / PIXEL_PER_UNIT);
     //   chY += (mtc.advanceY / PIXEL_PER_UNIT);
    }

}


void TextRender::flush(CommandBuffer& cmd,int currentFrame)
{
    
    textVert[currentFrame].copy(chars.data(),chars.size()*sizeof(CharInst));
    cmd.bindPipeLine(textPipe.get());
    setsTempBuffer[0] = g_set0[currentFrame];
    setsTempBuffer[1] = sets[currentFrame];
    cmd.bindSets(textPipe.getLayout(),setsTempBuffer,2);
    cmd.bindVertex(textVert[currentFrame].getBuffer());
    cmd.draw(6, chars.size());
    chars.clear();
    
}

void TextRender::clean()
{
    if (setLayout.layout)
        vkDestroyDescriptorSetLayout(Device::getInstance().device,setLayout.layout,0);
}

TextRender::~TextRender()
{
    clean();
}

bool TextAtlas::init(const std::string& fontPath, int fontSize)
{

    this->m_fontPath = fontPath;
    this->m_fontSize = fontSize;
    std::vector<char> buf;
   // int bts = FileUtils::GetInst()->Read(fontPath, buf);
    //if (bts <= 0) return false;

    if (!m_initialized)
    {
        if (FT_Init_FreeType(&m_ft))
        {
            Logger::getInstance().error("Failed to initialize FreeType");
            return false;
        }
        m_initialized = true;
    }


    if (FT_New_Face(m_ft,fontPath.c_str(), 0, &m_face))
    {

        Logger::getInstance().error("Failed to load font : %s", m_fontPath.c_str());
        return false;
    }
    //优先选择Unicode码表
    if (FT_Select_Charmap(m_face, FT_ENCODING_UNICODE))
    {
        int foundIndex = -1;
        for (int charmapIndex = 0; charmapIndex < m_face->num_charmaps; charmapIndex++)
        {
            if (m_face->charmaps[charmapIndex]->encoding != FT_ENCODING_NONE)
            {
                foundIndex = charmapIndex;
                break;
            }
        }

        if (foundIndex == -1)
        {
            Logger::getInstance().error("Failed to find charMap");
            return false;
        }

        m_encoding = m_face->charmaps[foundIndex]->encoding;
        if (FT_Select_Charmap(m_face, m_encoding))
        {
            Logger::getInstance().error("Failed to Select_Charmap");
            return false;
        }
    }


    // set the requested font size
    int dpi = 72;
    int fontSizePoints = (int)(64.f * fontSize);
    if (FT_Set_Char_Size(m_face, fontSizePoints, fontSizePoints, dpi, dpi))
    {
        Logger::getInstance().error("Failed to Set_Char_Size");
        return false;
    }
    return true;
}

void TextAtlas::clean()
{
    if (m_initialized)
    {
        FT_Done_FreeType(m_ft);
        m_initialized = false;

    }

}

void TextAtlas::genAtlasBitmap(std::vector<char>& atlasBitmap, AtlasBitmapInfo& atlasData)
{
    float maxHeight = 0;
    float width = 0;
    FT_GlyphSlot  slot = m_face->glyph;
    for (int i = 0; i < s2.size(); i++)
    {
        /* retrieve glyph index from character code */
        char16_t ch = s2[i];
        FT_UInt glyph_index = FT_Get_Char_Index(m_face, ch);
        /* load glyph image into the slot (erase previous one) */
        FT_Error error = FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT);
        if (error)
        {
            continue;
        }


        /* convert to an anti-aliased bitmap */
        error = FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_SDF);
        if (error)
        {
            continue;
        }



        width += slot->bitmap.width;
        if (slot->bitmap.rows > maxHeight)
            maxHeight = slot->bitmap.rows;



    }
    atlasData.height = maxHeight;
    atlasData.width = width;
    atlasBitmap.resize(maxHeight * width);
    float currentX = 0;
    for (int i = 0; i < s2.size(); i++)
    {
        /* retrieve glyph index from character code */
        FT_UInt glyph_index = FT_Get_Char_Index(m_face, s2[i]);
        /* load glyph image into the slot (erase previous one) */
        FT_Error error = FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT);
        if (error)
        {
            continue;
        }

        /* convert to an anti-aliased bitmap */
        error = FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_SDF);
        if (error)
        {
            continue;
        }

        Metric mtc;
        mtc.x = currentX;
        mtc.y = 0;
        mtc.w = slot->bitmap.width;
        mtc.h = slot->bitmap.rows;
        mtc.top = slot->bitmap_top;
        mtc.left = slot->bitmap_left;
        mtc.advanceX = slot->advance.x >> 6;
        mtc.advanceY = slot->advance.y >> 6;
        
        atlasData.metrics[s2[i]] = mtc;


        //复制bitmap到AtlasBitmap好吗
        for (int bty = 0; bty < mtc.h; bty++)
        {
            for (int btx = 0; btx < mtc.w; btx++)
            {
                float atlasX = btx + currentX;
                float atlasY = bty;
                atlasBitmap[atlasY * width + atlasX] = slot->bitmap.buffer[bty * (int)mtc.w + btx];
            }

        }
        currentX += slot->bitmap.width;
    }

}
bool TextAtlas::m_initialized = false;