#pragma once
#include <string>
#include "ModelShader.h"

namespace ShaderGenerator
{
    struct Result
    {
        std::string vertex;
        std::string fragment;
    };

    Result Generate(const ModelShader::Config & config);
}
