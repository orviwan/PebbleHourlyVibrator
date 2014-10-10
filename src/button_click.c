#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static bool timer_is_scheduled = false;


static void my_wakeup_handler(WakeupID wakeup_id, int32_t reason) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Woke up due to: %lu", reason);
	//doesn't need to do anything, tick timer should deal with everything
}


static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	if (units_changed & HOUR_UNIT) {
		//vibe!
		vibes_double_pulse();
	}

	if(!timer_is_scheduled) {			
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Current time: %d:%d:%d", tick_time->tm_hour, tick_time->tm_min, tick_time->tm_sec);	
		
		time_t wakeup_time;
		wakeup_time = (tick_time->tm_hour * 60 * 60) + 3598; //current hour + 59 minutes, 58 seconds
		tick_time = (localtime(&wakeup_time));
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Next wake: %d:%d:%d", tick_time->tm_hour, tick_time->tm_min, tick_time->tm_sec);
		
		wakeup_cancel_all(); //just in case
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Scheduling Wakeup Timer");	
		wakeup_schedule(wakeup_time, 3, true);
		wakeup_service_subscribe(my_wakeup_handler);		
		timer_is_scheduled = true;
	}
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Hourly double vibe");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = false;
  window_stack_push(window, animated);	
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}