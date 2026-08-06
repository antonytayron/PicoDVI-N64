/**
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2023 Konrad Beckmann
 */

#pragma once

#include "joybus.h"

/**
 * @brief Key combination to enter the OSD menu.
 *
 * This macro checks if the specified key combination is pressed.
 * The key combination for the OSD menu is L + R + C-DOWN + DPAD-DOWN.
 *
 * @param __keys__ The current state of the keys.
 * @return True if the OSD shortcut keys are pressed, false otherwise.
 */
#define OSD_SHORTCUT(__keys__) ( \
    TL_BUTTON(__keys__) && \
    TR_BUTTON(__keys__) && \
    CD_BUTTON(__keys__) && \
    DD_BUTTON(__keys__)    \
)

typedef enum {
    OSD_DONE = 0,
    OSD_SKIP_NEXT_FRAME,
    OSD_AGAIN
} osd_ret_t;

void osd_apply_audio_settings(void);
void osd_apply_video_settings(void);
void osd_trigger_reset(void);

/**
 * @brief Run the OSD.
 *
 * This function runs the OSD. It should be called in your main loop.
 */
osd_ret_t osd_run(void);
