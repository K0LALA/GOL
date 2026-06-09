#ifndef _MAIN_H
#define _MAIN_H 1

#include "cells.h"

void addCell(int16_t x, int16_t y);
void initCells(unsigned int w, unsigned int h);
void initPattern(char* fileName, int offsetX, int offsetY);
void displayCells();
void nextStep();

#endif /* _MAIN_H */
