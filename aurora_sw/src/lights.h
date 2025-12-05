#pragma once

#include <OctoWS2811.h>


// Hell-Lighting config
#define LED_BRIGHTNESS 100  // out of 255

#define N_STRIPS 2
#define LEDS_PER_STRIP 529
#define N_LEDS 1058

const int n_leds[N_STRIPS] = {529, 529};
const int led_dirs[N_STRIPS] = {-1, 1};

const byte pin_list[8] = {1, 2, 3, 4, 5, 6, 7};


namespace lights {

void init();
void show();
void blank();
void setRGB(int idx, double r, double g, double b);
void setHSV(int idx, double h, double s, double v);
void getRGB(int idx, double *r, double *g, double *b);

void hsv2rgb(double h, double s, double v, double *r, double *g, double *b);

extern OctoWS2811 leds;

}
