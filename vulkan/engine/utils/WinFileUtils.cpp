#include "WinFileUtils.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>
#include "../logger/Logger.h"

static std::string runtimePath = "";

std::wstring StringUtf8ToWideChar(const std::string& strUtf8);
std::string UTF8StringToMultiByte(const std::string& strUtf8)
{
    std::string ret;
    if (!strUtf8.empty())
    {
        std::wstring strWideChar = StringUtf8ToWideChar(strUtf8);
        int nNum = WideCharToMultiByte(CP_ACP, 0, strWideChar.c_str(), -1, nullptr, 0, nullptr, FALSE);
        if (nNum)
        {
            char* ansiString = new char[nNum + 1];
            ansiString[0] = 0;

            nNum = WideCharToMultiByte(CP_ACP, 0, strWideChar.c_str(), -1, ansiString, nNum + 1, nullptr, FALSE);

            ret = ansiString;
            delete[] ansiString;
        }
        else
        {
            Logger::getInstance().error("Wrong convert to Ansi code:0x%x", GetLastError());
        }
    }

    return ret;
}
std::wstring StringUtf8ToWideChar(const std::string& strUtf8)
{
    std::wstring ret;
    if (!strUtf8.empty())
    {
        int nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, nullptr, 0);
        if (nNum)
        {
            WCHAR* wideCharString = new WCHAR[nNum + 1];
            wideCharString[0] = 0;

            nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wideCharString, nNum + 1);

            ret = wideCharString;
            delete[] wideCharString;
        }
        else
        {
            Logger::getInstance().error("Wrong convert to WideChar code:0x%x", GetLastError());
        }
    }
    return ret;
}

std::string StringWideCharToUtf8(const std::wstring& strWideChar)
{
    std::string ret;
    if (!strWideChar.empty())
    {
        int nNum = WideCharToMultiByte(CP_UTF8, 0, strWideChar.c_str(), -1, nullptr, 0, nullptr, FALSE);
        if (nNum)
        {
            char* utf8String = new char[nNum + 1];
            utf8String[0] = 0;

            nNum = WideCharToMultiByte(CP_UTF8, 0, strWideChar.c_str(), -1, utf8String, nNum + 1, nullptr, FALSE);

            ret = utf8String;
            delete[] utf8String;
        }
        else
        {
            Logger::getInstance().error("Wrong convert to Utf8 code:0x%x", GetLastError());
        }
    }

    return ret;
}
static inline std::string convertPathFormatToUnixStyle(const std::string& path)
{
    std::string ret = path;
    int len = ret.length();
    for (int i = 0; i < len; ++i)
    {
        if (ret[i] == '\\')
        {
            ret[i] = '/';
        }
    }
    return ret;
}
void GetRuntimePath() {

     if (runtimePath.empty())
     {
        WCHAR utf16Path[MAX_PATH] = { 0 };
        GetModuleFileNameW(NULL, utf16Path, MAX_PATH - 1);
        WCHAR* pUtf16ExePath = &(utf16Path[0]);

        // We need only directory part without exe
        WCHAR* pUtf16DirEnd = wcsrchr(pUtf16ExePath, L'\\');

        char utf8ExeDir[MAX_PATH] = { 0 };
        int nNum = WideCharToMultiByte(CP_UTF8, 0, pUtf16ExePath, pUtf16DirEnd - pUtf16ExePath + 1, utf8ExeDir, sizeof(utf8ExeDir), nullptr, nullptr);

        runtimePath = convertPathFormatToUnixStyle(utf8ExeDir);
     }
}

bool WinFileUtils::CreateDir(const std::string& dir)
{
    if (IsDirExist(dir))
        return true;

    std::wstring path = StringUtf8ToWideChar(dir);

    // Split the path
    size_t start = 0;
    size_t found = path.find_first_of(L"/\\", start);
    std::wstring subpath;
    std::vector<std::wstring> dirs;

    if (found != std::wstring::npos)
    {
        while (true)
        {
            subpath = path.substr(start, found - start + 1);
            if (!subpath.empty())
                dirs.push_back(subpath);
            start = found + 1;
            found = path.find_first_of(L"/\\", start);
            if (found == std::wstring::npos)
            {
                if (start < path.length())
                {
                    dirs.push_back(path.substr(start));
                }
                break;
            }
        }
    }

    if ((GetFileAttributes(path.c_str())) == INVALID_FILE_ATTRIBUTES)
    {
        subpath = L"";
        for (unsigned int i = 0, size = dirs.size(); i < size; ++i)
        {
            subpath += dirs[i];

            std::string utf8Path = StringWideCharToUtf8(subpath);
            if (!IsDirExist(utf8Path))
            {
                BOOL ret = CreateDirectory(subpath.c_str(), NULL);
                if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
                {
                    Logger::getInstance().error("Fail create directory %s !Error code is 0x%x", utf8Path.c_str(), GetLastError());
                    return false;
                }
            }
        }
    }
    return true;
}


bool WinFileUtils::IsFullPath(const std::string& strPath)
{
    if ((strPath.length() > 2
        && ((strPath[0] >= 'a' && strPath[0] <= 'z') || (strPath[0] >= 'A' && strPath[0] <= 'Z'))
        && strPath[1] == ':') || (strPath[0] == '/' && strPath[1] == '/'))
    {
        return true;
    }
    return false;
}

bool WinFileUtils::IsDirExist(const std::string& dirPath)
{
    unsigned long fAttrib = GetFileAttributes(StringUtf8ToWideChar(dirPath).c_str());
    if (fAttrib != INVALID_FILE_ATTRIBUTES &&
        (fAttrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        return true;
    }
    return false;
}

long WinFileUtils::GetFileSize(const std::string& path)
{
    struct _stat tmp;
    if (_stat(path.c_str(), &tmp) == 0)
    {
        return (long)tmp.st_size;
    }
    return 0;
}

bool WinFileUtils::IsFileExist(const std::string& strFilePath)
{
    if (strFilePath.empty())
    {
        return false;
    }

    std::string strPath = strFilePath;
    if (!IsFullPath(strPath))
    { // Not absolute path, add the default root path at the beginning.
        strPath.insert(0, _rootPath);
    }

    DWORD attr = GetFileAttributesW(StringUtf8ToWideChar(strPath).c_str());
    if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
        return false;   //  not a file
    return true;
}

std::string WinFileUtils::TransPath(const std::string& path)
{
    return UTF8StringToMultiByte(path);
}

void WinFileUtils::Init()
{
    GetRuntimePath();
    _rootPath = runtimePath + "Resource/";
    _searchPathArray.push_back(_rootPath);
   
}

std::string WinFileUtils::GetFullPath(const char* filename)
{
    return std::string();
}

