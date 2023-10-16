//
// StandardFileSystem.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#pragma once

#include "GalaxyEngine/Core/FileSystem.h"

namespace Galaxy
{
    class StandardFileSystem : public FileSystem
    {
    public:
        virtual std::vector<char> ReadFileAllText(const std::string& fileName) override;
    };
} // namespace Galaxy
