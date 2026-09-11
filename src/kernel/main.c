/* SPDX-License-Identifier: GPL-3.0-only */

#include <stdint.h>

#include "kernel/limine.h"
#include "kernel/renderer.h"
#include "shell/shell.h"

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_requests_start_marker[] =
    LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] =
    LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_requests_end_marker[] =
    LIMINE_REQUESTS_END_MARKER;

void kmain(void)
{
    struct limine_framebuffer_response *response =
        framebuffer_request.response;

    if (response == 0 || response->framebuffer_count == 0) {
        for (;;)
            __asm__ volatile ("hlt");
    }

    struct limine_framebuffer *fb =
        response->framebuffers[0];

    renderer_init(fb);
    render_clear(0x00000000);

    render_printf("hi from Uorix!\n");
    render_printf("framebuffer: %ux%u\n",
                  fb->width,
                  fb->height);

    shell_init();
    shell_run();

    for (;;)
        __asm__ volatile ("hlt");
}
