#include "ModelLoader.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

int main(int32_t argc, char * argv[])
{
    for (int32_t i = 1; i < argc; ++i)
    {
        std::cout << "Processing: " << argv[i] << std::endl;
        auto data = LoadModel(argv[i]);
        if (!data)
        {
            std::cout << "Error loading!\n\n";
            continue;
        }

        std::filesystem::path path(argv[i]);
        path.replace_extension("model");

        flatbuffers::FlatBufferBuilder builder(1024);

        auto offset = ModelData::CreateModel(builder, &(*data));
        builder.Finish(offset);

        std::ofstream result(path.c_str(), std::ios_base::binary);
        if (!result)
        {
            std::cout << "Error opening result file "  << path.c_str() << "\n\n";
            continue;
        }

        result.write((const char*)builder.GetBufferPointer(), builder.GetSize());
        std::wcout << L"Success: " << path.c_str() << L"\n\n";
    }
}
