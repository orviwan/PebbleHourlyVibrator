#include <pebble.h>

#define WAKEUP_REASON 0
	
static Window *window;
static TextLayer *text_layer;
static WakeupId s_wakeup_id = 0;

static void wakeup_handler(WakeupId wakeup_id, int32_t reason) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Woke up due to: %lu", reason);
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	if (units_changed & HOUR_UNIT) {
		//vibe!
		vibes_double_pulse();
	}

  //Check the event is not already scheduled
  if(!wakeup_query(s_wakeup_id, NULL)) {
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Current time: %d:%d:%d", tick_time->tm_hour, tick_time->tm_min, tick_time->tm_sec);	
		
		time_t wakeup_time = (tick_time->tm_hour * 60 * 60) + 3598; //current hour + 59 minutes, 58 seconds
		tick_time = localtime(&wakeup_time);		
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Next wake: %d:%d:%d", tick_time->tm_hour, tick_time->tm_min, tick_time->tm_sec);
		
		// Schedule wakeup event
    s_wakeup_id = wakeup_schedule(wakeup_time, WAKEUP_REASON, true);	

		APP_LOG(APP_LOG_LEVEL_DEBUG, "WakeupId: %lu", s_wakeup_id);
	}
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Hourly double vibe");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
	
  // Subscribe to Wakeup API
  wakeup_service_subscribe(wakeup_handler);

  // Was this a wakeup?
  if(launch_reason() == APP_LAUNCH_WAKEUP) {
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "The app was started by a wakeup");
		
    // The app was started by a wakeup
    WakeupId id = 0;
    int32_t reason = 0;

    // Get details and handle the wakeup
    wakeup_get_launch_event(&id, &reason);
    wakeup_handler(id, reason);
  } 	
	
	// Subscribe to TICK
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
  window_stack_push(window, false);		
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}