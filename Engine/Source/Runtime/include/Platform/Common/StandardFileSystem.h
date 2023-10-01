#pragma once

#include "Core/FileSystem.h"

namespace Galaxy
{
    class StandardFileSystem : public FileSystem
    {
    public:
        virtual std::vector<char> ReadFileAllText(const std::string& fileName) override;
    };
} // namespace Galaxy
