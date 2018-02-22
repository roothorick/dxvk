#pragma once

#ifndef DXVK_EXPORT
#define DLLEXPORT __declspec(dllimport)
class D3D11Device;
class D3D11Texture2D;
#else
#define DLLEXPORT __declspec(dllexport)
#endif

/**
 * \brief Callback for providing instance extensions
 *
 * Callback for telling dxvk what instance extensions you need. Pass this to dxvkRegisterInstanceExtCallback().
 * The strings in the array and the array itself will be free()d by dxvk, so do not use them after your
 * callback returns.
 *
 * \param [out] An array of pointers to char arrays of the needed extensions you need
 * \returns The number of needed extensions
 */
typedef unsigned int(*instanceCallback)(char***);

/**
 * \brief Request instance extensions
 *
 * Registers a callback for when VkInstance creation is about to occur, giving you an opportunity to specify additional
 * instance extensions to be requested.
 * \param [in] cb Callback method to be registered.
 */
DLLEXPORT void __stdcall dxvkRegisterInstanceExtCallback(instanceCallback cb);

/**
 * \brief Callback for providing device extensions
 *
 * Callback for telling dxvk what device extensions you need. Pass this to dxvkRegisterDeviceExtCallback().
 * The strings in the array and the array itself will be free()d by dxvk, so do not use them after your
 * callback returns.
 *
 * \param [out] An array of pointers to char arrays of the needed extensions you need
 * \returns The number of needed extensions
 */
typedef unsigned int(*deviceCallback)(VkPhysicalDevice,char***);

/**
 * \brief Request device extensions (callback)
 *
 * Registers a callback for when VkDevice creation is about to occur, giving you an opportunity to specify additional
 * device extensions to be requested.
 * \param [in] cb Callback method to be registered. The passed parameter is the VkPhysicalDevice the application will use.
 */
DLLEXPORT void __stdcall dxvkRegisterDeviceExtCallback(deviceCallback cb);

/**
 * \brief Extract the VkImage from a D3D11Texture2D
 *
 * Extracts from the passed D3D11Texture2D the VkImage behind it and additional information critical to its use.
 */
DLLEXPORT void __stdcall dxvkGetVulkanImage(D3D11Texture2D* tex,
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
DLLEXPORT VkInstance __stdcall dxvkInstanceOfFactory(DXGIFactory* fac);

/**
 * \brief DXGI adapter index of a VkPhysicalDevice
 *
 * Maps the passed VkPhysicalDevice to a DXGI adapter index.
 * \param [in] fac DXGIFactory to be used for the lookup
 * \param [in] physDev The VkPhysicalDevice to be looked up
 * \returns The corresponding adapter index
 */
DLLEXPORT int32_t __stdcall dxvkPhysicalDeviceToAdapterIdx(DXGIFactory* fac, VkPhysicalDevice dev);

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
  uint32_t* queueFamily,
  VkQueue* queue
);
