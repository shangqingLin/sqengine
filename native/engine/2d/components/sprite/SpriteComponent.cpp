#include "SpriteComponent.h"
#include "../Transform2DComponent.h"
#include "../../../assets/AssetManager.h"
#include "../../render/BatcherRenderData.h"
#include "../../render/vertex-format.h"
#include "./assembler/SpriteSlicedAssembly.h"
#include "./assembler/SpriteSimpleAssembly.h"
#include "./assembler/SpriteTiledAssembly.h"
#include "./assembler/SpriteBarFilledAssembly.h"
#include "./assembler/SpriteRadialAssembly.h"
#include "./assembler/SpriteCustomMeshAssembly.h"
#include "../../../assets/BuildinResManager.h"

SpriteComponent::SpriteComponent() : UIContentComponent(),
									 type(SpriteType::SIMPLE),
									 fillType(FillType::HORIZONTAL),
									 assembly(nullptr),
									 fillRange(0.f),
									 fillStart(0.f)
{
}

SpriteComponent::~SpriteComponent()
{
	if (vertexData)
	{
		assembly->destroyRenderData(this);
	}
}

void SpriteComponent::onStart()
{
	vertexRenderData.setDirtyColor();
	if (!assembly)
	{
		updateAssembly();
	}
}

void SpriteComponent::setDirty()
{
	vertexRenderData.setDirtyVertex();
	UIContentComponent::setDirty();
}

void SpriteComponent::setMesh(Mesh *mesh)
{
	// 外部不能设置SpriteComponent的Mesh,如果你是在需要自定义Mesh就用别的组件
	SQ_ASSERT(false);
}

bool SpriteComponent::canRender()
{
	// if(node->nativeId == 62)
	// if(node->testNum != 0)
	// printf("%d assembly %p m %p texture %p size %f %f \n",node->nativeId,assembly,getMaterial() ,spriteFrame.has_value() ? spriteFrame.value().texture : nullptr ,
	// 	 props.width,props.height);

	return assembly != nullptr && spriteFrame.has_value() && spriteFrame.value().texture && getMaterial() != nullptr && props.width > 0.f && props.height > 0.f;
}

void SpriteComponent::setColor(Color &color)
{
	UIContentComponent::setColor(color);
	vertexRenderData.setDirtyColor();
}

void SpriteComponent::updateAssembly()
{
	ISpriteAssembly *newAssembly = nullptr;
	setDirty();
	switch (this->type)
	{
	case SpriteType::SIMPLE:
		newAssembly = SpriteSimpleAssembly::getInstance();
		break;
	case SpriteType::SLICED:
		newAssembly = SpriteSlicedAssembly::getInstance();
		break;
	case SpriteType::TILED:
		newAssembly = SpriteTiledAssembly::getInstance();
		break;
	case SpriteType::FILLED:
		if (fillType == FillType::RADIAL)
		{
			newAssembly = SpriteRadialAssembly::getInstance();
		}
		else
		{
			newAssembly = SpriteBarFilledAssembly::getInstance();
		}
		break;
	case SpriteType::CUSTOM_MESH:
		newAssembly = SpriteCustomMeshAssembly::getInstance();
		break;
	default:
		SQ_ASSERT(false);
	}
	SQ_ASSERT(newAssembly);

	if (assembly && newAssembly != assembly)
	{
		assembly->destroyRenderData(this);
		vertexRenderData.clearCustomData();
	}
	assembly = newAssembly;
	assembly->requestRenderData(this);
}

void SpriteComponent::setSpriteFrame(const SpriteFrame &spriteFrame)
{
	// printf("texture %d %s \n", texture->getId(), texture->getKeyUrl().c_str());
	// if (texture->getKeyUrl() == "map/level/level_2/ziyan_bg_jj")
	// {
	// 	Color color;
	// 	color.r = 1.0;
	// 	color.g = 0.0;
	// 	color.b = 0.0;
	// 	color.a = 1.0;
	// 	setColor(color);
	// }

	vertexRenderData.setDirtyVertex();
	vertexRenderData.setDirtyUv();
	setDirty();
	this->spriteFrame = spriteFrame;

	if (!material)
	{
		setMaterial(BuildinResManager::getMaterial("default-sprite"));
	}
}

void SpriteComponent::setSpriteType(SpriteType type)
{
	if (this->type != type)
	{
		this->type = type;
		updateAssembly();
	}
}

void SpriteComponent::setFillType(FillType type)
{
	if (this->fillType != type)
	{
		this->fillType = type;
		updateAssembly();
	}
}

void SpriteComponent::setFillRange(float value)
{
	if (fillRange != value)
	{
		fillRange = value;
		if (type == SpriteType::FILLED)
			vertexRenderData.setDirtyVertex();
	}
}

void SpriteComponent::setFillStart(float value)
{
	if (fillStart != value)
	{
		fillStart = value;
		if (type == SpriteType::FILLED)
			vertexRenderData.setDirtyVertex();
	}
}

void SpriteComponent::setFillCenter(const Vec2 &value)
{
	if (fillCenter != value)
	{
		fillCenter = value;
		if (type == SpriteType::FILLED)
			vertexRenderData.setDirtyVertex();
	}
}

void SpriteComponent::setMeshData(const float *vertexData, const float *uvData, int vertexNum, const unsigned short *indexData, int indexNum)
{
	SQ_ASSERT(type == SpriteType::CUSTOM_MESH);

	// 由于uv是不变的，所以直接存储中chunk中就行
	// 而顶点数据是与Node Transform相关的，所以这里需要存储一下原始顶点数据

	{
		float *newVertexList = nullptr;
		float vertextNumerCount = vertexNum * 2;
		if (vertexRenderData.customVertexList != nullptr)
		{
			if (this->vertexRenderData.customVertexListCount >= vertextNumerCount)
			{
				newVertexList = vertexRenderData.customVertexList;
			}
			else
			{
				delete[] vertexRenderData.customVertexList;
			}
		}

		if (newVertexList == nullptr)
		{
			// 如果vertexList足够存储，那么直接使用vertexList，节省内存
			// vertexList为32个数字，即8个顶点
			if (vertexNum <= 8)
			{
				newVertexList = vertexRenderData.vertexList;
			}
			else
			{
				newVertexList = new float[vertextNumerCount];
				vertexRenderData.customVertexList = newVertexList;
				vertexRenderData.customVertexListCount = vertextNumerCount;
			}
		}

		this->vertexRenderData.vertexNum = vertexNum;
		// for (int i = 0; i < vertexNum * 2; ++i)
		// {
		// 	newVertexList[i] = vertexData[i];
		// }
		memcpy((void *)newVertexList, (void *)vertexData, vertexNum * 2 * sizeof(float));
	}

	{
		unsigned short *newIndexList = nullptr;
		if (vertexRenderData.customIndexList != nullptr)
		{
			if (indexNum <= vertexRenderData.customIndexListCount)
			{
				newIndexList = vertexRenderData.customIndexList;
			}
			else
			{
				delete[] vertexRenderData.customIndexList;
				vertexRenderData.customIndexList = nullptr;
			}
		}

		if (newIndexList == nullptr)
		{
			vertexRenderData.customIndexList = new unsigned short[indexNum];
			vertexRenderData.customIndexListCount = indexNum;
			newIndexList = vertexRenderData.customIndexList;
		}
		vertexRenderData.customIndexNum = indexNum;
		memcpy((void *)newIndexList, (void *)indexData, indexNum * sizeof(unsigned short));
	}

	static_cast<SpriteCustomMeshAssembly *>(assembly)->updateMesh(this, uvData);
	setDirty();
}

void bindingProcessSpriteComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{

	// printf("SpriteComponent %d %d %p\n",nodeOp,op,node);

	if (nodeOp == 1)
	{
		node->addComponent<SpriteComponent>();
		return;
	}
	else if (nodeOp == 2)
	{
		node->removeComponent<SpriteComponent>();
		return;
	}

	SpriteComponent *sprite = node->getComponent<SpriteComponent>();
	SQ_ASSERT(sprite);
	if (op < 50)
	{
		processUIContentComponent(sprite, type, nodeOp, op, buffer, node);
		return;
	}

	switch (op)
	{
	case 50:
	{
		char state = *buffer.popp<char>();

		// printf("ffffffffffffffff %d \n",buffer.getCurrentPos());
		if (state == 0)
		{
			sprite->spriteFrame.reset();
		}
		else
		{
			SpriteFrame spriteFrame;

			int assetId = *buffer.popp<int>();
			Texture2d *texture = static_cast<Texture2d *>(AssetManager::getInstance()->findById(assetId));
			// printf("++++++++++++++++++++++ bindingProcessSpriteComponent set texture %d %d %d %p %p \n", state,node->nativeId, assetId, sprite, texture);
			SQ_ASSERT(texture);

			spriteFrame.texture = texture;
			spriteFrame.rect.x = *buffer.popp<int>();
			spriteFrame.rect.y = *buffer.popp<int>();
			spriteFrame.rect.width = *buffer.popp<int>();
			spriteFrame.rect.height = *buffer.popp<int>();
			spriteFrame.rotate = *buffer.popp<char>();

			for (int i = 0; i < 8; ++i)
			{
				spriteFrame.uv[i] = *buffer.popp<float>();
			}

			if (state & (1 << 2))
			{
				spriteFrame.grid9 = Vec4();
				spriteFrame.grid9.value().x = *buffer.popp<float>();
				spriteFrame.grid9.value().y = *buffer.popp<float>();
				spriteFrame.grid9.value().z = *buffer.popp<float>();
				spriteFrame.grid9.value().w = *buffer.popp<float>();

				spriteFrame.slicedUv = SpriteFrame::SpriteFrameSlicedUv();
				int uvCount = *buffer.popp<char>();
				for (int i = 0; i < uvCount; ++i)
				{
					spriteFrame.slicedUv.value().uv[i] = *buffer.popp<float>();
				}
			}

			sprite->setSpriteFrame(spriteFrame);
		}
		break;
	}
	case 51:
	{
		sprite->setSpriteType(SpriteType(*buffer.popp<char>()));
		break;
	}
	case 52:
	{
		sprite->setFillType(FillType(*buffer.popp<char>()));
		break;
	}
	case 56:
	{
		sprite->setFillCenter(Vec2(*buffer.popp<float>(), *buffer.popp<float>()));
		break;
	}
	case 57:
	{
		sprite->setFillStart(*buffer.popp<float>());
		break;
	}
	case 58:
	{
		sprite->setFillRange(*buffer.popp<float>());
		break;
	}
	default:
		SQ_ASSERT(false);
	}
}
