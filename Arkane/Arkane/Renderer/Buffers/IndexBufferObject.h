#pragma once

#include "../../Core/Defines.h"

#include "BaseBufferObject.h"


AK_NAMESPACE_BEGIN

class IndexBufferObject :  public BaseBufferObject
{
public:
	IndexBufferObject();
	~IndexBufferObject();

	// Allocate or free the buffer.
	bool AllocBufferObject(const void* _data, uint32_t _allocSize, EBufferUsage _usage);
	void FreeBufferObject();

	// Make this buffer a reference to another buffer.
	void Reference(const IndexBufferObject& _other);
	void Reference(const IndexBufferObject& _other, uint32_t _refOffset, uint32_t _refSize);

	// Copies data to the buffer. 'size' may be less than the originally allocated size.
	void Update(const void* _data, uint32_t _size, uint32_t _offset = 0) const;

	void* MapBuffer(EBufferMappingType _mapType);
	void UnmapBuffer();

private:
	void ClearWithoutFreeing();

	IndexBufferObject(const IndexBufferObject&);
	void operator=(const IndexBufferObject&);
};

AK_NAMESPACE_END