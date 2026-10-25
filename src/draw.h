#pragma once
#include "utils.h"
#include "bsp.h"
#include <stdbool.h>

void frameupd(frame *);
void pixel(uv2, rgb, frame *);
bool rgbequal(rgb, rgb);

void fillframe(frame *, rgb);

void drawwall(wall, pos, frame *, trg);
void drawside(vec2 *, u32, rgb, frame *, trg);
void drawbsp(bspnode *, pos, frame *, trg);
