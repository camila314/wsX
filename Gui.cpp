#include "pch.h"
#include <Windows.h>
#include <commdlg.h>
#include <cstdlib>
#include "Gui.h"
#include "InputBox.h"

void getFileOpenName(bool (*callback)(const char*)) {
    OPENFILENAMEA info;
    ZeroMemory(&info, sizeof info);
    CHAR fileName[MAX_PATH] = "";
    info.lStructSize = sizeof info;
    info.hwndOwner = NULL;
    info.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
    info.lpstrFile = fileName;
    info.nMaxFile = MAX_PATH;
    info.lpstrDefExt = "xgd";
    if (GetOpenFileNameA(&info)) {
        callback(info.lpstrFile);
    }
}

void getFileSaveName(bool (*callback)(const char*)) {
    OPENFILENAMEA info;
    ZeroMemory(&info, sizeof info);
    CHAR fileName[MAX_PATH] = "";
    info.lStructSize = sizeof info;
    info.hwndOwner = NULL;
    info.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
    info.lpstrFile = fileName;
    info.nMaxFile = MAX_PATH;
    info.lpstrDefExt = "xgd";
    if (GetSaveFileNameA(&info)) {
        callback(info.lpstrFile);
    }
}

void getSpeed(void (*callback)(float)) {
    char* out = InputBox((char*)"Change speed", (char*)"wsX", (char*)"60.0");
    char* didWork;
    double num = strtod(out, &didWork);
    callback(num);
}

void getFps(bool (*callback)(const double&)) {
    char* out = InputBox((char*)"Change FPS", (char*)"wsX", (char*)"60.0");
    char* didWork;
    double num = strtod(out, &didWork);
    callback(num);
}