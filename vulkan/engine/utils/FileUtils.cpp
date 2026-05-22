#include "FileUtils.h"
#include <fstream>
//#include "../logger/Logger.h"
#include "WinFileUtils.h"
#include <algorithm> 
std::string FileUtils::GetFileExtension(const std::string& filePath)
{
    std::string fileExtension;
    size_t pos = filePath.find_last_of('.');
    if (pos != std::string::npos)
    {
        fileExtension = filePath.substr(pos, filePath.length());
        
        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    }

    return fileExtension;
}
std::string FileUtils::TransPath(const std::string& path)
{
    return path;
}
int FileUtils::Read(const std::string& filepath, std::vector<char>& buf)
{
    if (!IsFileExist(filepath)) return 0;
    std::string fullpath = GetFileFullPath(filepath);
    std::ifstream file(fullpath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = GetFileSize(fullpath);
    if(buf.size()<fileSize)
	buf.resize(fileSize);   
    file.seekg(0);
    file.read(buf.data(), fileSize);
    file.close();
    return fileSize;
}
std::string FileUtils::Combine(const std::string& dir, const std::string& other)
{
    std::string ret = dir;
    if (dir.size() && dir[dir.size() - 1] != '/') {
        ret += '/';
    }
    ret += other;
    return ret;
}



std::string FileUtils::GetFileFullPath(const std::string& filename)
{
    if (filename.empty())
    {
        return "";
    }

    if (IsFullPath(filename))
    {
        return filename;
    }

    // Already Cached ?
    auto cacheIter = _fullPathCache.find(filename);
    if (cacheIter != _fullPathCache.end())
    {
        return cacheIter->second;
    }

    std::string fullpath;

    for (const auto& searchIt : _searchPathArray)
    {
        fullpath = Combine(searchIt, filename);
        if (IsFileExist(fullpath))
        {
            _fullPathCache[filename] = fullpath;
            _fullPathCache.emplace(filename,fullpath);
            return fullpath;
        }

    }
    return "";

}

std::string FileUtils::GetDirFullPath(const std::string& dirName)
{
    if (dirName.empty())
    {
        return "";
    }

    if (IsFullPath(dirName))
    {
        return dirName;
    }

    // Already Cached ?
    auto cacheIter = _fullPathCacheDir.find(dirName);
    if (cacheIter != _fullPathCacheDir.end())
    {
        return cacheIter->second;
    }

    std::string fullpath;

    for (const auto& searchIt : _searchPathArray)
    {

        fullpath = Combine(searchIt, dirName);
        if (IsDirExist(fullpath))
        {
            // Using the filename passed in as key.
            _fullPathCacheDir.emplace(dirName, fullpath);
            return fullpath;
        }

    }

    // The file wasn't found, return empty string.
    return "";

}

void FileUtils::SetSearchPaths(const std::vector<std::string>& searchPaths)
{

     bool existDefaultRootPath = false;
     _originalSearchPaths = searchPaths;

     _fullPathCache.clear();
     _fullPathCacheDir.clear();
     _searchPathArray.clear();

     for (const auto& path : _originalSearchPaths)
     {
         std::string prefix;
         std::string fullPath;

         if (!IsFullPath(path))
         { // Not an absolute path
             prefix = _rootPath;
         }
         fullPath = prefix + path;
         if (!path.empty() && path[path.length() - 1] != '/')
         {
             fullPath += "/";
         }
         if (!existDefaultRootPath && path == _rootPath)
         {
             existDefaultRootPath = true;
         }
         _searchPathArray.push_back(fullPath);
     }

     if (!existDefaultRootPath)
     {

         _searchPathArray.push_back(_rootPath);
     }


}
FileUtils* FileUtils::inst = 0;
FileUtils* FileUtils::GetInst()
{
    if (inst == nullptr)
    {
#ifdef _WIN32
        inst = new WinFileUtils();

#endif // _WIN32
      
        inst->Init();
    }
    return inst;

}
