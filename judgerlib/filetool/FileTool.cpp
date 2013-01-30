#include "FileTool.h"

#include <locale>

#include "../../thirdpartylib/boost/filesystem.hpp"

#define BOOST_UTF8_BEGIN_NAMESPACE
#define BOOST_UTF8_END_NAMESPACE
#define BOOST_UTF8_DECL
#include "../../thirdpartylib/boost/detail/utf8_codecvt_facet.hpp"
#include "../../thirdpartylib/boost/detail/utf8_codecvt_facet.ipp"

namespace IMUST
{

namespace fs = ::boost::filesystem;

namespace FileTool
{

namespace
{

typedef OJInt32_t FileType;

const FileType  Directory       =       0;
const FileType  RegularFile     =       10;

bool IsFileExistImpl(const OJString &path, const FileType &filetype)
{
    fs::path fullpath(path);

    if (fs::exists(fullpath))
    {
        if (fs::is_regular_file(fullpath))
        {
            if (RegularFile == filetype)
                return true;
            else
                return false;
        }
        else if (fs::is_directory(fullpath))
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

bool GetSpecificExtFilesImpl(FileNameList &files,
                            const OJString &path,
                            const OJString &ext,
                            const bool withPath)
{
    fs::path fullpath(path);
    if (!fs::exists(fullpath))
        return false;
    
    fs::directory_iterator end_iter;
    for (fs::directory_iterator iter(fullpath); iter != end_iter; ++iter)
    {
        if (fs::is_regular_file(iter->status()))
        {
            if (ext == GetFileExt(iter->path().wstring()))
            {
                if (withPath)
                    files.push_back(iter->path().wstring());
                else
                    files.push_back(GetFullFileName(iter->path().wstring()));
            }
                
        }

        if (fs::is_directory(iter->status()))
            GetSpecificExtFilesImpl(files, iter->path().wstring(), ext, withPath);
    }

    return true;
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
        if (1 == fs::remove(filename))
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
        return fs::create_directory(path);

    return true;
}

OJString GetFullFileName(const OJString &path)
{
    return fs::path(path).filename().wstring();
}

OJString GetFilePath(const OJString &path)
{
    return fs::path(path).remove_filename().wstring();
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
    return fs::path(path).extension().wstring();
}

bool GetSpecificExtFiles(FileNameList &files,
    const OJString &path,
    const OJString &ext,
    const bool withPath)
{
    files.clear();

    return GetSpecificExtFilesImpl(files, path, ext, withPath);
}

bool ReadFile(std::vector<OJChar_t> &buffer,
    const OJString &filename,
    const bool isBinary)
{
    buffer.clear();
    buffer.reserve(1024 * 20);

    static std::locale old_locale;
    static std::locale utf8_locale(old_locale, new utf8_codecvt_facet);
    OJIfstream file;

    if (isBinary)
        file.open(filename.c_str(), OJIfstream::binary);
    else
        file.open(filename.c_str());

    if (!file.good())
        return false;

    if (isBinary)
    {
        // TODO
    } 
    else
    {
        file.imbue(utf8_locale);
        
        static OJChar_t buf[100 * 1024];
        static OJString str;

        while(file.getline(buf, 100 * 1024))
        {
            str = buf;
            std::copy(str.begin(), str.end(), std::back_inserter(buffer));
            buffer.push_back(OJCh('\n'));
        }
    }

    file.close();

    return true;
}

bool WriteFile(std::vector<OJChar_t> &buffer,
    const OJString &filename,
    const bool isBinary)
{
    static std::locale old_locale;
    static std::locale utf8_locale(old_locale, new utf8_codecvt_facet);
    // 二进制写入待完成
    OJOfstream file(filename);

    file.imbue(utf8_locale);
    std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<OJChar_t, OJChar_t>(file));
    file.close();

    return true;
}


}   // namespace IMUST
}   // namespace IMUST