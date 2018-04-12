#pragma once

#include "../dxvk/dxvk_shader.h"

#include "dxbc_chunk_isgn.h"
#include "dxbc_chunk_shex.h"
#include "dxbc_header.h"
#include "dxbc_options.h"
#include "dxbc_reader.h"

// References used for figuring out DXBC:
// - https://github.com/tgjones/slimshader-cpp
// - Wine

namespace dxvk {
  
  class DxbcAnalyzer;
  class DxbcCompiler;
  
  /**
   * \brief DXBC shader module
   * 
   * Reads the DXBC byte code and extracts information
   * about the resource bindings and the instruction
   * stream. A module can then be compiled to SPIR-V.
   */
  class DxbcModule {
    
  public:
    
    DxbcModule(DxbcReader& reader);
    ~DxbcModule();
    
    /**
     * \brief Shader type and version
     * \returns Shader type and version
     */
    DxbcProgramVersion version() const {
      return m_shexChunk->version();
    }
    
    /**
     * \brief Input and output signature chunks
     * 
     * Parts of the D3D11 API need access to the
     * input or output signature of the shader.
     */
    Rc<DxbcIsgn> isgn() const { return m_isgnChunk; }
    Rc<DxbcIsgn> osgn() const { return m_osgnChunk; }
    
    /**
     * \brief Compiles DXBC shader to SPIR-V module
     * 
     * \param [in] options DXBC compiler options
     * \returns The compiled shader object
     */
    Rc<DxvkShader> compile(const DxbcOptions& options) const;
    
  private:
    
    DxbcHeader m_header;
    
    Rc<DxbcIsgn> m_isgnChunk;
    Rc<DxbcIsgn> m_osgnChunk;
    Rc<DxbcShex> m_shexChunk;
    
    void runAnalyzer(
            DxbcAnalyzer&       analyzer,
            DxbcCodeSlice       slice) const;
    
    void runCompiler(
            DxbcCompiler&       compiler,
            DxbcCodeSlice       slice) const;
    
  };
  
}