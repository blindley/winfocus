
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#undef min
#undef max

#include <algorithm>
#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <optional>
#include <format>

#include "rectangle.h"
#include "config.h"

#define MIN_WINDOW_SIZE 200
#define MIN_VISIBLE_AREA 100000

typedef struct WindowData {
    HWND hwnd;
    HWND owner;
    HWND parent;
    LONG styles;
    LONG exstyles;
    int level;
    DWORD pid;
    std::string caption;
    std::string image;

    Rectangle_t windowRect;

    bool isVisible() const {
        return (styles & WS_VISIBLE) != 0;
    }
} WindowData;

std::optional<WindowData> getWindowData(HWND hwnd) {
    static HWND desktop = GetDesktopWindow();

    if (!IsWindow(hwnd)) {
        return std::nullopt;
    }

    WindowData winData = {};
    winData.hwnd = hwnd;
    winData.styles = GetWindowLong(hwnd, GWL_STYLE);
    winData.exstyles = GetWindowLong(hwnd, GWL_EXSTYLE);
    winData.owner = GetWindow(hwnd, GW_OWNER);
    winData.parent = GetParent(hwnd);

    if (hwnd == desktop)
        winData.level = 0;
    else {
        winData.level = 1;
        auto parent = winData.parent;
        while ((parent != NULL) && (parent != desktop)) {
            winData.level++;
            parent = GetParent(parent);
        }
    }

    RECT rect;
    GetWindowRect(hwnd, &rect);
    winData.windowRect.left = rect.left;
    winData.windowRect.top = rect.top;
    winData.windowRect.right = rect.right;
    winData.windowRect.bottom = rect.bottom;

    if (winData.level > 1) {
        POINT pt;
        pt.x = winData.windowRect.left;
        pt.y = winData.windowRect.top;
        ScreenToClient(winData.parent, &pt);
        winData.windowRect.left = pt.x;
        winData.windowRect.top = pt.y;
    }

    GetWindowThreadProcessId(hwnd, &winData.pid);

    {
        char imageFilename[MAX_PATH];
        GetWindowModuleFileNameA(hwnd, imageFilename, MAX_PATH);
        winData.image = imageFilename;
    }

    {
        int len = GetWindowTextLengthA(hwnd);
        if (len) {
            std::vector<char> data;
            data.resize(len + 1);
            GetWindowTextA(hwnd, &data[0], data.size());
            winData.caption = data.data();
            for (auto & c : winData.caption) {
                if (c == '\n')
                    c = ' ';
            }
        }
        else {
            char data[256];
            GetClassNameA(hwnd, data, 256);
            winData.caption = data;
        }
    }

    return winData;
}

template<typename HandlerFunc>
BOOL CALLBACK enumWindowProc(HWND hwnd, LPARAM lParam) {
    HandlerFunc* pHandler = (HandlerFunc*) lParam;

    if (auto theWindow = getWindowData(hwnd)) {
        (*pHandler)(*theWindow);
    }

    return TRUE;
}

template<typename HandlerFunc>
void enumAllWindows(HandlerFunc handler) {
    EnumWindows(&enumWindowProc<HandlerFunc>, (LPARAM) &handler);
}


BOOL monitorEnumProc(HMONITOR hmon, HDC hdc, LPRECT rect, LPARAM lparam) {
    std::vector<Rectangle_t>* recVec = (std::vector<Rectangle_t>*) lparam;

    Rectangle_t r;
    r.left = rect->left;
    r.top = rect->top;
    r.right = rect->right;
    r.bottom = rect->bottom;

    recVec->emplace_back(r);
    return TRUE;
}

std::vector<Rectangle_t> getMonitors() {
    std::vector<Rectangle_t> result;

    EnumDisplayMonitors(NULL, NULL, monitorEnumProc, (LPARAM) &result);

    return result;
}

std::ostream& operator<<(std::ostream& ostr, Rectangle_t const& rect) {
    ostr << "[" << rect.left << ", " << rect.top << ", " << rect.right << ", " << rect.bottom << "]";
    return ostr;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    auto monitors = getMonitors();

    Rectangle_t infinityRect { INT_MIN, INT_MIN, INT_MAX, INT_MAX };
    std::vector<Rectangle_t> occlusionRects;
    occlusionRects.push_back(infinityRect);
    for (auto& mon : monitors) {
        std::vector<Rectangle_t> newOcclusionRects;
        for (auto const& br : occlusionRects) {
            auto nbr = clipAgainst(br, mon);
            nbr.appendToContainer(newOcclusionRects);
        }
        occlusionRects = newOcclusionRects;
    }

    std::vector<WindowData> candidateWindows;

    enumAllWindows([&](WindowData theWindow) {
        if (theWindow.isVisible()) {
            std::deque<Rectangle_t> windowVisibility;
            windowVisibility.push_back(theWindow.windowRect);
            
            for (auto const& oc : occlusionRects) {
                if (windowVisibility.empty())
                    break;
                int oldSize = windowVisibility.size();
                for (int i = 0; i < oldSize; i++) {
                    auto sections = clipAgainst(windowVisibility[0], oc);
                    sections.appendToContainer(windowVisibility);
                    windowVisibility.pop_front();
                }
            }

            int totalVisibleArea = 0;
            for (auto& area : windowVisibility) {
                totalVisibleArea += area.width() * area.height();
                occlusionRects.push_back(area);
            }

            int width = theWindow.windowRect.height();
            int height = theWindow.windowRect.width();
            if (width > MIN_WINDOW_SIZE && height > MIN_WINDOW_SIZE &&
                totalVisibleArea > MIN_VISIBLE_AREA)
            {
                candidateWindows.push_back(theWindow);
            }
        }


    });

    std::sort(candidateWindows.begin(), candidateWindows.end(),
        [](WindowData const& a, WindowData const& b) {
            return a.windowRect.left < b.windowRect.left;
        });

    auto config = parseCommandLine();
    auto vpConfig = config.visualProminenceConfig;
    int windowIndex = vpConfig.windowIndex;

    if (windowIndex >= 0 && windowIndex < candidateWindows.size()) {
        auto const& window = candidateWindows[windowIndex];
        std::cout << window.caption << '\n';
        SetForegroundWindow(window.hwnd);
    }
}
