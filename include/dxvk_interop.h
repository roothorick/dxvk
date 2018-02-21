#pragma once

#ifndef DXVK_EXPORT
#define DLLEXPORT __declspec(dllimport)
class D3D11Device;
class D3D11Texture2D;
#else
#define DLLEXPORT __declspec(dllexport)
#endif

/**
 * \brief Request instance extensions (callback)
 *
 * Registers a callback for when VkInstance creation is about to occur, giving you an opportunity to specify additional
 * instance extensions to be requested.
 * \param [in] cb Callback method to be registered.
 */
DLLEXPORT void __stdcall dxvkRegisterInstanceExtCallback(unsigned int(*cb)(char***));

/**
 * \brief Request device extensions (callback)
 *
 * Registers a callback for when VkDevice creation is about to occur, giving you an opportunity to specify additional
 * device extensions to be requested.
 * \param [in] cb Callback method to be registered. The passed parameter is the VkPhysicalDevice the application will use.
 */
DLLEXPORT void __stdcall dxvkRegisterDeviceExtCallback(unsigned int(*cb)(VkPhysicalDevice,char***));

/**
 * \brief Extract the VkImage from a D3D11Texture2D
 *
 * Extracts from the passed D3D11Texture2D the VkImage behind it and additional information critical to its use.
 */
DLLEXPORT void __stdcall dxvkGetVulkanImage(D3D11Texture2D* tex,
  VkImage* img,
  uint32_t width,
  uint32_t height,
  uint32_t format,
  uint32_t sampleCt
);

/**
 * \brief DXGI adapter index of a VkPhysicalDevice
 *
 * Maps the passed VkPhysicalDevice to a DXGI adapter index.
 * \param [in] physDev The VkPhysicalDevice
 * \returns the adapter index
 */
DLLEXPORT int32_t __stdcall dxvkPhysicalDeviceToAdapterIdx(VkPhysicalDevice dev);

/**
 * \brief DXGI adapter LUID of a VkPhysicalDevice
 *
 * Maps the passed VkPhysicalDevice to a DXGI adapter LUID.
 * \param [in] physDev the VkPhysicalDevice
 * \param [out] luid the adapter LUID
 */
DLLEXPORT void __stdcall dxvkPhysicalDeviceToAdapterLUID(VkPhysicalDevice dev, uint64_t* luid);

/**
 * \brief Get Vulkan handles for doing Vulkan operations yourself
 *
 * Retrieves the instance, physical device, logical device, graphics queue family, and graphics queue associated with 
 * the passed D3D11Device.
 */
DLLEXPORT void __stdcall dxvkGetHandlesForVulkanOps(
  D3D11Device* dxdev,
  VkInstance* inst,
  VkPhysicalDevice* pdev,
  VkDevice* ldev,
  uint32_t queueFamily,
  VkQueue* queue
);
