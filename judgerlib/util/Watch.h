#pragma once

#include <memory>
#include <map>
#include <list>

#include "../platformlayer/PlatformLayer.h"

namespace IMUST
{
    namespace ValueProxyType
    {
        const OJInt32_t Bool = 1;
        const OJInt32_t Char = 2;
        const OJInt32_t Int32 = 3;
        const OJInt32_t UInt32 = 4;
        const OJInt32_t Float16 = 5;
        const OJInt32_t Float32 = 6;
        const OJInt32_t String = 7;
        const OJInt32_t UserType = 0xff;
    }

    ///可被监视的值
    class IValueProxy
    {
    public:
        IValueProxy();
        virtual ~IValueProxy();

        virtual OJInt32_t getType() const = 0;
        virtual bool isEqual(const IValueProxy * v) const = 0;

        virtual bool asBool() const { return false; }
        virtual OJChar_t asChar() const { return OJStr('\0'); }
        virtual OJInt32_t asInt32() const { return 0; }
        virtual OJUInt32_t asUInt32() const { return 0; }
        virtual OJFloat16_t asFloat16() const { return 0.0f;}
        virtual OJFloat32_t asFloat32() const { return 0.0;}
        virtual OJString asString() const { return EmptyString; }
    };

    typedef std::shared_ptr<IValueProxy> ValueProxyPtr;

    ValueProxyPtr buildWatchValue(const bool & v);
    ValueProxyPtr buildWatchValue(const OJChar_t & v);
    ValueProxyPtr buildWatchValue(const OJInt32_t & v);
    ValueProxyPtr buildWatchValue(const OJUInt32_t & v);
    ValueProxyPtr buildWatchValue(const OJFloat16_t & v);
    ValueProxyPtr buildWatchValue(const OJFloat32_t & v);
    ValueProxyPtr buildWatchValue(const OJString & v);
    ValueProxyPtr buildWatchValue(const OJChar_t *& v);

    class Watch;

    class IWatchListener
    {
    public:
        IWatchListener();
        virtual ~IWatchListener();

        virtual void listen(ValueProxyPtr v) = 0;
    };

    ///监视器。一个监视器可以用于若干子监视器。
    class Watch
    {
    public:
        typedef std::map<OJString, Watch*> WatchMap;

        ///构造函数。name中不可含符号'/'。
        explicit Watch(const OJString & name);
        ~Watch();

        ///获取一个子监视器
        Watch * getChild(const OJString & name, bool createIfMiss = true);

        ///递归的获取一个子监视器。name的格式可以是a/b/c
        Watch * getWatch(const OJString & name, bool createIfMiss = true);

        ///删除一叶节点监视器。name的格式可以是a/b/c，此时c会被删除。
        void delWatch(const OJString & name);

        ///添加一个监视
        void addWatch(const OJString & name, ValueProxyPtr v);

        ///用于监视时间段
        void startWatch();
        void endWatch();
        void startWatch(const OJString & name);
        void endWatch(const OJString & name);
        

        ///监视的数据
        template<typename T>
        void watch(const OJString & name, const T & v)
        {
            addWatch(name, buildWatchValue(v) );
        }

        void update(ValueProxyPtr v);

        void addListener(IWatchListener * listener);
        void delListener(IWatchListener * listener);

        ValueProxyPtr getValue() const { return value_;}
        const OJString & getName() const { return name_; }

        WatchMap::iterator begin(){ return children_.begin(); }
        WatchMap::iterator end(){ return children_.end(); }

    protected:

        ///通知所有的listener
        void doNotify();

        ///禁止拷贝构造
        Watch(const Watch &);
        const Watch & operator=(const Watch &);


        OJString name_;         ///<监视器的名称
        ValueProxyPtr value_;   ///<被监视的值
        std::list<IWatchListener*> listeners_;///<当被监视的值发生变化时，会通知给每个listener
        WatchMap children_; ///<子监视器
        OJFloat32_t startTime_;
        
    };

    template<typename T, typename FUN>
    class MethodWatchListener : public IWatchListener
    {
        T   *pTarget_;
        FUN pFun_;
    public:

        MethodWatchListener(T *pTarget, FUN pFun)
            : pTarget_(pTarget)
            , pFun_(pFun)
        {
            assert(pTarget && pFun && "MethodWatchListener construct");
        }

        ~MethodWatchListener()
        {
        }

        virtual void listen(ValueProxyPtr v)
        {
            (pTarget_->*pFun_)(v);
        }
    };

    namespace WatchTool
    {

        void DoWatchTest();

        Watch * Root();

        void LockRoot();
        void UnlockRoot();

        template<typename T>
        void AddWatch(const OJString & name, const T & v)
        {
            LockRoot();
            Root()->addWatch(name, buildWatchValue(v) );
            UnlockRoot();
        }

        template<typename T>
        void WatchCount(const OJString & name, T & value, T delta)
        {
            LockRoot();
            value += delta;
            Root()->watch(name, value);
            UnlockRoot();
        }

        void StartWatch(const OJString & name);
        void EndWatch(const OJString & name);

        template<typename T, typename FUN>
        IWatchListener * MakeWatchListener(T *pTarget, FUN pFun)
        {
            return new MethodWatchListener<T, FUN>(pTarget, pFun);
        }

    }
}