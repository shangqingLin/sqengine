#pragma once

#include "Material.h"
#include <unordered_map>

class BuildinResManager
{
private:
public:
    static void onGetAsset(std::string, int, int);
    static Material *getMaterial(std::string name);
};
