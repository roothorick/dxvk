#pragma once

#include <dxvk_device.h>

#include "d3d11_device_child.h"
#include "d3d11_util.h"

namespace dxvk {
  
  class D3D11Device;
  
  class D3D11BlendState : public D3D11DeviceChild<ID3D11BlendState1> {
    
  public:
    
    using DescType = D3D11_BLEND_DESC1;
    
    D3D11BlendState(
            D3D11Device*       device,
      const D3D11_BLEND_DESC1& desc);
    ~D3D11BlendState();
    
    HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID  riid,
            void**  ppvObject) final;
    
    void STDMETHODCALLTYPE GetDevice(
            ID3D11Device **ppDevice) final;
    
    void STDMETHODCALLTYPE GetDesc(
            D3D11_BLEND_DESC* pDesc) final;
    
    void STDMETHODCALLTYPE GetDesc1(
            D3D11_BLEND_DESC1* pDesc) final;
    
    void BindToContext(
      const Rc<DxvkContext>&  ctx,
            UINT              sampleMask) const;
    
    static D3D11_BLEND_DESC1 DefaultDesc();
    
    static D3D11_BLEND_DESC1 PromoteDesc(
      const D3D11_BLEND_DESC*   pSrcDesc);
    
    static HRESULT NormalizeDesc(
            D3D11_BLEND_DESC1*  pDesc);
    
  private:
    
    D3D11Device* const            m_device;
    D3D11_BLEND_DESC1             m_desc;
    
    std::array<DxvkBlendMode, 8>  m_blendModes;
    DxvkMultisampleState          m_msState;
    DxvkLogicOpState              m_loState;
    
    static DxvkBlendMode DecodeBlendMode(
      const D3D11_RENDER_TARGET_BLEND_DESC1& BlendDesc);
    
    static VkBlendFactor DecodeBlendFactor(
            D3D11_BLEND BlendFactor,
            bool        IsAlpha);
    
    static VkBlendOp DecodeBlendOp(
            D3D11_BLEND_OP BlendOp);
    
    static VkLogicOp DecodeLogicOp(
            D3D11_LOGIC_OP LogicOp);
    
  };
  
}
