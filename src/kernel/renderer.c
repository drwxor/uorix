/* SPDX-License-Identifier: GPL-3.0-only */

#include "kernel/renderer.h"

#include <stdarg.h>
#include <stdint.h>

#include "kernel/io.h"

static struct limine_framebuffer *framebuffer;

static uint64_t cursor_x;
static uint64_t cursor_y;

static const uint64_t font_width = 8;
static const uint64_t font_height = 8;
static const uint64_t font_spacing = 1;

static const uint32_t foreground = 0x00FFFFFF;

static const uint8_t font[26][8] = {
    /* a */
    {
        0x00, 0x00, 0x3C, 0x06,
        0x3E, 0x66, 0x3E, 0x00
    },

    /* b */
    {
        0x60, 0x60, 0x6C, 0x76,
        0x66, 0x66, 0x7C, 0x00
    },

    /* c */
    {
        0x00, 0x00, 0x3C, 0x66,
        0x60, 0x66, 0x3C, 0x00
    },

    /* d */
    {
        0x06, 0x06, 0x36, 0x6E,
        0x66, 0x66, 0x3E, 0x00
    },

    /* e */
    {
        0x00, 0x00, 0x3C, 0x66,
        0x7E, 0x60, 0x3C, 0x00
    },

    /* f */
    {
        0x0E, 0x18, 0x18, 0x3E,
        0x18, 0x18, 0x18, 0x00
    },

    /* g */
    {
        0x00, 0x00, 0x3E, 0x66,
        0x66, 0x3E, 0x06, 0x3C
    },

    /* h */
    {
        0x60, 0x60, 0x6C, 0x76,
        0x66, 0x66, 0x66, 0x00
    },

    /* i */
    {
        0x18, 0x00, 0x38, 0x18,
        0x18, 0x18, 0x3C, 0x00
    },

    /* j */
    {
        0x06, 0x00, 0x0E, 0x06,
        0x06, 0x66, 0x3C, 0x00
    },

    /* k */
    {
        0x60, 0x60, 0x6C, 0x78,
        0x78, 0x6C, 0x66, 0x00
    },

    /* l */
    {
        0x38, 0x18, 0x18, 0x18,
        0x18, 0x18, 0x3C, 0x00
    },

    /* m */
    {
        0x00, 0x00, 0x6C, 0x7E,
        0x6B, 0x6B, 0x63, 0x00
    },

    /* n */
    {
        0x00, 0x00, 0x7C, 0x66,
        0x66, 0x66, 0x66, 0x00
    },

    /* o */
    {
        0x00, 0x00, 0x3C, 0x66,
        0x66, 0x66, 0x3C, 0x00
    },

    /* p */
    {
        0x00, 0x00, 0x7C, 0x66,
        0x66, 0x7C, 0x60, 0x60
    },

    /* q */
    {
        0x00, 0x00, 0x3E, 0x66,
        0x66, 0x3E, 0x06, 0x06
    },

    /* r */
    {
        0x00, 0x00, 0x6C, 0x76,
        0x60, 0x60, 0x60, 0x00
    },

    /* s */
    {
        0x00, 0x00, 0x3E, 0x60,
        0x3C, 0x06, 0x7C, 0x00
    },

    /* t */
    {
        0x18, 0x18, 0x7E, 0x18,
        0x18, 0x1C, 0x0E, 0x00
    },

    /* u */
    {
        0x00, 0x00, 0x66, 0x66,
        0x66, 0x66, 0x3E, 0x00
    },

    /* v */
    {
        0x00, 0x00, 0x66, 0x66,
        0x66, 0x3C, 0x18, 0x00
    },

    /* w */
    {
        0x00, 0x00, 0x63, 0x6B,
        0x6B, 0x7F, 0x36, 0x00
    },

    /* x */
    {
        0x00, 0x00, 0x66, 0x3C,
        0x18, 0x3C, 0x66, 0x00
    },

    /* y */
    {
        0x00, 0x00, 0x66, 0x66,
        0x3E, 0x06, 0x3C, 0x00
    },

    /* z */
    {
        0x00, 0x00, 0x7E, 0x0C,
        0x18, 0x30, 0x7E, 0x00
    }
};


static const uint8_t digits[10][8] = {
    /* 0 */
    {
        0x3C, 0x66, 0x6E, 0x76,
        0x66, 0x66, 0x3C, 0x00
    },

    /* 1 */
    {
        0x18, 0x38, 0x18, 0x18,
        0x18, 0x18, 0x7E, 0x00
    },

    /* 2 */
    {
        0x3C, 0x66, 0x06, 0x0C,
        0x30, 0x60, 0x7E, 0x00
    },

    /* 3 */
    {
        0x3C, 0x66, 0x06, 0x1C,
        0x06, 0x66, 0x3C, 0x00
    },

    /* 4 */
    {
        0x0C, 0x1C, 0x2C, 0x4C,
        0x7E, 0x0C, 0x0C, 0x00
    },

    /* 5 */
    {
        0x7E, 0x60, 0x7C, 0x06,
        0x06, 0x66, 0x3C, 0x00
    },

    /* 6 */
    {
        0x1C, 0x30, 0x60, 0x7C,
        0x66, 0x66, 0x3C, 0x00
    },

    /* 7 */
    {
        0x7E, 0x06, 0x0C, 0x18,
        0x30, 0x30, 0x30, 0x00
    },

    /* 8 */
    {
        0x3C, 0x66, 0x66, 0x3C,
        0x66, 0x66, 0x3C, 0x00
    },

    /* 9 */
    {
        0x3C, 0x66, 0x66, 0x3E,
        0x06, 0x0C, 0x38, 0x00
    }
};

static const uint8_t glyph_space[8] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

static const uint8_t glyph_bang[8] = {
    0x18, 0x18, 0x18, 0x18,
    0x18, 0x00, 0x18, 0x00
};

static const uint8_t glyph_dot[8] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x00
};

static const uint8_t glyph_colon[8] = {
    0x00, 0x18, 0x18, 0x00,
    0x00, 0x18, 0x18, 0x00
};

static const uint8_t glyph_dash[8] = {
    0x00, 0x00, 0x00, 0x7E,
    0x00, 0x00, 0x00, 0x00
};

static const uint8_t glyph_slash[8] = {
    0x06, 0x0C, 0x18, 0x30,
    0x60, 0xC0, 0x80, 0x00
};

static const uint8_t glyph_percent[8] = {
    0x62, 0x64, 0x08, 0x10,
    0x20, 0x4C, 0x8C, 0x00
};

static const uint8_t glyph_dollar[8] = {
    0x08,
    0x3E,
    0x68,
    0x3C,
    0x16,
    0x7C,
    0x08,
    0x00
};

static const uint8_t glyph_underscore[8] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x7E,
    0x00
};

static const uint8_t glyph_lparen[8] = {
    0x0C,
    0x18,
    0x30,
    0x30,
    0x30,
    0x18,
    0x0C,
    0x00
};

static const uint8_t glyph_rparen[8] = {
    0x30,
    0x18,
    0x0C,
    0x0C,
    0x0C,
    0x18,
    0x30,
    0x00
};

#define COM1 0x3F8

static void
serial_putc(char c)
{
    while (!(inb(COM1 + 5) & 0x20))
        ;

    outb(COM1, c);
}

static const
uint8_t
*glyph(char c)
{
    if (c >= 'a' && c <= 'z')
        return font[c - 'a'];

    if (c >= 'A' && c <= 'Z')
        return font[c - 'A'];

    if (c >= '0' && c <= '9')
        return digits[c - '0'];

    switch (c)
    {
    case ' ':
        return glyph_space;

    case '!':
        return glyph_bang;

    case '.':
        return glyph_dot;

    case '_':
        return glyph_underscore;

    case '$':
        return glyph_dollar;

    case ':':
        return glyph_colon;

    case '-':
        return glyph_dash;

    case '/':
        return glyph_slash;

    case '%':
        return glyph_percent;

    case '(':
        return glyph_lparen;

    case ')':
        return glyph_rparen;

    default:
        return glyph_space;
    }
}

static
void
putpixel(uint64_t x, uint64_t y, uint32_t color)
{
    if (framebuffer == 0)
        return;

    if (x >= framebuffer->width || y >= framebuffer->height)
        return;

    uint32_t *pixel =
        (uint32_t *)(
            (uint8_t *)framebuffer->address +
            y * framebuffer->pitch
        );

    pixel[x] = color;
}

void
renderer_init(struct limine_framebuffer *fb)
{
    framebuffer = fb;

    cursor_x = 32;
    cursor_y = 32;
}

void
render_clear(uint32_t color)
{
    if (framebuffer == 0)
        return;

    for (uint64_t y = 0; y < framebuffer->height; y++)
    {
        uint32_t *row =
            (uint32_t *)(
                (uint8_t *)framebuffer->address +
                y * framebuffer->pitch
            );

        for (uint64_t x = 0; x < framebuffer->width; x++)
            row[x] = color;
    }

    cursor_x = 32;
    cursor_y = 32;
}

static
void
draw_glyph(const uint8_t *g, uint64_t x, uint64_t y)
{
    for (uint64_t row = 0; row < font_height; row++)
    {
        for (uint64_t col = 0; col < font_width; col++)
        {
            if (g[row] & (1 << (7 - col)))
                putpixel(x + col, y + row, foreground);
        }
    }
}

void
render_putc(char c)
{
    serial_putc(c);

    if (framebuffer == 0)
        return;

    if (c == '\n') {
        cursor_x = 32;
        cursor_y += font_height + font_spacing;
        return;
    }

    if (c == '\r') {
        cursor_x = 32;
        return;
    }

    if (c == '\b') {
        if (cursor_x >= 32 + font_width + font_spacing) {
            cursor_x -= font_width + font_spacing;

            for (uint64_t row = 0; row < font_height; row++) {
                for (uint64_t col = 0;
                     col < font_width + font_spacing;
                     col++) {
                    putpixel(
                        cursor_x + col,
                        cursor_y + row,
                        0x00000000
                    );
                }
            }
        }

        return;
    }

    draw_glyph(glyph(c), cursor_x, cursor_y);

    cursor_x += font_width + font_spacing;

    if (cursor_x + font_width >= framebuffer->width)
    {
        cursor_x = 32;
        cursor_y += font_height + font_spacing;
    }

    if (cursor_y + font_height >= framebuffer->height)
        cursor_y = 32;
}

void
render_puts(const char *s)
{
    while (*s)
        render_putc(*s++);
}

static
void
render_uint(uint64_t value, uint32_t base)
{
    char buffer[32];
    uint32_t length = 0;

    if (value == 0)
    {
        render_putc('0');
        return;
    }

    while (value != 0)
    {
        uint32_t digit = value % base;

        if (digit < 10)
            buffer[length++] = '0' + digit;
        else
            buffer[length++] = 'a' + digit - 10;

        value /= base;
    }

    while (length != 0)
        render_putc(buffer[--length]);
}

static
void
render_int(int64_t value)
{
    if (value < 0)
    {
        render_putc('-');
        render_uint((uint64_t)(-value), 10);
    }
    else
    {
        render_uint((uint64_t)value, 10);
    }
}

void
render_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    while (*fmt != '\0')
    {
        if (*fmt != '%')
        {
            render_putc(*fmt);
            fmt++;
            continue;
        }

        fmt++;

        switch (*fmt)
        {
        case '%':
            render_putc('%');
            break;

        case 'c':
            render_putc((char)va_arg(args, int));
            break;

        case 's':
            render_puts(va_arg(args, const char *));
            break;

        case 'u':
            render_uint(va_arg(args, uint64_t), 10);
            break;

        case 'x':
            render_uint(va_arg(args, uint64_t), 16);
            break;

        case 'd':
            render_int(va_arg(args, int64_t));
            break;

        default:
            render_putc('%');
            render_putc(*fmt);
            break;
        }

        fmt++;
    }

    va_end(args);
}
