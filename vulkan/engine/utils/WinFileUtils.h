#pragma once
#include "FileUtils.h"
class WinFileUtils :
    public FileUtils
{
   
public:
    bool CreateDir(const std::string& dir) override;

    // 通过 FileUtils 继承
    bool IsFullPath(const std::string& strPath) override;
    bool IsDirExist(const std::string& path) override;
    long GetFileSize(const std::string& path) override;
    bool IsFileExist(const std::string& path) override;
    std::string TransPath(const std::string& path) override;
    void Init() override;
    std::string GetFullPath(const char* filename) override;
};

