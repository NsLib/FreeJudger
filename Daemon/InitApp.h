#ifndef IMUST_OJ_INIT_APP_H
#define IMUST_OJ_INIT_APP_H

namespace IMUST
{

bool InitApp();

namespace details
{
static bool InitLogger();
static bool InitAppConfig();
}   // namespace details


}   // namespace IMUST

#endif  // IMUST_OJ_INIT_APP_H