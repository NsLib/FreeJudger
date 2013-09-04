
#ifndef IMUST_OJ_FILE_TOOL_H
#define IMUST_OJ_FILE_TOOL_H

#include "../platformlayer/PlatformLayer.h"

#include <vector>

namespace IMUST
{
namespace FileTool
{

typedef std::vector<OJString> FileNameList;

bool IsFileExist(const OJString &filename);
bool RemoveFile(const OJString &filename);
bool IsDirExist(const OJString &path);
bool MakeDir(const OJString &path);
OJString GetFullFileName(const OJString &path);
OJString GetFilePath(const OJString &path);
OJString GetFileName(const OJString &path);
OJString GetFileExt(const OJString &path);
OJString RemoveFileExt(const OJString & path);
bool GetSpecificExtFiles(FileNameList &files,
    const OJString &path,
    const OJString &ext,
    const bool withPath);
bool ReadFile(std::vector<OJChar_t> &buffer,
    const OJString &filename,
    const bool isBinary = false);
bool WriteFile(std::vector<OJChar_t> &buffer,
    const OJString &filename,
    const bool isBinary = false);
bool WriteFile(const OJString &buffer,
    const OJString &filename);

}   // namespace IMUST
}   // namespace IMUST

#endif  // IMUST_OJ_FILE_TOOL_H
