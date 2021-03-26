#pragma once

#include "../../Core/Defines.h"

#include "BaseBufferObject.h"


AK_NAMESPACE_BEGIN

class VertexBufferObject : public BaseBufferObject
{
public:
	VertexBufferObject();
	~VertexBufferObject();

	// Allocate or free the buffer.
	bool AllocBufferObject(const void* _data, uint32_t _allocSize, EBufferUsage _usage);
	void FreeBufferObject();

	// Make this buffer a reference to another buffer.
	void Reference(const VertexBufferObject& _other);
	void Reference(const VertexBufferObject& _other, uint32_t _refOffset, uint32_t _refSize);

	// Copies data to the buffer. 'size' may be less than the originally allocated size.
	void Update(const void* _data, uint32_t _size, uint32_t _offset = 0) const;

	void* MapBuffer(EBufferMappingType _mapType);
	void UnmapBuffer();

private:
	void ClearWithoutFreeing();

	VertexBufferObject(const VertexBufferObject&);
	void operator=(const VertexBufferObject&);
};

AK_NAMESPACE_END