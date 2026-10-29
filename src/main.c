#include <pebble.h>
/* AppMessage uses RAW NUMERIC key IDs. CloudPebble's toolchain never emits
 * the message-key table (build log: "message_key_header: -> ..." has no
 * sources), so symbolic-keyed fields are dropped by the phone at send time
 * ("k: empty"). The JS companion therefore sends {10001: sunrise, 10002:
 * sunset, 10003: 'T:'+temp, 10004: 'D:'+desc, 10005: 'L:'+place, 10006:
 * 'A:'+aqi}; the IDs map to the toolchain's auto-assignment base 10000 from
 * package.json messageKeys (request=10000 unused, then sunrise..weather_aqi
 * = 10001..10006). dict_find() on those IDs is the primary path
 * (order-independent); the marker/value scan and the "k:" diag dump in
 * prv_app_message_handler are kept as fallback + diagnostic (the watch has
 * no shell access). */
/* Hardcoded tick locations per screen below (the upload toolchain drops
 * separate .h files, so the tables are inlined). Each tick runs from the
 * actual screen edge (base) to its inner tip (kept from the existing design);
 * rectangles follow the perimeter from 12 o'clock clockwise; rounds follow
 * the circular edge. Diagonal 45-deg ticks at minutes 7/23/37/53 only on
 * rectangle screens. Platforms: 144x168 (aplite/basalt/diorite/flint),
 * 200x228 (emery), 180 round (chalk), 260 round (gabbro). */
/* Tick table format. Every 60-entry table below encodes one tick mark as a
 * line from (x1,y1) to (x2,y2). The base (x1,y1) sits on the actual screen
 * edge; the tip (x2,y2) points inward and is kept from the existing design.
 * Rectangles: vertical ticks (x1==x2), horizontal ticks (y1==y2), plus four
 * corner diagonals at minutes 7/23/37/53. Rounds: all ticks are radial (both
 * coordinates change). Entries run in minute order starting at 12 o'clock
 * going clockwise. Tables were generated/edited once by script (no trig at
 * runtime), with the tips 2 px deeper than the original design and the
 * bases extended to the screen edge (see _OpenCode_understanding.txt
 * entries (m)/(n)/(hh)). */
typedef struct { int16_t x1; int16_t y1; int16_t x2; int16_t y2; } TickLoc;
#define TICK_COUNT 60
#if PBL_DISPLAY_WIDTH == 144
/* aplite/basalt/diorite/flint 144x168 - 60 ticks */
#define LINE_T 4
static const TickLoc TICKS[TICK_COUNT] = {
    { 72, 0, 72, 9 },
    { 82, 0, 82, 7 },
    { 92, 0, 92, 7 },
    { 103, 0, 103, 7 },
    { 113, 0, 113, 7 },
    { 123, 0, 123, 9 },
    { 133, 0, 133, 7 },
    { 143, 1, 138, 6 },
    { 143, 13, 137, 13 },
    { 143, 23, 137, 23 },
    { 143, 33, 135, 33 },
    { 143, 43, 137, 43 },
    { 143, 53, 137, 53 },
    { 143, 64, 137, 64 },
    { 143, 74, 137, 74 },
    { 143, 84, 135, 84 },
    { 143, 94, 137, 94 },
    { 143, 104, 137, 104 },
    { 143, 115, 137, 115 },
    { 143, 125, 137, 125 },
    { 143, 135, 135, 135 },
    { 143, 145, 137, 145 },
    { 143, 155, 137, 155 },
    { 143, 167, 138, 162 },
    { 133, 167, 133, 161 },
    { 123, 167, 123, 159 },
    { 113, 167, 113, 161 },
    { 103, 167, 103, 161 },
    { 92, 167, 92, 161 },
    { 82, 167, 82, 161 },
    { 72, 167, 72, 159 },
    { 62, 167, 62, 161 },
    { 52, 167, 52, 161 },
    { 41, 167, 41, 161 },
    { 31, 167, 31, 161 },
    { 21, 167, 21, 159 },
    { 11, 167, 11, 161 },
    { 1, 167, 6, 162 },
    { 0, 155, 7, 155 },
    { 0, 145, 7, 145 },
    { 0, 135, 9, 135 },
    { 0, 125, 7, 125 },
    { 0, 115, 7, 115 },
    { 0, 104, 7, 104 },
    { 0, 94, 7, 94 },
    { 0, 84, 9, 84 },
    { 0, 74, 7, 74 },
    { 0, 64, 7, 64 },
    { 0, 53, 7, 53 },
    { 0, 43, 7, 43 },
    { 0, 33, 9, 33 },
    { 0, 23, 7, 23 },
    { 0, 13, 7, 13 },
    { 0, 0, 6, 6 },
    { 11, 0, 11, 7 },
    { 21, 0, 21, 9 },
    { 31, 0, 31, 7 },
    { 41, 0, 41, 7 },
    { 52, 0, 52, 7 },
    { 62, 0, 62, 7 },
};
#elif PBL_DISPLAY_WIDTH == 200
/* emery   200x228 rect - 60 ticks */
#define LINE_T 5
static const TickLoc TICKS[TICK_COUNT] = {
    { 100, 0, 100, 12 },
    { 114, 0, 114, 9 },
    { 128, 0, 128, 9 },
    { 142, 0, 142, 9 },
    { 156, 0, 156, 9 },
    { 170, 0, 170, 12 },
    { 184, 0, 184, 9 },
    { 199, 1, 192, 8 },
    { 199, 16, 191, 16 },
    { 199, 30, 191, 30 },
    { 199, 44, 188, 44 },
    { 199, 58, 191, 58 },
    { 199, 72, 191, 72 },
    { 199, 86, 191, 86 },
    { 199, 100, 191, 100 },
    { 199, 114, 188, 114 },
    { 199, 128, 191, 128 },
    { 199, 142, 191, 142 },
    { 199, 156, 191, 156 },
    { 199, 170, 191, 170 },
    { 199, 184, 188, 184 },
    { 199, 198, 191, 198 },
    { 199, 212, 191, 212 },
    { 199, 227, 192, 220 },
    { 184, 227, 184, 219 },
    { 170, 227, 170, 216 },
    { 156, 227, 156, 219 },
    { 142, 227, 142, 219 },
    { 128, 227, 128, 219 },
    { 114, 227, 114, 219 },
    { 100, 227, 100, 216 },
    { 86, 227, 86, 219 },
    { 72, 227, 72, 219 },
    { 58, 227, 58, 219 },
    { 44, 227, 44, 219 },
    { 30, 227, 30, 216 },
    { 16, 227, 16, 219 },
    { 1, 227, 8, 220 },
    { 0, 212, 9, 212 },
    { 0, 198, 9, 198 },
    { 0, 184, 12, 184 },
    { 0, 170, 9, 170 },
    { 0, 156, 9, 156 },
    { 0, 142, 9, 142 },
    { 0, 128, 9, 128 },
    { 0, 114, 12, 114 },
    { 0, 100, 9, 100 },
    { 0, 86, 9, 86 },
    { 0, 72, 9, 72 },
    { 0, 58, 9, 58 },
    { 0, 44, 12, 44 },
    { 0, 30, 9, 30 },
    { 0, 16, 9, 16 },
    { 0, 0, 8, 8 },
    { 16, 0, 16, 9 },
    { 30, 0, 30, 12 },
    { 44, 0, 44, 9 },
    { 58, 0, 58, 9 },
    { 72, 0, 72, 9 },
    { 86, 0, 86, 9 },
};
#elif PBL_DISPLAY_WIDTH == 180
/* chalk   180x180 round - 60 ticks */
#define LINE_T 5
static const TickLoc TICKS[TICK_COUNT] = {
    { 90, 0, 90, 11 },
    { 99, 0, 99, 9 },
    { 109, 2, 105, 11 },
    { 118, 5, 114, 13 },
    { 126, 8, 122, 16 },
    { 135, 12, 128, 22 },
    { 143, 17, 137, 25 },
    { 150, 23, 144, 30 },
    { 157, 30, 150, 36 },
    { 163, 37, 155, 43 },
    { 168, 45, 158, 51 },
    { 172, 54, 164, 58 },
    { 175, 62, 167, 66 },
    { 178, 71, 169, 75 },
    { 179, 81, 171, 81 },
    { 179, 90, 169, 90 },
    { 179, 99, 171, 99 },
    { 178, 109, 169, 105 },
    { 175, 118, 167, 114 },
    { 172, 126, 164, 122 },
    { 168, 135, 158, 128 },
    { 163, 143, 155, 137 },
    { 157, 150, 150, 144 },
    { 150, 157, 144, 150 },
    { 143, 163, 137, 155 },
    { 135, 168, 128, 158 },
    { 126, 172, 122, 164 },
    { 118, 175, 114, 167 },
    { 109, 178, 105, 169 },
    { 99, 179, 99, 171 },
    { 90, 179, 90, 169 },
    { 81, 179, 81, 171 },
    { 71, 178, 75, 169 },
    { 62, 175, 66, 167 },
    { 54, 172, 58, 164 },
    { 45, 168, 52, 158 },
    { 37, 163, 43, 155 },
    { 30, 157, 36, 150 },
    { 23, 150, 30, 144 },
    { 17, 143, 25, 137 },
    { 12, 135, 22, 129 },
    { 8, 126, 16, 122 },
    { 5, 118, 13, 114 },
    { 2, 109, 11, 105 },
    { 0, 99, 9, 99 },
    { 0, 90, 11, 90 },
    { 0, 81, 9, 81 },
    { 2, 71, 11, 75 },
    { 5, 62, 13, 66 },
    { 8, 54, 16, 58 },
    { 12, 45, 22, 51 },
    { 17, 37, 25, 43 },
    { 23, 30, 30, 36 },
    { 30, 23, 36, 30 },
    { 37, 17, 43, 25 },
    { 45, 12, 52, 22 },
    { 54, 8, 58, 16 },
    { 62, 5, 66, 13 },
    { 71, 2, 75, 11 },
    { 81, 0, 81, 9 },
};
#elif PBL_DISPLAY_WIDTH == 260
/* gabbro  260x260 round - 60 ticks */
#define LINE_T 6
static const TickLoc TICKS[TICK_COUNT] = {
    { 130, 0, 130, 14 },
    { 144, 1, 143, 12 },
    { 157, 3, 153, 14 },
    { 170, 6, 165, 17 },
    { 183, 11, 177, 21 },
    { 195, 17, 187, 30 },
    { 206, 25, 199, 34 },
    { 217, 34, 209, 42 },
    { 226, 43, 218, 51 },
    { 235, 54, 226, 61 },
    { 242, 64, 230, 73 },
    { 249, 77, 239, 83 },
    { 254, 90, 243, 95 },
    { 257, 103, 246, 107 },
    { 259, 116, 248, 117 },
    { 259, 130, 246, 130 },
    { 259, 144, 248, 143 },
    { 257, 157, 246, 153 },
    { 254, 170, 243, 165 },
    { 249, 183, 239, 177 },
    { 243, 195, 230, 187 },
    { 235, 206, 226, 199 },
    { 226, 217, 218, 209 },
    { 217, 226, 209, 218 },
    { 206, 235, 199, 226 },
    { 195, 243, 187, 230 },
    { 183, 249, 177, 239 },
    { 170, 254, 165, 243 },
    { 157, 257, 153, 246 },
    { 144, 259, 143, 248 },
    { 130, 259, 130, 246 },
    { 116, 259, 117, 248 },
    { 103, 257, 107, 246 },
    { 90, 254, 95, 243 },
    { 77, 249, 83, 239 },
    { 65, 243, 73, 230 },
    { 54, 235, 61, 226 },
    { 43, 226, 51, 218 },
    { 34, 217, 42, 209 },
    { 25, 206, 34, 199 },
    { 18, 196, 30, 187 },
    { 11, 183, 21, 177 },
    { 6, 170, 17, 165 },
    { 3, 157, 14, 153 },
    { 1, 144, 12, 143 },
    { 0, 130, 14, 130 },
    { 1, 116, 12, 117 },
    { 3, 103, 14, 107 },
    { 6, 90, 17, 95 },
    { 11, 77, 21, 83 },
    { 18, 64, 30, 73 },
    { 25, 54, 34, 61 },
    { 34, 43, 42, 51 },
    { 43, 34, 51, 42 },
    { 54, 25, 61, 34 },
    { 65, 17, 73, 30 },
    { 77, 11, 83, 21 },
    { 90, 6, 95, 17 },
    { 103, 3, 107, 14 },
    { 116, 1, 117, 12 },
};
#else
#error "OldEyes: unsupported display resolution"
#endif


/* Layout & fonts (directive (ii), see _OpenCode_understanding.txt sec. 5).
 * Rectangle screens split into a top region (holds the hours) and a bottom
 * region (date then steps). ROUND screens split the face in two instead:
 * hours in the top half, date + steps stacked in the bottom half. Every
 * platform below has its own explicit, hardcoded font type, font pixel size
 * and element locations - no shared or derived-from-screen-size macros. One
 * documented exception: the overlay desc/AQI/place row Y positions are
 * measured dynamically (word wrap) so a location name can wrap to 2 lines
 * (directive (nn)) - fonts/px, band, gaps and raise remain per-platform. */
#define SCR_W PBL_DISPLAY_WIDTH
#define SCR_H PBL_DISPLAY_HEIGHT

#if PBL_DISPLAY_WIDTH == 144
  /* aplite/basalt/diorite. Directives: (kk) date/steps Gothic 28 and
   * lifted 3 px up (DATE/STEPS_CENTER_OFF 3/30 vs the old 6/33);
   * (ll) overlay text Gothic 24. */
  #define HOURS_FONT        FONT_KEY_ROBOTO_BOLD_SUBSET_49
  #define OVERLAY_TEMP_FONT FONT_KEY_BITHAM_42_BOLD
  #define DATE_FONT         FONT_KEY_GOTHIC_28_BOLD
  #define OVERLAY_TEXT_FONT FONT_KEY_GOTHIC_24_BOLD
  #define ROUND_DATE_FONT   FONT_KEY_GOTHIC_24_BOLD
  #define TOP_REGION_H      112
  #define BOTTOM_REGION_Y   112
  #define BOTTOM_REGION_H   56
  #define HOURS_BOX_W       129
  #define HOURS_BOX_X       7
  #define HOURS_CENTER_Y    3
  #define DATE_CENTER_OFF   3
  #define STEPS_CENTER_OFF  30
  #define OVERLAY_TEMP_Y    3
  #define OVERLAY_GAP       3
  #define OVERLAY_GAP_DA    0
  #define OVERLAY_ROW_RAISE 8
  #define TEMP_BAND_H       50
  #define BATTERY_TOP           9
  #define BATTERY_RIGHT_INSET   11
#elif PBL_DISPLAY_WIDTH == 200
  /* emery 200x228. */
  #define HOURS_FONT        FONT_KEY_ROBOTO_BOLD_SUBSET_49
  #define OVERLAY_TEMP_FONT FONT_KEY_BITHAM_42_BOLD
  #define DATE_FONT         FONT_KEY_GOTHIC_24_BOLD
  #define OVERLAY_TEXT_FONT FONT_KEY_GOTHIC_24_BOLD
  #define ROUND_DATE_FONT   FONT_KEY_GOTHIC_24_BOLD
  #define TOP_REGION_H      152
  #define BOTTOM_REGION_Y   152
  #define BOTTOM_REGION_H   76
  #define HOURS_BOX_W       180
  #define HOURS_BOX_X       10
  #define HOURS_CENTER_Y    3
  #define DATE_CENTER_OFF   16
  #define STEPS_CENTER_OFF  45
  #define OVERLAY_TEMP_Y    3
  #define OVERLAY_GAP       3
  #define OVERLAY_GAP_DA    0
  #define OVERLAY_ROW_RAISE 8
  #define TEMP_BAND_H       68
  #define BATTERY_TOP           12
  #define BATTERY_RIGHT_INSET   14
#elif PBL_DISPLAY_WIDTH == 180
  /* chalk 180x180 round. */
  #define HOURS_FONT        FONT_KEY_ROBOTO_BOLD_SUBSET_49
  #define OVERLAY_TEMP_FONT FONT_KEY_BITHAM_42_BOLD
  #define DATE_FONT         FONT_KEY_GOTHIC_24_BOLD
  #define OVERLAY_TEXT_FONT FONT_KEY_GOTHIC_18_BOLD
  #define ROUND_DATE_FONT   FONT_KEY_GOTHIC_18_BOLD
  #define TOP_REGION_H      120
  #define BOTTOM_REGION_Y   120
  #define BOTTOM_REGION_H   60
  #define HOURS_BOX_W       162
  #define HOURS_BOX_X       9
  #define HOURS_CENTER_Y    3
  #define DATE_CENTER_OFF   12
  #define STEPS_CENTER_OFF  33
  #define OVERLAY_TEMP_Y    3
  #define OVERLAY_GAP       3
  #define OVERLAY_GAP_DA    0
  #define OVERLAY_ROW_RAISE 8
  #define TEMP_BAND_H       54
  #define BATTERY_RIGHT_INSET   14
#elif PBL_DISPLAY_WIDTH == 260
  /* gabbro 260x260 round. */
  #define HOURS_FONT        FONT_KEY_ROBOTO_BOLD_SUBSET_49
  #define OVERLAY_TEMP_FONT FONT_KEY_BITHAM_42_BOLD
  #define DATE_FONT         FONT_KEY_GOTHIC_28_BOLD
  #define OVERLAY_TEXT_FONT FONT_KEY_GOTHIC_28_BOLD
  #define ROUND_DATE_FONT   FONT_KEY_GOTHIC_28_BOLD
  #define TOP_REGION_H      173
  #define BOTTOM_REGION_Y   173
  #define BOTTOM_REGION_H   87
  #define HOURS_BOX_W       234
  #define HOURS_BOX_X       13
  #define HOURS_CENTER_Y    3
  #define DATE_CENTER_OFF   18
  #define STEPS_CENTER_OFF  52
  #define OVERLAY_TEMP_Y    3
  #define OVERLAY_GAP       3
  #define OVERLAY_GAP_DA    0
  #define OVERLAY_ROW_RAISE 8
  #define TEMP_BAND_H       78
  #define BATTERY_RIGHT_INSET   17
#else
  #error "OldEyes: unsupported display resolution"
#endif

/* ---- Shared state -------------------------------------------------------
 * Overlay timers implement the shake sequence: by day it shows immediately,
 * at night 2 s after the tap ((ii)); the hide timer keeps it up 5 s.
 * s_messages_received / s_messages_dropped feed the overlay
 * diagnostics; s_sunrise / s_sunset drive day/night colour; s_weather_temp,
 * s_weather_desc, s_aqi and s_place_name hold the last values received from
 * the phone; s_diag accumulates the "k:" tuple dump drawn on the no-weather
 * overlay. */
static Window *s_window;
static Layer *s_canvas;
static AppTimer *s_overlay_show_timer;
static AppTimer *s_overlay_hide_timer;
static bool s_overlay_active;
static uint32_t s_messages_received;
static uint32_t s_messages_dropped;

/* Redraw suppression: while the user is inside their scheduled quiet-time
 * window (likely asleep) or the watch sits on the charger (off-wrist), skip
 * periodic repaints; the face is only repainted once the displayed
 * minute/hour would go stale. s_is_plugged is pushed by the battery handler;
 * quiet_time_is_active() is a free call - no extra sensor is used. */
static bool s_is_plugged;
static int s_last_drawn_hour = -1;
static int s_last_drawn_min = -1;

static time_t s_sunrise;
static time_t s_sunset;
static char s_weather_temp[16];
static char s_weather_desc[32];
static char s_place_name[48];
static char s_aqi[32];
static char s_diag[128];

static const char *const MONTHS[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/* Night (inverted colours) from sunset until sunrise. Until the phone has
 * delivered sunrise/sunset times, assume NIGHT so the face is readable on
 * the default black background. */
static bool prv_is_night(time_t now) {
  if (s_sunrise == 0 || s_sunset == 0) {
    return true;
  }
  return (now < s_sunrise) || (now >= s_sunset);
}

/* True while the face should not be repainted: the user's quiet-time window
 * (likely asleep) or the watch sitting on the charger (off-wrist). Both
 * signals are free (quiet_time_is_active() + the battery service), so this
 * costs no sensor power; it only skips the blit. */
static bool prv_redraw_suppressed(void) {
  return quiet_time_is_active() || s_is_plugged;
}

/* Draw text centred horizontally AND vertically inside `box`. The Pebble
 * text API only centres one way at a time, so the rendered content size is
 * measured first and the box is nudged vertically to compensate. */
static void prv_draw_text_centered(GContext *ctx, const char *text, GFont font,
                                   GRect box, GColor ink) {
  if (!text || !text[0]) {
    return;
  }
  GSize sz = graphics_text_layout_get_content_size(text, font, box,
                                                   GTextOverflowModeWordWrap,
                                                   GTextAlignmentCenter);
  GRect r = GRect(box.origin.x,
                  box.origin.y + (box.size.h - sz.h) / 2,
                  box.size.w,
                  sz.h);
  graphics_context_set_text_color(ctx, ink);
  graphics_draw_text(ctx, text, font, r, GTextOverflowModeWordWrap,
                     GTextAlignmentCenter, NULL);
}

/* Text drawn top-anchored in a row of known height `h`, centred horizontally.
 * Unlike prv_draw_text_centered this does NOT vertical-centre, so rows can be
 * packed with exact gaps between them (the round overlay layout). */
static void prv_draw_text_hcentered(GContext *ctx, const char *text, GFont font,
                                    int16_t y, int16_t h, GColor ink) {
  if (!text || !text[0]) {
    return;
  }
  graphics_context_set_text_color(ctx, ink);
  graphics_draw_text(ctx, text, font, GRect(0, y, SCR_W, h),
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

/* The minute progress line (minute 0 = nothing, minute 59 = the full
 * border). It starts at 12 o'clock and runs clockwise for minute/59 of the
 * perimeter. Round screens draw a swept circle arc; rectangle screens walk
 * the five polyline segments around the edge. The stroke width (LINE_T) and
 * the small inset `c` keep the line just inside the tick ring at the rim. */
static void prv_draw_minute_line(GContext *ctx, int32_t minute) {
  if (minute <= 0) {
    return;
  }

#if defined(PBL_ROUND)
  /* Round screens: an arc around the circular edge. */
  const int16_t center = SCR_W / 2;
  const int16_t radius = (SCR_W / 2) - (LINE_T / 2);
  int32_t sweep = (int32_t)(((int64_t)minute * TRIG_MAX_ANGLE) / 59);
  if (sweep >= TRIG_MAX_ANGLE) {
    sweep = TRIG_MAX_ANGLE - 1;
  }
  int32_t start = (TRIG_MAX_ANGLE / 4 - sweep) & (TRIG_MAX_ANGLE - 1);
  GRect arc_box = GRect(center - radius, center - radius, radius * 2, radius * 2);
  graphics_context_set_stroke_width(ctx, LINE_T);
  graphics_draw_arc(ctx, arc_box, GOvalScaleModeFitCircle, start, TRIG_MAX_ANGLE / 4);
#else
  /* Rectangular screens: a polyline along the screen edge, starting at
   * 12 o'clock (top edge centre) running clockwise, minute/59 of the way. */
  const int8_t c = (LINE_T / 2) + 1;
  const int16_t w = SCR_W - c;
  const int16_t h = SCR_H - c;

  GPoint corners[5];
  corners[0] = GPoint(w, c);             /* top-right */
  corners[1] = GPoint(w, h);             /* bottom-right */
  corners[2] = GPoint(c, h);             /* bottom-left */
  corners[3] = GPoint(c, c);             /* top-left */
  corners[4] = GPoint(SCR_W / 2, c);     /* back to 12 o'clock */

  const int32_t seg_len[5] = {
    SCR_W / 2 - c,
    SCR_H - 2 * c,
    SCR_W - 2 * c,
    SCR_H - 2 * c,
    SCR_W / 2 - c
  };
  const int32_t perimeter = seg_len[0] + seg_len[1] + seg_len[2] +
                            seg_len[3] + seg_len[4];
  int32_t rem = (int32_t)(((int64_t)perimeter * minute) / 59);

  GPoint pts[6];
  int n = 0;
  pts[n++] = GPoint(SCR_W / 2, c);

  for (int k = 0; k < 5 && rem > 0; k++) {
    if (rem >= seg_len[k]) {
      pts[n++] = corners[k];
      rem -= seg_len[k];
    } else {
      int32_t x = corners[k].x;
      int32_t y = corners[k].y;
      GPoint from = pts[n - 1];
      if (seg_len[k] > 0) {
        x = from.x + ((corners[k].x - from.x) * rem) / seg_len[k];
        y = from.y + ((corners[k].y - from.y) * rem) / seg_len[k];
      }
      pts[n++] = GPoint((int16_t)x, (int16_t)y);
      rem = 0;
    }
  }

  graphics_context_set_stroke_width(ctx, LINE_T);
  for (int qi = 1; qi < n; qi++) {
    graphics_draw_line(ctx, pts[qi - 1], pts[qi]);
  }
#endif
}

/* Battery indicator (directives (oo)/(pp)). Reads the charge state from
 * battery_state_service_peek(); while the charger is plugged in it shows
 * "CHRG", otherwise "<n>%" (no 100% special case). Rectangles place it in
 * the top-right corner (BATTERY_TOP / BATTERY_RIGHT_INSET); rounds hang it
 * 3 px above the date text on the far right. Font is the platform's
 * DATE_FONT / ROUND_DATE_FONT. `box` is a right-edge area: the text is drawn
 * right-aligned so its right edge sits at SCR_W - BATTERY_RIGHT_INSET. */
static void prv_draw_battery(GContext *ctx, GRect box, GColor ink) {
  BatteryChargeState s = battery_state_service_peek();
  static char buf[8];
  if (s.is_plugged) {
    snprintf(buf, sizeof(buf), "CHRG");
  } else {
    snprintf(buf, sizeof(buf), "%d%%", s.charge_percent);
  }
  GFont font;
#if defined(PBL_ROUND)
  font = fonts_get_system_font(ROUND_DATE_FONT);
#else
  font = fonts_get_system_font(DATE_FONT);
#endif
  graphics_context_set_text_color(ctx, ink);
  graphics_draw_text(ctx, buf, font, box, GTextOverflowModeWordWrap,
                     GTextAlignmentRight, NULL);
}

/* The normal watchface: background fill, the 60 ticks, the minute progress
 * line, the hours text, then the date and steps lines in the bottom
 * region. Everything is recoloured per day/night via `bg`/`ink`. */
static void prv_draw_normal(GContext *ctx, struct tm *t) {
  const time_t now = time(NULL);
  const bool night = prv_is_night(now);
  const GColor bg = night ? GColorBlack : GColorWhite;
  const GColor ink = night ? GColorWhite : GColorBlack;

  graphics_context_set_fill_color(ctx, bg);
  graphics_fill_rect(ctx, GRect(0, 0, SCR_W, SCR_H), 0, GCornerNone);

  /* ticks: all 60 hardcoded, each starts on the screen edge and points
   * inward (entry (hh) in _OpenCode_understanding.txt) */
  graphics_context_set_stroke_width(ctx, 2);
  graphics_context_set_stroke_color(ctx, ink);
  for (int i = 0; i < TICK_COUNT; i++) {
    graphics_draw_line(ctx,
                       GPoint(TICKS[i].x1, TICKS[i].y1),
                       GPoint(TICKS[i].x2, TICKS[i].y2));
  }

  /* minute progress line */
  graphics_context_set_stroke_width(ctx, LINE_T);
  graphics_context_set_stroke_color(ctx, ink);
  prv_draw_minute_line(ctx, t->tm_min);

  /* hours: 24h, no leading zero, centred in the 75%-wide box. Round
   * screens split the face in two - the hour sits in the TOP half,
   * centred HOURS_CENTER_Y px below the split line (mockup-approved);
   * rectangle screens keep the whole top region (TOP_REGION_H). */
  static char hour_buf[4];
  snprintf(hour_buf, sizeof(hour_buf), "%d", t->tm_hour);
#if defined(PBL_ROUND)
  GRect hours_box = GRect(HOURS_BOX_X, HOURS_CENTER_Y, HOURS_BOX_W, SCR_H / 2);
#else
  GRect hours_box = GRect(HOURS_BOX_X, 0, HOURS_BOX_W, TOP_REGION_H);
#endif
  prv_draw_text_centered(ctx, hour_buf, fonts_get_system_font(HOURS_FONT),
                         hours_box, ink);

  static char date_buf[16];
  snprintf(date_buf, sizeof(date_buf), "%d-%s-%d",
           t->tm_mday, MONTHS[t->tm_mon], t->tm_year + 1900);

#if defined(PBL_ROUND)
  /* Round screens: date and steps live in the BOTTOM half. The date is
   * centred 25% down into it; the steps line is raised so its top edge sits
   * exactly 3 px below the measured bottom edge of the date text. */
  GRect measure = GRect(0, 0, SCR_W - 8, 2000);
  GFont round_font = fonts_get_system_font(ROUND_DATE_FONT);
  const int16_t half_h = SCR_H / 2;
  const int16_t date_c = half_h + half_h / 4;
  const GSize date_sz = graphics_text_layout_get_content_size(
      date_buf, round_font, measure, GTextOverflowModeWordWrap,
      GTextAlignmentCenter);
  GRect date_box = GRect(0, date_c - half_h / 2, SCR_W, half_h);
  prv_draw_text_centered(ctx, date_buf, round_font, date_box, ink);
#if defined(PBL_HEALTH)
  static char steps_buf[16];
  snprintf(steps_buf, sizeof(steps_buf), "%lu steps",
           (unsigned long)health_service_sum_today(HealthMetricStepCount));
  const GSize steps_sz = graphics_text_layout_get_content_size(
      steps_buf, round_font, measure, GTextOverflowModeWordWrap,
      GTextAlignmentCenter);
  const int16_t steps_c = date_c + date_sz.h / 2 + 3 + steps_sz.h / 2;
  GRect steps_box = GRect(0, steps_c - half_h / 2, SCR_W, half_h);
  prv_draw_text_centered(ctx, steps_buf, round_font, steps_box, ink);
#endif
  /* Battery: 3 px above the measured date text, right edge at the far right
   * (directives (oo)/(pp)). */
  const int16_t battery_bottom = date_c - date_sz.h / 2 - 3;
  GRect battery_box = GRect(0, battery_bottom - date_sz.h,
                            SCR_W - BATTERY_RIGHT_INSET, date_sz.h);
  prv_draw_battery(ctx, battery_box, ink);
#else
  /* bottom region (rectangle screens): date (DD-MMM-YYYY) then steps.
   * Each line gets its own full-height box centred on its anchor, so
   * text centres on the anchor even if it wraps to two lines. */
  GRect date_box = GRect(0, BOTTOM_REGION_Y + DATE_CENTER_OFF - BOTTOM_REGION_H / 2,
                         SCR_W, BOTTOM_REGION_H);
  prv_draw_text_centered(ctx, date_buf, fonts_get_system_font(DATE_FONT),
                         date_box, ink);
#if defined(PBL_HEALTH)
  static char steps_buf[16];
  snprintf(steps_buf, sizeof(steps_buf), "%lu steps",
           (unsigned long)health_service_sum_today(HealthMetricStepCount));
  GRect steps_box = GRect(0, BOTTOM_REGION_Y + STEPS_CENTER_OFF - BOTTOM_REGION_H / 2,
                          SCR_W, BOTTOM_REGION_H);
  prv_draw_text_centered(ctx, steps_buf, fonts_get_system_font(DATE_FONT),
                         steps_box, ink);
#endif
  /* Battery: top-right corner (directives (oo)/(pp)). */
  GRect battery_box = GRect(0, BATTERY_TOP, SCR_W - BATTERY_RIGHT_INSET, 40);
  prv_draw_battery(ctx, battery_box, ink);
#endif
}

/* Shake overlay: a weather card covering the whole screen. Order is
 * temperature, description, AQI, place name. Below the top-anchored temp
 * band, desc/AQI/place are measured with word wrap and packed on EVERY
 * shape (directive (nn)): zero gap between the description and AQI
 * (OVERLAY_GAP_DA), OVERLAY_GAP after that, and the whole block is lifted
 * OVERLAY_ROW_RAISE px so a 2-line location name has headroom below it.
 * The dynamic row Y positions are the documented exception to directive
 * (ii); fonts/px, the band, gaps and raise stay per-platform. If no
 * weather has ever been received, draw a diagnostic status instead
 * (message counts and the "k:" tuple dump), since the watch itself has no
 * shell access. */
static void prv_draw_overlay(GContext *ctx) {
  const bool night = prv_is_night(time(NULL));
  const GColor bg = night ? GColorBlack : GColorWhite;
  const GColor ink = night ? GColorWhite : GColorBlack;

  graphics_context_set_fill_color(ctx, bg);
  graphics_fill_rect(ctx, GRect(0, 0, SCR_W, SCR_H), 0, GCornerNone);

  if (s_weather_temp[0] == '\0' && s_weather_desc[0] == '\0' &&
      s_place_name[0] == '\0') {
    GRect full = GRect(0, 0, SCR_W, SCR_H);
    char status[64];
    if (s_messages_received == 0) {
      snprintf(status, sizeof(status), "No data from\nphone yet\n(no messages)");
      prv_draw_text_centered(ctx, status, fonts_get_system_font(DATE_FONT), full, ink);
    } else {
      snprintf(status, sizeof(status), "Phone reached,\nbut no weather\n(%lu msgs%s)",
               (unsigned long)s_messages_received,
               s_messages_dropped > 0 ? ", " : "");
      if (s_messages_dropped > 0) {
        snprintf(status + strlen(status), sizeof(status) - strlen(status),
                 "%lu dropped", (unsigned long)s_messages_dropped);
      }
      GRect status_box = GRect(0, 0, SCR_W, (SCR_H * 5) / 10);
      prv_draw_text_centered(ctx, status, fonts_get_system_font(DATE_FONT), status_box, ink);
      if (s_diag[0]) {
        GRect diag_box = GRect(3, (SCR_H * 5) / 10, SCR_W - 6, (SCR_H * 5) / 10);
        prv_draw_text_centered(ctx, s_diag, fonts_get_system_font(DATE_FONT), diag_box, ink);
      }
    }
    return;
  }

  GFont temp_font = fonts_get_system_font(OVERLAY_TEMP_FONT);
  GFont text_font = fonts_get_system_font(OVERLAY_TEXT_FONT);

  GRect temp_box = GRect(0, OVERLAY_TEMP_Y, SCR_W, TEMP_BAND_H);
  prv_draw_text_centered(ctx, s_weather_temp, temp_font, temp_box, ink);

  GRect measure = GRect(0, 0, SCR_W - 8, 2000);
  const char *rows[3] = { s_weather_desc, s_aqi, s_place_name };
  uint8_t src[3];
  int16_t row_h[3];
  int n = 0;
  int16_t block_h = 0;
  for (int i = 0; i < 3; i++) {
    if (rows[i][0]) {
      src[n] = (uint8_t)i;
      row_h[n] = graphics_text_layout_get_content_size(
          rows[i], text_font, measure, GTextOverflowModeWordWrap,
          GTextAlignmentCenter).h;
      block_h += row_h[n];
      n++;
    }
  }
  int16_t gaps_total = 0;
  for (int k = 0; k + 1 < n; k++) {
    gaps_total += (src[k] == 0) ? OVERLAY_GAP_DA : OVERLAY_GAP;
  }
  const int16_t avail = SCR_H - TEMP_BAND_H;
  int16_t y = TEMP_BAND_H + (avail - block_h - gaps_total) / 2 - OVERLAY_ROW_RAISE;
  for (int i = 0; i < n; i++) {
    prv_draw_text_hcentered(ctx, rows[src[i]], text_font, y, row_h[i], ink);
    y += row_h[i] + ((i + 1 < n) ? ((src[i] == 0) ? OVERLAY_GAP_DA : OVERLAY_GAP) : 0);
  }
}

/* Redraw dispatcher: overlay weather card when one is showing, otherwise
 * the normal face. */
static void prv_update_proc(Layer *layer, GContext *ctx) {
  (void)layer;
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  if (!t) {
    return;
  }
  if (s_overlay_active) {
    prv_draw_overlay(ctx);
  } else {
    prv_draw_normal(ctx, t);
  }
}

/* Shake/overlay sequence. A shake (tap) shows the weather overlay right
 * away during the DAY; at NIGHT it is delayed by 2 s so nothing flashes
 * while the screen is still too dark to matter (directive (ii)). The hide
 * timer keeps the overlay on screen for 5 s, then the normal face
 * returns. Taps while the overlay is showing are ignored. */
static void prv_hide_overlay(void *data) {
  (void)data;
  s_overlay_hide_timer = NULL;
  s_overlay_active = false;
  layer_mark_dirty(s_canvas);
}

static void prv_show_overlay(void *data) {
  (void)data;
  s_overlay_show_timer = NULL;
  s_overlay_active = true;
  layer_mark_dirty(s_canvas);
  s_overlay_hide_timer = app_timer_register(5000, prv_hide_overlay, NULL);
}

static void prv_tap_handler(AccelAxisType axis, int32_t direction) {
  (void)axis;
  (void)direction;
  if (s_overlay_active) {
    return;
  }
  if (s_overlay_show_timer) {
    app_timer_cancel(s_overlay_show_timer);
    s_overlay_show_timer = NULL;
  }
  if (s_overlay_hide_timer) {
    app_timer_cancel(s_overlay_hide_timer);
    s_overlay_hide_timer = NULL;
  }
  if (prv_is_night(time(NULL))) {
    /* Night: hold the overlay 2 s after the tap. */
    s_overlay_show_timer = app_timer_register(2000, prv_show_overlay, NULL);
  } else {
    /* Day: show immediately, no delay. */
    prv_show_overlay(NULL);
  }
}

/* One repaint per minute (plus on load). Sufficient because the only
 * time-varying elements are the minute line, date and day/night colour.
 * Skipped while prv_redraw_suppressed() holds (quiet time / on the charger);
 * when it lifts the next tick repaints because the drawn minute/hour no
 * longer matches. */
static void prv_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  (void)units_changed;
  if (s_overlay_active || prv_redraw_suppressed()) {
    return;
  }
  if (tick_time->tm_hour == s_last_drawn_hour &&
      tick_time->tm_min == s_last_drawn_min) {
    return;
  }
  s_last_drawn_hour = tick_time->tm_hour;
  s_last_drawn_min = tick_time->tm_min;
  layer_mark_dirty(s_canvas);
}

/* Repaint when the charge state changes (plug/unplug), so the %/CHRG label
 * updates without waiting for the next minute. The plugged state also drives
 * the redraw suppression: plugging in goes quiet, unplugging repaints. */
static void prv_battery_handler(BatteryChargeState charge) {
  s_is_plugged = charge.is_plugged;
  if (s_overlay_active || prv_redraw_suppressed()) {
    return;
  }
  layer_mark_dirty(s_canvas);
}

/* Entry for every phone->watch message (one combined weather payload from
 * src/pkjs/index.js). Field identification scheme is in the top-of-file
 * comment: numeric IDs 10001..10006 are the primary path; the marker/value
 * scan below is fallback + diagnostics. */
static void prv_app_message_handler(DictionaryIterator *iter, void *context) {
  (void)context;
  APP_LOG(APP_LOG_LEVEL_INFO, "OldEyes: appmessage received");
  s_messages_received++;
  s_diag[0] = '\0';
  snprintf(s_diag, sizeof(s_diag), "k:");
  size_t dlen = strlen(s_diag);
  /* Primary path: raw NUMERIC IDs (10001..10006) from src/pkjs/index.js,
   * which the phone passes verbatim; symbolic names were dropped ("k:
   * empty"). Values are marker-prefixed ("T:" temp, "D:" desc, "L:" place,
   * "A:" aqi), stripped below. dict_find is order-independent. */
  int32_t sunvals[2] = {0, 0};
  int suncount = 0;
  Tuple *tk;
  if ((tk = dict_find(iter, 10001)) && tk->type == TUPLE_INT) {
    s_sunrise = (time_t)tk->value->int32;
  }
  if ((tk = dict_find(iter, 10002)) && tk->type == TUPLE_INT) {
    s_sunset = (time_t)tk->value->int32;
  }
  if ((tk = dict_find(iter, 10003)) && tk->type == TUPLE_CSTRING &&
      strncmp(tk->value->cstring, "T:", 2) == 0) {
    snprintf(s_weather_temp, sizeof(s_weather_temp), "%s", tk->value->cstring + 2);
  }
  if ((tk = dict_find(iter, 10004)) && tk->type == TUPLE_CSTRING &&
      strncmp(tk->value->cstring, "D:", 2) == 0) {
    snprintf(s_weather_desc, sizeof(s_weather_desc), "%s", tk->value->cstring + 2);
  }
  if ((tk = dict_find(iter, 10005)) && tk->type == TUPLE_CSTRING &&
      strncmp(tk->value->cstring, "L:", 2) == 0) {
    snprintf(s_place_name, sizeof(s_place_name), "%s", tk->value->cstring + 2);
  }
  if ((tk = dict_find(iter, 10006)) && tk->type == TUPLE_CSTRING &&
      strncmp(tk->value->cstring, "A:", 2) == 0) {
    snprintf(s_aqi, sizeof(s_aqi), "%s", tk->value->cstring + 2);
  }
  Tuple *t = dict_read_first(iter);
  /* Fallback + diagnostics: also scan every tuple (by marker/value) so a
   * different ID scheme or tuple order still parses, and record the k: map. */
  while (t) {
    if (dlen < sizeof(s_diag) - 20) {
      int n = snprintf(s_diag + dlen, sizeof(s_diag) - dlen, " %lu:%s",
                       (unsigned long)t->key,
                       t->type == TUPLE_INT ? "i" :
                       (t->type == TUPLE_CSTRING ? "s" : "-"));
      if (n > 0) {
        dlen += (size_t)n;
      }
    }
    if (t->type == TUPLE_INT) {
      int32_t v = t->value->int32;
      if (v > 1000000000 && suncount < 2) {
        sunvals[suncount++] = v;
      }
    } else if (t->type == TUPLE_CSTRING) {
      const char *c = t->value->cstring;
      if (strncmp(c, "T:", 2) == 0) {
        snprintf(s_weather_temp, sizeof(s_weather_temp), "%s", c + 2);
      } else if (strncmp(c, "D:", 2) == 0) {
        snprintf(s_weather_desc, sizeof(s_weather_desc), "%s", c + 2);
      } else if (strncmp(c, "L:", 2) == 0) {
        snprintf(s_place_name, sizeof(s_place_name), "%s", c + 2);
      } else if (strncmp(c, "A:", 2) == 0) {
        snprintf(s_aqi, sizeof(s_aqi), "%s", c + 2);
      }
    }
    t = dict_read_next(iter);
  }
  if (dlen <= 2) {
    snprintf(s_diag, sizeof(s_diag), "k: empty");
  }
  if (suncount == 2) {
    s_sunrise = sunvals[0] <= sunvals[1] ? sunvals[0] : sunvals[1];
    s_sunset = sunvals[0] <= sunvals[1] ? sunvals[1] : sunvals[0];
  } else if (suncount == 1) {
    s_sunrise = s_sunset = sunvals[0];
  }
  APP_LOG(APP_LOG_LEVEL_INFO,
          "OldEyes: temp='%s' desc='%s' aqi='%s' place='%s' sun=%ld/%ld diag='%s'",
          s_weather_temp, s_weather_desc, s_aqi, s_place_name,
          (long)s_sunrise, (long)s_sunset, s_diag);
  layer_mark_dirty(s_canvas);
}

/* Should not happen with the 512 B inbox and the small payload, but if a
 * message is dropped the counter keeps the overlay diagnostics honest. */
static void prv_inbox_dropped_cb(AppMessageResult reason, void *context) {
  (void)context;
  s_messages_dropped++;
  APP_LOG(APP_LOG_LEVEL_ERROR, "OldEyes: inbox dropped: %d", (int)reason);
}

/* Window setup: create the canvas layer, force the first paint (so the face
 * is never blank on load), register AppMessage handlers with generous
 * buffers, and subscribe to shakes, the minute timer and battery changes. */
static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  s_canvas = layer_create(GRect(0, 0, SCR_W, SCR_H));
  layer_set_update_proc(s_canvas, prv_update_proc);
  layer_add_child(window_layer, s_canvas);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  if (t) {
    /* First paint is always drawn, even in quiet time / on the charger, so
     * the face is never blank on load; record it as the last drawn minute. */
    s_is_plugged = battery_state_service_peek().is_plugged;
    s_last_drawn_hour = t->tm_hour;
    s_last_drawn_min = t->tm_min;
    layer_mark_dirty(s_canvas);
  }

  app_message_register_inbox_received(prv_app_message_handler);
  app_message_register_inbox_dropped(prv_inbox_dropped_cb);
  /* Generous buffer sizes: the MINIMUM inbox (124 B) silently drops weather
   * payloads (temp + desc + 2 epochs + long city name can exceed it). 512 B
   * inbox comfortably fits everything; the outbox (256 B) is unused (C
   * never sends). */
  app_message_open(512, 256);
  accel_tap_service_subscribe(prv_tap_handler);
  tick_timer_service_subscribe(MINUTE_UNIT, prv_tick_handler);
  battery_state_service_subscribe(prv_battery_handler);
  APP_LOG(APP_LOG_LEVEL_INFO, "OldEyes: window loaded, services subscribed");
  s_overlay_hide_timer = NULL;
  s_overlay_show_timer = NULL;
}

static void prv_window_unload(Window *window) {
  (void)window;
  if (s_overlay_show_timer) {
    app_timer_cancel(s_overlay_show_timer);
    s_overlay_show_timer = NULL;
  }
  if (s_overlay_hide_timer) {
    app_timer_cancel(s_overlay_hide_timer);
    s_overlay_hide_timer = NULL;
  }
  accel_tap_service_unsubscribe();
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  layer_destroy(s_canvas);
}

/* App lifecycle: create the window, then sit in the event loop. */
static void init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload
  });
  window_stack_push(s_window, true);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
