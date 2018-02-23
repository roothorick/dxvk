#pragma once

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllimport)
class D3D11Device;
class D3D11Texture2D;
#endif

/**
 * \brief Extract the VkImage from a D3D11Texture2D
 *
 * Extracts from the passed D3D11Texture2D the VkImage behind it and additional information critical to its use.
 */
extern "C" DLLEXPORT void __stdcall dxvkGetVulkanImage(D3D11Texture2D* tex,
  VkImage* img,
  uint32_t* width,
  uint32_t* height,
  uint32_t* format,
  uint32_t* sampleCt
);

/**
 * \brief Get instance of factory
 *
 * Retrieves the VkInstance behind a DXGIFactory
 * \param [in] fac The DXGIFactory to be resolved
 * \returns Its VkInstance
 */
extern "C" DLLEXPORT VkInstance __stdcall dxvkInstanceOfFactory(IDXGIFactory* fac);

/**
 * \brief DXGI adapter index of a VkPhysicalDevice
 *
 * Maps the passed VkPhysicalDevice to a DXGI adapter index.
 * \param [in] fac DXGIFactory to be used for the lookup
 * \param [in] physDev The VkPhysicalDevice to be looked up
 * \returns The corresponding adapter index
 */
extern "C" DLLEXPORT int32_t __stdcall dxvkPhysicalDeviceToAdapterIdx(IDXGIFactory* fac, VkPhysicalDevice dev);

/**
 * \brief DXGI adapter LUID of a VkPhysicalDevice
 *
 * Maps the passed VkPhysicalDevice to a DXGI adapter LUID.
 * \param [in] physDev the VkPhysicalDevice
 * \param [out] luid the adapter LUID
 */
extern "C" DLLEXPORT void __stdcall dxvkPhysicalDeviceToAdapterLUID(VkPhysicalDevice dev, uint64_t* luid);

/**
 * \brief Get Vulkan handles for doing Vulkan operations yourself
 *
 * Retrieves the instance, physical device, logical device, graphics queue family, and graphics queue associated with 
 * the passed D3D11Device.
 */
extern "C" DLLEXPORT void __stdcall dxvkGetHandlesForVulkanOps(
  D3D11Device* dxdev,
  VkInstance* inst,
  VkPhysicalDevice* pdev,
  VkDevice* ldev,
  uint32_t* queueFamily,
  VkQueue* queue
);
