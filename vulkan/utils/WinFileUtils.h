#pragma once
#include "FileUtils.h"
#include "../singleton.h"
class WinFileUtils :
    public FileUtils
{
   
public:
    bool CreateDir(const std::string& dir) override;

    // Í¨¹ý FileUtils ¼Ì³Ð
    bool IsFullPath(const std::string& strPath) override;
    bool IsDirExist(const std::string& path) override;
    long GetFileSize(const std::string& path) override;
    bool IsFileExist(const std::string& path) override;
    std::string TransPath(const std::string& path) override;
    void Init() override;
    std::string GetFullPath(const char* filename) override;
};

