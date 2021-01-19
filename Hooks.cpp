#include "pch.h"
#include "Symbols.h"
#include "Hooks.h"
#include "Gui.h"

#define rd_route(a, b, c) MH_CreateHook((void*)(a), b, (void**)&c); \
    MH_EnableHook((void*)(a))
#define unimplemented() printf("%s\n", "unimplemented~!")

void changeSpeed(float num) {
    if (num == 0.0) return;
    //float n = (1.0 / (FPS * num));
    set_time_scale(shared_scheduler(shared_director()), 1. / num);
    SPEED = num;
}

void changeFps(double fps) {
    if (fps == 0.0) return;
    setAnimInt(sharedApplication(), 1. / fps);
    FPS = fps;
}

bool loadFromFile(const char* fileName) {
    FILE* saveLocation;
    fopen_s(&saveLocation, fileName, "rb");
    if (!saveLocation) return false;
    fread(Macro, sizeof(MType), arraySize, saveLocation);
    fclose(saveLocation);
    return true;
}

bool saveToFile(const char* fileName) {
    FILE* saveLocation;
    fopen_s(&saveLocation, fileName, "wb");
    if (!saveLocation) return false;
    fwrite(Macro, sizeof(MType), arraySize, saveLocation);
    fclose(saveLocation);
    return true;
}

void rout_rec(wptr a, double b) {
    if (arrayCounter >= arraySize) return;


    if (prev_xpos == 0.0 && practice_record_mode) {
        //printf("we at 0,  checkweight is %lf\n", practice_hiddencheckweight);
        //practicePrune(practice_hiddencheckweight);
    }

    if (practice_record_mode) {
        /*int64_t playobj = *((int64_t*)(a + 0x380));
        prev_xpos = *((float*)(playobj + 0x7c8));*/
    }
    else {
        prev_xpos = b;
    }

    /*if(play_record==3)
        b -= 1.0/(SPEED*FPS);*/
    if (modifier1 == 1) {
        if (!practice_record_mode) {
            MType tmp;
            tmp.xpos = b;
            tmp.key = ARROW;
            tmp.down = modifier1_keyDown;
            Macro[arrayCounter] = tmp;
            printf("added to macro\n");
            //if (remotePort && CFMessagePortIsValid(remotePort))
            //    sendAdd(arrayCounter, &tmp);
        }
        else {
            unimplemented();
            /*MType tmp;
            double xpos = prev_xpos;
            tmp.xpos = xpos;
            tmp.key = ARROW;
            tmp.down = modifier1_keyDown;
            PracticeMode[arrayCounter] = tmp;*/
        }
        ++arrayCounter;
    }
    if (modifier2 == 1) {
        if (!practice_record_mode) {
            MType tmp;
            tmp.xpos = b;
            tmp.key = SPACE;
            tmp.down = modifier1_keyDown;
            Macro[arrayCounter] = tmp;
            //if (remotePort && CFMessagePortIsValid(remotePort))
            //    sendAdd(arrayCounter, &tmp);
        }
        else {
            unimplemented();
            /* MType tmp;
            double xpos = prev_xpos;
            tmp.xpos = b;
            tmp.key = SPACE;
            tmp.down = modifier1_keyDown;
            PracticeMode[arrayCounter] = tmp;*/
        }
        ++arrayCounter;
    }
    modifier1 = 0;
    modifier2 = 0;
}

void rout_play(wptr a, double b) {
    b -= 0.0000001;
    if (b <= 1.0f / (SPEED * FPS)) {
        printf("beginning of level\n");
        macro_counter = 0;
        stop_spam_prev = 0.0;

        if (audioEnabled) {
            printf("%s\n", "unimplemented~!");
        }
    }
    if (macro_counter >= arraySize) {
        printf("too large\n");
        return;
    }
    MType currnt = Macro[macro_counter];
    register double macroXpos = currnt.xpos;
    //printf("epic %lf moment, macro index %d, real position %lf\n", macroXpos, macro_counter, b);
    if (macroXpos <= b && macroXpos != 0 && macroXpos > stop_spam_prev) {
        printf("playback\n");
        dispatch_og(dispatcherObject, currnt.key, currnt.down);

        stop_spam_prev = macroXpos;
        //if (remotePort && CFMessagePortIsValid(remotePort))
        //    sendSelect(macro_counter);

        macro_counter++;
        arrayCounter = macro_counter;
    }
}

void* __cdecl routBoth() {
    wptr a = 0;
    __asm mov[a], ecx;
    double b = *reinterpret_cast<double*>(a + 0x450); // this -> field 0x450
    printf("x positon is %f\n", b);
    //void* ret_val = og(a, b);
    if (play_record == 1 || play_record == 3) {
        rout_rec(a, b);
    }
    else if (play_record == 0) {
        rout_play(a, b);
    }
    return og(a,b);
}

void __fastcall eventTapCallback(void* inst, void*, int key, bool isdown) {
    dispatcherObject = inst;

    dispatch_og(inst, key, isdown);

    printf("press key: %d\n", key);
    if (key == ARROW || key == SPACE) {
        // printf("swag   play_record = %d, arrayCounter = %d, arraySize = %d\n", play_record, arrayCounter, arraySize);
        if (play_record == 1 && arrayCounter < arraySize) {
            if (key == ARROW && modifier1_keyDown != isdown) {
                printf("ok\n");
                modifier1 = 1;
                modifier1_keyDown = isdown;
            }
            else if (key == SPACE && modifier2_keyDown != isdown) {
                modifier2 = 1;
                modifier2_keyDown = isdown;
            }
        }
        return;
    }
    else if (isdown) {
        if (key == 9)
            keybinds = !keybinds;
        if (keybinds) {
            switch (key) {
            case 65:
                arrayCounter = 0;
                return;
            case 87:
                play_record = 2;
                return;
            case 80:
                printf("play mode enabled\n");
                play_record = 0;
                return;
            case 82:
                printf("record mode enabled\n");
                play_record = 1;
                return;
            case 69:
                printf("pickup mode enabled\n");
                play_record = 3;
                return;
            case 75:
                doIPaste = true;
                return;
            case 67:
                getSpeed(changeSpeed);
                //unimplemented();
                return;
            case 70:
                getFps(changeFps);
                //unimplemented();
                return;
            case 83:
                getFileSaveName(saveToFile);
                //unimplemented();
                return;
            case 76:
                getFileOpenName(loadFromFile);
                //unimplemented();
                return;
            case 77:
                audioEnabled = false;
                return;
            case 84:
                //getWavFile(loadWav);
                unimplemented();
                return;
            default:
                return;
            }
        }
        else {
            return;
        }
    }
}

void __fastcall deltaOverride(wptr a, void*, float b) {
    playupdate(a, 1.0f / (SPEED * FPS));
}

void __fastcall fpsLock(void* instance, void* dummy, double fps) {
    if (instance) return;
    setAnimInt(instance, fps);
}

void setupAddresses() {
    MH_Initialize();
    base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    cocosbase = GetModuleHandleA("libcocos2d.dll");

    if (!cocosbase) return;

    set_time_scale = reinterpret_cast<void(__thiscall*)(void*,float)>(GetProcAddress(cocosbase, "?setTimeScale@CCScheduler@cocos2d@@QAEXM@Z"));
    shared_director = reinterpret_cast<void*(__cdecl*)(void)>(GetProcAddress(cocosbase, "?sharedDirector@CCDirector@cocos2d@@SAPAV12@XZ"));
    shared_scheduler = reinterpret_cast<void*(__thiscall*)(void*)>(GetProcAddress(cocosbase, "?getShaderProgram@CCTexture2D@cocos2d@@UAEPAVCCGLProgram@2@XZ"));
    sharedApplication = reinterpret_cast<void* (__stdcall*)(void)>(GetProcAddress(cocosbase, "?sharedApplication@CCApplication@cocos2d@@SAPAV12@XZ"));
    setAnimInt = reinterpret_cast<void(__thiscall*)(void*, double)>(GetProcAddress(cocosbase, "?setAnimationInterval@CCApplication@cocos2d@@UAEXN@Z"));
    rd_route(base + 0x20af40, routBoth, og);
    rd_route(base + 0x2029c0, deltaOverride, playupdate);

    void* cocos_dispatch = GetProcAddress(cocosbase, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z");
    rd_route(cocos_dispatch, eventTapCallback, dispatch_og);

    void* fps_lock = GetProcAddress(cocosbase, "?setAnimationInterval@CCApplication@cocos2d@@UAEXN@Z");
    rd_route(fps_lock, fpsLock, setAnimInt);

    printf("its injected: %p\n", cocos_dispatch);
}