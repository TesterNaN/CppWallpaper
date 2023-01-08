#include<windows.h>
#include<WinUser.h>
#include<iostream>
#define ssize_t SSIZE_T
#include<vlc/vlc.h> 

HWND workerw;
static HWND s_hProgmanWnd = nullptr;
static HWND s_hWorkerWnd = nullptr;

BOOL CALLBACK EnumWindowProcFindDesktopWindow(HWND hwnd, LPARAM lparam){
	HWND p = ::FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr);
	if(p == nullptr) return 1;
	s_hWorkerWnd = ::FindWindowExW(nullptr, hwnd, L"WorkerW", nullptr);
} 
int main() {
    if (s_hProgmanWnd == nullptr) {
        // 先找到Progman 窗口
        s_hProgmanWnd = ::FindWindowExW(GetDesktopWindow(), nullptr, L"Progman", L"Program Manager");
        if (s_hProgmanWnd == nullptr) {
            std::cout<<"找不到Progman窗体"<<std::endl;
            return 1;
        }
        DWORD_PTR lpdwResult = 0;
        ::SendMessage(s_hProgmanWnd, 0x052C, 0xD, 0);
        ::SendMessage(s_hProgmanWnd, 0x052C, 0xD, 1);
        ::SendMessageTimeoutW(s_hProgmanWnd, 0x052C, 0, 0, SMTO_NORMAL, 1000, &lpdwResult);
        ::SendMessageTimeoutW(s_hProgmanWnd, 0x052C, 0, 1, SMTO_NORMAL, 1000, &lpdwResult);

        EnumWindows(EnumWindowProcFindDesktopWindow, 0);
        // ::ShowWindowAsync(s_hWorkerWnd, SW_HIDE);
        ::ShowWindow(s_hWorkerWnd, SW_NORMAL);
    }
    if (s_hWorkerWnd == nullptr) {
        ::SendMessage(s_hProgmanWnd, 0x052C, 0, 0);
        EnumWindows(EnumWindowProcFindDesktopWindow, 0);
    }
    if (s_hWorkerWnd != nullptr) {
        OSVERSIONINFOEX osvi;
        DWORDLONG dwlConditionMask = 0;
        ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        osvi.dwMajorVersion = 6;
        osvi.dwMinorVersion = 2;
        VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
        VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
        if (!VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION, dwlConditionMask)) {
            ShowWindow(s_hWorkerWnd, SW_HIDE);
            s_hWorkerWnd = s_hProgmanWnd;
        }
    }
    if (s_hWorkerWnd == nullptr) {
        std::cout<< "找不到Progman窗体" << std::endl;
        return 1;
    }
	//DWORD_PTR result = 0;
	//SendMessageTimeout(s_hProgmanWnd, 0x052c, NULL, NULL, SMTO_NORMAL, 1000, &result);
	//EnumWindows(EnumWindowProcFindDesktopWindow, NULL);
	
	char videopath[256]= "video.mp4";
	//WideCharToMultiByte(CP_UTF8, 0, L"video.mp4", -1, videopath, 128, NULL , 0);
	
    libvlc_instance_t* inst = libvlc_new(0, nullptr);
    if (inst == nullptr) {
        std::cout << "Unkonwn Error"<< std::endl;
        return 1;
    }
    libvlc_media_t* m = libvlc_media_new_path(inst, videopath);
    libvlc_media_list_t* ml = libvlc_media_list_new(inst);
    libvlc_media_player_t* mp = libvlc_media_player_new_from_media(m);
	libvlc_media_list_player_t* mlp = libvlc_media_list_player_new(inst);
	
	libvlc_media_list_add_media(ml, m);
	libvlc_media_release(m);
	
	libvlc_media_list_player_set_media_list(mlp, ml);
	libvlc_media_list_player_set_media_player(mlp, mp);
	libvlc_media_list_player_set_playback_mode(mlp, libvlc_playback_mode_loop);
	libvlc_media_player_set_hwnd(mp, s_hWorkerWnd);
	libvlc_audio_set_volume(mp, 100);
	libvlc_media_list_player_play(mlp);
	std::cin.get();
}
