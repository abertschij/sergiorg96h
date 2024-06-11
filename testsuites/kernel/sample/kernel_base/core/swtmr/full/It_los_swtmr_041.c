/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "It_los_swtmr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

static VOID SwtmrF01(UINT32 arg)
{
    UINT32 ret;

    TEST_HwiClear(HWI_NUM_TEST);
    ret = LOS_SwtmrDelete(g_swTmrID1);

    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);

    ret = LOS_SwtmrDelete(g_swTmrID2);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);
    g_testCount1++;

    TEST_HwiDelete(HWI_NUM_TEST);
    return;

EXIT:
    LOS_SwtmrDelete(g_swTmrID1);
    LOS_SwtmrDelete(g_swTmrID2);
    TEST_HwiDelete(HWI_NUM_TEST);
    return;
}

static VOID SwtmrF02(UINT32 arg)
{
    g_testCount1++;

    return;
}

static VOID SwtmrF03(UINT32 arg)
{
    g_testCount1++;

    return;
}


static UINT32 Testcase(VOID)
{
    UINT32 ret;
    HWI_PRIOR_T hwiPrio = 3;
    HWI_MODE_T hwiMode = 0;
    HWI_ARG_T arg = 0;
    g_testCount1 = 0;

    TEST_HwiClear(HWI_NUM_TEST);

    // 10, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(10, LOS_SWTMR_MODE_ONCE, (SWTMR_PROC_FUNC)SwtmrF03, &g_swTmrID1, arg);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    // 20, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(20, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)SwtmrF02, &g_swTmrID2, arg);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT3);

    ret = TEST_HwiCreate(HWI_NUM_TEST, hwiPrio, hwiMode, (HWI_PROC_FUNC)SwtmrF01, (HwiIrqParam *)arg);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT5);
#ifdef LOSCFG_KERNEL_SMP
    HalIrqSetAffinity(HWI_NUM_TEST, CPUID_TO_AFFI_MASK(ArchCurrCpuid()));
#endif

    ret = LOS_SwtmrStart(g_swTmrID2);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT5);

    ret = LOS_SwtmrStart(g_swTmrID1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT5);
    TestHwiTrigger(HWI_NUM_TEST);

    ret = LOS_TaskDelay(10); // 10, set delay time
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ICUNIT_GOTO_EQUAL(g_testCount1, 1, g_testCount1, EXIT5);
    TEST_HwiDelete(HWI_NUM_TEST);
    return LOS_OK;

EXIT5:
    TEST_HwiDelete(HWI_NUM_TEST);

EXIT3:
    LOS_SwtmrDelete(g_swTmrID2);

EXIT:
    LOS_SwtmrDelete(g_swTmrID1);
}

VOID ItLosSwtmr041(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosSwtmr041", Testcase, TEST_LOS, TEST_SWTMR, TEST_LEVEL2, TEST_FUNCTION);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

