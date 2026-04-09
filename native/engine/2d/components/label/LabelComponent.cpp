#include "LabelComponent.h"
#include "../../../assets/AssetManager.h"
#include "../../assembler/LabelAssembler.h"

LabelComponent::LabelComponent() : UIContentComponent(),
								   labelRender(this)
{
}

// void LabelComponent::setText(std::string &text)
// {
// 	if(this->text != text){
// 		this->text = text;
// 		labelRender.setDirty();
// 	}
// }

void LabelComponent::setText(char *text)
{
	// this->text = text;
	// labelRender.setDirty();
}

void LabelComponent::setHorizontalAlign(HorizontalTextAlignment align)
{
	if (_horizontalAlign != align)
	{
		_horizontalAlign = align;
		labelRender.setDirty();
	}
}

void LabelComponent::setVerticalAlign(VerticalTextAlignment align)
{
	if (_verticalAlign != align)
	{
		_verticalAlign = align;
		labelRender.setDirty();
	}
}

void LabelComponent::setFontSize(int size)
{
	if (fontSize != size)
	{
		fontSize = size;
		labelRender.setDirty();
	}
}

void LabelComponent::setLineHeight(int height)
{
	if (lineHeight != height)
	{
		lineHeight = height;
		labelRender.setDirty();
	}
}

void LabelComponent::setSpacingX(int value)
{
	if (spacingX != value)
	{
		spacingX = value;
		labelRender.setDirty();
	}
};

void LabelComponent::setIsBold(bool b)
{
	if (_isBold != b)
	{
		_isBold = b;
		labelRender.setDirty();
	}
};

void LabelComponent::setIsItalic(bool b)
{
	if (_isItalic != b)
	{
		_isItalic = b;
		labelRender.setDirty();
	}
};

// void LabelComponent::setDirty();()
// {
// 	UIRenderComponent::setDirty();();
// 	labelRender.setDirty();
// }

// void LabelComponent::_render(Batcher2D *batch)
// {
// 	// printf("=================LabelComponent _render %d\n",node->nativeId);
// 	batch->commitTextureModel(this, texture, assembler);
// }

void LabelComponent::setFont(Font *font)
{
	if (this->font != font)
	{
		this->font = font;
		labelRender.setDirty();
	}
}

// void LabelComponent::flushAssembler()
// {
// 	if (!assembler)
// 	{
// 		assembler = LabelAssembler::getInstance();
// 	}
// }

void LabelComponent::enableWrapText(bool e)
{
	if (_enableWrapText != e)
	{
		_enableWrapText = e;
		labelRender.setDirty();
	}
}

bool LabelComponent::canRender()
{
	if (!textTexture || !font || textCodes.size() == 0 && getMaterial() != nullptr)
	{
		return false;
	}
	return true;
}

void LabelComponent::onUpdate()
{
	labelRender.render();
}

void LabelComponent::setIsUnderline(bool b)
{
}

void LabelComponent::setUnderlineHeihgt(float h)
{
}

LabelComponent::~LabelComponent()
{
	if (vertData)
	{
		vertData->recycle();
		vertData = nullptr;
	}
}

void bindingProcessLabelComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{

	if (nodeOp == 1)
	{
		node->addComponent<LabelComponent>();
		return;
	}
	else if (nodeOp == 2)
	{
		node->removeComponent<LabelComponent>();
		return;
	}

	LabelComponent *compnent = node->getComponent<LabelComponent>();
	SQ_ASSERT(compnent);

	if (op < 50)
	{
		processUIContentComponent(compnent, type, nodeOp, op, buffer, node);
		return;
	}

	switch (op)
	{
	case 50:
	{

		// printf("?????????????????????????parse text pos %d \n",buffer.getCurrentPos());
		std::vector<unsigned int> &text = compnent->getTextCode();
		text.clear();
		buffer.popUTFString(text);
		// printf("???parse end %s \n",(unsigned char*)text.data());

		compnent->setDirty();
		// std::vector<unsigned int> text;
		// buffer.popUTFString(text);
		break;
	}
	case 51:
	{
		int assetId = *buffer.popp<int>();
		Font *font = static_cast<Font *>(AssetManager::getInstance()->findById(assetId));
		compnent->setFont(font);
		break;
	}
	case 52:
	{
		compnent->setFontSize(*buffer.popp<float>());
		break;
	}
	case 53:
	{
		compnent->setLineHeight(*buffer.popp<float>());
		break;
	}
	case 54:
	{
		compnent->setSpacingX(*buffer.popp<float>());
		break;
	}
	case 55:
	{
		compnent->enableWrapText(*buffer.popp<char>());
		break;
	}
	case 56:
	{
		compnent->setIsBold(*buffer.popp<char>());
		break;
	}
	case 57:
	{
		compnent->setIsItalic(*buffer.popp<char>());
		break;
	}
	case 58:
	{
		compnent->setIsUnderline(*buffer.popp<char>());
		break;
	}
	case 59:
	{
		compnent->setUnderlineHeihgt(*buffer.popp<float>());
		break;
	}
	default:
		// printf("...op %d \n",op);
		SQ_ASSERT(false);
	}
}