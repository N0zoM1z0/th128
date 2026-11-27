#include "AsciiScreenDraw.hpp"

#include <time.h>

namespace th128 {

int __stdcall AsDrawReplayMenu(AsReplayMenu *menu) {
    switch (menu->state) {
    case 2: {
        ZunVec3 pos;
        pos.x = 58.0f;
        pos.y = 80.0f;
        pos.z = 0.0f;
        g_AsciiInf->alternateMode = 1;
        for (int i = 0; i < 25; ++i) {
            g_AsciiInf->color.value = (menu->cursor == i) ? 0xffffff00u : 0xff808080u;
            if (menu->slots[i] != 0) {
                AsReplayHeader *hdr = menu->slots[i]->header;
                struct tm *t = _localtime64(&hdr->timestamp);
                g_AsciiInf->CreateTextF(
                    &pos,
                    "No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %8d%d %s %s %2.1f%%",
                    i + 1,
                    hdr->name,
                    t->tm_year % 100,
                    t->tm_mon + 1,
                    t->tm_mday,
                    t->tm_hour,
                    t->tm_min,
                    hdr->score,
                    hdr->lastDigit,
                    g_AsRouteNames[hdr->route],
                    g_AsDiffNames[hdr->difficulty],
                    (double)hdr->slowRate
                );
            } else {
                g_AsciiInf->CreateTextF(
                    &pos,
                    "No.%.2d -------- --/--/-- --:-- --------- ------- ------ ---%%",
                    i + 1
                );
            }
            pos.y = (float)(pos.y + 15.0);
        }
        g_AsciiInf->alternateMode = 0;
        g_AsciiInf->color.value = 0xffffffffu;
        break;
    }
    case 3: {
        ZunVec3 pos;
        pos.x = 58.0f;
        pos.y = 240.0f;
        pos.z = 0.0f;
        int slot = menu->selectedSlot;
        if (menu->slideTimer < 10) {
            pos.y = (float)(((10.0 - menu->slideTimerF) * ((double)(slot * 15 + 80) - 240.0)) / 10.0 + 240.0);
        }
        AsReplayHeader *hdr = g_AsReplayMgr->curHeader;
        struct tm *t = _localtime64(&hdr->timestamp);
        g_AsciiInf->CreateTextF(
            &pos,
            "No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %8d%d %s %s %2.1f%%",
            slot + 1,
            "        ",
            t->tm_year % 100,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            hdr->score,
            hdr->lastDigit,
            g_AsRouteNames[hdr->route],
            g_AsDiffNames[hdr->difficulty],
            (double)hdr->slowRate
        );
        if (menu->slideTimer >= 10) {
            pos.x = 112.0f;
            pos.y = 240.0f;
            pos.z = 0.0f;
            g_AsciiInf->color.value = 0xffffffffu;
            g_AsciiInf->CreateTextF(&pos, "%s", menu->nameBuf);
            pos.x = (float)(menu->nameLen * 9 + 112.0);
            if (menu->nameLen == 8) {
                pos.x = (float)(pos.x - 9.0);
            }
            g_AsciiInf->color.value = 0xffffff00u;
            g_AsciiInf->CreateTextF(&pos, "_");
            pos.x = 212.0f;
            pos.y = 360.0f;
            pos.z = 0.0f;
            g_AsciiInf->color.value = 0xffffffffu;
            for (int i = 0; i < 91; ++i) {
                g_AsciiInf->color.value = (menu->kbCursor == i) ? 0xffffff00u : 0xff808080u;
                if (i < 88) {
                    g_AsciiInf->CreateTextF(&pos, "%c", g_AsKeyboardChars[i]);
                } else {
                    int ch = (i == 88) ? 0x81 : ((i == 89) ? 0x7f : 0x80);
                    g_AsciiInf->CreateTextF(&pos, "%c", ch);
                }
                if (i % 13 == 12) {
                    pos.x = 212.0f;
                    pos.y = (float)(pos.y + 16.0);
                } else {
                    pos.x = (float)(pos.x + 18.0);
                }
            }
            g_AsciiInf->color.value = 0xffffffffu;
        }
        g_AsciiInf->alternateMode = 0;
        g_AsciiInf->color.value = 0xffffffffu;
        break;
    }
    }
    return 1;
}

int __stdcall AsDrawHighScoreMenu(AsScoreMenu *menu) {
    switch (menu->state) {
    case 2: {
        ZunVec3 pos;
        pos.x = 48.0f;
        pos.y = 160.0f;
        pos.z = 0.0f;
        g_AsciiInf->alternateMode = 1;
        int alpha = 255;
        for (int i = 0; i < 10; ++i) {
            if (menu->isViewingOnly != 0) {
                g_AsciiInf->color.value = (alpha << 16) | (alpha << 8) | 0xff0000ffu;
            } else {
                g_AsciiInf->color.value = (menu->cursor == i) ? 0xffffffffu : 0xff404040u;
            }
            AsScoreEntry *e = &g_AsScoreData->routes[g_AsRoute].entries[g_AsDifficulty][i];
            if (e->timestamp != 0) {
                struct tm *t = _localtime64(&e->timestamp);
                g_AsciiInf->CreateTextF(
                    &pos,
                    "%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%",
                    i + 1,
                    e->name,
                    e->score,
                    e->cont,
                    t->tm_year + 1900,
                    t->tm_mon + 1,
                    t->tm_mday,
                    t->tm_hour,
                    t->tm_min,
                    g_AsStageNames[e->stage],
                    (double)e->slowRate
                );
            } else {
                g_AsciiInf->CreateTextF(
                    &pos,
                    "%2d  %s  %9ld%d  ----/--/-- --:--  Stage -     ---%%",
                    i + 1,
                    e->name,
                    e->score,
                    e->cont,
                    (double)e->slowRate
                );
            }
            pos.y = (float)(pos.y + 18.0);
            alpha -= 16;
        }
        if (menu->isViewingOnly == 0) {
            pos.x = 84.0f;
            pos.y = (float)(menu->cursor * 18.0 + 160.0);
            pos.z = 0.0f;
            g_AsciiInf->color.value = 0xffffffffu;
            g_AsciiInf->CreateTextF(&pos, "%s", menu->nameBuf);
            pos.x = (float)(menu->nameLen * 9 + 84.0);
            if (menu->nameLen == 8) {
                pos.x = (float)(pos.x - 9.0);
            }
            g_AsciiInf->color.value = 0xffffff00u;
            g_AsciiInf->CreateTextF(&pos, "_");
            pos.x = 212.0f;
            pos.y = 360.0f;
            pos.z = 0.0f;
            g_AsciiInf->color.value = 0xffffffffu;
            for (int i = 0; i < 91; ++i) {
                g_AsciiInf->color.value = (menu->kbCursor == i) ? 0xffffff00u : 0xff808080u;
                if (i < 88) {
                    g_AsciiInf->CreateTextF(&pos, "%c", g_AsKeyboardChars[i]);
                } else {
                    int ch = (i == 88) ? 0x81 : ((i == 89) ? 0x7f : 0x80);
                    g_AsciiInf->CreateTextF(&pos, "%c", ch);
                }
                if (i % 13 == 12) {
                    pos.x = 212.0f;
                    pos.y = (float)(pos.y + 16.0);
                } else {
                    pos.x = (float)(pos.x + 18.0);
                }
            }
            g_AsciiInf->color.value = 0xffffffffu;
        }
        break;
    }
    }
    return 1;
}

int __stdcall AsDrawPlayerDataMenu(AsScoreMenu *menu) {
    switch (menu->state) {
    case 2: {
        ZunVec3 pos;
        pos.x = 48.0f;
        pos.y = 160.0f;
        pos.z = 0.0f;
        int diff = (menu->cursor == 6) ? 4 : menu->diffCursor;
        g_AsciiInf->alternateMode = 1;
        if (menu->hideScores == 0) {
            int alpha = 255;
            for (int i = 0; i < 10; ++i) {
                g_AsciiInf->color.value = (alpha << 16) | (alpha << 8) | 0xff0000ffu;
                AsScoreEntry *e = &g_AsScoreData->routes[menu->cursor].entries[diff][i];
                if (e->timestamp != 0) {
                    struct tm *t = _localtime64(&e->timestamp);
                    g_AsciiInf->CreateTextF(
                        &pos,
                        "%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%",
                        i + 1,
                        e->name,
                        e->score,
                        e->cont,
                        t->tm_year + 1900,
                        t->tm_mon + 1,
                        t->tm_mday,
                        t->tm_hour,
                        t->tm_min,
                        g_AsStageNames[e->stage],
                        (double)e->slowRate
                    );
                } else {
                    g_AsciiInf->CreateTextF(
                        &pos,
                        "%2d  %s  %9ld%d  ----/--/-- --:--  Stage ----  ---%%",
                        i + 1,
                        e->name,
                        e->score,
                        e->cont,
                        (double)e->slowRate
                    );
                }
                pos.y = (float)(pos.y + 18.0);
                alpha -= 16;
            }
        }
        g_AsciiInf->color.value = 0xffffffffu;
        pos.x = 328.0f;
        pos.y = 378.0f;
        int totalClear = 0;
        for (int r = 0; r < 7; ++r) {
            totalClear += g_AsScoreData->routes[r].clearCount;
        }
        g_AsciiInf->CreateTextF(&pos, "    %5d", totalClear);
        pos.y = 396.0f;
        __int64 totalSec = g_AsScoreData->totalPlayTime / 60;
        int sec = (int)((g_AsScoreData->totalPlayTime / 100) % 60);
        int min = (int)((totalSec / 100) % 60);
        int hour = (int)((totalSec / 60) / 100);
        g_AsciiInf->CreateTextF(&pos, "%3d:%.2d:%.2d", hour, min, sec);
        pos.y = 414.0f;
        int totalPlay = 0;
        for (int r = 0; r < 7; ++r) {
            for (int d = 0; d < 5; ++d) {
                totalPlay += g_AsScoreData->routes[r].playCounts[d];
            }
        }
        g_AsciiInf->CreateTextF(&pos, "    %5d", totalPlay);
        g_AsciiInf->color.value = 0xffffffffu;
        g_AsciiInf->alternateMode = 0;
        break;
    }
    }
    return 1;
}

int __stdcall AsDrawFreezeHud(AsFreezeHud *hud) {
    if (hud->flags & 1) {
        ZunVec3 pos;
        pos.x = 320.0f;
        pos.y = 160.0f;
        pos.z = 0.0f;
        g_AsciiInf->alignmentX = 11;
        float pct = hud->percent;
        if (hud->flags & 2) {
            pos.y = 64.0f;
        }
        pct = (float)((hud->animTimer < 30) ? (hud->animTimerF * pct / 30.0) : pct);
        int pct100 = (int)(pct * 100.0);
        g_AsciiInf->scaleX = 2.0f;
        g_AsciiInf->scaleY = 2.0f;
        g_AsciiInf->unknown1C878 = 2;
        g_AsciiInf->unknown1C87C = 1;
        g_AsciiInf->color.value = 0xffb0e0ffu;
        g_AsciiInf->alignmentY = 1;
        int a1;
        if (hud->flags & 8) {
            a1 = (hud->fadeTimer > 8) ? 0x40 : (unsigned char)(0xff - (hud->fadeTimer << 4));
        } else {
            a1 = (hud->fadeTimer > 8) ? 0xff : (unsigned char)((hud->fadeTimer + 4) << 4);
        }
        g_AsciiInf->color.channels.alpha = (unsigned char)a1;
        int whole = pct100 / 100;
        g_AsciiInf->CreateTextF(&pos, "%d", whole);
        g_AsciiInf->scaleX = 1.0f;
        g_AsciiInf->scaleY = 1.0f;
        g_AsciiInf->unknown1C878 = 1;
        g_AsciiInf->unknown1C87C = 1;
        pos.y = (float)(pos.y + 20.0);
        g_AsciiInf->CreateTextF(&pos, ".");
        pos.x = 332.0f;
        g_AsciiInf->CreateTextF(&pos, "%.2d", pct100 - whole * 100);
        g_AsciiInf->scaleX = 2.0f;
        g_AsciiInf->scaleY = 2.0f;
        pos.x = 371.0f;
        pos.y = (float)(pos.y - 20.0);
        g_AsciiInf->CreateTextF(&pos, "%%");
        g_AsciiInf->scaleX = 1.0f;
        g_AsciiInf->scaleY = 1.0f;
        g_AsciiInf->alignmentX = 5;
        g_AsciiInf->unknown1C878 = 0;
        g_AsciiInf->unknown1C87C = 1;
        g_AsciiInf->color.value = 0xfff0f0d0u;
        int a2;
        if (hud->flags & 8) {
            a2 = (hud->fadeTimer > 8) ? 0x40 : (unsigned char)((hud->fadeTimer << 6) - 1);
        } else {
            a2 = (hud->fadeTimer > 8) ? 0xff : (unsigned char)((1 - hud->fadeTimer) << 6);
        }
        g_AsciiInf->color.channels.alpha = (unsigned char)a2;
        pos.x = 320.0f;
        pos.y = (float)(pos.y + 50.0);
        if (hud->animTimer >= 30) {
            g_AsciiInf->CreateTextF(&pos, "%.2d", hud->bonusVal);
        }
        g_AsciiInf->scaleX = 1.0f;
        g_AsciiInf->scaleY = 1.0f;
        g_AsciiInf->unknown1C878 = 1;
        g_AsciiInf->unknown1C87C = 1;
        g_AsciiInf->alignmentY = 0;
        g_AsciiInf->color.value = 0xffffffffu;
    }
    return 1;
}

void __stdcall AsDrawNameEntry(AsNameEntry *entry, ZunVec3 pos) {
    g_AsciiInf->CreateTextF(&pos, "%s", entry->nameBuf);
    pos.x = (float)(entry->nameLen * 9 + pos.x);
    if (entry->nameLen == 8) {
        pos.x = (float)(pos.x - 9.0);
    }
    g_AsciiInf->color.value = 0xffffff00u;
    g_AsciiInf->CreateTextF(&pos, "_");
    g_AsciiInf->color.value = 0xffffffffu;
    pos.x = 144.0f;
    ZunVec3 kbPos;
    kbPos.x = 208.0f;
    kbPos.y = 320.0f;
    kbPos.z = 0.0f;
    for (int i = 0; i < 91; ++i) {
        g_AsciiInf->color.value = (entry->kbCursor == i) ? 0xffffff00u : 0xff808080u;
        if (i < 88) {
            g_AsciiInf->CreateTextF(&kbPos, "%c", g_AsKeyboardChars[i]);
        } else {
            int ch = (i == 88) ? 0x81 : ((i == 89) ? 0x7f : 0x80);
            g_AsciiInf->CreateTextF(&kbPos, "%c", ch);
        }
        if (i % 13 == 12) {
            kbPos.x = 208.0f;
            kbPos.y = (float)(kbPos.y + 16.0);
        } else {
            kbPos.x = (float)(kbPos.x + 18.0);
        }
    }
    g_AsciiInf->color.value = 0xffffffffu;
}

} // namespace th128
