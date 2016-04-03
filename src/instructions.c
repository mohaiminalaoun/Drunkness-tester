#include <pebble.h>
#include "instructions.h"
#ifndef REPEAT_INTERVAL_MS
#define REPEAT_INTERVAL_MS 50
#endif

static void click_config_provider(void *context);
static void create_icons();
Window *instr_window;
TextLayer *text_layer_1a, *text_layer_2a, *text_layer_3a, *text_layer_4a;
static ActionBarLayer *s_action_bar;
static GBitmap *s_icon_reset, *s_icon_play, *s_icon_pause, *s_icon_calib;


static void window_load(Window *window)
{
  create_icons();
  Layer *window_layer = window_get_root_layer(instr_window);

  text_layer_1a = text_layer_create(GRect(0, 18, 104, 30));
  text_layer_set_text_color(text_layer_1a, GColorBlack);
  text_layer_set_background_color(text_layer_1a, GColorGreen);
  text_layer_set_font(text_layer_1a, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_1a));

  
  
  text_layer_2a = text_layer_create(GRect(0, 68, 104, 30));
  text_layer_set_text_color(text_layer_1a, GColorBlack);
  text_layer_set_background_color(text_layer_2a, GColorCadetBlue);
  text_layer_set_font(text_layer_2a, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_2a));
  
  text_layer_3a = text_layer_create(GRect(0, 120, 104, 30));
  text_layer_set_text_color(text_layer_1a, GColorBlack);
  text_layer_set_background_color(text_layer_3a, GColorRed);
  text_layer_set_font(text_layer_3a, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_3a));
  
  text_layer_4a = text_layer_create(GRect(0, 150, 104, 20));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_4a));
  
  s_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(s_action_bar, instr_window);

  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_DOWN, s_icon_reset);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, s_icon_play);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_UP, s_icon_calib);
  
  window_set_click_config_provider(window, click_config_provider);
  text_layer_set_text(text_layer_1a, "CALIBRATE");
  text_layer_set_text(text_layer_2a, "START");
  text_layer_set_text(text_layer_3a, "CLEAR");
  text_layer_set_text(text_layer_4a, "SELECT to continue");
  text_layer_set_text_alignment(text_layer_1a,GTextAlignmentRight);
  text_layer_set_text_alignment(text_layer_2a,GTextAlignmentRight);
  text_layer_set_text_alignment(text_layer_3a,GTextAlignmentRight);
}

// static void click_config_provider(void *context) {
//   window_single_repeating_click_subscribe(BUTTON_ID_SELECT, REPEAT_INTERVAL_MS, dummy_handler);
//   window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_INTERVAL_MS, dummy_handler);
//   window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_INTERVAL_MS, dummy_handler);
// }

// static void dummy_handler(ClickRecognizerRef recognizer, void *context) {
  
// }

static void window_unload(Window *window)
{
  // Call this before destroying text_layer, because it can change the text
  // and this must only happen while the layer exists.

  text_layer_destroy(text_layer_2a);
  text_layer_destroy(text_layer_1a);
  text_layer_destroy(text_layer_3a);
  text_layer_destroy(text_layer_4a);
  gbitmap_destroy(s_icon_reset);
  gbitmap_destroy(s_icon_play);
  gbitmap_destroy(s_icon_pause);
  gbitmap_destroy(s_icon_calib);
}

static void create_icons() {
  s_icon_reset = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CROSS);
  s_icon_play = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PLAY);
  s_icon_pause = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PAUSE);
  s_icon_calib = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CALIB);
}
static void exit_click_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop(true);
}

static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, REPEAT_INTERVAL_MS, exit_click_handler);

}
void instruction_window_push() {
  if(!instr_window) {
    instr_window = window_create();
    //window_set_background_color(instr_window, GColorBlack);
    window_set_window_handlers(instr_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(instr_window, true);
}