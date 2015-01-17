#include <pebble.h>

#define KEY_NOTES 0
#define KEY_LENGTH 1
  
static Window* s_main_window;
static TextLayer* s_notes_layer;
static char* notes_text = NULL;
static int notes_length;

static void main_window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  
  s_notes_layer = text_layer_create(GRect(0, 0, 144, 168));
  text_layer_set_background_color(s_notes_layer, GColorClear);
  text_layer_set_text_color(s_notes_layer, GColorWhite);
  text_layer_set_text_alignment(s_notes_layer, GTextAlignmentLeft);
  text_layer_set_font(s_notes_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  
  notes_length = persist_read_int(KEY_LENGTH);
  if (notes_text != NULL) {
    free(notes_text);
  }
  notes_text = malloc(notes_length + 1);
  persist_read_string(KEY_NOTES, notes_text, notes_length + 1);
  APP_LOG(APP_LOG_LEVEL_INFO, "Init text: %s", notes_text);
  APP_LOG(APP_LOG_LEVEL_INFO, "Init text length: %d", notes_length);
  text_layer_set_text(s_notes_layer, notes_text);
  
  layer_add_child(window_layer, text_layer_get_layer(s_notes_layer));
}

static void main_window_unload(Window* window) {
  text_layer_destroy(s_notes_layer);
  if (notes_text != NULL) {
    free(notes_text);
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *text = dict_find(iterator, KEY_NOTES);
  Tuple *length = dict_find(iterator, KEY_LENGTH);
  if (text && length) {
      text_layer_set_text(s_notes_layer, text->value->cstring);
      persist_write_string(KEY_NOTES, text->value->cstring);
      persist_write_int(KEY_LENGTH, length->value->uint8);
      APP_LOG(APP_LOG_LEVEL_INFO, "Notes text: %s", text->value->cstring);
      APP_LOG(APP_LOG_LEVEL_INFO, "Notes length: %d", length->value->uint8);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  window_set_background_color(s_main_window, GColorBlack);
  
  notes_text = malloc(notes_length + 1);
  notes_length = persist_read_int(KEY_LENGTH);
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}