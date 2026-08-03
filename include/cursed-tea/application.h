#pragma once

#include <cursed-tea/logger.h>
#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>

struct CtModel;

// ~~[ CURSED TEA APPLICATION
// Starts internal structures. Has to be called before model setups as some
// models might enqueue events on their setup
void ct_app_init();

// Start drawing to the screen
void ct_app_start(struct CtModel *root);
