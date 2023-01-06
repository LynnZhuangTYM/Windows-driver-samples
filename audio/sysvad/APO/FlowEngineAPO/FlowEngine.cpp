//
// FlowEngine.cpp -- Copyright (c) Microsoft Corporation. All rights reserved.
//
// Description:
//
//  Implementation of FlowEngineSamples
//
#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <atlsync.h>
#include <mmreg.h>

#include <audioenginebaseapo.h>
#include <baseaudioprocessingobject.h>
#include <resource.h>

#include <float.h>

#include "FlowEngineAPO.h"
#include "FlowEngine.h"

#pragma AVRT_CODE_BEGIN
void WriteSilence(
    _Out_writes_(u32FrameCount * u32SamplesPerFrame)
        FLOAT32 *pf32Frames,
    UINT32 u32FrameCount,
    UINT32 u32SamplesPerFrame )
{
    ZeroMemory(pf32Frames, sizeof(FLOAT32) * u32FrameCount * u32SamplesPerFrame);
}
#pragma AVRT_CODE_END

#pragma AVRT_CODE_BEGIN
void CopyFrames(
    _Out_writes_(u32FrameCount * u32SamplesPerFrame)
        FLOAT32 *pf32OutFrames,
    _In_reads_(u32FrameCount * u32SamplesPerFrame)
        const FLOAT32 *pf32InFrames,
    UINT32 u32FrameCount,
    UINT32 u32SamplesPerFrame )
{
    CopyMemory(pf32OutFrames, pf32InFrames, sizeof(FLOAT32) * u32FrameCount * u32SamplesPerFrame);
}
#pragma AVRT_CODE_END

#pragma AVRT_CODE_BEGIN
void ProcessFlowEngine(
    FLOAT32 *pf32OutputFrames,
    const FLOAT32 *pf32InputFrames,
    UINT32   u32ValidFrameCount,
    UINT32   u32SamplesPerFrame )
{
    UINT32   u32SampleIndex;
    FLOAT32  fFlowEngine32;

    ASSERT_REALTIME();
    ATLASSERT( IS_VALID_TYPED_READ_POINTER(pf32InputFrames) );
    ATLASSERT( IS_VALID_TYPED_WRITE_POINTER(pf32OutputFrames) );

    // loop through samples
    while (u32ValidFrameCount--)
    {
        for (u32SampleIndex=0; u32SampleIndex+1<u32SamplesPerFrame; u32SampleIndex += 2)
        {
            // apply FlowEngine
            fFlowEngine32 = *pf32InputFrames;
            *pf32OutputFrames = *(pf32InputFrames + 1);
            pf32OutputFrames++;
            *pf32OutputFrames = fFlowEngine32;
            pf32OutputFrames++;
            pf32InputFrames += 2;
        }
    }
}
#pragma AVRT_CODE_END


#pragma AVRT_CODE_BEGIN

#pragma AVRT_CODE_END
