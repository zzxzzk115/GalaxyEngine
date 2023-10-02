#include "GalaxyEngine/Platform/Common/StandardFileSystem.h"
#include "GalaxyEngine/Core/Macro.h"

namespace Galaxy
{
    std::vector<char> StandardFileSystem::ReadFileAllText(const std::string& fileName)
    {
        std::ifstream file(fileName, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            GAL_CORE_ERROR("[FileSystem] Failed to open file: {0}", fileName);
        }

        size_t            fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
} // namespace Galaxy
