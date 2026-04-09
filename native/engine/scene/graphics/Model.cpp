#include "Model.h"

void Model::setCamera(Camera *camera)
{
    for (int i = 0; i < meshInstances.size(); ++i)
    {
        meshInstances[i]->setCamera(camera);
    }
}

void Model::addMeshInstance(MeshInstance *meshInstance)
{
    meshInstances.push_back(meshInstance);
}

void Model::removeMeshInstance(MeshInstance *meshInstance)
{
    for (int i = 0; i < meshInstances.size(); ++i)
    {
        if (meshInstances[i] == meshInstance)
        {
            delete meshInstances[i];
            meshInstances.erase(meshInstances.begin() + i);
            break;
        }
    }
}

Model::~Model()
{
    for (int i = 0; i < meshInstances.size(); ++i)
    {
        delete meshInstances[i];
    }
    meshInstances.clear();
}