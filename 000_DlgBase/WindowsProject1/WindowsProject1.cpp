// WindowsProject1.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "WindowsProject1.h"
#include "resource.h"

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス

// このコード モジュールに含まれる関数の宣言を転送します:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK MyDlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    hInst = hInstance;

    DialogBox(hInst, L"MyTestDlgBase_Main", NULL, (DLGPROC)MyDlgProc);

    return (int)0;
}

// ダイアログプロシージャ
BOOL CALLBACK MyDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDOK:
            EndDialog(hDlg, IDOK);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        case IDC_BUTTON1:
            MessageBox(NULL, L"", L"", MB_OK);
            break;
        }
        return FALSE;
    }
    return FALSE;
}
