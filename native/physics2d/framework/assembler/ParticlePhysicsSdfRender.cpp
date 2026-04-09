#include "ParticlePhysicsSdfRender.h"
#include "../../../engine/2d/render/vertex-format.h"
#include "../component/ParticlePhysics2DComponent.h"
#include "../../../engine/core/sqstd/Array.h"
#include "../../../engine/core/sqstd/StackTempArenaAllocator.h"
#include "../../../engine/2d/components/Transform2DComponent.h"
#include "../PhysicsSystem.h"
#include "../../../engine/rendering/BuildInMeshDataManager.h"

using namespace physics2d;

ParticlePhysicsSdfRender::ParticlePhysicsSdfRender(ParticlePhysics2DComponent *component) : ParticlePhysicsRender(component),
                                                                                            posBuffer(nullptr),
                                                                                            sdfBuffer(nullptr),
                                                                                            sdfBufferTexture(nullptr),
                                                                                            sdfBufferUpdateMinX{0xffffff},
                                                                                            sdfBufferUpdateMinY{0xffffff},
                                                                                            sdfBufferUpdateMaxX{0},
                                                                                            sdfBufferUpdateMaxY{0},
                                                                                            mesh(nullptr),
                                                                                            rowColumnTexture(nullptr),
                                                                                            rowColumnBuffer(nullptr),
                                                                                            viewPosLeftX{0.f}, viewPosLeftY{0.f}
{

    std::vector<Attribute> aParticleIndex;
    aParticleIndex.assign(1, Attribute());
    aParticleIndex[0].name = "aParticleIndex";
    aParticleIndex[0].format = Format::R32F;
    aParticleIndex[0].custom = AttributeCustomSetting();
    aParticleIndex[0].custom->offset = 0;
    aParticleIndex[0].custom->count = 1;
    aParticleIndex[0].custom->stride = 4;
    aParticleIndex[0].custom->type = Format::R32F;
    aParticleIndex[0].custom->instanceStride = 1;

    mesh = new Mesh();
    mesh->autoDeleteBuffer = false;

    // 绘制一个矩形作为粒子
    IndexBuffer *indicesBuffer = pipeline::BuildInMeshDataManager::getIntance()->getPosIndexBuffer();
    mesh->setIndexBuffer(indicesBuffer);

    vParticleIndex = new VertexBuffer();
    mesh->addVertextBuffer(vParticleIndex, aParticleIndex);
    mesh->addVertextBuffer(pipeline::BuildInMeshDataManager::getIntance()->getPosVertexBuffer(), pipeline::BuildInMeshDataManager::posVertexAttributes);
    component->setMesh(mesh);
}

void ParticlePhysicsSdfRender::update()
{

    const int count = component->system->GetParticleCount();
    if (count == 0)
        return;

    SQ_ASSERT(sdfBuffer);

    {
        int unit = physics2d::PhysicsSystem::getInstance()->getUnitsPerMeter();
        int textureWidth = sdfBufferTexture->getWidth();
        int textureHeight = sdfBufferTexture->getHeight();

        // 所有单元格重置为0。0表示该单元格没有粒子
        std::fill(
            sdfBuffer,
            sdfBuffer + textureWidth * textureHeight * 4,
            0.0f);

        // for(int i = 0; i <  textureWidth * textureHeight * 4 ; ++i){
        //     SQ_ASSERT(sdfBuffer[i] == 0.f);
        // }

        float d = component->system->GetInverseDiameter();

        int minColumn = sdfBufferUpdateMinX;
        int maxColumn = sdfBufferUpdateMaxX;

        int minRow = sdfBufferUpdateMinY;
        int maxRow = sdfBufferUpdateMaxY;

        // printf("fuck prev %u %u \n",sdfBufferMinIndex,sdfBufferMaxIndex);

        int rowColumnIndex = 0;
        for (int i = 0; i < count; ++i)
        {
            phxy::SqParticle *particle = component->system->findById(component->system->findParticleIdByIndex(i));
            phxy::SqVec2 &pos = particle->getPosition();

            unsigned int tag = phxy::computeTag(pos.x * d, pos.y * d);
            unsigned int column = phxy::computeColumn(tag);
            unsigned int row = phxy::computeRow(tag);

            if (column < viewportMinX || column > viewportMaxX || row < viewportMinY || row > viewportMaxY)
            {
                // column -= viewportMinX;
                // row -= viewportMinY;
                // rowColumnBuffer[rowColumnIndex++] = (float)column;
                // rowColumnBuffer[rowColumnIndex++] = (float)row;
                // printf("out \n");
                rowColumnIndex += 2;
                continue;
            }

            unsigned int rc = column, rr = row;

            column -= viewportMinX;
            row -= viewportMinY;

            // printf("aPos %f %f rang %u %u %u %u c %u r %u \n", pos.x, pos.y,
            //     minViewColumn, minViewRow,maxViewColumn,maxViewRow,
            //     column, row);

            unsigned int index = row * textureWidth + column;

            // printf("%d index %u pos %f %f grid %u %u fuck %u %u view %u %u \n", i, index, pos.x * 50, pos.y * 50, column, row, rc,rr, viewportMinX, viewportMinY);

            // SQ_ASSERT(index <= textureWidth * textureHeight);

            if (column < minColumn)
            {
                minColumn = column;
            }

            if (column > maxColumn)
            {
                maxColumn = column;
            }

            if (row < minRow)
            {
                minRow = row;
            }

            if (row > maxRow)
            {
                maxRow = row;
            }

            rowColumnBuffer[rowColumnIndex++] = (float)column;
            rowColumnBuffer[rowColumnIndex++] = (float)row;
            index *= 4u;

            if (sdfBuffer[index + 2] == 1.0)
            {
                // printf("+++++重叠囖\n");
                // 同一个单元格上多个Cell重叠了
                continue;
            }

            // 将位置转换到屏幕空间[-1,1]，为了SDF的计算必须转换到屏幕空间
            // 为了减少着色器中计算，在CPU端计算吧
            sdfBuffer[index] = pos.x * unit / viewWidth;
            sdfBuffer[index + 1] = pos.y * unit / viewHeight;
            sdfBuffer[index + 2] = 1.0; // 使用1表示当前格子有粒子
            sdfBuffer[index + 3] = 0.f; // 暂时没有东西

            // printf(" index %u v %u nIndex %u  v %u bufferIndex %d low(%d %d) up(%d %d)\n", index, sdfBuffer[index],
            //        index + 1, sdfBuffer[index + 1], bufferIndex,

            //        (int)Math::floor(index / 4.f) % textureWidth,
            //        (int)Math::floor(index / 4) / textureWidth,

            //        (int)Math::floor((index + 1) / 4) % textureWidth,
            //        (int)Math::floor((index + 1) / 4) / textureWidth);
        }

        /**
         * 如果使用真实的更新区域去覆盖纹理上的一个区域
         * 那么我们需要在CPU端重新创建一个Buffer，这个Buffer的结构（大小）符合BufferTextureCopyRegion中的设置
         * 还需要将sdfBuffer的数据复制到这个新的Buffer中（数据量大的时候复制造成卡）
         * 所以，我们不构建真实的区域，只按照行围成的区域更新，列还是按照[0,textureWidht]的来更新就不用考虑上面的问题，虽然更新区域大了一些
         */
        minColumn = 0;
        maxColumn = textureWidth - 1;

        sdfBufferUpdateMinX = minColumn;
        sdfBufferUpdateMinY = minRow;
        sdfBufferUpdateMaxX = maxColumn;
        sdfBufferUpdateMaxY = maxRow;

        BufferTextureCopyRegion region;
        region.texOffsetX = minColumn;
        region.texOffsetY = minRow;
        region.texWidth = maxColumn - minColumn + 1;
        region.texHeight = maxRow - minRow + 1;
        const float *startBuffer = sdfBuffer + (minRow * textureWidth + minColumn) * 4;
        sdfBufferTexture->updateData((unsigned char *)startBuffer, region);
        // sdfBufferTexture->updateData((unsigned char *)sdfBuffer);
    }

    {
        BufferTextureCopyRegion region;
        int updatePixel = Math::ceil(count * 0.5f); // count * 2 / 4
        region.texWidth = posBuffer->getWidth();
        region.texHeight = Math::ceil(updatePixel / posBuffer->getWidth()) + 1;
        // printf("update %d %d \n",region.texWidth,region.texHeight);
        posBuffer->updateData((unsigned char *)component->system->GetPositionBuffer(), region);

        // printf("get pos %f \n", component->system->GetPositionBuffer()[0].x);

        // posBuffer->updateData((unsigned char *)component->system->GetPositionBuffer());
        rowColumnTexture->updateData((unsigned char *)rowColumnBuffer, region);
    }
}

void ParticlePhysicsSdfRender::setViewportSize(float w, float h)
{
    if (viewWidth == w && viewHeight == h)
    {
        return;
    }
    viewWidth = w;
    viewHeight = h;
    createGridTexture();
    udpateViewport();
}

void ParticlePhysicsSdfRender::setViewportPos(float x, float y)
{
    if (viewPosLeftX == x && viewPosLeftX == y)
        return;
    viewPosLeftX = x;
    viewPosLeftY = y;
    udpateViewport();
}

void ParticlePhysicsSdfRender::udpateViewport()
{

    int unit = physics2d::PhysicsSystem::getInstance()->getUnitsPerMeter();
    float d = component->system->GetInverseDiameter();
    float posX = -viewWidth * 0.5 + viewPosLeftX;
    float posY = -viewHeight * 0.5 + viewPosLeftY;

    // 转换到物理世界单位下
    posX /= unit;
    posY /= unit;

    // 转换到粒子的Grid下
    posX *= d;
    posY *= d;

    // 计算当前位置所在的grid的行列
    unsigned int viewTag = phxy::computeTag(posX, posY);
    viewportMinX = phxy::computeColumn(viewTag);
    viewportMinY = phxy::computeRow(viewTag);

    posX = viewWidth * 0.5 + viewPosLeftX;
    posY = viewHeight * 0.5 + viewPosLeftY;
    posX /= unit;
    posY /= unit;
    posX *= d;
    posY *= d;
    viewTag = phxy::computeTag(posX, posY);
    viewportMaxX = phxy::computeColumn(viewTag) - 1;
    viewportMaxY = phxy::computeRow(viewTag) - 1;

    // printf("??? offset %f %f minX %u minY %u maxX %u maxY %d  size %u %u\n",viewPosLeftX,viewPosLeftY,viewportMinX,
    //     viewportMinY,viewportMaxX,viewportMaxY,
    //     viewportMaxX - viewportMinX,
    //     viewportMaxY - viewportMinY
    // );
}

void ParticlePhysicsSdfRender::allocate(int particleNum)
{

    vParticleIndex->resizeBuffer(particleNum << 2, true);
    for (int i = 0; i < particleNum; ++i)
    {
        vParticleIndex->append<float>((float)i);
    }
    vParticleIndex->setDirty(true);
    setParticleNum(particleNum);

    // float points[30] = {
    //     2049, 2048,
    //     234.2342, 35.78876,
    //     -100.2, -35.78876};

    // unsigned int c = 0;
    // printf(" check %u \n", c + (-1));

    // // printf("%f \n", component->system->GetInverseDiameter());

    // for (int i = 0; i < 6; i += 2)
    // {
    //     float x = points[i];     // * component->system->GetInverseDiameter();
    //     float y = points[i + 1]; // * component->system->GetInverseDiameter();

    //     unsigned int tag = phxy::computeTag(x, y);
    //     printf(" p %f %f tag %u %u %u \n", x, y, tag, phxy::computeColumn(tag), phxy::computeRow(tag));

    //     unsigned int reTag = phxy::computeRelativeTag(tag, -1, -1);
    //     printf("?? rev tag %u %u %u \n", reTag, phxy::computeColumn(reTag), phxy::computeRow(reTag));
    // }
}

void ParticlePhysicsSdfRender::createGridTexture()
{
    /**
     * 为了实现粒子的SDF渲染，我们需要将粒子系统的Grid存储到Texture，记录每个单元格上有哪个粒子，
     * 以便在着色器中查询一个像素临近的粒子都是哪些。
     *
     * 粒子系统使用 [0,4095] 范围的单元格，我们需要将粒子的ParticeBufferIndex存储到Grid中，所以纹理中一个数字存储一个粒子index
     * 所以一个像素有4个数字，则一个像素可以存储4个粒子index
     */

    if (sdfBufferTexture)
    {
        delete sdfBufferTexture;
        delete[] sdfBuffer;
    }

    ITexture2DCreateInfo textureDefine;

    /**
     * 使用RGBA8来存储BufferIndex，每两个通道存储一个BufferIndex，共16位可以表示最大可以表示unsigned int类型到65535个粒子，足够了。
     * 这样节省了内存开销，不用4个字节来存储一个bufferIndex
     */
    textureDefine.format = toNumber(Format::RGBA32F);
    SamplerInfo sampler;
    sampler.minFilter = TextureFilter::NEAREST;
    sampler.magFilter = TextureFilter::NEAREST;
    textureDefine.sampler = sampler;

    int unit = physics2d::PhysicsSystem::getInstance()->getUnitsPerMeter();
    float d = component->system->GetInverseDiameter();
    int textureWidth = Math::ceil(viewWidth / unit * d);
    int textureHeight = Math::ceil(viewHeight / unit * d);

    // printf("unit %d d %f textureWidth %d  textureHeight %d  \n", unit, d, textureWidth, textureHeight);

    textureDefine.width = textureWidth;
    textureDefine.height = textureHeight;
    sdfBufferTexture = new Texture2d();
    sdfBufferTexture->create(&textureDefine);
    sdfBuffer = new float[textureWidth * textureHeight * 4];
    onUpdateMaterial();
}

void ParticlePhysicsSdfRender::setParticleNum(int particleNum)
{
    if (posBuffer)
    {
        delete posBuffer;
        delete rowColumnTexture;
    }

    // 计算需要多大的分辨率
    int textureWidth, textureHeight;

    ITexture2DCreateInfo textureDefine;

    // 必须是浮点数纹理，因为位置等信息都是浮点数，一个分量是32位的浮点数
    textureDefine.format = toNumber(Format::RGBA32F);

    // 设置纹理过滤，告诉不要进行插值，直接使用最近的像素(相当于在着色器中使用数组index方式访问)
    SamplerInfo sampler;
    sampler.minFilter = TextureFilter::NEAREST;
    sampler.magFilter = TextureFilter::NEAREST;
    textureDefine.sampler = sampler;

    posBuffer = new Texture2d();

    // 一个像素存储两个粒子
    int pixelCount = Math::ceil(particleNum * 0.5f); // count * 2 / 4
    utils::calculateTextureSize(pixelCount, textureWidth, textureHeight, false);
    // printf(">>>>>>>> %d %d %d %d \n",pixelCount,textureWidth,textureHeight,particleNum);
    textureDefine.width = textureWidth;
    textureDefine.height = textureHeight;
    posBuffer->create(&textureDefine);

    rowColumnTexture = new Texture2d();
    rowColumnTexture->create(&textureDefine);
    rowColumnBuffer = new float[textureWidth * textureHeight * 4];

    // printf("Particle renderTextSize count %d  textureWidth %d textureHeight %d\n", particleNum, textureWidth, textureHeight);

    onUpdateMaterial();
}

void ParticlePhysicsSdfRender::onUpdateMaterial()
{
    Material *waterMaterial = component->getMaterial();
    if (waterMaterial)
    {
        if (posBuffer && sdfBufferTexture)
        {
            waterMaterial->setTexture("posBuffer", posBuffer);
            waterMaterial->setTexture("rowColumnTexture", rowColumnTexture);
            float size[2];
            size[0] = posBuffer->getWidth();
            size[1] = posBuffer->getHeight();
            waterMaterial->setProperty("posTexSize", size);

            waterMaterial->setTexture("sdfBufferTexture", sdfBufferTexture);
            size[0] = sdfBufferTexture->getWidth();
            size[1] = sdfBufferTexture->getHeight();
            waterMaterial->setProperty("sdfTexSize", size);
        }
    }
}

ParticlePhysicsSdfRender::~ParticlePhysicsSdfRender()
{

    if (mesh)
    {
        component->setMesh(nullptr);
        delete mesh;
        mesh = nullptr;
    }

    if (posBuffer)
    {

        delete posBuffer;
        posBuffer = nullptr;
    }

    if (vParticleIndex)
    {

        delete vParticleIndex;
        vParticleIndex = nullptr;
    }

    if (sdfBufferTexture)
    {
        delete sdfBufferTexture;
        delete[] sdfBuffer;
        sdfBufferTexture = nullptr;
    }

    if (rowColumnTexture)
    {
        delete rowColumnTexture;
        delete[] rowColumnBuffer;
        rowColumnTexture = nullptr;
    }
}
