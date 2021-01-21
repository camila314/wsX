#include "pch.h"
#include <Windows.h>
#include <commdlg.h>
#include <cstdlib>
#include <cstdio>
#include "Gui.h"
#include "InputBox.h"

void fileDialogWrapper(BOOL WINAPI method(LPOPENFILENAMEA),
    bool (*callback)(const char*)) 
{
    OPENFILENAMEA info;
    ZeroMemory(&info, sizeof info);
    CHAR fileName[MAX_PATH] = "";
    info.lStructSize = sizeof info;
    info.hwndOwner = NULL;
    info.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
    info.lpstrFile = fileName;
    info.nMaxFile = MAX_PATH;
    info.lpstrDefExt = "xgd";
    if (method(&info)) 
        callback(info.lpstrFile);
}

void getFileOpenName(bool (*callback)(const char*)) 
{
    fileDialogWrapper(GetOpenFileNameA, callback);
}

void getFileSaveName(bool (*callback)(const char*)) 
{
    fileDialogWrapper(GetSaveFileNameA, callback);
}

template  <typename T, typename R> 
void inputBoxWrapper(const char* prompt, 
    const char* title, 
    const char* default_input, 
    T(*callback)(R)) 
{
    const char* out = InputBox(
        (char*)(prompt),
        (char*)(title),
        (char*)(default_input));
    char* didWork;
    double num = strtod(out, &didWork);
    if (*didWork)
        printf("Invalid number\n");
    else
        callback(num);
}

void getSpeed(void (*callback)(float)) 
{
    inputBoxWrapper("Change speed", "wsX", "1.0", callback);
}

void getFps(void (*callback)(double)) 
{
    inputBoxWrapper("Change FPS", "wsX", "60.0", callback);
}