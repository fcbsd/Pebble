#include <pebble.h>

static Window *s_main_window;

static TextLayer *s_time_layer;

static void update_time() {
  // Get tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  // Write the current hours and minutes to buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
          "%H:%M" : "%I:%M", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
    GRect(0, 52, bounds.size.w, 50));
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // Add it as a child layer to the window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_unload(Window *window){
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void handle_init(void) {
  // Create main Window element and assign pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elemenet inside the window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Register with the TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Show the window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is shown from the start
  update_time();
}

void handle_deinit(void) {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}