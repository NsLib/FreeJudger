#include "StringTool.h"

#pragma warning(push)
#pragma warning(disable: 4996)

namespace IMUST
{
    
    static bool narrowStringToWide(std::wstring & dest, const std::string & src, DWORD code)
    {
        int lLen = MultiByteToWideChar(code, 0, src.c_str(), src.length(), NULL, 0);
        if(lLen <= 0) return false;

        dest.resize(lLen, 0);
        MultiByteToWideChar(code, 0, src.c_str(), src.length(), &dest[0], lLen);
        
        size_t pos = dest.find(L'\0');
        if(pos != dest.npos)
        {
            dest.erase(pos);
        }

        return true;
    }

    static bool wideStringToNarrow(std::string & dest, const std::wstring & src, DWORD code)
    {
        int lLen = WideCharToMultiByte(code, 0, src.c_str(), -1, NULL, 0, NULL, NULL);
        if(lLen <= 0) return false;

        dest.resize(lLen, 0);
        WideCharToMultiByte(code, 0, src.c_str(), -1, &dest[0], lLen, NULL, NULL);

        size_t pos = dest.find('\0');
        if(pos != dest.npos)
        {
            dest.erase(pos);
        }

        return true;
    }


    bool OJString2UTF8(std::string & strDst, const OJString & strSrc)
    {
        return wideStringToNarrow(strDst, strSrc, CP_UTF8);
    }

    bool UTF82OJString(OJString & strDst, const std::string & strSrc)
    {
        return narrowStringToWide(strDst, strSrc, CP_UTF8);
    }


    bool FormatStringVS(OJString & dest, const OJChar_t * pFormat, va_list pArgList)
    {
        int len = _vscwprintf(pFormat, pArgList) + 1;
        if(len < 0)
        {
            return false;
        }
        dest.resize(len);

        len = vswprintf(&dest[0], pFormat, pArgList);
    
        if(len < 0)
        {
            dest.clear();
            return false;
        }

        dest.erase(len);
        return true;
    }

    bool FormatString(OJString & dest, const OJChar_t * pFormat, ...)
    {
        va_list pArgList;
        va_start (pArgList, pFormat);

        bool ret = FormatStringVS(dest, pFormat, pArgList);

        va_end(pArgList);

        return ret;
    }

    void DebugMessage(const OJChar_t * pFormat, ...)
    {
        OJString dest;
        va_list pArgList;
        va_start (pArgList, pFormat);

        if(FormatStringVS(dest, pFormat, pArgList))
        {
            DEBUG_MSG(dest.c_str());
        }

        va_end(pArgList);
    }

}//end namespace IMUST

#pragma warning(pop)