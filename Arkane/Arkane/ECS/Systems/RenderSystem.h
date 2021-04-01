#pragma once

#include "../../Core/Assertions.h"
#include "../../Core/SmartPointers.h"

#include "System.h"
#include "../../Renderer/VertexCache.h"

#pragma warning(disable : 4251)


AK_NAMESPACE_BEGIN

class AK_DLL RenderSystem : public System
{
public:
	void Init();
	void Update(float _dt);
	void Destroy();

private:
	VertexCacheHandle m_vertexCache;
	VertexCacheHandle m_indexCache;
	VertexCacheHandle m_jointCache;
};

AK_NAMESPACE_END
