/**
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2023 Konrad Beckmann
 */

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "gfx.h"
#include "osd.h"
#include "joybus.h"

typedef enum item_type {
    ITEM_TYPE_TEXT = 0,
    ITEM_TYPE_VALUE_RW_U32,
    ITEM_TYPE_VALUE_RW_I32,
    ITEM_TYPE_VALUE_RO_U32,
    ITEM_TYPE_VALUE_RO_I32,
    ITEM_TYPE_MENU,
    ITEM_TYPE_BACK,
    ITEM_TYPE_EXIT,
    ITEM_TYPE_ACTION,
} item_type_t;

typedef struct menu_item {
    char *text;
    item_type_t type;
    union {
        uint32_t *value_u32;
        int32_t *value_i32;
        void *value_ptr;
    } value;
    void (*action)(void);
} menu_item_t;

#define IS_FOCUSABLE(__x__) ( \
    ((__x__) == ITEM_TYPE_VALUE_RW_U32) || \
    ((__x__) == ITEM_TYPE_VALUE_RW_I32) || \
    ((__x__) == ITEM_TYPE_VALUE_RO_U32) || \
    ((__x__) == ITEM_TYPE_VALUE_RO_I32) || \
    ((__x__) == ITEM_TYPE_MENU) || \
    ((__x__) == ITEM_TYPE_BACK) || \
    ((__x__) == ITEM_TYPE_EXIT) || \
    ((__x__) == ITEM_TYPE_ACTION) \
)

static void audio_volume_up(void)
{
    if (g_config.audio_volume_percent < 100) {
        g_config.audio_volume_percent += 5;
    }
    osd_apply_audio_settings();
}

static void audio_volume_down(void)
{
    if (g_config.audio_volume_percent > 0) {
        g_config.audio_volume_percent -= 5;
    }
    osd_apply_audio_settings();
}

static void audio_toggle_mute(void)
{
    g_config.audio_mute = !g_config.audio_mute;
    osd_apply_audio_settings();
}

static void audio_set_rate_32000(void)
{
    g_config.audio_out_sample_rate = 32000;
    osd_apply_audio_settings();
}

static void audio_set_rate_44100(void)
{
    g_config.audio_out_sample_rate = 44100;
    osd_apply_audio_settings();
}

static void audio_set_rate_48000(void)
{
    g_config.audio_out_sample_rate = 48000;
    osd_apply_audio_settings();
}

static void audio_set_rate_96000(void)
{
    g_config.audio_out_sample_rate = 96000;
    osd_apply_audio_settings();
}

static void video_crop_x_up(void)
{
    if (g_config.video_crop_x < 200) {
        g_config.video_crop_x++;
    }
    osd_apply_video_settings();
}

static void video_crop_x_down(void)
{
    if (g_config.video_crop_x > 0) {
        g_config.video_crop_x--;
    }
    osd_apply_video_settings();
}

static void video_crop_y_up(void)
{
    if (g_config.video_crop_y < 200) {
        g_config.video_crop_y++;
    }
    osd_apply_video_settings();
}

static void video_crop_y_down(void)
{
    if (g_config.video_crop_y > 0) {
        g_config.video_crop_y--;
    }
    osd_apply_video_settings();
}

static void reset_pico_action(void)
{
    osd_trigger_reset();
}

menu_item_t menu_audio[] = {
    {
        .text = "Audio Settings",
    },
    {
        .text = "Volume +",
        .type = ITEM_TYPE_ACTION,
        .action = audio_volume_up,
    },
    {
        .text = "Volume -",
        .type = ITEM_TYPE_ACTION,
        .action = audio_volume_down,
    },
    {
        .text = "Mute/Unmute",
        .type = ITEM_TYPE_ACTION,
        .action = audio_toggle_mute,
    },
    {
        .text = "Rate 32 kHz",
        .type = ITEM_TYPE_ACTION,
        .action = audio_set_rate_32000,
    },
    {
        .text = "Rate 44.1 kHz",
        .type = ITEM_TYPE_ACTION,
        .action = audio_set_rate_44100,
    },
    {
        .text = "Rate 48 kHz",
        .type = ITEM_TYPE_ACTION,
        .action = audio_set_rate_48000,
    },
    {
        .text = "Rate 96 kHz",
        .type = ITEM_TYPE_ACTION,
        .action = audio_set_rate_96000,
    },
    {
        .text = "Back",
        .type = ITEM_TYPE_BACK,
    },
    {
        .text = NULL,
    }
};

menu_item_t menu_video[] = {
    {
        .text = "Video Settings",
    },
    {
        .text = "Crop X +",
        .type = ITEM_TYPE_ACTION,
        .action = video_crop_x_up,
    },
    {
        .text = "Crop X -",
        .type = ITEM_TYPE_ACTION,
        .action = video_crop_x_down,
    },
    {
        .text = "Crop Y +",
        .type = ITEM_TYPE_ACTION,
        .action = video_crop_y_up,
    },
    {
        .text = "Crop Y -",
        .type = ITEM_TYPE_ACTION,
        .action = video_crop_y_down,
    },
    {
        .text = "Back",
        .type = ITEM_TYPE_BACK,
    },
    {
        .text = NULL,
    }
};

menu_item_t menu_about[] = {
    {
        .text = "About",
    },
    {
        .text = "Firmware v" FIRMWARE_VERSION,
    },
    {
        .text = "GitHub:",
    },
    {
        .text = FIRMWARE_GITHUB_URL,
    },
    {
        .text = "Original project",
    },
    {
        .text = "Back",
        .type = ITEM_TYPE_BACK,
    },
    {
        .text = NULL,
    }
};

menu_item_t menu[] = {
    {
        .text = "PicoDVI-N64 by @kbeckmann",
    },
    {
        .text = "Firmware v" FIRMWARE_VERSION,
    },
    {
        .text = "Build date: 2026-08-06",
    },
    {
        .text = "",
    },
    {
        .text = "OSD Menu",
    },
    {
        .text = "Audio",
        .type = ITEM_TYPE_MENU,
        .value.value_ptr = menu_audio,
    },
    {
        .text = "Video",
        .type = ITEM_TYPE_MENU,
        .value.value_ptr = menu_video,
    },
    {
        .text = "Reset Pico",
        .type = ITEM_TYPE_ACTION,
        .action = reset_pico_action,
    },
    {
        .text = "About",
        .type = ITEM_TYPE_MENU,
        .value.value_ptr = menu_about,
    },
    {
        .text = "Exit OSD",
        .type = ITEM_TYPE_EXIT,
    },
    {
        .text = NULL,
    }
};

static struct {
    uint32_t last_buttons;
    menu_item_t *root;
    menu_item_t *current_root;
    menu_item_t *focused_item;
    menu_item_t *stack[8];
    uint32_t stack_depth;
    bool open;
} state = {
    .root = menu,
    .current_root = menu,
    .stack_depth = 0,
};

#define BUTTON_PRESSED(__op__) (!__op__(state.last_buttons) && __op__(buttons))

static void render_status_line(uint32_t y)
{
    char status[64];

    if (state.current_root == menu_audio) {
        snprintf(status, sizeof(status), "A: vol=%u%% mute=%s rate=%u",
            g_config.audio_volume_percent,
            g_config.audio_mute ? "on" : "off",
            g_config.audio_out_sample_rate);
        gfx_puttextf(OSD_X_OFFSET, y * 8, RGB888_TO_RGB565(0x00, 0x80, 0xff), RGB888_TO_RGB565(0xff, 0xff, 0xff), "%s", status);
    } else if (state.current_root == menu_video) {
        snprintf(status, sizeof(status), "V: crop x=%u y=%u",
            g_config.video_crop_x,
            g_config.video_crop_y);
        gfx_puttextf(OSD_X_OFFSET, y * 8, RGB888_TO_RGB565(0x00, 0x80, 0xff), RGB888_TO_RGB565(0xff, 0xff, 0xff), "%s", status);
    }
}

osd_ret_t osd_run(void)
{
    uint32_t buttons = joybus_rx_get_latest();
    if (OSD_SHORTCUT(buttons)) {
        state.open = true;
        state.last_buttons = buttons;
        state.root = menu;
        state.current_root = menu;
        state.focused_item = NULL;
        state.stack_depth = 0;
        memset(state.stack, 0, sizeof(state.stack));
    }

    if (!state.open) {
        return OSD_DONE;
    }

    bool rerender = false;

    menu_item_t *item = state.current_root;
    uint32_t y = OSD_Y_OFFSET;
    uint32_t x = OSD_X_OFFSET;

    if (state.focused_item == NULL) {
        while (item->text) {
            if (IS_FOCUSABLE(item->type)) {
                state.focused_item = item;
                break;
            }
            item++;
        }
    }

    if (state.current_root == menu_audio || state.current_root == menu_video) {
        render_status_line(y++);
    }

    item = state.current_root;
    while (item && item->text) {
        uint16_t bg_color = (item == state.focused_item) ? RGB888_TO_RGB565(0xff, 0x00, 0xff) : RGB888_TO_RGB565(0x00, 0x00, 0x00);
        uint16_t fg_color = RGB888_TO_RGB565(0xff, 0xff, 0xff);

        gfx_puttextf(x, y++ * 8, bg_color, fg_color, "%s", item->text);
        item++;
    }

    buttons = joybus_rx_get_latest();
    if (BUTTON_PRESSED(DD_BUTTON)) {
        menu_item_t *it = state.focused_item ? (state.focused_item + 1) : state.current_root;
        while (it->text) {
            if (IS_FOCUSABLE(it->type)) {
                state.focused_item = it;
                break;
            }
            it++;
        }
    }
    else if (BUTTON_PRESSED(DU_BUTTON)) {
        menu_item_t *it = state.focused_item ? state.focused_item : (state.current_root + 1);
        if (it != state.current_root) {
            it--;
            while (it >= state.current_root) {
                if (IS_FOCUSABLE(it->type)) {
                    state.focused_item = it;
                    break;
                }
                it--;
            }
        }
    }
    else if (BUTTON_PRESSED(A_BUTTON)) {
        if (state.focused_item != NULL) {
            if (state.focused_item->type == ITEM_TYPE_MENU) {
                if (state.stack_depth < 8) {
                    state.stack[state.stack_depth++] = state.current_root;
                    state.current_root = (menu_item_t *)state.focused_item->value.value_ptr;
                    state.focused_item = NULL;
                    rerender = true;
                }
            }
            else if (state.focused_item->type == ITEM_TYPE_BACK) {
                if (state.stack_depth > 0) {
                    state.current_root = state.stack[--state.stack_depth];
                    state.focused_item = NULL;
                    rerender = true;
                }
            }
            else if (state.focused_item->type == ITEM_TYPE_EXIT) {
                state.open = false;
            }
            else if (state.focused_item->type == ITEM_TYPE_ACTION) {
                if (state.focused_item->action != NULL) {
                    state.focused_item->action();
                }
                rerender = true;
            }
        }
    }

    state.last_buttons = buttons;

    if (rerender) {
        return OSD_SKIP_NEXT_FRAME;
    }

    return OSD_AGAIN;
}
