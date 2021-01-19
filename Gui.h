#pragma once

void getFileOpenName(bool (*callback)(const char*));
void getFileSaveName(bool (*callback)(const char*));
void getSpeed(void (*callback)(float));
void getFps(void (*callback)(double));