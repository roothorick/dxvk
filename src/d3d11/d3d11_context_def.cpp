#include "d3d11_context_def.h"

namespace dxvk {
  
  D3D11DeferredContext::D3D11DeferredContext(
    D3D11Device*    pParent,
    Rc<DxvkDevice>  Device,
    UINT            ContextFlags)
  : D3D11DeviceContext(pParent, Device),
    m_contextFlags(ContextFlags),
    m_commandList (CreateCommandList()) {
    ClearState();
  }
  
  
  D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE D3D11DeferredContext::GetType() {
    return D3D11_DEVICE_CONTEXT_DEFERRED;
  }
  
  
  UINT STDMETHODCALLTYPE D3D11DeferredContext::GetContextFlags() {
    return m_contextFlags;
  }
  
  
  void STDMETHODCALLTYPE D3D11DeferredContext::Flush() {
    Logger::err("D3D11: Flush called on a deferred context");
  }
  
  
  void STDMETHODCALLTYPE D3D11DeferredContext::ExecuteCommandList(
          ID3D11CommandList*  pCommandList,
          BOOL                RestoreContextState) {
    FlushCsChunk();
    
    static_cast<D3D11CommandList*>(pCommandList)->EmitToCommandList(m_commandList.ptr());
    
    if (RestoreContextState)
      RestoreState();
    else
      ClearState();
  }
  
  
  HRESULT STDMETHODCALLTYPE D3D11DeferredContext::FinishCommandList(
          BOOL                RestoreDeferredContextState,
          ID3D11CommandList   **ppCommandList) {
    FlushCsChunk();
    
    if (ppCommandList != nullptr)
      *ppCommandList = m_commandList.ref();
    m_commandList = CreateCommandList();
    
    if (RestoreDeferredContextState)
      RestoreState();
    else
      ClearState();
    
    m_mappedResources.clear();
    return S_OK;
  }
  
  
  HRESULT STDMETHODCALLTYPE D3D11DeferredContext::Map(
          ID3D11Resource*             pResource,
          UINT                        Subresource,
          D3D11_MAP                   MapType,
          UINT                        MapFlags,
          D3D11_MAPPED_SUBRESOURCE*   pMappedResource) {
    D3D11_RESOURCE_DIMENSION resourceDim = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    pResource->GetType(&resourceDim);
    
    if (pMappedResource != nullptr) {
      pMappedResource->pData      = nullptr;
      pMappedResource->RowPitch   = 0;
      pMappedResource->DepthPitch = 0;
    }
    
    if (MapType == D3D11_MAP_WRITE_DISCARD) {
      D3D11DeferredContextMapEntry entry;
      
      HRESULT status = resourceDim == D3D11_RESOURCE_DIMENSION_BUFFER
        ? MapBuffer(pResource,              MapType, MapFlags, &entry)
        : MapImage (pResource, Subresource, MapType, MapFlags, &entry);
      
      if (FAILED(status))
        return status;
      
      // Adding a new map entry actually overrides the
      // old one in practice because the lookup function
      // scans the array in reverse order
      m_mappedResources.push_back(entry);
      
      // Fill mapped resource structure
      pMappedResource->pData      = entry.DataSlice.ptr();
      pMappedResource->RowPitch   = entry.RowPitch;
      pMappedResource->DepthPitch = entry.DepthPitch;
      return S_OK;
    } else if (MapType == D3D11_MAP_WRITE_NO_OVERWRITE) {
      // The resource must be mapped with D3D11_MAP_WRITE_DISCARD
      // before it can be mapped with D3D11_MAP_WRITE_NO_OVERWRITE.
      auto entry = FindMapEntry(pResource, Subresource);
      
      if (entry == m_mappedResources.rend())
        return E_INVALIDARG;
      
      // Return same memory region as earlier
      entry->MapType = D3D11_MAP_WRITE_NO_OVERWRITE;
      
      pMappedResource->pData      = entry->DataSlice.ptr();
      pMappedResource->RowPitch   = entry->RowPitch;
      pMappedResource->DepthPitch = entry->DepthPitch;
      return S_OK;
    } else {
      // Not allowed on deferred contexts
      return E_INVALIDARG;
    }
  }
  
  
  void STDMETHODCALLTYPE D3D11DeferredContext::Unmap(
          ID3D11Resource*             pResource,
          UINT                        Subresource) {
    D3D11_RESOURCE_DIMENSION resourceDim = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    pResource->GetType(&resourceDim);
    
    auto entry = FindMapEntry(pResource, Subresource);
    
    if (entry == m_mappedResources.rend()) {
      Logger::err("D3D11DeferredContext::Unmap: Subresource not mapped");
      return;
    }
    
    if (entry->MapType == D3D11_MAP_WRITE_DISCARD) {
      if (resourceDim == D3D11_RESOURCE_DIMENSION_BUFFER)
        UnmapBuffer(pResource, &(*entry));
      else
        UnmapImage(pResource, Subresource, &(*entry));
    }
  }
  
  
  HRESULT D3D11DeferredContext::MapBuffer(
          ID3D11Resource*               pResource,
          D3D11_MAP                     MapType,
          UINT                          MapFlags,
          D3D11DeferredContextMapEntry* pMapEntry) {
    const D3D11Buffer* pBuffer = static_cast<D3D11Buffer*>(pResource);
    const Rc<DxvkBuffer> buffer = pBuffer->GetBuffer();
    
    if (!(buffer->memFlags() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
      Logger::err("D3D11: Cannot map a device-local buffer");
      return E_INVALIDARG;
    }
    
    pMapEntry->pResource    = pResource;
    pMapEntry->Subresource  = 0;
    pMapEntry->MapType      = D3D11_MAP_WRITE_DISCARD;
    pMapEntry->RowPitch     = pBuffer->GetSize();
    pMapEntry->DepthPitch   = pBuffer->GetSize();
    pMapEntry->DataSlice    = AllocUpdateBufferSlice(pBuffer->GetSize());
    return S_OK;
  }
  
  
  HRESULT D3D11DeferredContext::MapImage(
          ID3D11Resource*               pResource,
          UINT                          Subresource,
          D3D11_MAP                     MapType,
          UINT                          MapFlags,
          D3D11DeferredContextMapEntry* pMapEntry) {
    const D3D11CommonTexture* pTexture = GetCommonTexture(pResource);
    const Rc<DxvkImage> image = pTexture->GetImage();
    
    if (pTexture->GetMapMode() == D3D11_COMMON_TEXTURE_MAP_MODE_NONE) {
      Logger::err("D3D11: Cannot map a device-local image");
      return E_INVALIDARG;
    }
    
    const DxvkFormatInfo* formatInfo = imageFormatInfo(image->info().format);
    
    VkImageSubresource subresource =
      pTexture->GetSubresourceFromIndex(
        VK_IMAGE_ASPECT_COLOR_BIT, Subresource);
    
    VkExtent3D levelExtent = image->mipLevelExtent(subresource.mipLevel);
    VkExtent3D blockCount = util::computeBlockCount(
      levelExtent, formatInfo->blockSize);
    
    VkDeviceSize eSize = formatInfo->elementSize;
    VkDeviceSize xSize = blockCount.width  * eSize;
    VkDeviceSize ySize = blockCount.height * xSize;
    VkDeviceSize zSize = blockCount.depth  * ySize;
    
    pMapEntry->pResource    = pResource;
    pMapEntry->Subresource  = Subresource;
    pMapEntry->MapType      = D3D11_MAP_WRITE_DISCARD;
    pMapEntry->RowPitch     = xSize;
    pMapEntry->DepthPitch   = ySize;
    pMapEntry->DataSlice    = AllocUpdateBufferSlice(zSize);
    return S_OK;
  }
  
  
  void D3D11DeferredContext::UnmapBuffer(
          ID3D11Resource*               pResource,
    const D3D11DeferredContextMapEntry* pMapEntry) {
    D3D11Buffer* pBuffer = static_cast<D3D11Buffer*>(pResource);
    
    EmitCs([
      cDstBuffer = pBuffer->GetBuffer(),
      cDataSlice = pMapEntry->DataSlice
    ] (DxvkContext* ctx) {
      DxvkPhysicalBufferSlice slice = cDstBuffer->allocPhysicalSlice();
      std::memcpy(slice.mapPtr(0), cDataSlice.ptr(), cDataSlice.length());
      ctx->invalidateBuffer(cDstBuffer, slice);
    });
  }
  
  
  void D3D11DeferredContext::UnmapImage(
          ID3D11Resource*               pResource,
          UINT                          Subresource,
    const D3D11DeferredContextMapEntry* pMapEntry) {
    // TODO If the texture itself is mapped to host-visible
    // memory, write the data slice directly to the image.
    const D3D11CommonTexture* pTexture = GetCommonTexture(pResource);
    
    EmitCs([
      cImage              = pTexture->GetImage(),
      cSubresource        = pTexture->GetSubresourceFromIndex(
        VK_IMAGE_ASPECT_COLOR_BIT, Subresource),
      cDataSlice          = pMapEntry->DataSlice,
      cDataPitchPerRow    = pMapEntry->RowPitch,
      cDataPitchPerLayer  = pMapEntry->DepthPitch
    ] (DxvkContext* ctx) {
      VkImageSubresourceLayers srLayers;
      srLayers.aspectMask     = cSubresource.aspectMask;
      srLayers.mipLevel       = cSubresource.mipLevel;
      srLayers.baseArrayLayer = cSubresource.arrayLayer;
      srLayers.layerCount     = 1;
      
      ctx->updateImage(
        cImage, srLayers, VkOffset3D { 0, 0, 0 },
        cImage->mipLevelExtent(cSubresource.mipLevel),
        cDataSlice.ptr(),
        cDataPitchPerRow,
        cDataPitchPerLayer);
    });
  }
  
  
  Com<D3D11CommandList> D3D11DeferredContext::CreateCommandList() {
    return new D3D11CommandList(m_parent, m_contextFlags);
  }
  
  
  void D3D11DeferredContext::EmitCsChunk(Rc<DxvkCsChunk>&& chunk) {
    m_commandList->AddChunk(std::move(chunk), m_drawCount);
    m_drawCount = 0;
  }

}