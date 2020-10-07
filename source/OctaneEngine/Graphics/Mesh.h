/******************************************************************************/
/*!
  \par        Project Octane
  \file       Mesh.h
  \author     Zach Rammell
  \date       2020/10/03
  \brief      Classes to contain information for vertex meshes

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <winrt/base.h>

#include <DirectXMath.h>
#include <d3d11.h>

#include <EASTL/vector.h>
#include <cstdint>

namespace Octane
{

/*!
 * \brief A simple mesh.
 * If the size of index_buffer is zero, it is not indexed.
 */
struct Mesh
{
  /*!
   * \brief One vertex component of a Mesh.
   *  It contains the bare minimum information that every mesh needs.
   *  If you cannot represent your data in this, make a new Vertex and Mesh type along with the DX11 plumbing.
   */
  struct Vertex
  {
    //! Position of the vertex in model space.
    DirectX::XMFLOAT3 position = {};
    //! Normal vector of the vertex.
    DirectX::XMFLOAT3 normal = {};
    //! Texture coordinate of the vertex. TODO: implement texture coordinates
    //DirectX::XMFLOAT2 uv = {};
  };
  using Index = uint32_t;

  //! Holds Mesh::Vertex data CPU-side.
  eastl::vector<Vertex> vertex_buffer;
  //! Holds Mesh::Index data CPU-side. If size() is zero, the mesh is not indexed.
  eastl::vector<Index> index_buffer;
};

/*!
 * \brief A DirectX 11 GPU resource holder corresponding to a Mesh.
 * Created by RenderDX11::CreateMesh.
 */
class MeshDX11
{
  friend class RenderDX11;
  MeshDX11(size_t vertex_size, size_t vertex_count, size_t index_count)
    : vertex_size_ {vertex_size},
      vertex_count_ {vertex_count},
      index_count_ {index_count}
  {
  }
  //! Size of a single vertex. The same GPU resource can be used for multiple sizes of vertex.
  size_t const vertex_size_;
  //! Number of vertices in the mesh.
  size_t vertex_count_;
  //! Number of indices in the mesh. If zero, mesh is not indexed.
  size_t index_count_;
  //! Pointer to the vertex buffer DX11 GPU resource.
  winrt::com_ptr<ID3D11Buffer> vertex_buffer_;
  //! Pointer to the index buffer DX11 GPU resource.
  winrt::com_ptr<ID3D11Buffer> index_buffer_;
};

} // namespace Octane