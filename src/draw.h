#pragma once
#include "utils.h"
#include <stdbool.h>

void pixel(uv2, rgb, frame *);
bool rgbequal(rgb, rgb);

void fillframe(frame *, rgb);

void drawwall(wall, pos, frame *, trg);
void drawside(vec2 *, u32, rgb, frame *, trg);
