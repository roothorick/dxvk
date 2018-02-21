#pragma once

#include <vulkan.h>

// Avoid including DirectX headers
class D3D11Texture2D;

/**
 * \brief Request instance extensions (callback)
 *
 * Registers a callback for when VkInstance creation is about to occur, giving you an opportunity to specify additional
 * instance extensions to be requested.
 * \param [in] cb Callback method to be registered.
 */
void dxvkRegisterInstanceExtCallback(unsigned int(*cb)(char***));

/**
 * \brief Request device extensions (callback)
 *
 * Registers a callback for when VkDevice creation is about to occur, giving you an opportunity to specify additional
 * device extensions to be requested.
 * \param [in] cb Callback method to be registered. The passed parameter is the VkPhysicalDevice the application will use.
 */
void dxvkRegisterDeviceExtCallback(unsigned int(*cb)(VkPhysicalDevice*,char***));

/**
 * \brief Extract the VkImage from a D3D11Texture2D
 *
 * Extracts from the passed D3D11Texture2D the VkImage behind it and additional information critical to its use.
 */
void dxvkGetVulkanImage(D3D11Texture2D* tex,
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
int32_t dxvkPhysicalDeviceToAdapterIdx(VkPhysicalDevice dev);

/**
 * \brief DXGI adapter LUID of a VkPhysicalDevice
 *
 * Maps the passed VkPhysicalDevice to a DXGI adapter LUID.
 * \param [in] physDev the VkPhysicalDevice
 * \param [out] luid the adapter LUID
 */
void dxvkPhysicalDeviceToAdapterLUID(VkPhysicalDevice dev, uint64_t* luid);

/**
 * \brief Get Vulkan handles for doing Vulkan operations yourself
 *
 * Retrieves the instance, physical device, logical device, graphics queue family, and graphics queue associated with 
 * the passed D3D11Device.
 */
void dxvkGetHandlesForVulkanOps(
  D3D11Device* dxdev,
  VkInstance* inst,
  VkPhysicalDevice* pdev,
  VkDevice* ldev,
  uint32_t queueFamily,
  VkQueue* queue
);
