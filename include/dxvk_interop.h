#pragma once

/**
 * \brief Request instance extensions (callback)
 *
 * Registers a callback for when VkInstance creation is about to occur, giving you an opportunity to specify additional
 * instance extensions to be requested.
 * \param [in] cb Callback method to be registered.
 */
void dxvkRegisterInstanceExtCallback(char**(*cb)());

/**
 * \brief Request device extensions (callback)
 *
 * Registers a callback for when VkDevice creation is about to occur, giving you an opportunity to specify additional
 * device extensions to be requested.
 * \param [in] cb Callback method to be registered. The passed parameter is the VkPhysicalDevice the application will use.
 */
void dxvkRegisterDeviceExtCallback(char**(*cb)(VkPhysicalDevice*));


/**
 * \brief Extract the VkImage from a D3D11Texture2D
 *
 * Extracts from the passed D3D11Texture2D the VkImage behind it and additional information critical to its use.
 */
void dxvkExtractVkImage(D3D11Texture2D tex, VkImage* img, uint32_t width, uint32_t height, uint32_t format, uint32_t sampleCt);

/**
 * \brief DXGI adapter index of a VkPhysicalDevice
 *
 * Maps the passed VkPhysicalDevice to a DXGI adapter index.
 * \param [in] physDev The VkPhysicalDevice
 * \returns the adapter index
 */
int32_t dxvkPhysicalDeviceToAdapterIdx(VkPhysicalDevice);

/**
 * \brief DXGI adapter LUID of a VkPhysicalDevice
 *
 * Maps the passed VkPhysicalDevice to a DXGI adapter LUID.
 * \param [in] physDev the VkPhysicalDevice
 * \param [out] luid the adapter LUID
 */
void dxvkPhysicalDeviceToAdapterLUID(VkPhysicalDevice, uint64_t* luid);
