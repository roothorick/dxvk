#pragma once

#include "d3d11_context.h"

namespace dxvk {
  
  class D3D11CommandList : public D3D11DeviceChild<ID3D11CommandList> {
    
  public:
    
    D3D11CommandList(
            D3D11Device*  pDevice,
            UINT          ContextFlags);
    
    ~D3D11CommandList();
    
    HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID  riid,
            void**  ppvObject) final;
    
    void STDMETHODCALLTYPE GetDevice(
            ID3D11Device **ppDevice) final;
    
    UINT STDMETHODCALLTYPE GetContextFlags() final;
    
    void AddChunk(
            Rc<DxvkCsChunk>&&   Chunk,
            UINT                DrawCount);
    
    void EmitToCommandList(
            ID3D11CommandList*  pCommandList);
    
    void EmitToCsThread(
            DxvkCsThread*       CsThread);
    
    UINT GetDrawCount() const {
      return m_drawCount;
    }
    
  private:
    
    D3D11Device* const m_device;
    UINT         const m_contextFlags;
    UINT               m_drawCount = 0;
    
    std::vector<Rc<DxvkCsChunk>> m_chunks;
    
  };
  
}