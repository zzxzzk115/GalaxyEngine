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

        virtual void InitExecutableDirectory(const char* executableFilePath)
        {
            std::filesystem::path exe_path(executableFilePath);
            m_ExeDirectory = exe_path.parent_path();
        }
        virtual const std::filesystem::path& GetExecutableDirectory() { return m_ExeDirectory; }
        virtual std::filesystem::path        GetExecutableRelativeDirectory(const std::filesystem::path& path)
        {
            return m_ExeDirectory / path;
        }

    private:
        std::filesystem::path m_ExeDirectory;
    };
} // namespace Galaxy