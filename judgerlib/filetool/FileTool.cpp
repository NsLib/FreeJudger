#include "FileTool.h"

#include "../../thirdpartylib/boost/filesystem.hpp"

namespace IMUST
{
namespace FileTool
{

namespace
{

typedef OJInt32_t FileType;

const FileType  Directory       =       0;
const FileType  RegularFile     =       10;

bool IsFileExistImpl(const OJString &path, const FileType &filetype)
{
    ::boost::filesystem::path fullpath(path);

    if (::boost::filesystem::exists(fullpath))
    {
        if (::boost::filesystem::is_regular_file(fullpath))
        {
            if (RegularFile == filetype)
                return true;
            else
                return false;
        }
        else if (::boost::filesystem::is_directory(fullpath))
        {
            if (Directory == filetype)
                return true;
            else
                return false;
        }
        else
        {
            return false;
        }
    }

    return false;
}

}   // namespace

bool IsFileExist(const OJString &filename)
{
    return IsFileExistImpl(filename, RegularFile);
}

bool RemoveFile(const OJString &filename)
{
    if (IsFileExist(filename))
    {
        if (1 == ::boost::filesystem::remove(filename))
            return true;
        else
            return false;
    }

    return true;
}

bool IsDirExist(const OJString &path)
{
    return IsFileExistImpl(path, Directory);
}

bool MakeDir(const OJString &path)
{
    if (!IsDirExist(path))
        return ::boost::filesystem::create_directory(path);

    return true;
}

OJString GetFullFileName(const OJString &path)
{
    return ::boost::filesystem::path(path).filename().wstring();
}

OJString GetFilePath(const OJString &path)
{
    return ::boost::filesystem::path(path).remove_filename().wstring();
}

OJString GetFileName(const OJString &path)
{
    OJString res(GetFullFileName(path));

    if (!res.empty())
    {
        OJString ext(GetFileExt(path));

        if (!ext.empty())
            res = res.substr(0, res.size() - ext.size());
    }

    return res;
}

OJString GetFileExt(const OJString &path)
{
    return ::boost::filesystem::path(path).extension().wstring();
}

bool GetSpecificExtFiles(FileNameList &files,
    const OJString &path,
    const OJString &ext,
    const bool isRecursion)
{
    return true;
}

bool ReadFile(OJString &buffer,
    const OJString &filename,
    const bool isBinary)
{
    return true;
}


}   // namespace IMUST
}   // namespace IMUST