/**
 * @file    Utility.h
 * @author  马冬亮
 * @brief   通用组件.
 */

/// @defgroup Utility

#ifndef IMUST_OJ_UTILITY_H
#define IMUST_OJ_UTILITY_H

#include "../platformlayer/PlatformLayer.h"

namespace IMUST
{

// 这是为了防止Argument Dependent Lookup出现问题
namespace Uncopyable_
{

/**
 * @brief   防止对象被复制, 特别适合对效率要求较高的代码使用.
 * @ingroup Utility
 *
 * @attention   一个类中只能声明一次.
 *
 * @code
 * // 示例：
 * class MyClass
 * {
 *      MAKE_CLASS_UNCOPYABLE(MyClass);
 *
 * public:
 *      // 具体代码...
 * };
 * @endcode
 */
#define MAKE_CLASS_UNCOPYABLE(classname)                                       \
private:                                                                       \
    classname(const classname &);                                              \
    classname &operator =(const classname &)

/**
 * @brief   防止对象被复制, 如果代码对性能要求较高, 推荐使用MAKE_CLASS_UNCOPYABLE宏.
 * @ingroup Utility
 *
 * @details 用户自定义类型通过继承IUncopyable接口, 实现阻止复制的目的, 
 *          详细资料请参考《Effective C++》.
 *          一般对于只有一个空基类的继承, 编译器一般都可以使用EBO机制优化.
 *          但是对于多个基类的时候, 一般的编译器就不能进行优化, 造成效率损耗, 
 *          所以对于性能要求较高的代码, 推荐使用MAKE_CLASS_UNCOPYABLE宏.
 * @code
 * // 示例：
 * class MyClass : private ::IMUST::IUncopyable
 * {
 *      // 具体代码...
 * };
 * @endcode
 * @see MAKE_CLASS_UNCOPYABLE
 */
class IUncopyable
{
    MAKE_CLASS_UNCOPYABLE(IUncopyable);

protected:
    IUncopyable() { }
    virtual ~IUncopyable() { }
};

}   // namespace Uncopyable_

typedef Uncopyable_::IUncopyable IUncopyable;

OJString String2OJString(const std::string & str);

OJFloat32_t GetTickTime();

}   // namespace IMUST

#endif  // IMUST_OJ_UTILITY_H
