#pragma once

#include "device.hpp"
#include "model.hpp"
#include "swap_chain.hpp"

#include <string>
#include <fstream>


namespace vulkan_engine {

class Pipeline {
public:

    Pipeline(Device & device, SwapChain & swapChain, std::string & vertFilepath, std::string & fragFilepath);
    ~Pipeline();

    Pipeline &operator=(const Pipeline&) = delete;
    Pipeline(const Pipeline&) = delete;

private:
    
    Device & device_;
    SwapChain & swapChain_;
    std::string vertFilepath_;
    std::string fragFilepath_;

    vk::PipelineLayout pipelineLayout_;
    vk::Pipeline graphicsPipeline_;
    
    static std::vector<char> readFile(const std::string& filename);
    void createGraphicsPipeline();
    vk::ShaderModule createShaderModule(const std::vector<char>& code);


};

}