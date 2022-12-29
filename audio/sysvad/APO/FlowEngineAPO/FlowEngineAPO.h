//
// FlowEngineAPO.h -- Copyright (c) Microsoft Corporation. All rights reserved.
//
// Description:
//
//   Declaration of the CFlowEngineAPO class.
//

#pragma once

#include <audioenginebaseapo.h>
#include <BaseAudioProcessingObject.h>
#include <FlowEngineAPOInterface.h>
#include <FlowEngineAPODll.h>

#include <commonmacros.h>
#include <devicetopology.h>

_Analysis_mode_(_Analysis_code_type_user_driver_)

#define PK_EQUAL(x, y)  ((x.fmtid == y.fmtid) && (x.pid == y.pid))

//
// Define a GUID identifying the type of this APO's custom effect.
//
// APOs generally should not define new GUIDs for types of effects and instead
// should use predefined effect types. Only define a new GUID if the effect is
// truly very different from all predefined types of effects.
//
// {B8EC75BA-00ED-434C-A732-064A0F00788E}
DEFINE_GUID(FlowEngineEffectId,       0xb8ec75ba, 0x00ed, 0x434c, 0xa7, 0x32, 0x06, 0x4a, 0x0f, 0x00, 0x78, 0x8e);

LONG GetCurrentEffectsSetting(IPropertyStore* properties, PROPERTYKEY pkeyEnable, GUID processingMode);

#pragma AVRT_VTABLES_BEGIN
// FlowEngine APO class - MFX
class CFlowEngineAPOMFX :
    public CComObjectRootEx<CComMultiThreadModel>,
    public CComCoClass<CFlowEngineAPOMFX, &CLSID_FlowEngineAPOMFX>,
    public CBaseAudioProcessingObject,
    public IMMNotificationClient,
    public IAudioSystemEffects2,
    // IAudioSystemEffectsCustomFormats may be optionally supported
    // by APOs that attach directly to the connector in the DEFAULT mode streaming graph
    public IAudioSystemEffectsCustomFormats, 
    public IFlowEngineAPOMFX
{
public:
    // constructor
    CFlowEngineAPOMFX()
    :   CBaseAudioProcessingObject(sm_RegProperties)
    ,   m_hEffectsChangedEvent(NULL)
    ,   m_AudioProcessingMode(AUDIO_SIGNALPROCESSINGMODE_DEFAULT)
    ,   m_fEnableFlowEngineMFX(FALSE)
    {
        m_pf32Coefficients = NULL;
    }

    virtual ~CFlowEngineAPOMFX();    // destructor

DECLARE_REGISTRY_RESOURCEID(IDR_FlowEngineAPOMFX)

BEGIN_COM_MAP(CFlowEngineAPOMFX)
    COM_INTERFACE_ENTRY(IFlowEngineAPOMFX)
    COM_INTERFACE_ENTRY(IAudioSystemEffects)
    COM_INTERFACE_ENTRY(IAudioSystemEffects2)
    // IAudioSystemEffectsCustomFormats may be optionally supported
    // by APOs that attach directly to the connector in the DEFAULT mode streaming graph
    COM_INTERFACE_ENTRY(IAudioSystemEffectsCustomFormats)
    COM_INTERFACE_ENTRY(IMMNotificationClient)
    COM_INTERFACE_ENTRY(IAudioProcessingObjectRT)
    COM_INTERFACE_ENTRY(IAudioProcessingObject)
    COM_INTERFACE_ENTRY(IAudioProcessingObjectConfiguration)
END_COM_MAP()

DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
    STDMETHOD_(void, APOProcess)(UINT32 u32NumInputConnections,
        APO_CONNECTION_PROPERTY** ppInputConnections, UINT32 u32NumOutputConnections,
        APO_CONNECTION_PROPERTY** ppOutputConnections);

    STDMETHOD(GetLatency)(HNSTIME* pTime);

    STDMETHOD(LockForProcess)(UINT32 u32NumInputConnections,
        APO_CONNECTION_DESCRIPTOR** ppInputConnections,  
        UINT32 u32NumOutputConnections, APO_CONNECTION_DESCRIPTOR** ppOutputConnections);

    STDMETHOD(Initialize)(UINT32 cbDataSize, BYTE* pbyData);

    // IAudioSystemEffects2
    STDMETHOD(GetEffectsList)(_Outptr_result_buffer_maybenull_(*pcEffects)  LPGUID *ppEffectsIds, _Out_ UINT *pcEffects, _In_ HANDLE Event);

    virtual HRESULT ValidateAndCacheConnectionInfo(
                                    UINT32 u32NumInputConnections, 
                                    APO_CONNECTION_DESCRIPTOR** ppInputConnections, 
                                    UINT32 u32NumOutputConnections, 
                                    APO_CONNECTION_DESCRIPTOR** ppOutputConnections);

    // IMMNotificationClient
    STDMETHODIMP OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) 
    { 
        UNREFERENCED_PARAMETER(pwstrDeviceId); 
        UNREFERENCED_PARAMETER(dwNewState); 
        return S_OK; 
    }
    STDMETHODIMP OnDeviceAdded(LPCWSTR pwstrDeviceId)
    { 
        UNREFERENCED_PARAMETER(pwstrDeviceId); 
        return S_OK; 
    }
    STDMETHODIMP OnDeviceRemoved(LPCWSTR pwstrDeviceId)
    { 
        UNREFERENCED_PARAMETER(pwstrDeviceId); 
        return S_OK; 
    }
    STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId)
    { 
        UNREFERENCED_PARAMETER(flow); 
        UNREFERENCED_PARAMETER(role); 
        UNREFERENCED_PARAMETER(pwstrDefaultDeviceId); 
        return S_OK; 
    }
    STDMETHODIMP OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);

    // IAudioSystemEffectsCustomFormats
    // This interface may be optionally supported by APOs that attach directly to the connector in the DEFAULT mode streaming graph
    STDMETHODIMP GetFormatCount(UINT* pcFormats);
    STDMETHODIMP GetFormat(UINT nFormat, IAudioMediaType** ppFormat);
    STDMETHODIMP GetFormatRepresentation(UINT nFormat, _Outptr_ LPWSTR* ppwstrFormatRep);

    // IAudioProcessingObject
    STDMETHODIMP IsOutputFormatSupported(IAudioMediaType *pOppositeFormat, IAudioMediaType *pRequestedOutputFormat, IAudioMediaType **ppSupportedOutputFormat);

    STDMETHODIMP CheckCustomFormats(IAudioMediaType *pRequestedFormat);
    
public:
    LONG                                    m_fEnableFlowEngineMFX;
    GUID                                    m_AudioProcessingMode;
    CComPtr<IPropertyStore>                 m_spAPOSystemEffectsProperties;
    CComPtr<IMMDeviceEnumerator>            m_spEnumerator;
    static const CRegAPOProperties<1>       sm_RegProperties;   // registration properties

    // Locked memory
    FLOAT32                                 *m_pf32Coefficients;

private:
    CCriticalSection                        m_EffectsLock;
    HANDLE                                  m_hEffectsChangedEvent;

    HRESULT ProprietaryCommunicationWithDriver(APOInitSystemEffects2 *_pAPOSysFxInit2);

};
#pragma AVRT_VTABLES_END


#pragma AVRT_VTABLES_BEGIN
// FlowEngine APO class - SFX
class CFlowEngineAPOSFX :
    public CComObjectRootEx<CComMultiThreadModel>,
    public CComCoClass<CFlowEngineAPOSFX, &CLSID_FlowEngineAPOSFX>,
    public CBaseAudioProcessingObject,
    public IMMNotificationClient,
    public IAudioSystemEffects2,
    public IFlowEngineAPOSFX
{
public:
    // constructor
    CFlowEngineAPOSFX()
    :   CBaseAudioProcessingObject(sm_RegProperties)
    ,   m_hEffectsChangedEvent(NULL)
    ,   m_AudioProcessingMode(AUDIO_SIGNALPROCESSINGMODE_DEFAULT)
    ,   m_fEnableFlowEngineSFX(FALSE)
    ,   m_fEnableDelaySFX(FALSE)
    {
    }

    virtual ~CFlowEngineAPOSFX();    // destructor

DECLARE_REGISTRY_RESOURCEID(IDR_FlowEngineAPOSFX)

BEGIN_COM_MAP(CFlowEngineAPOSFX)
    COM_INTERFACE_ENTRY(IFlowEngineAPOSFX)
    COM_INTERFACE_ENTRY(IAudioSystemEffects)
    COM_INTERFACE_ENTRY(IAudioSystemEffects2)
    COM_INTERFACE_ENTRY(IMMNotificationClient)
    COM_INTERFACE_ENTRY(IAudioProcessingObjectRT)
    COM_INTERFACE_ENTRY(IAudioProcessingObject)
    COM_INTERFACE_ENTRY(IAudioProcessingObjectConfiguration)
END_COM_MAP()

DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
    STDMETHOD_(void, APOProcess)(UINT32 u32NumInputConnections,
        APO_CONNECTION_PROPERTY** ppInputConnections, UINT32 u32NumOutputConnections,
        APO_CONNECTION_PROPERTY** ppOutputConnections);

    STDMETHOD(GetLatency)(HNSTIME* pTime);

    STDMETHOD(LockForProcess)(UINT32 u32NumInputConnections,
        APO_CONNECTION_DESCRIPTOR** ppInputConnections,  
        UINT32 u32NumOutputConnections, APO_CONNECTION_DESCRIPTOR** ppOutputConnections);

    STDMETHOD(Initialize)(UINT32 cbDataSize, BYTE* pbyData);

    // IAudioSystemEffects2
    STDMETHOD(GetEffectsList)(_Outptr_result_buffer_maybenull_(*pcEffects)  LPGUID *ppEffectsIds, _Out_ UINT *pcEffects, _In_ HANDLE Event);

    // IMMNotificationClient
    STDMETHODIMP OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) 
    { 
        UNREFERENCED_PARAMETER(pwstrDeviceId); 
        UNREFERENCED_PARAMETER(dwNewState); 
        return S_OK; 
    }
    STDMETHODIMP OnDeviceAdded(LPCWSTR pwstrDeviceId)
    { 
        UNREFERENCED_PARAMETER(pwstrDeviceId); 
        return S_OK; 
    }
    STDMETHODIMP OnDeviceRemoved(LPCWSTR pwstrDeviceId)
    { 
        UNREFERENCED_PARAMETER(pwstrDeviceId); 
        return S_OK; 
    }
    STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId)
    { 
        UNREFERENCED_PARAMETER(flow); 
        UNREFERENCED_PARAMETER(role); 
        UNREFERENCED_PARAMETER(pwstrDefaultDeviceId); 
        return S_OK; 
    }
    STDMETHODIMP OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);

public:
    LONG                                    m_fEnableFlowEngineSFX;
    LONG                                    m_fEnableDelaySFX;
    GUID                                    m_AudioProcessingMode;
    CComPtr<IPropertyStore>                 m_spAPOSystemEffectsProperties;
    CComPtr<IMMDeviceEnumerator>            m_spEnumerator;
    static const CRegAPOProperties<1>       sm_RegProperties;   // registration properties

    CCriticalSection                        m_EffectsLock;
    HANDLE                                  m_hEffectsChangedEvent;
};
#pragma AVRT_VTABLES_END

OBJECT_ENTRY_AUTO(__uuidof(FlowEngineAPOMFX), CFlowEngineAPOMFX)
OBJECT_ENTRY_AUTO(__uuidof(FlowEngineAPOSFX), CFlowEngineAPOSFX)

//
//   Declaration of the ProcessFlowEngine routine.
//
void ProcessFlowEngine(
    FLOAT32 *pf32OutputFrames,
    const FLOAT32 *pf32InputFrames,
    UINT32   u32ValidFrameCount,
    UINT32   u32SamplesPerFrame);

//
//   Declaration of the ProcessFlowEngineScale routine.
//
void ProcessFlowEngineScale(
    FLOAT32 *pf32OutputFrames,
    const FLOAT32 *pf32InputFrames,
    UINT32   u32ValidFrameCount,
    UINT32   u32SamplesPerFrame,
    FLOAT32 *pf32Coefficients );

//
//   Convenience methods
//

void WriteSilence(
    _Out_writes_(u32FrameCount * u32SamplesPerFrame)
        FLOAT32 *pf32Frames,
    UINT32 u32FrameCount,
    UINT32 u32SamplesPerFrame );

void CopyFrames(
    _Out_writes_(u32FrameCount * u32SamplesPerFrame)
        FLOAT32 *pf32OutFrames,
    _In_reads_(u32FrameCount * u32SamplesPerFrame)
        const FLOAT32 *pf32InFrames,
    UINT32 u32FrameCount,
    UINT32 u32SamplesPerFrame );
