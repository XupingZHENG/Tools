#pragma once

#ifdef _WIN32  
#include <direct.h>  
#include <io.h>  
#include <sys/stat.h>  
#elif __GNUC__
#include <unistd.h>
#include <stdarg.h>  
#include <sys/stat.h>  
#endif  

#ifdef _WIN32  
#define ACCESS _access  
#define MKDIR(a) _mkdir((a))  
#elif __GNUC__
#define ACCESS access  
#define MKDIR(a) mkdir((a),0755)  
#endif 

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>

#ifdef _WIN32
//#include "WinDirent.h"
#else
#include <dirent.h>
#endif

#ifndef _WIN32
#define stricmp strcasecmp
#endif

inline bool exists(const std::string& dir)
{
    return (ACCESS(dir.c_str(), 0) == 0);
}

/*!
This function creates directory recursively. 
If intermediate directories does not exist, they are first created before the final directory is created.
\param[in] dir   The directory to be created, actually the final directory to be created.
Code adapted from http://blog.csdn.net/sunnylgz/article/details/7710892
\return 0 if success, non zero if failure.
*/
inline int createDirectory(const std::string& dir)
{
    if (dir.empty())
        return 0;

    int length = (int)dir.length() + 1;
    std::vector<char> buf(length, 0);
    memcpy(buf.data(), dir.data(), length - 1);
    char* local = buf.data();

    // Create intermediate directories.
    // IMPORTANT NOTICE!!!
    // The first slash or backslash should not be changed to '\0'
    // in Linux system
    for (int i = 1; i < length; i++)
    {
        if (local[i] == '\\' || local[i] == '/')
        {
            local[i] = '\0';

            // If the directory does not exist, create it.
            int ret = ACCESS(local, 0);
            if (ret != 0)
            {
                ret = MKDIR(local);
                if (ret != 0)
                {
                    return -1;
                }
            }
            // Support linux and windows.
            local[i] = '/';
        }
    }

    return MKDIR(local);
}

inline bool isDirectory(const std::string& path)
{
#ifdef _WIN32
    struct _stat64 info;
    _stat64(path.c_str(), &info);
#else
    struct stat info;
    stat(path.c_str(), &info);
#endif
    return (info.st_mode & S_IFDIR) != 0;
}

inline bool isRegularFile(const std::string& path)
{
#ifdef _WIN32
    struct _stat64 info;
    _stat64(path.c_str(), &info);
#else
    struct stat info;
    stat(path.c_str(), &info);
#endif
    return (info.st_mode & S_IFREG) != 0;
}

inline long long int fileSize(const std::string& path)
{
#ifdef _WIN32
    struct _stat64 info;
    _stat64(path.c_str(), &info);
#else
    struct stat info;
    stat(path.c_str(), &info);
#endif
    return info.st_size;
}

inline bool isImage(const std::string& path)
{
    std::string::size_type pos = path.find_last_of('.');
    if (pos == std::string::npos)
        return false;
    std::string ext = path.substr(pos + 1);
    const char* p = ext.c_str();
    return ((stricmp(p, "bmp") == 0) ||
        (stricmp(p, "gif") == 0) ||
        (stricmp(p, "jpg") == 0) ||
        (stricmp(p, "jpeg") == 0) ||
        (stricmp(p, "png") == 0) ||
        (stricmp(p, "tif") == 0) ||
        (stricmp(p, "tiff") == 0));
}

inline bool endsWithSlash(const std::string& path)
{
    if (path.empty())
        return false;

    return (path.back() == '\\') || (path.back() == '/');
}

inline void readDirectory(const std::string& directoryName, std::vector<std::string>& fileNames, bool addDirectoryName = false)
{
    fileNames.clear();

    const std::string* rootDir;
    std::string dirName;
    if (endsWithSlash(directoryName))
    {
        dirName = directoryName.substr(0, directoryName.size() - 1);
        rootDir = &dirName;
    }
    else
        rootDir = &directoryName;

#ifdef _WIN32
    struct _finddata_t fileInfo;
    std::string str = *rootDir + "\\*.*";
    intptr_t fileHandle = _findfirst(str.c_str(), &fileInfo);
    if (fileHandle != static_cast<intptr_t>(-1))
    {
        do
        {
            if (strcmp(fileInfo.name, ".") == 0 ||
                strcmp(fileInfo.name, "..") == 0)
                continue;

            if (addDirectoryName)
                fileNames.push_back(*rootDir + "\\" + fileInfo.name);
            else
                fileNames.push_back((std::string)fileInfo.name);
        } while (_findnext(fileHandle, &fileInfo) == 0);
    }
    _findclose(fileHandle);
    //std::sort(fileNames.begin(), fileNames.end());
#else
    DIR *dir;
    struct dirent *ent;
    dir = opendir(rootDir->c_str());
    if (dir != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") == 0 ||
                strcmp(ent->d_name, "..") == 0)
                continue;

            if (addDirectoryName)
                fileNames.push_back(*rootDir + "/" + ent->d_name);
            else
                fileNames.push_back(std::string(ent->d_name));
        }
    }
#endif
}

template<typename Pred>
inline void readDirectory(const std::string& directoryName, std::vector<std::string>& fileNames,
    Pred func, bool addDirectoryName = false)
{
    fileNames.clear();

    const std::string* rootDir;
    std::string dirName;
    if (endsWithSlash(directoryName))
    {
        dirName = directoryName.substr(0, directoryName.size() - 1);
        rootDir = &dirName;
    }
    else
        rootDir = &directoryName;

#ifdef _WIN32
    struct _finddata_t fileInfo;
    std::string str = *rootDir + "\\*.*";
    intptr_t fileHandle = _findfirst(str.c_str(), &fileInfo);
    if (fileHandle != static_cast<intptr_t>(-1))
    {
        do
        {
            if (strcmp(fileInfo.name, ".") == 0 ||
                strcmp(fileInfo.name, "..") == 0)
                continue;

            std::string currName = *rootDir + "\\" + fileInfo.name;
            if (func(currName))
            {
                if (addDirectoryName)
                    fileNames.push_back(currName);
                else
                    fileNames.push_back((std::string)fileInfo.name);
            }
        } while (_findnext(fileHandle, &fileInfo) == 0);
    }
    _findclose(fileHandle);
    //std::sort(fileNames.begin(), fileNames.end());
#else
    DIR *dir;
    struct dirent *ent;
    dir = opendir(rootDir->c_str());
    if (dir != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") == 0 ||
                strcmp(ent->d_name, "..") == 0)
                continue;

            std::string currName = *rootDir + "/" + ent->d_name;
            if (func(currName))
            {
                if (addDirectoryName)
                    fileNames.push_back(*rootDir + "/" + ent->d_name);
                else
                    fileNames.push_back(std::string(ent->d_name));
            }
        }
    }
#endif
}

template<typename Pred>
inline void collectFilesRecursively(const std::string& directoryName, std::vector<std::string>& fileNames,
    Pred func)
{
    fileNames.clear();

    if (!isDirectory(directoryName))
        return;

    std::vector<std::string> stack;
    stack.push_back(directoryName);
    while (!stack.empty())
    {
        std::string dir = stack.back();
        stack.pop_back();
        
        std::vector<std::string> results;
        readDirectory(dir, results, true);
        int numResults = results.size();
        for (int i = 0; i < numResults; i++)
        {
            if (isDirectory(results[i]))
                stack.push_back(results[i]);
            if (func(results[i]))
                fileNames.push_back(results[i]);
        }
    }
}

inline std::string getFileNameExtension(const std::string& name)
{
    std::string::size_type pos = name.find_last_of(".");
    if (pos == std::string::npos)
        return std::string();
    return name.substr(pos + 1);
}

inline std::string getFileNameWithoutPath(const std::string& name)
{
    std::string::size_type posBegin = name.find_last_of("\\/");
    if (posBegin == std::string::npos)
        return name;
    else
        return name.substr(++posBegin);
}

inline std::string getFileNameWithoutPathAndExtension(const std::string& name)
{
    std::string::size_type posBegin = name.find_last_of("\\/");
    if (posBegin == std::string::npos)
        posBegin = 0;
    else
        posBegin++;
    std::string::size_type posEnd = name.find_last_of(".");
    if (posEnd == std::string::npos)
        posEnd = name.size();
    return name.substr(posBegin, posEnd - posBegin);
}

inline bool copyFile(const std::string& src, const std::string& dst)
{
    if (!isRegularFile(src))
        return false;

    std::ifstream ifs(src, std::ios_base::binary);
    if (!ifs)
        return false;

    std::ofstream ofs(dst, std::ios_base::binary);
    if (!ofs)
        return false;

    // http://blog.csdn.net/u012750702/article/details/52738859
    ofs << ifs.rdbuf();

    ifs.close();
    ofs.close();

    return true;
}

#ifndef _WIN32
#undef stricmp
#endif
