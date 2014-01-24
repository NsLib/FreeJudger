#include "stdafx.h"
#include "Excuter.h"
#include "ExeExcuter.h"
#include "JavaExcuter.h"


namespace IMUST
{
    
ExcuterFactory::ExcuterFactory()
{
}
ExcuterFactory::~ExcuterFactory()
{
}

/*static */ExcuterPtr ExcuterFactory::create(OJInt32_t language)
{
    if(language == AppConfig::Language::C \
        || language == AppConfig::Language::Cxx)
    {
        return ExcuterPtr(new ExeExcuter());
    }
    else if(language == AppConfig::Language::Java)
    {
        return ExcuterPtr(new JavaExcuter());
    }

    //TODO: remove this assert.
    assert(false && "unsupport code language!");

    return NULL;
}


}//namespace IMUST