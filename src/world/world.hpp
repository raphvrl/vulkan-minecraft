#pragma once

#include <FastNoiseLite.h>

#include <unordered_map>
#include <memory>
#include <unordered_set>
#include <algorithm>

#include "chunk.hpp"
#include "chunk_mesh.hpp"
#include "block.hpp"
#include "block_registry.hpp"
#include "core/camera/camera.hpp"
#include "graphics/vulkan_ctx.hpp"
#include "graphics/pipeline.hpp"
#include "graphics/texture.hpp"

namespace wld
{

class World {
public:
    void init(gfx::VulkanCtx &ctx);
    void destroy();

    void update(const glm::vec3 &playerPos);
    void render(const core::Camera &camera);

    BlockType getBlock(int x, int y, int z) const;

private:
    struct ChunkPos {
        int x, z;

        bool operator==(const ChunkPos &other) const {
            return x == other.x && z == other.z;
        }

        ChunkPos &operator=(const ChunkPos &other) {
            x = other.x;
            z = other.z;
            return *this;
        }
    };

    struct ChunkPosHash {
        std::size_t operator()(const ChunkPos &pos) const {
            return std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.z) << 1);
        }
    };

    void loadChunks(const ChunkPos &pos);
    void unloadChunks(const ChunkPos &pos);
    bool isChunkLoaded(const ChunkPos &pos);
    void generateChunk(Chunk &chunk, const ChunkPos &pos);
    const Chunk *getChunk(const ChunkPos &pos) const;

    static constexpr int RENDER_DISTANCE = 8;
    static constexpr int CHUNKS_PER_FRAME = 1;

    gfx::VulkanCtx *m_ctx;
    BlockRegistry m_blockRegistry;

    gfx::Pipeline m_pipeline;
    gfx::Texture m_texture;
    VkDescriptorSet m_blockSet;

    FastNoiseLite m_noise; 
    FastNoiseLite m_mountainNoise;
    FastNoiseLite m_detailNoise;

    using ChunkMap = std::unordered_map<ChunkPos,
        std::unique_ptr<Chunk>, 
        ChunkPosHash>;
    using ChunkMeshMap = std::unordered_map<ChunkPos, 
        std::unique_ptr<ChunkMesh>, 
        ChunkPosHash>;

    ChunkPos m_playerChunkPos;
    ChunkMap m_chunks;
    ChunkMeshMap m_meshes;
};

} // namespace wld