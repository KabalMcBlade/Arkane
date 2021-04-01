#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Defines.h"
#include "../Core/SmartPointers.h"

#include "Buffers/VertexBufferObject.h"
#include "Buffers/IndexBufferObject.h"
#include "Buffers/UniformBufferObject.h"

#include <string>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <atomic>


#pragma warning(disable : 4251)


AK_NAMESPACE_BEGIN


typedef uint64_t AK_MEMORY_ALIGNED VertexCacheHandle;

enum class ECacheType
{
	ECacheType_Vertex,
	ECacheType_Index,
	ECacheType_Joint
};

struct GeometryBufferSet 
{
	IndexBufferObject m_indexBuffer;
	VertexBufferObject m_vertexBuffer;
	UniformBufferObject m_jointBuffer;
	uint8_t* m_mappedVertexBase;
	uint8_t* m_mappedIndexBase;
	uint8_t* m_mappedJointBase;
	std::atomic_size_t	m_indexMemUsed;
	std::atomic_size_t	m_vertexMemUsed;
	std::atomic_size_t	m_jointMemUsed;
	uint32_t m_allocations;	// number of index and vertex allocations combined
};

class AK_DLL VertexCache 
{
public:
	static constexpr uint32_t kCacheIndexMemoryPerFrame = 63 * 1024 * 1024;
	static constexpr uint32_t kCacheVertexMemoryPerFrame = 63 * 1024 * 1024;
	static constexpr uint32_t kCacheJointMemoryPerFrame = 256 * 1024;


	static constexpr uint32_t kStaticIndexMemory = 63 * 1024 * 1024;
	static constexpr uint32_t kStaticVertexMemory = 63 * 1024 * 1024;			// make sure it fits in kVertexCacheOffsetMask!

	static constexpr uint32_t kCacheStatic = 1;				// in the static set, not the per-frame set

	// this packed the 64 bits VertexCacheHandle type
	static constexpr uint32_t kCacheSizeShift = 1;
	static constexpr uint32_t kCacheSizeMask = 0x7fffff;		// 8 MB 

	static constexpr uint32_t kCacheOffsetShift = 24;
	static constexpr uint32_t kCacheOffsetMask = 0x3ffffff;	// 64 MB 

	static constexpr uint32_t kCacheFrameShift = 49;
	static constexpr uint32_t kCacheFrameMask = 0x7fff;		// 15 bits = 32k frames to wrap around


	static constexpr uint32_t kVertexCacheAlign = 16;
	static constexpr uint32_t kIndexCacheAlign = 16;
	//static constexpr uint32_t kJointCacheAlign = 16;	// this one is using the properties from the device!


public:
	static AK_INLINE bool CacheIsStatic(const VertexCacheHandle _handle) { return (_handle & kCacheStatic) != 0; }

public:
	VertexCache(size_t _uniformBufferOffsetAlignment);
	~VertexCache();

	void PurgeAll();

	// call on loading a new level
	void FreeStaticData();

	// this data is only valid for one frame of rendering
	VertexCacheHandle AllocVertex(const void* _data, uint32_t _num, size_t _size);
	VertexCacheHandle AllocIndex(const void* _data, uint32_t _num, size_t _size);
	VertexCacheHandle AllocJoint(const void* _data, uint32_t _num, size_t _size);

	// this data is valid until the next level load
	VertexCacheHandle AllocStaticVertex(const void* _data, uint32_t _bytes);
	VertexCacheHandle AllocStaticIndex(const void* _data, uint32_t _bytes);

	uint8_t* MappedVertexBuffer(VertexCacheHandle _handle);
	uint8_t* MappedIndexBuffer(VertexCacheHandle _handle);

	// Returns false if it's been purged
	bool CacheIsCurrent(const VertexCacheHandle _handle);

	// vbo and ibo and ubo(jbo) as temporary reference so don't store it
	bool GetVertexBuffer(VertexCacheHandle _handle, VertexBufferObject* _vb);
	bool GetIndexBuffer(VertexCacheHandle _handle, IndexBufferObject* _ib);
	bool GetJointBuffer(VertexCacheHandle _handle, UniformBufferObject* _jb);

	void Update();

private:
	void CreateAll();
	void DestroyAll();

private:
	uint32_t m_currentFrame;	// for determining the active buffers
	uint32_t m_listNum;			// currentFrame % Frame::kNumOfFramesInFlight
	uint32_t m_drawListNum;		// (currentFrame - 1) % Frame::kNumOfFramesInFlight

	SharedPtr<GeometryBufferSet> m_staticData;
	std::vector<SharedPtr<GeometryBufferSet>> m_frameData;

	size_t m_uniformBufferOffsetAlignment;

	// High water marks for the per-frame buffers
	uint32_t m_mostUsedVertex;
	uint32_t m_mostUsedIndex;
	uint32_t m_mostUsedJoint;

	// Try to make room for bytes
	VertexCacheHandle Alloc(SharedPtr<GeometryBufferSet> _gbs, const void* _data, size_t _bytes, ECacheType _type);
};

AK_NAMESPACE_END