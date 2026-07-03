#pragma once
#define PIXEL_PER_UNIT 20
float PixelToUnit(float pixels);
float UnitToPixel(float unit);
void initGlobalRes();
void freeGlobalRes();