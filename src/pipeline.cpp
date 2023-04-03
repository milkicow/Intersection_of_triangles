#include "pipeline.hpp"
#include "descriptor.hpp"
#include <vulkan/vulkan_handles.hpp>

namespace vulkan_engine {

Pipeline::Pipeline(Device &device, SwapChain & swapChain, DescriptorSetLayout & descriptorSetLayout, std::string vertFilepath, std::string fragFilepath)
    : device_(device), swapChain_(swapChain), descriptorSetLayout_(descriptorSetLayout), vertFilepath_(vertFilepath), fragFilepath_(fragFilepath) {
                                                    
    auto vertShaderCode = readFile(vertFilepath_); // "../../shaders/vert.spv"
    auto fragShaderCode = readFile(fragFilepath_); // "../../shaders/frag.spv" 

    vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
        vk::PipelineShaderStageCreateFlags(),
        vk::ShaderStageFlagBits::eVertex,
        vertShaderModule,
        "main"
    };

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
        vk::PipelineShaderStageCreateFlags(),
        vk::ShaderStageFlagBits::eFragment,
        fragShaderModule,
        "main"
    };

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription = Model::Vertex::getBindingDescription();
    auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
        vk::PipelineVertexInputStateCreateFlags(),
        1,
        &bindingDescription,
        static_cast<uint32_t>(attributeDescriptions.size()),
        attributeDescriptions.data()
    };

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
        vk::PipelineInputAssemblyStateCreateFlags(),
        vk::PrimitiveTopology::eTriangleList,
        VK_FALSE
    };

    vk::PipelineViewportStateCreateInfo viewportState{
        vk::PipelineViewportStateCreateFlags(),
        1, // viewportCount
        {},
        1, // scissorCount
        {}
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer{
        vk::PipelineRasterizationStateCreateFlags(),
        VK_FALSE,
        VK_FALSE,
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eBack
    };

    rasterizer.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisampling{};

    vk::PipelineDepthStencilStateCreateInfo depthStencil{
        vk::PipelineDepthStencilStateCreateFlags(),
        VK_TRUE,
        VK_TRUE,
        vk::CompareOp::eLess,
        VK_FALSE
    };

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;

    vk::PipelineColorBlendStateCreateInfo colorBlending{
        vk::PipelineColorBlendStateCreateFlags(),
        VK_FALSE,
        vk::LogicOp::eCopy,
        1,
        &colorBlendAttachment
    };

    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{
        vk::PipelineDynamicStateCreateFlags(),
        dynamicStates
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        vk::PipelineLayoutCreateFlags(),
        1,
        descriptorSetLayout.getDescriptorSetLayout_()
    };

    vk::resultCheck(device.getDevice().createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout_), "failed to create pipeline layout!");

    vk::GraphicsPipelineCreateInfo pipelineInfo{
        vk::PipelineCreateFlags(),
        2,
        shaderStages,
        &vertexInputInfo,
        &inputAssembly,
        {},
        &viewportState,
        &rasterizer,
        &multisampling,
        &depthStencil,
        &colorBlending,
        &dynamicState,
        pipelineLayout_,
        swapChain_.getRenderPass()
    };

    vk::resultCheck(device.getDevice().createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline_), "failed to create graphics pipeline!");

    device.getDevice().destroyShaderModule(fragShaderModule, nullptr);
    device.getDevice().destroyShaderModule(vertShaderModule, nullptr);
}

Pipeline::~Pipeline() {
    device_.getDevice().destroyPipeline(graphicsPipeline_, nullptr);
    device_.getDevice().destroyPipelineLayout(pipelineLayout_, nullptr);
}

std::vector<char> Pipeline::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}



vk::ShaderModule Pipeline::createShaderModule(const std::vector<char>& code) {
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    vk::ShaderModule shaderModule;
    if (device_.getDevice().createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

}