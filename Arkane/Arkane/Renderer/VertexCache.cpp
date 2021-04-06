#include "VertexCache.h"

#include "../Core/Assertions.h"

#include "Frame.h"


AK_NAMESPACE_BEGIN

static void ClearGeoBufferSet(SharedPtr<GeometryBufferSet> _gbs)
{
	_gbs->m_indexMemUsed = 0;
	_gbs->m_vertexMemUsed = 0;
	_gbs->m_jointMemUsed = 0;
	_gbs->m_allocations = 0;
}

static void MapGeoBufferSet(SharedPtr<GeometryBufferSet> _gbs)
{
	if (_gbs->m_mappedVertexBase == nullptr)
	{
		_gbs->m_mappedVertexBase = (uint8_t*)_gbs->m_vertexBuffer.MapBuffer(EBufferMappingType::EBufferMappingType_Write);
	}

	if (_gbs->m_mappedIndexBase == nullptr)
	{
		_gbs->m_mappedIndexBase = (uint8_t*)_gbs->m_indexBuffer.MapBuffer(EBufferMappingType::EBufferMappingType_Write);
	}

	if (_gbs->m_mappedJointBase == nullptr && _gbs->m_jointBuffer.GetAllocedSize() != 0)
	{
		_gbs->m_mappedJointBase = (uint8_t*)_gbs->m_jointBuffer.MapBuffer(EBufferMappingType::EBufferMappingType_Write);
	}
}

static void UnmapGeoBufferSet(SharedPtr<GeometryBufferSet> _gbs)
{
	if (_gbs->m_mappedVertexBase != nullptr)
	{
		_gbs->m_vertexBuffer.UnmapBuffer();
		_gbs->m_mappedVertexBase = nullptr;
	}

	if (_gbs->m_mappedIndexBase != nullptr)
	{
		_gbs->m_indexBuffer.UnmapBuffer();
		_gbs->m_mappedIndexBase = nullptr;
	}

	if (_gbs->m_mappedJointBase != nullptr)
	{
		_gbs->m_jointBuffer.UnmapBuffer();
		_gbs->m_mappedJointBase = nullptr;
	}
}

static void AllocGeoBufferSet(SharedPtr<GeometryBufferSet> _gbs, const uint32_t vertexBytes, const uint32_t indexBytes, const uint32_t jointBytes, EBufferUsage usage)
{
	_gbs->m_vertexBuffer.AllocBufferObject(nullptr, vertexBytes, usage);
	_gbs->m_indexBuffer.AllocBufferObject(nullptr, indexBytes, usage);
	
	if (jointBytes > 0)
	{
		_gbs->m_jointBuffer.AllocBufferObject(nullptr, jointBytes, usage);
	}

	ClearGeoBufferSet(_gbs);
}

//////////////////////////////////////////////////////////////////////////

VertexCache::VertexCache(size_t _uniformBufferOffsetAlignment) : m_uniformBufferOffsetAlignment(_uniformBufferOffsetAlignment)
{
	CreateAll();
}

VertexCache::~VertexCache()
{
	DestroyAll();
}

void VertexCache::CreateAll()
{
	m_currentFrame = 0;
	m_listNum = 0;

	m_mostUsedVertex = 0;
	m_mostUsedIndex = 0;
	m_mostUsedJoint = 0;

	m_frameData.resize(Frame::kNumOfFramesInFlight);
	for (size_t i = 0; i < Frame::kNumOfFramesInFlight; ++i)
	{
		m_frameData[i] = MakeSharedPtr<GeometryBufferSet>();
		AllocGeoBufferSet(m_frameData[i], kCacheVertexMemoryPerFrame, kCacheIndexMemoryPerFrame, kCacheJointMemoryPerFrame, EBufferUsage::EBufferUsage_Dynamic);
	}

	m_staticData = MakeSharedPtr<GeometryBufferSet>();
#if 1
	AllocGeoBufferSet(m_staticData, kStaticVertexMemory, kStaticIndexMemory, 0, EBufferUsage::EBufferUsage_Static);
#else
	AllocGeoBufferSet(m_staticData, kStaticVertexMemory, kStaticIndexMemory, 0, EBufferUsage::EBufferUsage_Dynamic);
#endif

	MapGeoBufferSet(m_frameData[m_listNum]);
}

void VertexCache::DestroyAll()
{
	m_staticData->m_vertexBuffer.FreeBufferObject();
	m_staticData->m_indexBuffer.FreeBufferObject();
	m_staticData->m_jointBuffer.FreeBufferObject();
	m_staticData.reset();

	for (size_t i = 0; i < Frame::kNumOfFramesInFlight; ++i)
	{
		m_frameData[i]->m_vertexBuffer.FreeBufferObject();
		m_frameData[i]->m_indexBuffer.FreeBufferObject();
		m_frameData[i]->m_jointBuffer.FreeBufferObject();
		m_frameData[i].reset();
	}
	m_frameData.clear();
}

void VertexCache::PurgeAll() 
{
	DestroyAll();
	CreateAll();
}

void VertexCache::FreeStaticData()
{
	ClearGeoBufferSet(m_staticData);
	m_mostUsedVertex = 0;
	m_mostUsedIndex = 0;
	m_mostUsedJoint = 0;
}

VertexCacheHandle VertexCache::Alloc(SharedPtr<GeometryBufferSet> _gbs, const void* _data, size_t _bytes, ECacheType _type)
{
	if (_bytes == 0)
	{
		return (VertexCacheHandle)0;
	}


	akAssertReturnValue((((uintptr_t)(_data)) & 15) == 0, (VertexCacheHandle)0, "Pointer is misaligned");
	akAssertReturnValue((_bytes & 15) == 0, (VertexCacheHandle)0, "size is misaligned");


	size_t endPos = 0;
	size_t offset = 0;

	switch (_type)
	{
	case ECacheType::ECacheType_Index: 
	{
		_gbs->m_indexMemUsed.fetch_add(_bytes, std::memory_order_relaxed);
		endPos = _gbs->m_indexMemUsed.load();
		if (endPos > _gbs->m_indexBuffer.GetAllocedSize())
		{
			akAssertReturnValue(false, (VertexCacheHandle)0, "Out of index cache");
		}

		offset = endPos - _bytes;

		if (_data != nullptr)
		{
			if (_gbs->m_indexBuffer.GetUsage() == EBufferUsage::EBufferUsage_Dynamic)
			{
				MapGeoBufferSet(_gbs);
			}
			_gbs->m_indexBuffer.Update(_data, _bytes, offset);
		}
		break;
	}
	case ECacheType::ECacheType_Vertex: 
	{
		_gbs->m_vertexMemUsed.fetch_add(_bytes, std::memory_order_relaxed);
		endPos = _gbs->m_vertexMemUsed.load();
		if (endPos > _gbs->m_vertexBuffer.GetAllocedSize())
		{
			akAssertReturnValue(false, (VertexCacheHandle)0, "Out of vertex cache");
		}

		offset = endPos - _bytes;

		if (_data != nullptr)
		{
			if (_gbs->m_vertexBuffer.GetUsage() == EBufferUsage::EBufferUsage_Dynamic)
			{
				MapGeoBufferSet(_gbs);
			}
			_gbs->m_vertexBuffer.Update(_data, _bytes, offset);
		}

		break;
	}
	case ECacheType::ECacheType_Joint:
	{
		_gbs->m_jointMemUsed.fetch_add(_bytes, std::memory_order_relaxed);
		endPos = _gbs->m_jointMemUsed.load();
		if (endPos > _gbs->m_jointBuffer.GetAllocedSize())
		{
			akAssertReturnValue(false, (VertexCacheHandle)0, "Out of joint cache");
		}

		offset = endPos - _bytes;

		if (_data != nullptr)
		{
			if (_gbs->m_jointBuffer.GetUsage() == EBufferUsage::EBufferUsage_Dynamic)
			{
				MapGeoBufferSet(_gbs);
			}
			_gbs->m_jointBuffer.Update(_data, _bytes, offset);
		}

		break;
	}
	default:
		akAssertReturnValue(false, (VertexCacheHandle)0, "No valid cache type found.");
	}

	++_gbs->m_allocations;

	VertexCacheHandle handle =	((uint64_t)(m_currentFrame & kCacheFrameMask) << kCacheFrameShift) |
								((uint64_t)(offset & kCacheOffsetMask) << kCacheOffsetShift) |
								((uint64_t)(_bytes & kCacheSizeMask) << kCacheSizeShift);
	if (&_gbs == &m_staticData) 
	{
		handle |= kCacheStatic;
	}

	return handle;
}

VertexCacheHandle VertexCache::AllocVertex(const void* _data, uint32_t _num, size_t _size)
{
	size_t mask = kVertexCacheAlign - 1;
	size_t size = ((_num * _size) + mask) & ~mask;

	return Alloc(m_frameData[m_listNum], _data, size, ECacheType::ECacheType_Vertex);
}

VertexCacheHandle VertexCache::AllocIndex(const void* _data, uint32_t _num, size_t _size) 
{
	size_t mask = kIndexCacheAlign - 1;
	size_t size = ((_num * _size) + mask) & ~mask;

	return Alloc(m_frameData[m_listNum], _data, size, ECacheType::ECacheType_Index);
}

VertexCacheHandle VertexCache::AllocJoint(const void* _data, uint32_t _num, size_t _size)
{
	size_t mask = m_uniformBufferOffsetAlignment/*kJointCacheAlign*/ - 1;
	size_t size = ((_num * _size) + mask) & ~mask;

	return Alloc(m_frameData[m_listNum], _data, size, ECacheType::ECacheType_Joint);
}

VertexCacheHandle VertexCache::AllocStaticVertex(const void* _data, uint32_t _bytes) 
{
	akAssertReturnValue(m_staticData->m_vertexMemUsed.load() + _bytes <= kStaticVertexMemory, (VertexCacheHandle)0, "AllocStaticVertex failed, increase kStaticVertexMemory");
	return Alloc(m_staticData, _data, _bytes, ECacheType::ECacheType_Vertex);
}

VertexCacheHandle VertexCache::AllocStaticIndex(const void* _data, uint32_t _bytes) 
{
	akAssertReturnValue(m_staticData->m_indexMemUsed.load() + _bytes <= kStaticIndexMemory, (VertexCacheHandle)0, "AllocStaticVertex failed, increase kStaticIndexMemory");
	return Alloc(m_staticData, _data, _bytes, ECacheType::ECacheType_Index);
}

byte* VertexCache::MappedVertexBuffer(VertexCacheHandle _handle) 
{
	akAssertReturnValue(!CacheIsStatic(_handle), nullptr, "Cache is static!");

	const uint64_t offset = (uint32_t)(_handle >> kCacheOffsetShift) & kCacheOffsetMask;
	const uint64_t frameNum = (uint32_t)(_handle >> kCacheFrameShift) & kCacheFrameMask;

	akAssertReturnValue(frameNum == (m_currentFrame & kCacheFrameMask), nullptr, "Wrong frame!");

	return m_frameData[m_listNum]->m_mappedVertexBase + offset;
}

byte* VertexCache::MappedIndexBuffer(VertexCacheHandle _handle)
{
	akAssertReturnValue(!CacheIsStatic(_handle), nullptr, "Cache is static!");

	const uint64_t offset = (uint32_t)(_handle >> kCacheOffsetShift) & kCacheOffsetMask;
	const uint64_t frameNum = (uint32_t)(_handle >> kCacheFrameShift) & kCacheFrameMask;

	akAssertReturnValue(frameNum == (m_currentFrame & kCacheFrameMask), nullptr, "Wrong frame!");

	return m_frameData[m_listNum]->m_mappedIndexBase + offset;
}

/*
==============
VertexCache::CacheIsCurrent
==============
*/
bool VertexCache::CacheIsCurrent(const VertexCacheHandle _handle)
{
	const uint32_t isStatic = _handle & kCacheStatic;
	if (isStatic)
	{
		return true;
	}
	const uint64_t frameNum = (uint32_t)(_handle >> kCacheFrameShift) & kCacheFrameMask;
	if (frameNum != (m_currentFrame & kCacheFrameMask))
	{
		return false;
	}
	return true;
}

bool VertexCache::GetVertexBuffer(VertexCacheHandle _handle, VertexBufferObject* vb)
{
	const uint32_t isStatic = _handle & kCacheStatic;
	const uint64_t size = (uint32_t)(_handle >> kCacheSizeShift) & kCacheSizeMask;
	const uint64_t offset = (uint32_t)(_handle >> kCacheOffsetShift) & kCacheOffsetMask;
	const uint64_t frameNum = (uint32_t)(_handle >> kCacheFrameShift) & kCacheFrameMask;
	if (isStatic) 
	{
		vb->Reference(m_staticData->m_vertexBuffer, offset, size);
		return true;
	}
	if (frameNum != ((m_currentFrame - 1) & kCacheFrameMask)) 
	{
		return false;
	}
	vb->Reference(m_frameData[m_drawListNum]->m_vertexBuffer, offset, size);
	return true;
}

bool VertexCache::GetIndexBuffer(VertexCacheHandle _handle, IndexBufferObject* ib)
{
	const uint32_t isStatic = _handle & kCacheStatic;
	const uint64_t size = (uint32_t)(_handle >> kCacheSizeShift) & kCacheSizeMask;
	const uint64_t offset = (uint32_t)(_handle >> kCacheOffsetShift) & kCacheOffsetMask;
	const uint64_t frameNum = (uint32_t)(_handle >> kCacheFrameShift) & kCacheFrameMask;
	if (isStatic) 
	{
		ib->Reference(m_staticData->m_indexBuffer, offset, size);
		return true;
	}
	if (frameNum != ((m_currentFrame - 1) & kCacheFrameMask)) 
	{
		return false;
	}
	ib->Reference(m_frameData[m_drawListNum]->m_indexBuffer, offset, size);
	return true;
}

bool VertexCache::GetJointBuffer(VertexCacheHandle _handle, UniformBufferObject* jb)
{
	const uint32_t isStatic = _handle & kCacheStatic;
	const uint64_t numBytes = (uint32_t)(_handle >> kCacheSizeShift) & kCacheSizeMask;
	const uint64_t jointOffset = (uint32_t)(_handle >> kCacheOffsetShift) & kCacheOffsetMask;
	const uint64_t frameNum = (uint32_t)(_handle >> kCacheFrameShift) & kCacheFrameMask;
	if (isStatic)
	{
		jb->Reference(m_staticData->m_jointBuffer, jointOffset, numBytes);
		return true;
	}
	if (frameNum != ((m_currentFrame - 1) & kCacheFrameMask))
	{
		return false;
	}
	jb->Reference(m_frameData[m_drawListNum]->m_jointBuffer, jointOffset, numBytes);
	return true;
}

void VertexCache::Update()
{
	m_mostUsedVertex = std::max(m_mostUsedVertex, (uint32_t)m_frameData[m_listNum]->m_vertexMemUsed.load());
	m_mostUsedIndex = std::max(m_mostUsedIndex, (uint32_t)m_frameData[m_listNum]->m_indexMemUsed.load());
	m_mostUsedJoint = std::max(m_mostUsedJoint, (uint32_t)m_frameData[m_listNum]->m_jointMemUsed.load());

#if 0
	{
		printf("%08d: %d m_allocations, %dkB vertex, %dkB index, %kB joint : %dkB vertex, %dkB index, %kB joint\n",
			m_currentFrame, m_frameData[m_listNum]->m_allocations,
			m_frameData[m_listNum]->m_vertexMemUsed.load() / 1024,
			m_frameData[m_listNum]->m_indexMemUsed.load() / 1024,
			m_frameData[m_listNum]->m_jointMemUsed.load() / 1024,
			m_mostUsedVertex / 1024,
			m_mostUsedIndex / 1024,
			m_mostUsedJoint / 1024);
	}
#endif

	// unmap the current frame so the GPU can read it
	UnmapGeoBufferSet(m_frameData[m_listNum]);
	UnmapGeoBufferSet(m_staticData);

	m_drawListNum = m_listNum;

	// prepare the next frame for writing to by the CPU
	++m_currentFrame;

	m_listNum = m_currentFrame % Frame::kNumOfFramesInFlight;

	MapGeoBufferSet(m_frameData[m_listNum]);
	ClearGeoBufferSet(m_frameData[m_listNum]);
}

AK_NAMESPACE_END