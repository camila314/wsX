#include "pch.h"
#include "Symbols.h"
#include "Hooks.h"
#include "Gui.h"
#include <sstream>
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
    fwrite(&Macro, sizeof(MType), arraySize, saveLocation);
    fclose(saveLocation);
    return true;
}

void practicePrune(double pos) {
    for (int i = 0; i < arrayCounter; i++) {
        if (PracticeMode[i].xpos >= pos) {
            arrayCounter = i;
            break;
        }
    }
}

char const* getPickupString() {

    std::string output;

    for (int i = 0; i < arrayCounter; i++) {
        MType event = PracticeMode[i];
        if (event.key != SPACE && event.key != ARROW)
            break;
        int itemid = 1;
        int count = -1;
        if (event.down == true) {
            itemid = 0;
            count = 1;
        }
        printf("paste xpos: %f\n", event.xpos);
        char formatted[256];
        sprintf_s(formatted, "1,1817,2,%lf,3,%d,36,1,80,%d,77,%d;", event.xpos, 100 + (50 * itemid), itemid, count);
        output += formatted;
    }

    if (output.length() > 2) {
        output.pop_back();
        return output.c_str();
    }
    else {
        return "9";
    }
}

void pastePickups() {
    char const* lvlstring = getPickupString();

    if (*lvlstring == '9') {
        printf("why did you press k\n");
        return;
    }

    printf("before the standard string\n");
    std::istringstream stdstring(lvlstring);
    std::string s;
    printf("standard string\n");

    wptr state = *(wptr*)(base + 0x3222d0);
    wptr layer = *((wptr*)(state + 0x168));
    if (layer) {
        void* editor = *((void**)(layer + 0x380));
        while (getline(stdstring, s, ';')) {
            if (!s.empty()) {
                s.push_back(';');
                pasteObjects(editor, s);
            }
        }
    }
    else {
        printf("you arent even in the editor lmao\n");
    }
}

void rout_rec(wptr a, double b) {
    if (arrayCounter >= arraySize) return;

    if (practice_record_mode) {
        wptr playobj = *((wptr*)(a + 0x224));
        prev_xpos = *((float*)(playobj + 0x67c));
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
            //unimplemented();
            MType tmp;
            double xpos = prev_xpos;
            tmp.xpos = xpos;
            tmp.key = ARROW;
            tmp.down = modifier1_keyDown;
            PracticeMode[arrayCounter] = tmp;
            //printf("practice macro xpos: %lf\n", prev_xpos);

        }
        ++arrayCounter;
    }
    if (modifier2 == 1) {
        if (!practice_record_mode) {
            printf("added to macro\n");
            MType tmp;
            tmp.xpos = b;
            tmp.key = SPACE;
            tmp.down = modifier2_keyDown;
            Macro[arrayCounter] = tmp;
            //if (remotePort && CFMessagePortIsValid(remotePort))
            //    sendAdd(arrayCounter, &tmp);
        }
        else {
            //unimplemented();
            MType tmp;
            double xpos = prev_xpos;
            tmp.xpos = xpos;
            tmp.key = SPACE;
            tmp.down = modifier2_keyDown;
            PracticeMode[arrayCounter] = tmp;
            //printf("practice macro xpos: %lf\n", prev_xpos);

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
        printf("playback: %d with down: %d\n", currnt.key, currnt.down);
        dispatch_og(dispatcherObject, currnt.key, currnt.down);

        stop_spam_prev = macroXpos;
        //if (remotePort && CFMessagePortIsValid(remotePort))
        //    sendSelect(macro_counter);

        macro_counter++;
        arrayCounter = macro_counter;
    }
}

void __fastcall practice_toggle(void* instance, void*, bool toggle) {
    //uint32_t game_manager = *reinterpret_cast<uint32_t*>(base + 0x3222D0);
    //uint32_t play_layer = *reinterpret_cast<uint32_t*>(game_manager + 0x164);
    practice_og(/*reinterpret_cast<LPVOID>(play_layer)*/instance, toggle);
    practice_record_mode = toggle;
    practice_playerweight = 0;
    printf("me when i toggle practice mode:\n");
    if (toggle) arrayCounter = 0;
}

void* __stdcall newLevel(void* inst) {
    practice_record_mode = false;
    arrayCounter = 0;
    return createPlay(inst);
}

void* __fastcall practice_markCheckpoint(wptr instance) {
    float rota1, acc1, rota2, acc2;
    if (prev_xpos != 0) {
        wptr playob1 = *(wptr*)(instance + 0x224);
        wptr playob2 = *(wptr*)(instance + 0x228);
        if (playob1) {
            rota1 = *(float*)(playob1 + 0x20);
            acc1 = *(float*)(playob1 + 0x62C);
        }
        if (playob2) {
            rota2 = *(float*)(playob2 + 0x20);
            acc2 = *(float*)(playob2 + 0x62C);
        }
        Checkpoint ch = {prev_xpos, rota1, acc1, rota2, acc2};
        checkpoints->push_back(ch);
        practice_hiddencheckweight = prev_xpos;
        printf("added checkweight. weight: %lf\n", practice_hiddencheckweight);
    }
    return practice_ogCheckpoint(instance);
}
void __fastcall practice_removeCheckpoint(void* instance) {
    practice_ogRemove(instance);
    checkpoints->pop_back();
    practice_hiddencheckweight = checkpoints->back().xpos;
    printf("removed checkweight. weight: %lf\n", practice_hiddencheckweight);
}
void __fastcall practice_playerDies(void* instance, void*, void* player, void* game) {
    practice_ogDies(instance, player, game);
    if (practice_record_mode) {
        practice_playerweight = prev_xpos;
        practicePrune(practice_hiddencheckweight);
        prev_xpos = 0.0;
    }
}

void __fastcall resetLevel(wptr instance) {
    resetOg(instance);
    if (practice_record_mode && checkpoints->size()>0) {
            practicePrune(practice_hiddencheckweight);
            printf("size: %d", checkpoints->size());
            wptr playobj1 = *((wptr*)(instance + 0x224));
            wptr playobj2 = *((wptr*)(instance + 0x228));
            Checkpoint ch = checkpoints->back();
            if (!playobj1) return;
            *((float*)(playobj1 + 0x62C)) = ch.accel;
            *((float*)(playobj1 + 0x67c)) = ch.xpos;
            *((float*)(playobj1 + 0x20)) = ch.rotation;
            *((float*)(playobj1 + 0x24)) = ch.rotation;
            printf("does it?\n");
            if (!playobj2) return;
            *((float*)(playobj2 + 0x67c)) = ch.xpos;
            *((float*)(playobj2 + 0x62C)) = ch.accel2;
            *((float*)(playobj2 + 0x20)) = ch.rotation2;
            *((float*)(playobj2 + 0x24)) = ch.rotation2;
    }

}

void* __cdecl routBoth() {
    wptr a = 0;
    __asm mov[a], ecx;
    double b = *reinterpret_cast<double*>(a + 0x450); // this -> field 0x450
    //printf("x positon is %f\n", b);
    //void* ret_val = og(a, b);
    if (play_record == 1 || play_record == 3)
        rout_rec(a, b);
    else if (play_record == 0)
        rout_play(a, b);
    return og(a,b);
}

void __fastcall eventClickCallback(void* inst, void*, void* set, void* ev, unsigned int clic) {
    touches(inst, set, ev, clic);
    if (play_record == 1 && arrayCounter < arraySize) {
        if (clic != 0 && clic != 2) {
            return;
        }
        bool down = clic == 0;
        if (modifier2_keyDown!=down) {
            printf("click\n");
            modifier2 = 1;
            modifier2_keyDown = down;
        }
    }
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
                printf("ok\n");
                modifier2 = 1;
                modifier2_keyDown = isdown;
            }
        }
        return;
    }
    else if (isdown) {
        if (key == 190)
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
    playupdate(a, 1.0f / (SPEED*FPS));
}

void __fastcall fpsLock(void* instance, void* dummy, double fps) {
    if (instance) return;
    setAnimInt(instance, fps);
}

void __fastcall mainLoop(void* instance, void*, float delta) {
    if (doIPaste) {
        pastePickups();
        doIPaste = false;
    }
    ogMain(instance, delta);
}

void __fastcall pickuphook(void* instance, void* dummy, int a, int b) {
    //printf("first param %d second param %d\n", a, b)

    if (play_record == 3 && b!=0) {
        if (a == 628) {
            pickupCount = b;
        } else if (b == pickupCount || b == pickupCount-2) {

            int down = b==pickupCount;
            dispatch_og(dispatcherObject, 32, down);
            modifier1 = 1;
            modifier1_keyDown = down;
        }
    }
    pickup(instance, a, b);
}

void* __fastcall editorLock(void* inst, void* dummy, float delta) {
    return editorInit(inst, 1.0f / (SPEED * FPS));
}

void setupAddresses() {
    MH_Initialize();
    checkpoints = new std::vector<Checkpoint>();
    base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    cocosbase = GetModuleHandleA("libcocos2d.dll");

    pasteObjects = reinterpret_cast<void(__thiscall*)(void*, std::string)>(base + 0x88240);
    if (!cocosbase) return;

    set_time_scale = reinterpret_cast<void(__thiscall*)(void*,float)>(GetProcAddress(cocosbase, "?setTimeScale@CCScheduler@cocos2d@@QAEXM@Z"));
    shared_director = reinterpret_cast<void*(__cdecl*)(void)>(GetProcAddress(cocosbase, "?sharedDirector@CCDirector@cocos2d@@SAPAV12@XZ"));
    shared_scheduler = reinterpret_cast<void*(__thiscall*)(void*)>(GetProcAddress(cocosbase, "?getShaderProgram@CCTexture2D@cocos2d@@UAEPAVCCGLProgram@2@XZ"));
    sharedApplication = reinterpret_cast<void* (__stdcall*)(void)>(GetProcAddress(cocosbase, "?sharedApplication@CCApplication@cocos2d@@SAPAV12@XZ"));
    //setAnimInt = reinterpret_cast<void(__thiscall*)(void*, double)>(GetProcAddress(cocosbase, "?setAnimationInterval@CCApplication@cocos2d@@UAEXN@Z"));
    rd_route(base + 0x20af40, routBoth, og);
    rd_route(base + 0x2029c0, deltaOverride, playupdate);
    rd_route(base + 0x26a660, editorLock, editorInit);

    void* cocos_dispatch = GetProcAddress(cocosbase, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z");
    rd_route(cocos_dispatch, eventTapCallback, dispatch_og);

    void* fps_lock = GetProcAddress(cocosbase, "?setAnimationInterval@CCApplication@cocos2d@@UAEXN@Z");
    rd_route(fps_lock, fpsLock, setAnimInt);

    rd_route(base + 0x111890, pickuphook, pickup);

    void* touche = GetProcAddress(cocosbase, "?touches@CCTouchDispatcher@cocos2d@@QAEXPAVCCSet@2@PAVCCEvent@2@I@Z");
    rd_route(touche, eventClickCallback, touches);

    rd_route(base + 0x20D0D0, practice_toggle, practice_og);
    rd_route(base + 0x20b050, practice_markCheckpoint, practice_ogCheckpoint);
    rd_route(base + 0x20B830, practice_removeCheckpoint, practice_ogRemove);
    rd_route(base + 0x20a1a0, practice_playerDies, practice_ogDies);
    rd_route(base + 0x1fb6d0, newLevel, createPlay);
    rd_route(base + 0x20BF00, resetLevel, resetOg);

    void* mainthread = GetProcAddress(cocosbase, "?update@CCScheduler@cocos2d@@UAEXM@Z");
    rd_route(mainthread, mainLoop, ogMain);
    printf("its injected: %p\n", cocos_dispatch);
}