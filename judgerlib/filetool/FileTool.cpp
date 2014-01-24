#include "FileTool.h"

#include <locale>

#include "../../thirdpartylib/boost/filesystem.hpp"

#define BOOST_UTF8_BEGIN_NAMESPACE
#define BOOST_UTF8_END_NAMESPACE
#define BOOST_UTF8_DECL
#include "../../thirdpartylib/boost/detail/utf8_codecvt_facet.hpp"
#include "../../thirdpartylib/boost/detail/utf8_codecvt_facet.ipp"

#include "../logger/Logger.h"
#include "../util/Utility.h"
#include "../util/StringTool.h"

namespace IMUST
{

namespace fs    = ::boost::filesystem;
namespace sys   = ::boost::system;

namespace FileTool
{

namespace
{

typedef OJInt32_t FileType;

const FileType  Directory       =       0;
const FileType  RegularFile     =       10;

bool IsFileExistImpl(const OJString &path, const FileType &filetype)
{
    fs::path            fullpath(path);
    sys::error_code     err;

    bool res = fs::exists(fullpath, err);
    if (!(sys::errc::success == err || sys::errc::no_such_file_or_directory == err))
    {
        ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
        OJString msg(GetOJString("[filetool] - IMUST::IsFileExistImpl - access failed: "));
        msg += fullpath.c_str();
        msg += GetOJString(" - ");
        msg += String2OJString(err.message());
        logger->logError(msg);
        return false;
    }
    
    if (res)
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
    fs::path            fullpath(path);
    sys::error_code     err;

    bool res = fs::exists(fullpath, err);
    if (!(sys::errc::success == err || sys::errc::no_such_file_or_directory == err))
    {
        ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
        OJString msg(GetOJString("[filetool] - IMUST::GetSpecificExtFilesImpl - access failed: "));
        msg += fullpath.c_str();
        msg += GetOJString(" - ");
        msg += String2OJString(err.message());
        logger->logError(msg);
        return false;
    }
    if (!res)
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
        fs::path            fullpath(filename);
        sys::error_code     err;

        bool res = fs::remove(fullpath, err);

        if (!(sys::errc::success == err))
        {
            ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
            OJString msg(GetOJString("[filetool] - IMUST::RemoveFile - remove failed: "));
            msg += filename;
            msg += GetOJString(" - ");
            msg += String2OJString(err.message());
            logger->logError(msg);
            return false;
        } 

        return res;
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
    {
        fs::path            fullpath(path);
        sys::error_code     err;

        bool res = fs::create_directory(fullpath, err);

        if (!(sys::errc::success == err))
        {
            ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
            OJString msg(GetOJString("[filetool] - IMUST::MakeDir - make dir failed: "));
            msg += path;
            msg += GetOJString(" - ");
            msg += String2OJString(err.message());
            logger->logError(msg);
            return false;
        } 

        return res;
    }

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

OJString RemoveFileExt(const OJString & path)
{
    OJString res(path);
    if (!res.empty())
    {
        OJString::size_type pos = res.find_last_of(OJCh('.'));
        if(pos != res.npos)
        {
            res.erase(pos);
        }
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

        file.seekg(0, OJIfstream::end);
        OJIfstream::pos_type length = file.tellg();
        file.seekg(0, OJIfstream::beg);

        buffer.resize(length + OJIfstream::pos_type(1));
        file.read(&buffer[0], length);

        buffer[file.gcount()] = 0;
        buffer.resize(file.gcount());
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
bool WriteFile(const OJString &buffer,
    const OJString &filename)
{
    static std::locale old_locale;
    static std::locale utf8_locale(old_locale, new utf8_codecvt_facet);
    
    OJOfstream file(filename);
    if(!file.good())
    {
        return false;
    }

    file.imbue(utf8_locale);
    file.write(buffer.c_str(), buffer.size());
    file.close();

    return true;
}


bool ReadString(OJString & str, const OJString & filename)
{
    FILE *pFile = _wfopen(filename.c_str(), L"r");
    if(pFile == NULL) return false;

    int len = fseek(pFile, 0, SEEK_END);
    fseek(pFile, 0, SEEK_SET);

    std::string buffer(len, L'\0');

    if(len > 0)
    {
        fread(&buffer[0], 1, len, pFile);
    }

    fclose(pFile);
    return UTF82OJString(str, buffer);
}

bool WriteString(const OJString & str, const OJString & filename)
{
    if(str.empty()) return false;

    std::string buffer;
    if(!OJString2UTF8(buffer, str)) return false;

    FILE *pFile = _wfopen(filename.c_str(), L"w");
    if(pFile == NULL) return false;

    fwrite(buffer.c_str(), 1, buffer.length(), pFile);

    fclose(pFile);
    return true;
}

OJString GetModulePath()
{
    OJString buffer(MAX_PATH, L'\0');

    ::GetModuleFileName(NULL, &buffer[0], MAX_PATH);
    
    return GetFilePath(buffer.c_str());
}

OJString getCurPath()
{
    OJString buffer(MAX_PATH, 0);
    ::GetCurrentDirectory(MAX_PATH, &buffer[0]);

    size_t pos = buffer.find(L'\0');
    if(pos != buffer.npos) buffer.erase(pos);

    return buffer;
}

bool SetCurPath(const OJString & path)
{
    bool ret = !!::SetCurrentDirectory(path.c_str());
    
    DEBUG_MSG_VS(OJStr("Current Work Directory: %s"), getCurPath().c_str());
    return ret;
}

}   // namespace FileTool
}   // namespace IMUST