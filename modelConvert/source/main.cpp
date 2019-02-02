#include "ModelLoader.h"
#include "ModelChecker.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

bool SaveModel(const ModelData::ModelT * model, const std::string & path)
{
    flatbuffers::FlatBufferBuilder builder(1024);

    auto offset = ModelData::CreateModel(builder, model);
    builder.Finish(offset);

    std::ofstream result(path.c_str(), std::ios_base::binary);
    if (!result)
    {
        std::cout << "Error opening result file " << path.c_str() << "\n\n";
        return false;
    }

    result.write((const char*)builder.GetBufferPointer(), builder.GetSize());
    std::wcout << L"Success: " << path.c_str() << L"\n\n";

    return true;
}

int main(int32_t argc, char * argv[])
{
    for (int32_t i = 1; i < argc; ++i)
    {
        std::cout << "Processing: " << argv[i] << std::endl;

        std::filesystem::path path(argv[i]);

        if (path.extension() == ".model")
        {
            std::ifstream file(argv[i], std::ios::binary | std::ios::ate);
            size_t fileSize = file.tellg();
            file.seekg(std::ios::beg);

            std::vector<char> fileData(fileSize);
            file.read(fileData.data(), fileSize);

            auto data = ModelData::UnPackModel(fileData.data());
            if (CheckModel(*data))
                SaveModel(&(*data), path.string());
        }
        else
        {
            auto data = LoadModel(argv[i]);
            if (!data)
            {
                std::cout << "Error loading!\n\n";
                continue;
            }

            path.replace_extension("model");
            SaveModel(&(*data), path.string());
        }
    }
}
