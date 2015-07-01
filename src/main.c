/* Boxes Pebble Watchface
 * pg@binaerraum.de
 * 1.0 (29.06'15)
 */

#include <pebble.h>

static Window *s_main_window;
static Layer *s_box_layer;

static void draw_boxes_on_layer(Layer *layer, GContext *ctx) 
{
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    int m = tick_time->tm_min;
    int h = tick_time->tm_hour;
    
    bool pm = h >= 12;
    int _h = h % 12;
    int _m = ((m % 60) / 10);
    
    // 12 o'clock
    if (_h + _m == 0) {
        _h = 12;
    }

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Time is %i:%i (%i:%i)", h, m, _h, _m);

    // Clear bg
    graphics_context_set_fill_color(ctx,  pm ? GColorWhite : GColorBlack);
    graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);        

    // Draw h boxes
    int x = 0;
    int y = 0;
    int width = 144 / 3;
    int height = 168 / 4;
    
    graphics_context_set_fill_color(ctx, pm ? GColorBlack : GColorWhite);

    for (int i = 0; i < _h; i++) {
        int __w = x < 2*width ? -1 : 0; 
        int __h = y < 3*height ? -1 : 0; 
        graphics_fill_rect(ctx, GRect(x, y, width + __w, height + __h), 0, GCornerNone);        
        x += width;
        
        if (x >= 3*width) {
            x = 0;
            y += height;
        }
    }

    // Draw m boxes
    int x_m = 0;
    int y_m = 0;
    int width_m = 48 / 3;
    int height_m = 42 / 2;

    for (int i = 0; i < _m; i++) {
        int __w = x < 2*width || x_m < 2*width_m ? -1 : 0; 
        int __h = y < 3*height || y_m < 1*height_m ? -1 : 0; 
        graphics_fill_rect(ctx, GRect(x + x_m, y + y_m, width_m + __w, height_m + __h), 0, GCornerNone);
        x_m += width_m;
        
        if (x_m >= 3*width_m) {
            x_m = 0;
            y_m += height_m;
        }
    }
}

static void main_window_load(Window *window)
{
    window_set_background_color(window, GColorBlack);
    
    s_box_layer = layer_create(GRect(0, 0, 144, 168));
    layer_add_child(window_get_root_layer(window), s_box_layer);
    layer_set_update_proc(s_box_layer, &draw_boxes_on_layer);
}

static void main_window_unload(Window *window) 
{
    layer_destroy(s_box_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) 
{   
    if (tick_time->tm_min % 10 == 0) {
        layer_mark_dirty(s_box_layer);
    }
}

static void init(void) 
{
    s_main_window = window_create();
    
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    window_stack_push(s_main_window, true);
}

static void deinit(void) 
{
    window_destroy(s_main_window);
}

int main(void) 
{
    init();
    app_event_loop();
    deinit();
}
