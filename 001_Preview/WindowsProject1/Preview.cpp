#include <new>
#include <windows.h>
#include <mfplay.h>
#include <Dbt.h>
#include <shlwapi.h>

#include "Preview.h"

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "shlwapi.lib")

CPreview::CPreview() : 
    m_nRefCount(1)
{

}

CPreview::~CPreview()
{
}

//// IUnknown methods

ULONG CPreview::AddRef()
{
    return InterlockedIncrement(&m_nRefCount);
}

ULONG CPreview::Release()
{
    ULONG uCount = InterlockedDecrement(&m_nRefCount);
    if (uCount == 0)
    {
        delete this;
    }
    // For thread safety, return a temporary variable.
    return uCount;
}

HRESULT CPreview::QueryInterface(REFIID riid, void** ppv)
{
    static const QITAB qit[] = 
    {
        QITABENT(CPreview, IMFPMediaPlayerCallback),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

//// IMFPMediaPlayerCallback methods

void STDMETHODCALLTYPE CPreview::OnMediaPlayerEvent(MFP_EVENT_HEADER * pEventHeader)
{
    switch (pEventHeader->eEventType)
    {
    case MFP_EVENT_TYPE_MEDIAITEM_CREATED:
        OnMyMediaItemCreated(MFP_GET_MEDIAITEM_CREATED_EVENT(pEventHeader));
        break;

    case MFP_EVENT_TYPE_MEDIAITEM_SET:
        OnMyMediaItemSet(MFP_GET_MEDIAITEM_SET_EVENT(pEventHeader));
        break;
    }
}
