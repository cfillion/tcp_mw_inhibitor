#include <cstdio>
#include <wdltypes.h>

#define REAPERAPI_IMPLEMENT
#include <reaper_plugin_functions.h>

static HWND g_tcp = nullptr;
static WNDPROC g_originalProc = nullptr;

static void fatalError(const char *msg)
{
  MessageBox(Splash_GetWnd ? Splash_GetWnd() : nullptr,
    msg, "TCP Mousewheel Inhibitor - Fatal Error", MB_OK);
}

#define API_FUNC(name) {(void **)&name, #name}

static bool loadAPI(void *(*getFunc)(const char *))
{
  struct ApiFunc { void **ptr; const char *name; };

  const ApiFunc funcs[] = {

    API_FUNC(GetMainHwnd),
    API_FUNC(GetMasterTrack),
    API_FUNC(GetMasterTrackVisibility),
    API_FUNC(SetMasterTrackVisibility),
    API_FUNC(Splash_GetWnd),

  };

  for(const ApiFunc &func : funcs) {
    *func.ptr = getFunc(func.name);

    if(!*func.ptr) {
      char msg[1024] = {};
      snprintf(msg, sizeof(msg) * sizeof(char),
        "Unable to import the following API function: %s", func.name);
      fatalError(msg);

      return false;
    }
  }

  return true;
}

#undef API_FUNC

class EnsureMasterIsVisible {
public:
  enum {
    TCP = 1<<0,
    // MCP = 1<<1,
  };

  EnsureMasterIsVisible()
    : m_visibility(GetMasterTrackVisibility())
  {
    if(!visibleIn(TCP))
      SetMasterTrackVisibility(m_visibility | TCP);
  }

  ~EnsureMasterIsVisible()
  {
    if(!visibleIn(TCP))
      SetMasterTrackVisibility(m_visibility);
  }

private:
  bool visibleIn(const int f) const { return (m_visibility & f) != 0; }
  int m_visibility;
};

HWND findTcp()
{
  EnsureMasterIsVisible visibility;
  const void *master = static_cast<void *>(GetMasterTrack(nullptr));

  HWND mainWindow = GetMainHwnd();
  HWND parent = nullptr;

  // logic inspired by the SWS extension
  // https://github.com/reaper-oss/sws/blob/949beb11a78da6e4984853ed1125a9250a9ade42/Breeder/BR_Util.cpp#L2827
  while((parent = FindWindowEx(mainWindow, parent, nullptr, ""))) {
    HWND tcp = nullptr;

    while((tcp = FindWindowEx(parent, tcp, nullptr, ""))) {
      if(HWND track = FindWindowEx(tcp, nullptr, nullptr, "")) {
        if(master == (void *)GetWindowLongPtr(track, GWLP_USERDATA))
          return tcp;
      }
    }
  }

  return nullptr;
}

WDL_DLGRET InhibitMWProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if(msg == WM_MOUSEWHEEL && GetAsyncKeyState(VK_CONTROL) & 0x8000)
    return false;
  else
    return g_originalProc(handle, msg, wParam, lParam);
}

extern "C" REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(
  REAPER_PLUGIN_HINSTANCE instance, reaper_plugin_info_t *rec)
{
  if(!rec) {
    SetWindowLongPtr(g_tcp, GWLP_WNDPROC, (LONG_PTR)g_originalProc);
    return 0;
  }

  if(rec->caller_version != REAPER_PLUGIN_VERSION || !rec->GetFunc || !loadAPI(rec->GetFunc))
    return 0;

  if(!(g_tcp = findTcp())) {
    fatalError("Failed to locate the TCP inside of the REAPER window.");
    return 0;
  }

  g_originalProc = (WNDPROC)SetWindowLongPtr(g_tcp, GWLP_WNDPROC, (LONG_PTR)InhibitMWProc);
  return 1;
}
