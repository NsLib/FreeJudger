#include "stdafx.h"
#include "Compiler.h"

#include "CCompiler.h"
#include "CppCompiler.h"
#include "JavaCompiler.h"

namespace IMUST
{

    
CompilerFactory::CompilerFactory()
{
}

CompilerFactory::~CompilerFactory()
{
}

/*static */CompilerPtr CompilerFactory::create(OJInt32_t language)
{
    if(language == AppConfig::Language::C)
    {
        return CompilerPtr(new CCompiler());
    }
    else if(language == AppConfig::Language::Cxx)
    {
        return CompilerPtr(new CppCompiler());
    }
    else if(language == AppConfig::Language::Java)
    {
        return CompilerPtr(new JavaCompiler());
    }

    //TODO: remove this assert.
    assert(false && "unsupport code language!");

    return NULL;
}

}//namespace IMUST