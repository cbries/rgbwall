/*
 * Copyright (C) 2015 Christian Benjamin Ries
 * Website: www.christianbenjaminries.de
 * Source: https://github.com/cbries
 * License: MIT
 */

#ifndef __RGBHANDLING_H__
#define __RGBHANDLING_H__

#pragma once

typedef struct RgbColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RgbColor;

typedef struct HsvColor
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
} HsvColor;

RgbColor HsvToRgb(HsvColor hsv);
HsvColor RgbToHsv(RgbColor rgb);

#endif // __RGBHANDLING_H__

