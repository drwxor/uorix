/* SPDX-License-Identifier: GPL-3.0-only */

#include <stdint.h>

#include "kernel/limine.h"
#include "kernel/renderer.h"
#include "kernel/gdt.h"
#include "kernel/idt.h"
#include "kernel/paging.h"
#include "kernel/syscall.h"
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
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_requests_end_marker[] =
    LIMINE_REQUESTS_END_MARKER;

static uint8_t user_stack[16384] __attribute__((aligned(16)));
#define USER_STACK_TOP  ((uint64_t)&user_stack[sizeof(user_stack)])

static uint8_t kernel_stack[16384] __attribute__((aligned(16)));
#define KERNEL_STACK_TOP ((uint64_t)&kernel_stack[sizeof(kernel_stack)])

extern void user_enter(uint64_t entry, uint64_t user_stack);

void kmain(void)
{
    struct limine_framebuffer_response *fb_resp =
        framebuffer_request.response;

    if (fb_resp == 0 || fb_resp->framebuffer_count == 0) {
        for (;;)
            __asm__ volatile ("hlt");
    }

    struct limine_framebuffer *fb = fb_resp->framebuffers[0];

    renderer_init(fb);
    render_clear(0x00000000);

    render_printf("kernel ready\n");
    render_printf("framebuffer %ux%u ready\n", fb->width, fb->height);

    gdt_init(); render_printf("gdt ready\n");
    tss_set_rsp0(KERNEL_STACK_TOP); render_printf("tss ready\n");
    idt_init(); render_printf("idt ready\n");

    if (hhdm_request.response != 0) {
        paging_allow_user_access(hhdm_request.response->offset);
    } else {
        render_printf("warning: no HHDM, trying offset 0\n");
        paging_allow_user_access(0);
    }

    shell_init();

    user_enter((uint64_t)shell_run, USER_STACK_TOP);

    for (;;)
        __asm__ volatile ("hlt");
}
