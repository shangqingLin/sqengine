#ifndef _RENDERING_GLOBAL_DESCRIPTOR_SET_MANAGER_H_
#define _RENDERING_GLOBAL_DESCRIPTOR_SET_MANAGER_H_

namespace pipeline{
    class BuildInDescriptorSetManager
    {
    public:
       static BuildInDescriptorSetManager* getIntance();
       void initialize();
    };
    
  
    
}

#endif