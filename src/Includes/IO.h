#pragma once

#include "../Includes/defs.h"

struct IO;

struct IO* createIO();
void intializeLogFile(struct IO* output);
void intializePerfFile(struct IO* output);
void printLog(struct IO* output, PCB *running, int clock);
void printPerf(struct IO* output, float utilization, float avgWTA, float avgWaiting, float stdWTA);