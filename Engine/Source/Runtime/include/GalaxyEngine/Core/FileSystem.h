#pragma once

#include "GalaxyEngine/Core/Base.h"

namespace Galaxy
{
    class FileSystem
    {
    public:
        FileSystem()          = default;
        virtual ~FileSystem() = default;

        virtual std::vector<char> ReadFileAllText(const std::string& fileName) = 0;
    };
} // namespace Galaxy