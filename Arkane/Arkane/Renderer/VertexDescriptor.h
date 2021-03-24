
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "VertexLayout.h"

#include "../Core/Defines.h"

#include <vector>


#pragma warning( disable : 4251 )	//export std

AK_NAMESPACE_BEGIN

class AK_DLL VertexDescriptor final
{
public:
	VertexDescriptor(EVertexLayout _layout);
	~VertexDescriptor();

	AK_INLINE const VkPipelineVertexInputStateCreateInfo& GetVertexInput() const
	{
		return m_vertexInput;
	}

private:
	VkPipelineVertexInputStateCreateInfo m_vertexInput;
	std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
};

AK_NAMESPACE_END