#include "Watch.h"
#include "StringTool.h"
#include <algorithm>

namespace IMUST
{
    IValueProxy::IValueProxy()
    {
    }

    IValueProxy::~IValueProxy()
    {
    }

    ////////////////////////////////////////////////////////////////////
    Watch * rootWatch()
    {
        static Watch s_watch(OJStr("root"));
        return &s_watch;
    }


    Watch::Watch(const OJString & name)
        : name_(name)
    {
    }


    Watch::~Watch(void)
    {
        std::map<OJString, Watch*>::iterator it;
        for(it = children_.begin(); it != children_.end(); ++it)
        {
            delete it->second;
        }
    }

    Watch * Watch::getChild(const OJString & name, bool createIfMiss)
    {
        Watch * p = NULL;

        WatchMap::iterator it = children_.find(name);
        if(it != children_.end())
        {
            p = it->second;
        }
        else if(createIfMiss)
        {
            p = new Watch(name);
            children_.insert(std::pair<OJString, Watch*>(name, p));
        }

        return p;
    }

    Watch * Watch::getWatch(const OJString & name, bool createIfMiss)
    {
        size_t i = name.find(OJStr('/'));
        if(i == name.npos)
        {
            return getChild(name, createIfMiss);
        }
        else
        {
            Watch * child = getChild(name.substr(0, i), createIfMiss);

            if(child) 
                return child->getWatch(name.substr(i+1), createIfMiss);
            else
                return NULL;
        }
    }

    void Watch::delWatch(const OJString & name)
    {
        size_t i = name.find(OJStr('/'));
        if(i == name.npos)
        {
            WatchMap::iterator it = children_.find(name);
            if(it != children_.end())
            {
                children_.erase(it);
                delete it->second;
            }
        }
        else
        {
            Watch * child = getChild(name.substr(0, i), false);
            if(child)
                child->delWatch(name.substr(i+1));
        }
    }

    void Watch::update(ValueProxyPtr v)
    {
        value_ = v;

        doNotify();
    }

    void Watch::doNotify()
    {
        std::list<IWatchListener*>::iterator it = listeners_.begin();
        while(it != listeners_.end())
        {
            (*it)->listen(value_);
        }
    }

    void Watch::addListener(IWatchListener * listener)
    {
        std::list<IWatchListener*>::iterator it = std::find(
            listeners_.begin(), listeners_.end(), listener);

        if(it == listeners_.end())
        {
            listeners_.push_back(listener);
        }
    }

    void Watch::delListener(IWatchListener * listener)
    {
        listeners_.remove(listener);
    }

    ////////////////////////////////////////////////////////////////////

#define MAKE_VALUE_PROXY(NAME, TYPE) \
    class NAME##Proxy : public IValueProxy\
    {\
        TYPE value_;\
    public:\
        explicit NAME##Proxy(TYPE v) : value_(v) {}\
        virtual OJInt32_t getType() const { return ValueProxyType::NAME; }\
        virtual bool isEqual(const IValueProxy * v) const{ return value_ == v->as##NAME();}\
        virtual TYPE as##NAME() const { return value_; }\
    };\
    ValueProxyPtr buildWatchValue(const TYPE & v){ return ValueProxyPtr(new NAME##Proxy(v));}

    MAKE_VALUE_PROXY(Bool, bool)
    MAKE_VALUE_PROXY(Char, OJChar_t)
    MAKE_VALUE_PROXY(Int32, OJInt32_t)
    MAKE_VALUE_PROXY(UInt32, OJUInt32_t)
    MAKE_VALUE_PROXY(Float16, OJFloat16_t)
    MAKE_VALUE_PROXY(Float32, OJFloat32_t)
    MAKE_VALUE_PROXY(String, OJString)

    ValueProxyPtr buildWatchValue(const OJChar_t *& v)
    {
        return buildWatchValue(OJString(v));
    }

    static void _doTest(bool exp, const char * name = NULL)
    {
        if(exp)
        {
            OutputDebugStringA("Succed Test Case:");
            if(name) OutputDebugStringA(name);
        }
        else
        {
            OutputDebugStringA("Failed Test Case:");
            if(name) OutputDebugStringA(name);
            assert(false && "_doTest");
        }
    }

    void doWatchTest()
    {
        rootWatch()->watch(OJStr("core/totalTime"), 99.0);
        rootWatch()->watch(OJStr("core/running"), true);
        rootWatch()->watch(OJStr("core/name"), GetOJString("judgecore"));
        rootWatch()->watch(OJStr("core/numThread"), 3);

        Watch *p;

        p = rootWatch()->getWatch(OJStr("core/totalTime"));
        _doTest(p && p->getValue()->asFloat32() == 99.0, "core/totalTime");

        p = rootWatch()->getWatch(OJStr("core/running"));
        _doTest(p && p->getValue()->asBool() == true, "core/running");

        p = rootWatch()->getWatch(OJStr("core/name"));
        _doTest(p && p->getValue()->asString() == OJStr("judgecore"), "core/name");

        p = rootWatch()->getWatch(OJStr("core/numThread"));
        _doTest(p && p->getValue()->asInt32() == 3, "core/numThread");
    }

}//end namespace IMUST