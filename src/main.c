#include <pebble.h>
#include <math.h> 
#include "drunkwarning.h"
#include "welcome.h"
#include "instructions.h"

#define REPEAT_INTERVAL_MS 50
#define CALIB_PKEY 1
#define CALIB_DEFAULT 0

Window *window;
TextLayer *text_layer_1, *text_layer_2, *text_layer_3, *text_layer_4;
static ActionBarLayer *s_action_bar;
char tap_text[3];
static GBitmap *s_icon_reset, *s_icon_play, *s_icon_pause, *s_icon_calib;

int last_x = 0; //last known x-accelerometer value
int sum_x = 0; //cumulative sum of current trial
int pause = 0; //0-pause, 1-start
time_t start_time = 0; //start time of trial
time_t end_time = 0; //end time of trial
double elapsed = 0; //elapsed time of trial 
int result;
int calibrating = 0;
static void send(int key, int value);
static void display (TextLayer *textlayer, const char *format, int value);
static void accel_tap_handler(AccelAxisType axis, int32_t direction);
void accel_handler(AccelData *data, uint32_t num_samples)
{
  // data is an array of num_samples elements.
  // num_samples was set when calling accel_data_service_subscribe.
  if (pause == 1 && abs(data[0].x-last_x) > 30 && !data[0].did_vibrate) {
    sum_x = sum_x + abs(data[0].x-last_x);
    last_x = data[0].x;
    display(text_layer_1, "Score: %d", sum_x);
  }
}

static void pause_click_handler(ClickRecognizerRef recognizer, void *context) {
  int calib_stored = persist_exists(CALIB_PKEY) ? persist_read_int(CALIB_PKEY) : CALIB_DEFAULT;
  if (calib_stored != 0 && calibrating == 0) {
    pause++;
    pause = pause % 2;
    if (pause == 0) {
      end_time = time(NULL);
      double elapsed = difftime(end_time, start_time);
      result = (int)floor(sum_x/elapsed);
      display(text_layer_3, "Score: %d", result);
      action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, s_icon_play);
      int calib_stored = persist_exists(CALIB_PKEY) ? persist_read_int(CALIB_PKEY) : CALIB_DEFAULT;
      display(text_layer_4, "Calib: %d", calib_stored);
      text_layer_set_text(text_layer_2, "");
      if (result > 2*calib_stored) {
        send(0, 1);
        text_layer_set_text(text_layer_1, "YOU'RE DRUNK");
        dialog_message_window_push();
        
      }
      else {
        send(0, 0);
        text_layer_set_text(text_layer_1, "YOU'RE FINE");
        
      }
    }
    else {
      sum_x = 0;
      text_layer_set_text(text_layer_2, "RUNNING");
      text_layer_set_text(text_layer_3, "");
      action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, s_icon_pause);   
      start_time = time(NULL);
    }
  }
}

static void calib_click_handler(ClickRecognizerRef recognizer, void *context) {
  calibrating++;
  calibrating = calibrating%2;
  pause++;
  pause = pause % 2;
  if (pause == 0) {
    end_time = time(NULL);
    double elapsed = difftime(end_time, start_time);
    //display(text_layer_3, "Final: %d", (int)floor(sum_x/elapsed));
    text_layer_set_text(text_layer_1, "CALIBRATION");
    text_layer_set_text(text_layer_2, "DONE");
    action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, s_icon_play);
    persist_write_int(CALIB_PKEY, (int)floor(sum_x/elapsed));
    display(text_layer_3, "Calib: %d", (int)floor(sum_x/elapsed));
  }
  else {
    sum_x = 0;
    text_layer_set_text(text_layer_2, "CALIBRATING...");
    text_layer_set_text(text_layer_3, "");
    text_layer_set_text(text_layer_4, "");
    action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, s_icon_pause);
    start_time = time(NULL);
  }
}

static void display (TextLayer *textlayer, const char *format, int value) {
    char *ptr = (char*)malloc(18*sizeof(char));
    //char s_body_text[18];
    snprintf(ptr, 18, format, value);
    text_layer_set_text(textlayer, ptr);
}

static void clear_click_handler(ClickRecognizerRef recognizer, void *context) {
  int calib_stored = persist_exists(CALIB_PKEY) ? persist_read_int(CALIB_PKEY) : CALIB_DEFAULT;
  if (calib_stored != 0) {
    sum_x = 0;
    //display(text_layer_1,"Score: %d",sum_x);
    //text_layer_set_text(text_layer_3, "");
  } 
}

static void endtrial_callback() {
  pause++;
  vibes_short_pulse();
  end_time = time(NULL);
  double elapsed = difftime(end_time, start_time);
  result = (int)floor(sum_x/elapsed);
  
  display(text_layer_3, "Final: %d", result);
  
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, s_icon_play);
  int calib_stored = persist_exists(CALIB_PKEY) ? persist_read_int(CALIB_PKEY) : CALIB_DEFAULT;

  if (result > 2*calib_stored) {
    send(0,1);
    text_layer_set_text(text_layer_1, "YOU'RE DRUNK");
    text_layer_set_text(text_layer_4, "");
    dialog_message_window_push();
  }
  else {
    send(0,0);
    text_layer_set_text(text_layer_1, "YOU'RE FINE");
    text_layer_set_text(text_layer_2, "");
    text_layer_set_text(text_layer_4, "");
  }

}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
    accel_data_service_unsubscribe();
    vibes_short_pulse();
    psleep(2000);
    accel_data_service_subscribe(1, accel_handler);
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
    pause = 1;
    accel_tap_service_unsubscribe();
    text_layer_set_text(text_layer_2, "RUNNING");
    text_layer_set_text(text_layer_3, "");
    action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, s_icon_pause);   
    start_time = time(NULL);
    app_timer_register(10000, endtrial_callback, (void*)start_time);
}



static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, REPEAT_INTERVAL_MS, pause_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_INTERVAL_MS, clear_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_INTERVAL_MS, calib_click_handler);
}



void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);

  text_layer_1 = text_layer_create(GRect(0, 0, 104, 30));
  text_layer_set_text_color(text_layer_1, GColorBlack);
  text_layer_set_background_color(text_layer_1, GColorWhite);
  text_layer_set_font(text_layer_1, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_1));

  s_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(s_action_bar, window);
  action_bar_layer_set_click_config_provider(s_action_bar, click_config_provider);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_DOWN, s_icon_reset);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, s_icon_play);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_UP, s_icon_calib);
  
  text_layer_2 = text_layer_create(GRect(0, 30, 104, 40));
  text_layer_set_font(text_layer_2, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_2));
  
  text_layer_3 = text_layer_create(GRect(0, 70, 104, 20));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_3));
  text_layer_set_overflow_mode(text_layer_3, GTextOverflowModeWordWrap);
  
  text_layer_4 = text_layer_create(GRect(0, 90, 104, 20));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_4));
  int calib_stored = persist_exists(CALIB_PKEY) ? persist_read_int(CALIB_PKEY) : CALIB_DEFAULT;
  if (calib_stored != 0) {
    text_layer_set_text(text_layer_1, "HELLO,");
    text_layer_set_text(text_layer_2, "DRINK SAFE");
    text_layer_set_text(text_layer_4, "Flick to start");
    display(text_layer_3,"Calib: %d", calib_stored);
  }
  else {
    text_layer_set_text(text_layer_1, "PLEASE");
    text_layer_set_text(text_layer_2, "CALIBRATE");
    text_layer_set_text(text_layer_3, "Press UP button");
    text_layer_set_text(text_layer_4, "to calibrate.");
  }
  accel_data_service_subscribe(1, accel_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);

  accel_tap_service_subscribe(accel_tap_handler);
}

void window_unload(Window *window)
{
  // Call this before destroying text_layer, because it can change the text
  // and this must only happen while the layer exists.
  accel_data_service_unsubscribe();
  accel_tap_service_unsubscribe();

  text_layer_destroy(text_layer_2);
  text_layer_destroy(text_layer_1);
  text_layer_destroy(text_layer_3);
  text_layer_destroy(text_layer_4);
  gbitmap_destroy(s_icon_reset);
  gbitmap_destroy(s_icon_play);
  gbitmap_destroy(s_icon_pause);
  gbitmap_destroy(s_icon_calib);
}

void create_icons() {
  s_icon_reset = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CROSS);
  s_icon_play = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PLAY);
  s_icon_pause = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PAUSE);
  s_icon_calib = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CALIB);
}

static void send(int key, int value) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  dict_write_int(iter, key, &value, sizeof(int), true);

  app_message_outbox_send();
}


int main()
{
  create_icons();
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers)
  {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
  int calib_value = persist_exists(CALIB_PKEY) ? persist_read_int(CALIB_PKEY) : CALIB_DEFAULT;
  if(calib_value == 0) {
    instruction_window_push();
    welcome_window_push();
    
  }
  
  //App 
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
  
  
  
  app_event_loop();
  window_destroy(window);
}