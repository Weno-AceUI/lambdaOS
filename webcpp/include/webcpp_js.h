#ifndef WEBCPP_JS_H
#define WEBCPP_JS_H

#include <stdint.h>
#include <stdbool.h>

// JavaScript value types
typedef enum {
    JS_TYPE_UNDEFINED,
    JS_TYPE_NULL,
    JS_TYPE_BOOLEAN,
    JS_TYPE_NUMBER,
    JS_TYPE_STRING,
    JS_TYPE_OBJECT,
    JS_TYPE_FUNCTION,
    JS_TYPE_ARRAY
} js_type_t;

// JavaScript value
typedef struct {
    js_type_t type;
    union {
        bool boolean;
        double number;
        char* string;
        void* object;
        void* function;
        void* array;
    } value;
} js_value_t;

// JavaScript context
typedef struct {
    void* engine;           // JS engine instance (V8/JSC)
    void* global;          // Global object
    void* context;         // JS context
} js_context_t;

// Initialize JavaScript engine
bool js_init(const char* engine_name);

// Create new JS context
js_context_t* js_create_context(void);

// Execute JavaScript code
bool js_eval(js_context_t* ctx, const char* code, js_value_t* result);

// Load and execute JS file
bool js_load_file(js_context_t* ctx, const char* path, js_value_t* result);

// Convert C++ value to JS value
js_value_t js_from_bool(bool value);
js_value_t js_from_number(double value);
js_value_t js_from_string(const char* value);
js_value_t js_from_object(void* value);
js_value_t js_from_function(void* function_ptr);

// Convert JS value to C++ value
bool js_to_bool(js_value_t value);
double js_to_number(js_value_t value);
char* js_to_string(js_value_t value);
void* js_to_object(js_value_t value);
void* js_to_function(js_value_t value);

// Call JavaScript function
bool js_call_function(js_context_t* ctx, js_value_t function, js_value_t* args, uint32_t arg_count, js_value_t* result);

// Register C++ function
bool js_register_function(js_context_t* ctx, const char* name, void* function_ptr);

// Cleanup JS context
void js_destroy_context(js_context_t* ctx);

// Shutdown JavaScript engine
void js_shutdown(void);

#endif // WEBCPP_JS_H 