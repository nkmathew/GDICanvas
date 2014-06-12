#include <windows.h>
#include <map>
#include <string>

static std::map<std::string, int> virtualKeys = {
  {"<hover>", 1}, // Means nothing. It's here for convenience
  {"<timer>", 1}, // Means nothing. It's here for convenience
  {"<1>", VK_LBUTTON},
  {"<2>", VK_MBUTTON},
  {"<3>", VK_RBUTTON},
  {"backspace", VK_BACK},
  {"tab", VK_TAB},
  {"return", VK_RETURN},
  {"esc", VK_ESCAPE},
  {"escape", VK_ESCAPE},
  {"space", VK_SPACE},
  {"spacebar", VK_SPACE},
  {"spaceup", VK_PRIOR},
  {"spacedown", VK_NEXT},
  {"home", VK_HOME},
  {"end", VK_END},
  {"left", VK_LEFT},
  {"right", VK_RIGHT},
  {"up", VK_UP},
  {"down", VK_DOWN},
  {"0", 0x30},
  {"1", 0x31},
  {"2", 0x32},
  {"3", 0x33},
  {"4", 0x34},
  {"5", 0x35},
  {"6", 0x36},
  {"7", 0x37},
  {"8", 0x38},
  {"9", 0x39},
  {"a", 0x41},
  {"b", 0x42},
  {"c", 0x43},
  {"d", 0x44},
  {"e", 0x45},
  {"f", 0x46},
  {"g", 0x47},
  {"h", 0x48},
  {"i", 0x49},
  {"j", 0x4A},
  {"k", 0x4B},
  {"l", 0x4C},
  {"m", 0x4D},
  {"n", 0x4E},
  {"o", 0x4F},
  {"p", 0x50},
  {"q", 0x51},
  {"r", 0x52},
  {"s", 0x53},
  {"t", 0x54},
  {"u", 0x55},
  {"v", 0x56},
  {"w", 0x57},
  {"x", 0x58},
  {"y", 0x59},
  {"z", 0x5A},
  {"f1", VK_F1},
  {"f2", VK_F2},
  {"f3", VK_F3},
  {"f4", VK_F4},
  {"f5", VK_F5},
  {"f6", VK_F6},
  {"f7", VK_F7},
  {"f8", VK_F8},
  {"f9", VK_F9},
  {"f10", VK_F10},
  {"f11", VK_F11},
  {"f12", VK_F12},
  {"f13", VK_F13},
  {"f14", VK_F14},
  {"f15", VK_F15},
  {"f16", VK_F16},
  {"f17", VK_F17},
  {"f18", VK_F18},
  {"f18", VK_F19},
  {"f20", VK_F20},
  {"f21", VK_F21},
  {"f22", VK_F22},
  {"f23", VK_F23},
  {"f24", VK_F24}
};

