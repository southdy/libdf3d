#pragma once

namespace df3d {

class GLSLPreprocess
{
public:
    static std::string preprocess(const std::string &input, const std::string &shaderPath);
};

}
