#pragma once

#include <df3d/engine/render/Vertex.h>

namespace df3d {

const int DFMESH_MAX_MATERIAL_ID = 128;
const char DFMESH_MAGIC[4] = { 'D', 'F', 'M', 'E' };
const uint16_t DFMESH_VERSION = 1;

// File format:
// |----------------------|
// | DFMeshHeader         |
// |----------------------|
// | DFMeshSubmeshHeader  |
// | chunk vertex data    |
// | chunk index data     |
// |----------------------|
// | DFMeshSubmeshChunk   |
// | etc ...              |
// |----------------------|
// | DFMeshMaterialHeader |
// | material data        |
// | ---------------------|
// TODO: bounding volumes.

#pragma pack(push, 1)

struct DFMeshHeader
{
    uint32_t magic;
    uint16_t version;
    uint16_t vertexFormat;
    uint32_t indexSize;

    // ! Number of submesh chunks.
    uint16_t submeshesCount;
    //! Offset to submeshes data relative to the start.
    uint32_t submeshesOffset;
};

struct DFMeshSubmeshHeader
{
    uint32_t chunkSize;
    uint32_t vertexDataSizeInBytes;
    uint32_t indexDataSizeInBytes;

    char materialId[DFMESH_MAX_MATERIAL_ID];

    // Vertex data.
    // Index data.
};

#pragma pack(pop)

// TODO: refactor.
VertexFormat VertexFormat_dfmesh(uint16_t id);

struct MeshResourceData;
class ResourceDataSource;

MeshResourceData* MeshLoader_dfmesh(ResourceDataSource &dataSource, Allocator &alloc);

}
