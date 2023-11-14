// mediafoundation
#include <mfapi.h>
#include <mfplay.h>
#include <Dbt.h>

// IMFPMediaPlayerCallback を継承したクラス
#include "Preview.h"

// その他諸々
#include <strsafe.h>
#include "framework.h"
#include "WindowsProject1.h"
#include "resource.h"

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

// MediaFoundation関連
IMFPMediaPlayer* m_pPlayer;
IMFMediaSource* m_pSource = NULL;

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
HWND g_hWnd;
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK MyDlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    hInst = hInstance;
    DialogBox(hInst, L"MyTestDlgBase_Main", NULL, (DLGPROC)MyDlgProc);
    return (int)0;
}
// 関数CreateMediaItemFromObject()を読んだら来る
// SetMediaItem()しないと、m_pPlayer->Play()をしても動作再生されない
void OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT* pEvent)
{
    HRESULT hr = S_OK;

    if (m_pPlayer)
    {
        BOOL bHasVideo = FALSE, bIsSelected = FALSE;

        hr = pEvent->pMediaItem->HasVideo(&bHasVideo, &bIsSelected);
        hr = m_pPlayer->SetMediaItem(pEvent->pMediaItem);
    }
}

// m_pPlayer->SetMediaItem()が呼ばれたら来る
// MSのサンプル通り、動画のサイズに合わせてDlgのサイズ合わせしてるが、別にやらなくてもよし。
// やらない場合は、現状のDlgのサイズに合わせて動画が表示されてくれる
void OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT* /*pEvent*/)
{
    HRESULT hr = S_OK;

    SIZE szVideo = { 0 };
    RECT rc = { 0 };

    // ウインドウサイズを、映像に合うように合わせる
    hr = m_pPlayer->GetNativeVideoSize(&szVideo, NULL);

    if (SUCCEEDED(hr))
    {
        SetRect(&rc, 0, 0, szVideo.cx, szVideo.cy);
        AdjustWindowRect(&rc, GetWindowLong(g_hWnd, GWL_STYLE), TRUE);
        SetWindowPos(g_hWnd, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOOWNERZORDER);
    }
}

// 一番の肝の部分。
void StartPreview(HWND hDlg)
{
    CPreview* g_pPreview = NULL;
    IMFActivate** ppDevices;
    WCHAR* m_pwszSymbolicLink;
    UINT32 m_cchSymbolicLink;
    UINT32 count = 0;

    // IMFPMediaPlayerCallback を継承したクラスを作成
    g_pPreview = new CPreview();
    IMFAttributes* pAttributes = NULL;

    // メソッドをセット
    g_pPreview->OnMyMediaItemCreated = OnMediaItemCreated;
    g_pPreview->OnMyMediaItemSet = OnMediaItemSet;

    // MediaFoundationを初期化
    auto hr = MFCreateAttributes(&pAttributes, 1);

    // カメラを取得できるようにGUIDをセット
    hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);

    // デバイス(カメラ)を列挙
    // 実験では、とりあえず見つかった先頭のカメラを入れる
    hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
    for (int i = 0; i < count; i++)
    {
        // IMFPMediaPlayerCallback を継承したクラスから、メディアプレーヤー(m_pPlayer)を作成
        hr = MFPCreateMediaPlayer(NULL, FALSE, 0, g_pPreview, hDlg, &m_pPlayer);
        // メディア(カメラ)をActivateして、そのオブジェクト(m_pSource)を作成
        hr = ppDevices[i]->ActivateObject(__uuidof(IMFMediaSource), (void**)&m_pSource);
        // シンボリックリンク(≒デバイスID)を取得
        hr = ppDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, &m_pwszSymbolicLink, &m_cchSymbolicLink);
        // オブジェクト(m_pSource)から、MediaItemを作成
        hr = m_pPlayer->CreateMediaItemFromObject(m_pSource, FALSE, 0, NULL);
        // →これを読んだら、CPreviewの中の「OnMediaItemCreated()」が呼ばれる
        // 　→さらに、そこでm_pPlayer->SetMediaItem()が呼ばれたら「OnMediaItemSet()」が呼ばれる

        m_pSource->AddRef();
        m_pSource->Release();

        break;
    }

    // 解放処理
    SafeRelease(&pAttributes);
    CoTaskMemFree(ppDevices);
}

// ダイアログプロシージャ
BOOL CALLBACK MyDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
        case WM_INITDIALOG:
        {
            g_hWnd = hDlg;
            StartPreview(hDlg);
            return TRUE;
        }

        case WM_COMMAND:
            switch (LOWORD(wp)) {
            case IDC_BUTTON1:
                // 再生開始
                auto hr = m_pPlayer->Play();
                break;
            }
            return FALSE;

        case WM_CLOSE:
        {
            if (m_pPlayer)
            {
                m_pPlayer->Shutdown();
                m_pPlayer->Release();
                m_pPlayer = NULL;
            }

            if (m_pSource)
            {
                m_pSource->Shutdown();
                m_pSource->Release();
                m_pSource = NULL;
            }

            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
    }
    return FALSE;
}
