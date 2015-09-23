// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef THREADSAFEGLOBALMOCK_H
#define THREADSAFEGLOBALMOCK_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#ifdef _MSC_VER

#include "stdafx.h"
#include "mock.h"
#include "mockcallrecorder.h"
#include "micromockexception.h"

template<class C>
class CThreadSafeGlobalMock : public CMock<C>
{
public:
    CThreadSafeGlobalMock(_In_ AUTOMATIC_CALL_COMPARISON performAutomaticCallComparison = AUTOMATIC_CALL_COMPARISON_ON) :
        CMock<C>(performAutomaticCallComparison)
    {
        if (NULL != InterlockedCompareExchangePointer((void**)&m_GlobalMockInstance, this, NULL))
        {
            TCHAR errorString[1024];
            _stprintf_s(errorString, COUNT_OF(errorString), _T("Attempting to use mock %S in a multithreading environment"), typeid(C).name());
            MOCK_THROW(CMicroMockException(MICROMOCK_EXCEPTION_INTERNAL_ERROR,
                errorString));
        }
    }

    virtual ~CThreadSafeGlobalMock()
    {
        if (this != InterlockedCompareExchangePointer((void**)&m_GlobalMockInstance, NULL, this))
        {
            TCHAR errorString[1024];
            _stprintf_s(errorString, COUNT_OF(errorString), _T("Mock global instance for mock %S has been changed while the mock was used"), typeid(C).name());
            MOCK_THROW(CMicroMockException(MICROMOCK_EXCEPTION_INTERNAL_ERROR,
                errorString));
        }
    }

    _Check_return_
    _Post_satisfies_(return != NULL)
    static CThreadSafeGlobalMock<C>* GetSingleton()
    {
        if (NULL == m_GlobalMockInstance)
        {
            TCHAR errorString[1024];
            _stprintf_s(errorString, COUNT_OF(errorString), _T("Error retrieving singleton for mock %S"), typeid(C).name());
            MOCK_THROW(CMicroMockException(MICROMOCK_EXCEPTION_INTERNAL_ERROR,
                errorString));
        }

        return m_GlobalMockInstance;
    }

protected:
    static CThreadSafeGlobalMock<C>* m_GlobalMockInstance;
};

template<class C>
CThreadSafeGlobalMock<C>* CThreadSafeGlobalMock<C>::m_GlobalMockInstance;

#endif

#endif // THREADSAFEGLOBALMOCK_H