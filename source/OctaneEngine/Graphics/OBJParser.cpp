#include <OctaneEngine/Graphics/OBJParser.h>

#include <algorithm>
#include <cstdio>
#include <cctype>
#include <cinttypes>

#include <OctaneEngine/Math.h>

namespace
{

void ReadUntil(FILE* fp, char const c)
{
  int read = fgetc(fp);
  while (read != c && read != EOF)
  {
    read = fgetc(fp);
  }
}

size_t WhitespaceCountUntil(FILE* fp, char const c)
{
  size_t whitespace_count = 0;
  bool is_new_whitespace = true;
  int read = fgetc(fp);
  while (read != c && read != EOF)
  {
    int const is_whitespace = isspace(read);
    if (is_whitespace && is_new_whitespace)
    {
      is_new_whitespace = false;
      ++whitespace_count;
    }
    else if (!is_whitespace)
    {
      is_new_whitespace = true;
    }
    read = fgetc(fp);
  }
  return whitespace_count;
}

// Parse an index of one of the following forms:
// "i0"
// "i0/i1"
// "i0/i1/i2"
Octane::Mesh::Index ParseIndex(FILE* fp)
{
  Octane::Mesh::Index idx[3] = { 0 };
  int current_parsing = 0;

  while (true)
  {
    int c = fgetc(fp);
    if (isdigit(c))
    {
      idx[current_parsing] *= 10;
      idx[current_parsing] += c - '0';
    }
    else if (c == '/')
    {
      ++current_parsing;
    }
    else if (isspace(c))
    {
      if (c == '\n')
      {
        ungetc(c, fp);
      }
      break;
    }
    else if (c == EOF)
    {
      break;
    }
  }

  // OBJ is 1-indexed, this makes it 0-indexed
  return idx[0] - 1;
}

}

namespace Octane
{

Mesh OBJParser::ParseOBJ(wchar_t const* filepath)
{
  FILE* fp;
  if (_wfopen_s(&fp, filepath, L"rt") != 0)
  {
    _wperror(L"fopen() failed");
    assert(!"Failed to open file.");
  }
  Mesh mesh{};

  // Initial parsing pass to count number of vertices and indices
  {
    bool initial_parsing = true;
    while (initial_parsing)
    {
      switch (fgetc(fp))
      {
      case '#':
      {
        ReadUntil(fp, '\n');
      } break;
      case 'v':
      {
        if (!isalpha(fgetc(fp))) // don't increase vertex count for vt or vn
        {
          ++(mesh.vertex_count);
        }
        ReadUntil(fp, '\n');
      } break;
      case 'f':
      {
        mesh.index_count += WhitespaceCountUntil(fp, '\n');
      } break;
      case EOF:
      {
        initial_parsing = false;
      } break;
      case'\n': break;
      default:
      {
        ReadUntil(fp, '\n');
      } break;
      }
    }
  }

  // Reset file to beginning
  clearerr(fp);
  fseek(fp, 0, SEEK_SET);

  // Allocate buffers for the mesh's vertices and indices
  mesh.vertex_buffer = new Mesh::Vertex[mesh.vertex_count];
  mesh.index_buffer = new Mesh::Index[mesh.index_count];

  DirectX::XMFLOAT3 mn{ FLT_MAX, FLT_MAX , FLT_MAX };
  DirectX::XMFLOAT3 mx{ -FLT_MAX , -FLT_MAX , -FLT_MAX };

  // Parse data and fill buffers
  {
    size_t vertices_processed = 0;
    size_t indices_processed = 0;
    bool parsing = true;
    while (parsing)
    {
      switch (fgetc(fp))
      {
      case '#':
      {
        ReadUntil(fp, '\n');
      } break;
      case 'v':
      {
        if (!isalpha(fgetc(fp))) // don't parse vt or vn as vertices
        {
          DirectX::XMFLOAT3* position = &(mesh.vertex_buffer[vertices_processed].position);
          fscanf_s(fp, "%f %f %f", &(position->x), &(position->y), &(position->z));

          mn.x = std::min(mn.x, position->x);
          mn.y = std::min(mn.y, position->y);
          mn.z = std::min(mn.z, position->z);

          mx.x = std::max(mx.x, position->x);
          mx.y = std::max(mx.y, position->y);
          mx.z = std::max(mx.z, position->z);

          ++vertices_processed;
        }
        ReadUntil(fp, '\n');
      } break;
      case 'f':
      {
        // discard space
        (fgetc(fp));
        Mesh::Index indices[3];
        for (int i = 0; i < 3; ++i)
        {
          indices[i] = ParseIndex(fp);
          mesh.index_buffer[indices_processed + i] = indices[i];
        }

        {
          using namespace DirectX;
          XMVECTOR positions[3];
          for (int i = 0; i < 3; ++i)
          {
            positions[i] = XMLoadFloat3(&(mesh.vertex_buffer[indices[i]].position));
          }
          XMVECTOR const face_normal = XMVector3Cross(positions[0] - positions[1], positions[2] - positions[1]);
          for (int i = 0; i < 3; ++i)
          {
            XMVECTOR const added_normal = XMLoadFloat3(&(mesh.vertex_buffer[indices[i]].normal)) + face_normal;
            XMStoreFloat3(&(mesh.vertex_buffer[indices[i]].normal), added_normal);
          }
        }

        indices_processed += 3;
        ReadUntil(fp, '\n');
      } break;
      case EOF:
      {
        parsing = false;
      } break;
      case'\n': break;
      default:
      {
        ReadUntil(fp, '\n');
      } break;
      }
    }

  }

  float scale = 1.0f / std::max(mx.x - mn.y, std::max(mx.y - mn.y, mx.z - mn.z));
  DirectX::XMMATRIX normalizing_matrix = DirectX::XMMatrixTranslation(-avg(mn.x, mx.x), -avg(mn.y, mx.y), -avg(mn.z, mx.z));
  normalizing_matrix *= DirectX::XMMatrixScaling(scale, scale, scale);
  for (size_t i = 0; i < mesh.vertex_count; ++i)
  {
    DirectX::XMStoreFloat3(&(mesh.vertex_buffer[i].position),
      DirectX::XMVector3Transform(
        DirectX::XMLoadFloat3(&(mesh.vertex_buffer[i].position)),
        normalizing_matrix
      )
    );
  }

  return mesh;
}

}
