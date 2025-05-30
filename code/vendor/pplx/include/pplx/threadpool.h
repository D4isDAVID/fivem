/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
*
* Simple Linux implementation of a static thread pool.
*
* For the latest on this and related APIs, please see: https://github.com/Microsoft/cpprestsdk
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
***/
#pragma once

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Winfinite-recursion"
#endif
#include "boost/asio.hpp"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#if defined(__ANDROID__)
#include <atomic>
#include <jni.h>
#include "pplx/pplx.h"
#endif

#include "pplx/compat.h"

namespace crossplat {

#if defined(__ANDROID__)
// IDEA: Break this section into a separate android/jni header
extern std::atomic<JavaVM*> JVM;
JNIEnv* get_jvm_env();

struct java_local_ref_deleter
{
    void operator()(jobject lref) const
    {
        crossplat::get_jvm_env()->DeleteLocalRef(lref);
    }
};

template<class T>
using java_local_ref = std::unique_ptr<typename std::remove_pointer<T>::type, java_local_ref_deleter>;
#endif

class threadpool
{
public:
    static threadpool& shared_instance();
    _ASYNCRTIMP static std::unique_ptr<threadpool> __cdecl construct(size_t num_threads);

    virtual ~threadpool() = default;

    template<typename T>
    CASABLANCA_DEPRECATED("Use `.service().post(task)` directly.")
    void schedule(T task)
    {
        service().post(task);
    }

    boost::asio::io_service& service() { return m_service; }

protected:
    threadpool(size_t num_threads) : m_service(num_threads) {}

    boost::asio::io_service m_service;
};

}
