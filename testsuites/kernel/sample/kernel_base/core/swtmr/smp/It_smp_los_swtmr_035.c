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

static UINT32 g_sztskId[LOSCFG_KERNEL_CORE_NUM] = {0};
static UINT32 g_ret = 0xff;
static VOID SwtmrF01(void)
{
    LOS_AtomicInc(&g_testCount);
}

static VOID TaskF01(void)
{
    UINT32 ret, i;

    for (i = 0; i < TRandom() % 500; i++) { // 500, Gets a random number between 0 and 500
    }

    ret = LOS_SwtmrStart(g_swTmrID);
    g_ret = ret;
    return;
EXIT:
    LOS_SwtmrDelete(g_swTmrID);
    LOS_TaskDelete(g_testTaskID01);
    LOS_TaskDelete(g_testTaskID02);
    return;
}

static VOID TaskF02(void)
{
    UINT32 ret, i;

    for (i = 0; i < TRandom() % 500; i++) { // 500, Gets a random number between 0 and 500
    }

    ret = LOS_SwtmrDelete(g_swTmrID);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    return;
EXIT:
    LOS_SwtmrDelete(g_swTmrID);
    LOS_TaskDelete(g_testTaskID01);
    LOS_TaskDelete(g_testTaskID02);
    return;
}

static UINT32 Testcase(void)
{
    TSK_INIT_PARAM_S testTask;
    UINT32 ret;
    UINT32 currCpuid;
    UINT32 i;

    g_testPeriod = 10; // period is 10

    currCpuid = (ArchCurrCpuid() + 1) % (LOSCFG_KERNEL_CORE_NUM);

    for (i = 0; i < LOOP; i++) {
        g_testCount = 0;

        ret = LOS_SwtmrCreate(g_testPeriod, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)SwtmrF01, &g_swTmrID, 0);
        ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

        TEST_TASK_PARAM_INIT_AFFI(testTask, "it_swtmr_033_task1", TaskF01, TASK_PRIO_TEST_SWTMR - 1,
            CPUID_TO_AFFI_MASK(currCpuid)); // current cpu
        ret = LOS_TaskCreate(&g_testTaskID01, &testTask);
        ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

        TEST_TASK_PARAM_INIT_AFFI(testTask, "it_swtmr_033_task1", TaskF02, TASK_PRIO_TEST_SWTMR - 1,
            CPUID_TO_AFFI_MASK(ArchCurrCpuid())); // other cpu
        ret = LOS_TaskCreate(&g_testTaskID02, &testTask);
        ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

        LOS_TaskDelay(1);

        if ((g_ret != LOS_OK) && (g_ret != LOS_ERRNO_SWTMR_NOT_CREATED)) {
            ICUNIT_GOTO_EQUAL(1, 0, g_ret, EXIT);
        }

        LOS_TaskDelay(g_testPeriod + 10); // g_testPeriod+10, delay enough time
        ICUNIT_GOTO_EQUAL(g_testCount, 0, g_testCount, EXIT);

        LOS_SwtmrDelete(g_swTmrID);
    }

EXIT:
    LOS_TaskDelete(g_testTaskID01);
    LOS_TaskDelete(g_testTaskID02);
    LOS_SwtmrDelete(g_swTmrID);
    return LOS_OK;
}

VOID ItSmpLosSwtmr035(VOID)
{
    TEST_ADD_CASE("ItSmpLosSwtmr035", Testcase, TEST_LOS, TEST_SWTMR, TEST_LEVEL1, TEST_FUNCTION);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

