#pragma once
#include <optional>
#include "model_generated.h"

std::optional<ModelData::ModelT> LoadModel(const char * path);
