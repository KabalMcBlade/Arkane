#pragma once

#include "../../Core/Defines.h"

#include "BaseBufferObject.h"


AK_NAMESPACE_BEGIN

// in UniformBufferObject, the offset is limited to 256 bytes, so joint offsets, which are multiples of 48 bytes, must be in multiples of 16 = 768 bytes.
class AK_DLL UniformBufferObject : public BaseBufferObject
{
public:
	UniformBufferObject();
	~UniformBufferObject();

	// Allocate or free the buffer.
	bool AllocBufferObject(const void* _data, size_t _allocSize, EBufferUsage _usage = EBufferUsage::EBufferUsage_Dynamic);
	void FreeBufferObject();

	// Make this buffer a reference to another buffer.
	void Reference(const UniformBufferObject& _other);
	void Reference(const UniformBufferObject& _other, size_t _refOffset, size_t _refSize);

	// Copies data to the buffer. 'size' may be less than the originally allocated size.
	void Update(const void* _data, size_t _size, size_t _offset = 0) const;

	void* MapBuffer(EBufferMappingType _mapType);
	void UnmapBuffer();

private:
	void ClearWithoutFreeing();

	UniformBufferObject(const UniformBufferObject&);
	void operator=(const UniformBufferObject&);
};

AK_NAMESPACE_END