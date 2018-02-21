#include "dxvk_instance.h"

#include "dxvk_interop_internal.h"

#include <cstdlib>
#include <cstring>

namespace dxvk {
  
  DxvkInstance::DxvkInstance()
  : m_vkl(new vk::LibraryFn()),
    m_vki(new vk::InstanceFn(this->createInstance())) {
    
  }
  
  
  DxvkInstance::~DxvkInstance() {
    
  }
  
  
  std::vector<Rc<DxvkAdapter>> DxvkInstance::enumAdapters() {
    uint32_t numAdapters = 0;
    if (m_vki->vkEnumeratePhysicalDevices(m_vki->instance(), &numAdapters, nullptr) != VK_SUCCESS)
      throw DxvkError("DxvkInstance::enumAdapters: Failed to enumerate adapters");
    
    std::vector<VkPhysicalDevice> adapters(numAdapters);
    if (m_vki->vkEnumeratePhysicalDevices(m_vki->instance(), &numAdapters, adapters.data()) != VK_SUCCESS)
      throw DxvkError("DxvkInstance::enumAdapters: Failed to enumerate adapters");
    
    std::vector<Rc<DxvkAdapter>> result;
    for (uint32_t i = 0; i < numAdapters; i++)
      result.push_back(new DxvkAdapter(this, adapters[i]));
    return result;
  }
  
  
  VkInstance DxvkInstance::createInstance() {
    auto enabledLayers     = this->getLayers();
    auto enabledExtensions = this->getExtensions(enabledLayers);
    
    Logger::info("Enabled instance layers:");
    this->logNameList(enabledLayers);
    Logger::info("Enabled instance extensions:");
    this->logNameList(enabledExtensions);
    
    VkApplicationInfo appInfo;
    appInfo.sType                 = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext                 = nullptr;
    appInfo.pApplicationName      = nullptr;
    appInfo.applicationVersion    = 0;
    appInfo.pEngineName           = "DXVK";
    appInfo.engineVersion         = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion            = VK_MAKE_VERSION(1, 0, 47);
    
    VkInstanceCreateInfo info;
    info.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext                    = nullptr;
    info.flags                    = 0;
    info.pApplicationInfo         = &appInfo;
    info.enabledLayerCount        = enabledLayers.size();
    info.ppEnabledLayerNames      = enabledLayers.data();
    info.enabledExtensionCount    = enabledExtensions.size();
    info.ppEnabledExtensionNames  = enabledExtensions.data();
    
    VkInstance result = VK_NULL_HANDLE;
    if (m_vkl->vkCreateInstance(&info, nullptr, &result) != VK_SUCCESS)
      throw DxvkError("DxvkInstance::createInstance: Failed to create Vulkan instance");
    
    // Done with extension names now. We use free() for compatibility with C apps/libraries
    for (auto e: enabledExtensions)
      free( (char*) e);
    
    return result;
  }
  
  
  vk::NameList DxvkInstance::getLayers() {
    std::vector<const char*> layers = { };
    
    if (env::getEnvVar(L"DXVK_DEBUG_LAYERS") == "1")
      layers.push_back("VK_LAYER_LUNARG_standard_validation");
    
    const vk::NameSet layersAvailable
      = vk::NameSet::enumerateInstanceLayers(*m_vkl);
    
    vk::NameList layersEnabled;
    for (auto l : layers) {
      if (layersAvailable.supports(l))
        layersEnabled.push_back(l);
    }
    
    return layersEnabled;
  }
  
  
  vk::NameList DxvkInstance::getExtensions(const vk::NameList& layers) {
    std::vector<const char*> extOptional = { };
    std::vector<const char*> extRequired = { };
    std::vector<const char*> extReqInternal = {
      VK_KHR_SURFACE_EXTENSION_NAME,
      VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
    };
    
    vk::NameList extExternal = interop::getExternalInstanceExtensions();
    
    for (auto e : extReqInternal)
    {
      // Make malloc()d copies so we don't have to distinguish between internal and external when free()ing
      const char* copy = (const char*) malloc(sizeof(char) * strlen(e));
      strcpy( (char*) copy, e);
      extRequired.push_back(copy);
    }
    
    for (auto e: extExternal)
      extRequired.push_back(e);
    
    const vk::NameSet extensionsAvailable
      = vk::NameSet::enumerateInstanceExtensions(*m_vkl, layers);
    vk::NameList extensionsEnabled;
    
    for (auto e : extOptional) {
      if (extensionsAvailable.supports(e))
      {
        // Again, make malloc()d copies so we don't have to distinguish between internal and external when free()ing
        const char* copy = (const char*) malloc(sizeof(char) * strlen(e));
        strcpy( (char*) copy, e);
        extensionsEnabled.push_back(copy);
      }
    }
    
    for (auto e : extRequired) {
      if (!extensionsAvailable.supports(e))
        throw DxvkError(str::format("DxvkInstance::getExtensions: Extension ", e, " not supported"));
      extensionsEnabled.push_back(e);
    }
    
    return extensionsEnabled;
  }
  
  
  void DxvkInstance::logNameList(const vk::NameList& names) {
    for (uint32_t i = 0; i < names.size(); i++)
      Logger::info(str::format("  ", names[i]));
  }
  
}
