/* SPDX-License-Identifier: GPL-3.0-only */

#ifndef UORIX_RENDERER_H
#define UORIX_RENDERER_H

#include <stdint.h>

#include "kernel/limine.h"

void renderer_init(struct limine_framebuffer *fb,struct limine_flanterm_fb_init_params *font_params);

void render_clear(uint32_t color);
void render_putc(char c);
void render_puts(const char *s);
void render_printf(const char *fmt, ...);

#endif
