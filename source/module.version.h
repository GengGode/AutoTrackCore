#pragma once
#include "ErrorCode.h"
#include "utils/convect.string.h"
#include "resource/version.h"
#ifndef err
    #define err ErrorCode::getInstance()
#endif
#include <global/global.include.h>
#define regerr(msg) err = { error(msg), utils::to_gbk(msg) }

inline bool GetCoreCompileVersion(char* version_buff, int buff_size)
{
    if (version_buff == nullptr || buff_size < 1)
    {
        regerr("缓存区为空指针或是缓存区大小为小于1");
        return false;
    }
    if (TianLi::Version::build_version.size() > buff_size)
    {
        // err("缓存区大小不足");
        regerr("缓存区大小不足");
        return false;
    }
    strcpy_s(version_buff, buff_size, TianLi::Version::build_version.c_str());
    return true;
}

inline bool GetCoreCompileTime(char* time_buff, int buff_size)
{
    if (time_buff == nullptr || buff_size < 1)
    {
        // err("缓存区为空指针或是缓存区大小为小于1");
        regerr("缓存区为空指针或是缓存区大小为小于1");
        return false;
    }
    if (TianLi::Version::build_time.size() > buff_size)
    {
        // err("缓存区大小不足");
        regerr("缓存区大小不足");
        return false;
    }
    strcpy_s(time_buff, buff_size, TianLi::Version::build_time.c_str());
    return true;
}
#undef err