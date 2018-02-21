#pragma once

#include "vulkan/dxvk_vulkan_extensions.h"

#include <vector>

namespace dxvk::interop {
  
  /**
   * \brief Register callbacks from early init DLLs
   *
   * Loads DLLS from the DXVK_EARLY_INIT_DLLs environment variable, then tries to find callback signatures in their
   * symbol table. If any are found, they're added to the appropriate list automatically.
   */
  void findEarlyInitCallbacks();
  
  /**
   * \brief Get externally required instance extensions
   *
   * This is called by DxvkInstance shortly before VkInstance creation.
   *
   * \returns Merged list of instance extensions requested by external libraries
   */
    vk::NameList getExternalInstanceExtensions();
   
   /**
    * \brief Get externally required device extensions
    *
    * This is called by DxvkAdapter shortly before VkDevice creation.
    *
    * \param [in] physDev The physical device the application intends to use.
    * \returns Merged list of device extensions requested by external libraries
    */
    vk::NameList getExternalDeviceExtensions(VkPhysicalDevice* physDev);
}
