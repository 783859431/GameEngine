#pragma once
#include <string>
#include <unordered_map>
class FileUtils
{
	
	
	static FileUtils* inst;
public:
	
	static FileUtils* GetInst();
	virtual void Init() = 0;
	virtual std::string GetFullPath(const char* filename) =0;
	virtual bool IsFullPath(const std::string& strPath) =0;
	virtual bool IsDirExist(const std::string& path) =0;
	virtual bool IsFileExist(const std::string& path) = 0;
	virtual long GetFileSize(const std::string& path) =0;
	virtual std::string GetFileExtension(const std::string& path);
	virtual bool CreateDir(const std::string& dir) =0;
	virtual std::string TransPath(const std::string& path);
	std::string Combine(const std::string& dir, const std::string& ohter);
	std::string GetFileFullPath(const std::string& filename);
	std::string GetDirFullPath(const std::string& dirName);
	void SetSearchPaths(const std::vector<std::string>& searchPaths);
	std::string _rootPath;
	std::vector<std::string> _searchPathArray;

	/**
	 *  The full path cache for normal files. When a file is found, it will be added into this cache.
	 *  This variable is used for improving the performance of file search.
	 */
	mutable std::unordered_map<std::string, std::string> _fullPathCache;


	/**
	 *  The full path cache for directories. When a diretory is found, it will be added into this cache.
	 *  This variable is used for improving the performance of file search.
	 */
	mutable std::unordered_map<std::string, std::string> _fullPathCacheDir;

	/**
 * The search paths which was set by 'setSearchPaths' / 'addSearchPath'.
 */
	std::vector<std::string> _originalSearchPaths;
};

