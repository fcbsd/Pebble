#include <pebble.h>

Window *my_window;

static Layer *s_canvas_layer;
static GColor myfill, mystroke; 
static BatteryStateHandler mbh;

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  /* Custom drawing happens in here. */
  /* set up some stuff */
  uint16_t radius = 7;
  int start = 24;
  int pos = 0;
  int vpos = 150;
  int i;
  BatteryChargeState cs = battery_state_service_peek();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "charge state %d", cs.charge_percent);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "charging %d", cs.is_charging);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "plugged in %d", cs.is_plugged); 
  /* set default colors */
  mystroke = GColorRed;
  if (cs.is_charging == 1) {
    myfill = GColorGreen;
  } else {
    myfill = GColorBlue;
  }
  
  /* set the line colour */
  graphics_context_set_stroke_color(ctx, mystroke);
  /* set the fill colour */
  graphics_context_set_fill_color(ctx, myfill);
  /* set stroke width - must be odd integer */
  graphics_context_set_stroke_width(ctx, 3);
  /* draw 5 circles */
  for (i = 1; i < 6; i++) {
    pos = start * i;
    /* draw a circle */
    GPoint center = GPoint(pos,vpos);
    /* draw outline of circle */
    graphics_draw_circle(ctx, center, radius);
    /* fill circle */
    graphics_fill_circle(ctx, center, radius);
  }
}

void handle_init(void) {
  battery_state_service_subscribe(mbh);
  my_window = window_create();
  GRect bounds = layer_get_bounds(window_get_root_layer(my_window));
  /* Create canvas layer */
  s_canvas_layer = layer_create(bounds);
  /* Assign the custom drawing station */
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  /* Add to window */
  layer_add_child(window_get_root_layer(my_window), s_canvas_layer);
  /* Redraw as soon as possible */
  layer_mark_dirty(s_canvas_layer);
   
  window_stack_push(my_window, true);
}


void handle_deinit(void) {
  battery_state_service_unsubscribe();
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
