#ifndef SANDBOX_GRAPHICS_VULKAN_BUFFER_VULKAN_VERTEX_INPUT_H_
#define SANDBOX_GRAPHICS_VULKAN_BUFFER_VULKAN_VERTEX_INPUT_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/Vulkan.h"

namespace sandbox {

class VulkanVertexInput : public Component {
public:
	VulkanVertexInput(size_t stride, int startLocation = 0, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX) : stride(stride), startLocation(startLocation), inputRate(inputRate) { addType<VulkanVertexInput>(); }
	virtual ~VulkanVertexInput() {}

	virtual VkVertexInputBindingDescription getBindingDescription(int binding) const {
		VkVertexInputBindingDescription bindingDescription = {};
	    bindingDescription.binding = binding;
	    bindingDescription.stride = stride;//sizeof(Vertex);
	    bindingDescription.inputRate = inputRate;

	    return bindingDescription;
	}

	std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(int binding) const {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		attributeDescriptions.resize(attributes.size());

		for (int f = 0; f < attributes.size(); f++) {
			attributeDescriptions[f].binding = binding;
	        attributeDescriptions[f].location = startLocation+f;
	        attributeDescriptions[f].format = attributes[f].format;
	        attributeDescriptions[f].offset = attributes[f].offset;
		}

	    return attributeDescriptions;
	}

	void addAttribute(VkFormat format, size_t offset) {
		Attribute att;
		att.format = format;
		att.offset = offset;
		attributes.push_back(att);
	}

private:
	struct Attribute { VkFormat format; size_t offset; };
	std::vector<Attribute> attributes;
	size_t stride;
	int startLocation;
	VkVertexInputRate inputRate;
};



}

#endif