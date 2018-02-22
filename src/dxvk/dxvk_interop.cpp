#include "dxvk_interop_internal.h"

#include "../d3d11/d3d11_device.h"
#include "../d3d11/d3d11_texture.h"
#include "../dxgi/dxgi_factory.h"

using namespace dxvk;

#define DXVK_EXPORT
#include <dxvk_interop.h>

#include <vector>
#include <functional>
#include <cstring>
#include <cstdlib>

// Doesn't work with the typedefs. Not easy to fix.
std::vector<std::function<unsigned int(char***)>> instCallbacks;
std::vector<std::function<unsigned int(VkPhysicalDevice, char***)>> devCallbacks;

DLLEXPORT void __stdcall dxvkRegisterInstanceExtCallback(instanceCallback cb)
{
  Logger::trace("Adding instance cb");
  std::function<unsigned int(char***)> fnobj = *cb;
  instCallbacks.push_back(fnobj);
}

DLLEXPORT void __stdcall dxvkRegisterDeviceExtCallback(deviceCallback cb)
{
  Logger::trace("Adding device cb");
  std::function<unsigned int(VkPhysicalDevice, char***)> fnobj = *cb;
  devCallbacks.push_back(fnobj);
}

DLLEXPORT void __stdcall dxvkGetVulkanImage(D3D11Texture2D* tex,
    VkImage* img,
    uint32_t* width,
    uint32_t* height,
    uint32_t* format,
    uint32_t* sampleCt
  )
{
  D3D11_TEXTURE2D_DESC* desc = tex->GetDescInternal();
  D3D11TextureInfo* texInfo = tex->GetTextureInfo();
  
  *img = texInfo->image->handle();
  *width = desc->Width;
  *height = desc->Height;
  *format = texInfo->image->info().format;
  *sampleCt = texInfo->image->info().sampleCount;
}

DLLEXPORT VkInstance __stdcall dxvkInstanceOfFactory(IDXGIFactory* fac)
{
  // XXX: I don't actually know how safe this is
  return ( (DxgiFactory*) fac )->GetInstanceInternal();
}

DLLEXPORT int32_t __stdcall dxvkPhysicalDeviceToAdapterIdx(IDXGIFactory* fac, VkPhysicalDevice dev)
{
  UINT i=0;
  HRESULT hr = S_OK;
  while(hr != DXGI_ERROR_NOT_FOUND)
  {
    DxgiAdapter* adp;
    hr = fac->EnumAdapters(i, (IDXGIAdapter**) &adp);
    if( adp->GetDXVKAdapterInternal()->handle() == dev)
      return i;
    
    i++;
  }
  
  // Not found
  return -1;
}

DLLEXPORT void __stdcall dxvkPhysicalDeviceToAdapterLUID(VkPhysicalDevice dev, uint64_t* luid)
{
  // TODO
  Logger::err("dxvkPhysicalDeviceToAdapterLUID: stub");
  *luid = 0;
}

DLLEXPORT void __stdcall dxvkGetHandlesForVulkanOps(
    D3D11Device* dxdev,
    VkInstance* inst,
    VkPhysicalDevice* pdev,
    VkDevice* ldev,
    uint32_t* queueFamily,
    VkQueue* queue
  )
{
  Rc<DxvkDevice> realdev = dxdev->GetDXVKDevice();
  Rc<DxvkAdapter> adp = realdev->adapter();
  
  *inst = adp->instance()->handle();
  *pdev = adp->handle();
  *ldev = realdev->handle();
  *queueFamily = adp->graphicsQueueFamily();
  *queue = realdev->getGraphicsQueue();
}

namespace dxvk::interop {
  void findEarlyInitCallbacks()
  {
    // TODO
  }
  
  vk::NameList getExternalInstanceExtensions()
  {
    vk::NameList ret;
    
    for(unsigned int i=0;i<instCallbacks.size();i++)
    {
      char** newExts;
      unsigned int newExtCt = instCallbacks[i](&newExts);
      
      for(unsigned int j=0;j<newExtCt;j++)
        ret.push_back(newExts[j]);

      // Freeing just the array itself. It is expected that the caller malloc()d the individual strings separately.
      free(newExts);
    }
    
    return ret;
  }
  
  vk::NameList getExternalDeviceExtensions(VkPhysicalDevice* physDev)
  {
    vk::NameList ret;
    
    for(unsigned int i=0;i<devCallbacks.size();i++)
    {
      char** newExts;
      unsigned int newExtCt = devCallbacks[i](*physDev,&newExts);
      
      for(unsigned int j=0;j<newExtCt;j++)
        ret.push_back(newExts[j]);
        
      // Freeing just the array itself. It is expected that the caller malloc()d the individual strings separately.
      free(newExts);
    }
    
    return ret;
  }
}
