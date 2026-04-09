#ifndef _SPINE_MESHISNTANCE_H_
#define _SPINE_MESHISNTANCE_H_
#include <engine/scene/graphics/MeshInstance.h>
#include <engine/assets/Texture2d.h>

class MeshInstanceSpine : public MeshInstance
{
public:
    std::vector<Texture2d *> textures;
    virtual void clear();
};

#endif