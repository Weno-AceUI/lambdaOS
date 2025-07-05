#include "../include/webcpp.h"
#include "../include/webcpp_gpu.h"
#include "../include/webcpp_threading.h"
#include "../../ui/include/aceui_widget.h"
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // For directory listing
#include <stdio.h>
#include <vector> // For argument processing in V8 callbacks

// V8 Integration - Ensure V8 headers are in your include path.
#include "../../v8/include/v8.h"
#include "../../v8/include/libplatform/libplatform.h" // For V8 platform initialization

// Forward declaration
static void GenericV8FunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args);

// Internal structure to hold V8 specific context
typedef struct {
    v8::Isolate* isolate;
    v8::Global<v8::Context> context;
    v8::Global<v8::Object> global_object; // To attach functions to 'window'
    // Store registered function metadata if needed for complex dispatch or cleanup
} webcpp_v8_engine_context_t;

static bool g_initialized = false;
static webcpp_config_t g_config;
static v8::Platform* g_v8_platform = nullptr; // V8 platform instance

bool webcpp_init(const webcpp_config_t* config) {
    if (g_initialized) return false;
    
    // Copy configuration
    memcpy(&g_config, config, sizeof(webcpp_config_t));
    
    // Initialize V8
    if (strcmp(g_config.js_engine, "v8") == 0) {
        if (!g_v8_platform) {
            v8::V8::InitializeICUDefaultLocation(nullptr); // Adjust if ICU data is elsewhere
            v8::V8::InitializeExternalStartupData(nullptr); // Adjust if snapshot is elsewhere
            g_v8_platform = v8::platform::NewDefaultPlatform().release();
            v8::V8::InitializePlatform(g_v8_platform);
        }
        v8::V8::Initialize();
        printf("WebCpp: V8 initialized.\n");
    } else {
        fprintf(stderr, "WebCpp Error: Unsupported JS engine: %s\n", g_config.js_engine);
        return false;
    }

    // Initialize subsystems
    // Note: GPU and threading init might need to be aware of V8's threading model
    if (g_config.enable_gpu && !gpu_init()) {
        return false;
    }
    
    if (g_config.enable_threading && !threading_init()) {
        if (g_config.enable_gpu) gpu_cleanup();
        return false;
    }
    
    g_initialized = true;
    return true;
}

webcpp_context_t* webcpp_create_context(void) {
    if (!g_initialized) return NULL;
    
    webcpp_context_t* context = (webcpp_context_t*)malloc(sizeof(webcpp_context_t));
    if (!context) return NULL;
    memset(context, 0, sizeof(webcpp_context_t)); // Zero out the struct

    if (strcmp(g_config.js_engine, "v8") == 0) {
        webcpp_v8_engine_context_t* v8_ctx = (webcpp_v8_engine_context_t*)malloc(sizeof(webcpp_v8_engine_context_t));
        if (!v8_ctx) {
            free(context);
            return NULL;
        }
        v8::Isolate::CreateParams create_params;
        create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        v8_ctx->isolate = v8::Isolate::New(create_params);
        // TODO: Set up memory limits based on g_config.max_memory if V8 API allows per-isolate limits easily

        v8::Isolate::Scope isolate_scope(v8_ctx->isolate);
        v8::HandleScope handle_scope(v8_ctx->isolate);
        
        v8::Local<v8::Context> local_context = v8::Context::New(v8_ctx->isolate);
        v8_ctx->context.Reset(v8_ctx->isolate, local_context);
        v8_ctx->global_object.Reset(v8_ctx->isolate, local_context->Global());

        context->js_context = v8_ctx;
        printf("WebCpp: V8 context created.\n");
    } else {
        // Should not happen if init succeeded with "v8"
        free(context);
        return NULL;
    }
    
    context->dom_context = NULL;   // Will be set by DOM implementation
    context->render_context = NULL; // Will be set by renderer
    context->context_id = 0;       // Will be set by context manager
    
    return context;
}

bool webcpp_eval_js(webcpp_context_t* context, const char* js_code) {
    if (!context || !js_code || !context->js_context) return false;

    webcpp_v8_engine_context_t* v8_ctx = (webcpp_v8_engine_context_t*)context->js_context;
    v8::Isolate* isolate = v8_ctx->isolate;
    v8::Locker locker(isolate); // If multi-threaded V8 access
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> local_v8_context = v8_ctx->context.Get(isolate);
    v8::Context::Scope context_scope(local_v8_context);

    v8::Local<v8::String> source;
    if (!v8::String::NewFromUtf8(isolate, js_code, v8::NewStringType::kNormal).ToLocal(&source)) {
        fprintf(stderr, "WebCpp Error: Could not create V8 string from JS code.\n");
        return false;
    }

    v8::Local<v8::Script> script;
    v8::TryCatch try_catch(isolate);
    if (!v8::Script::Compile(local_v8_context, source).ToLocal(&script)) {
        v8::String::Utf8Value error(isolate, try_catch.Exception());
        fprintf(stderr, "WebCpp JS Compile Error: %s\n", *error);
        return false;
    }

    v8::Local<v8::Value> result;
    if (!script->Run(local_v8_context).ToLocal(&result)) {
        v8::String::Utf8Value error(isolate, try_catch.Exception());
        fprintf(stderr, "WebCpp JS Runtime Error: %s\n", *error);
        return false;
    }
    return true;
}

bool webcpp_load_js_file(webcpp_context_t* context, const char* file_path) {
    if (!context || !file_path) return false;
    char* js_code = webcpp_read_entire_file(file_path);
    if (!js_code) {
        fprintf(stderr, "WebCpp Error: Could not read JS file %s\n", file_path);
        return false;
    }
    bool success = webcpp_eval_js(context, js_code);
    free(js_code);
    return success;
}

// Helper struct to pass C++ function pointer and WebCpp context to V8 callbacks
typedef struct {
    webcpp_context_t* webcpp_ctx_ptr; // Pointer to the user's WebCpp context
    void* c_function_ptr;          // Pointer to the actual C/C++ function
    // We need a way to determine the signature of c_function_ptr.
    // For this example, we'll infer based on the name or assume a limited set.
    // A more robust system would pass signature metadata.
    const char* function_name_for_dispatch; // For dispatching if using one generic V8 callback
} V8CallbackData;

// This is a generic V8 callback. It will try to dispatch to the correct C++ function.
// This is complex because C++ functions have different signatures.
static void GenericV8FunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    v8::Local<v8::External> ext_data = v8::Local<v8::External>::Cast(args.Data());
    V8CallbackData* cb_data = static_cast<V8CallbackData*>(ext_data->Value());

    webcpp_context_t* webcpp_ctx = cb_data->webcpp_ctx_ptr;
    void* c_func_ptr = cb_data->c_function_ptr;
    const char* func_name = cb_data->function_name_for_dispatch;

    // --- Argument and Return Value Handling ---
    // This section needs to be carefully implemented based on the expected signatures
    // of the C++ functions you are bridging.

    // Example for functions taking (webcpp_context_t*, const char*) and returning bool
    if (strcmp(func_name, "HomeScreenAPI_launchApplication") == 0 ||
        strcmp(func_name, "BrowserAPI_loadUrl") == 0) {
        if (args.Length() < 1 || !args[0]->IsString()) {
            isolate->ThrowException(v8::String::NewFromUtf8Literal(isolate, "Expected 1 string argument."));
            return;
        }
        v8::String::Utf8Value arg0_v8(isolate, args[0]);
        const char* arg0_c = *arg0_v8;

        typedef bool (*FuncType_Ctx_Str_Bool)(webcpp_context_t*, const char*);
        FuncType_Ctx_Str_Bool actual_func = reinterpret_cast<FuncType_Ctx_Str_Bool>(c_func_ptr);
        bool result = actual_func(webcpp_ctx, arg0_c);
        args.GetReturnValue().Set(v8::Boolean::New(isolate, result));
        return;
    }

    // Example for functions taking (webcpp_context_t*, const char*, const char*) and returning bool
    if (strcmp(func_name, "CameraAPI_savePhoto") == 0) {
         if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
            isolate->ThrowException(v8::String::NewFromUtf8Literal(isolate, "Expected 2 string arguments."));
            return;
        }
        v8::String::Utf8Value arg0_v8(isolate, args[0]);
        const char* arg0_c = *arg0_v8;
        v8::String::Utf8Value arg1_v8(isolate, args[1]);
        const char* arg1_c = *arg1_v8;

        typedef bool (*FuncType_Ctx_Str_Str_Bool)(webcpp_context_t*, const char*, const char*);
        FuncType_Ctx_Str_Str_Bool actual_func = reinterpret_cast<FuncType_Ctx_Str_Str_Bool>(c_func_ptr);
        bool result = actual_func(webcpp_ctx, arg0_c, arg1_c);
        args.GetReturnValue().Set(v8::Boolean::New(isolate, result));
        return;
    }

    // Example for functions taking (webcpp_context_t*, const char*) and returning const char*
    if (strcmp(func_name, "PhotosAPI_listDirectoryItems") == 0 ||
        strcmp(func_name, "PhotosAPI_getImageDataUrl") == 0) {
        if (args.Length() < 1 || !args[0]->IsString()) {
            isolate->ThrowException(v8::String::NewFromUtf8Literal(isolate, "Expected 1 string argument."));
            return;
        }
        v8::String::Utf8Value arg0_v8(isolate, args[0]);
        const char* arg0_c = *arg0_v8;

        typedef const char* (*FuncType_Ctx_Str_StrPtr)(webcpp_context_t*, const char*);
        FuncType_Ctx_Str_StrPtr actual_func = reinterpret_cast<FuncType_Ctx_Str_StrPtr>(c_func_ptr);
        const char* result_c_str = actual_func(webcpp_ctx, arg0_c);

        if (result_c_str) {
            args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, result_c_str, v8::NewStringType::kNormal).ToLocalChecked());
            // IMPORTANT: If result_c_str was allocated by the C++ function (e.g., strdup),
            // it needs to be freed. WebCpp should provide a JS-callable C function to free such strings.
            // For example: webcpp_free_returned_string(result_c_str);
            // This is a common pattern for C/JS bridges.
            // For now, this example assumes the C++ function's string is either static or its lifetime is managed elsewhere.
            // The PhotosAPI wrappers currently return strdup'd strings, which will leak here without a free mechanism.
        } else {
            args.GetReturnValue().Set(v8::Null(isolate));
        }
        return;
    }

    // TabBar and Tab widget creation functions
    if (strcmp(func_name, "WidgetAPI_createTabBar") == 0) {
        if (args.Length() < 1 || !args[0]->IsString()) {
            isolate->ThrowException(v8::String::NewFromUtf8Literal(isolate, "Expected 1 string argument (id)."));
            return;
        }
        v8::String::Utf8Value arg0_v8(isolate, args[0]);
        const char* arg0_c = *arg0_v8;

        typedef void* (*FuncType_Ctx_Str_Ptr)(webcpp_context_t*, const char*);
        FuncType_Ctx_Str_Ptr actual_func = reinterpret_cast<FuncType_Ctx_Str_Ptr>(c_func_ptr);
        void* result = actual_func(webcpp_ctx, arg0_c);
        
        if (result) {
            args.GetReturnValue().Set(v8::External::New(isolate, result));
        } else {
            args.GetReturnValue().Set(v8::Null(isolate));
        }
        return;
    }

    if (strcmp(func_name, "WidgetAPI_createTab") == 0) {
        if (args.Length() < 3 || !args[0]->IsString() || !args[1]->IsString()) {
            isolate->ThrowException(v8::String::NewFromUtf8Literal(isolate, "Expected 3 arguments: id (string), title (string), content_widget (object)."));
            return;
        }
        v8::String::Utf8Value arg0_v8(isolate, args[0]);
        const char* arg0_c = *arg0_v8;
        v8::String::Utf8Value arg1_v8(isolate, args[1]);
        const char* arg1_c = *arg1_v8;
        
        void* content_widget = NULL;
        if (!args[2]->IsNull() && args[2]->IsExternal()) {
            content_widget = v8::Local<v8::External>::Cast(args[2])->Value();
        }

        typedef void* (*FuncType_Ctx_Str_Str_Ptr_Ptr)(webcpp_context_t*, const char*, const char*, void*);
        FuncType_Ctx_Str_Str_Ptr_Ptr actual_func = reinterpret_cast<FuncType_Ctx_Str_Str_Ptr_Ptr>(c_func_ptr);
        void* result = actual_func(webcpp_ctx, arg0_c, arg1_c, content_widget);
        
        if (result) {
            args.GetReturnValue().Set(v8::External::New(isolate, result));
        } else {
            args.GetReturnValue().Set(v8::Null(isolate));
        }
        return;
    }

    if (strcmp(func_name, "WidgetAPI_addTabToTabBar") == 0) {
        if (args.Length() < 2 || !args[0]->IsExternal() || !args[1]->IsExternal()) {
            isolate->ThrowException(v8::String::NewFromUtf8Literal(isolate, "Expected 2 arguments: tabbar_widget (object), tab_widget (object)."));
            return;
        }
        
        void* tabbar_widget = v8::Local<v8::External>::Cast(args[0])->Value();
        void* tab_widget = v8::Local<v8::External>::Cast(args[1])->Value();

        typedef bool (*FuncType_Ctx_Ptr_Ptr_Bool)(webcpp_context_t*, void*, void*);
        FuncType_Ctx_Ptr_Ptr_Bool actual_func = reinterpret_cast<FuncType_Ctx_Ptr_Ptr_Bool>(c_func_ptr);
        bool result = actual_func(webcpp_ctx, tabbar_widget, tab_widget);
        
        args.GetReturnValue().Set(v8::Boolean::New(isolate, result));
        return;
    }

    // Fallback if no signature matched
    fprintf(stderr, "WebCpp Error: No matching C++ signature found for JS function call: %s\n", func_name);
    isolate->ThrowException(v8::String::NewFromUtf8(isolate, "WebCpp Error: Unhandled C++ function signature.", v8::NewStringType::kNormal).ToLocalChecked());
}

bool webcpp_register_function(webcpp_context_t* context, const char* name, void* function_ptr) {
    if (!context || !name || !function_ptr || !context->js_context) {
        fprintf(stderr, "WebCpp Error: Invalid arguments or uninitialized JS context for webcpp_register_function.\n");
        return false;
    }

    webcpp_v8_engine_context_t* v8_ctx = (webcpp_v8_engine_context_t*)context->js_context;
    v8::Isolate* isolate = v8_ctx->isolate;
    v8::Locker locker(isolate);
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> local_v8_context = v8_ctx->context.Get(isolate);
    v8::Context::Scope context_scope(local_v8_context);

    // Create data payload to pass to the V8 callback
    V8CallbackData* cb_data = (V8CallbackData*)malloc(sizeof(V8CallbackData));
    if (!cb_data) {
        fprintf(stderr, "WebCpp Error: Failed to allocate memory for callback data.\n");
        return false;
    }
    cb_data->webcpp_ctx_ptr = context;
    cb_data->c_function_ptr = function_ptr;
    cb_data->function_name_for_dispatch = strdup(name); // For dispatching in GenericV8FunctionCallback

    v8::Local<v8::External> v8_ext_data = v8::External::New(isolate, cb_data);

    v8::Local<v8::FunctionTemplate> func_template = v8::FunctionTemplate::New(isolate, GenericV8FunctionCallback, v8_ext_data);
    
    v8::Local<v8::Function> func;
    if (!func_template->GetFunction(local_v8_context).ToLocal(&func)) {
        fprintf(stderr, "WebCpp Error: Failed to create JS function from template for %s.\n", name);
        free(cb_data->function_name_for_dispatch);
        free(cb_data);
        return false;
    }

    v8::Local<v8::String> js_name = v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::Object> global = v8_ctx->global_object.Get(isolate);
    
    v8::Maybe<bool> result = global->Set(local_v8_context, js_name, func);

    if (result.IsNothing() || !result.FromJust()) {
        fprintf(stderr, "WebCpp Error: Failed to set function '%s' on global JS object.\n", name);
        // Note: cb_data and cb_data->function_name_for_dispatch would leak here if not cleaned up.
        // V8 typically takes ownership of External data if it's for a persistent handle,
        // but for FunctionTemplate data, cleanup might be manual or via weak handles.
        // For simplicity, this example doesn't show advanced V8 resource management.
        free(cb_data->function_name_for_dispatch);
        free(cb_data);
        return false;
    }
    
    printf("WebCpp: Successfully registered function '%s' to JS global scope.\n", name);
    return true; // cb_data and its strdup'd name are intentionally leaked here for simplicity,
                 // a real implementation needs robust cleanup, e.g., when context is destroyed.
}

static char* webcpp_read_entire_file(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        // In a real library, you might have a WebCpp-specific error logging mechanism
        fprintf(stderr, "WebCpp Error: Could not open file %s\n", file_path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "WebCpp Error: Could not allocate memory to read file %s\n", file_path);
        fclose(file);
        return NULL;
    }

    if (fread(buffer, 1, length, file) != (size_t)length) {
        fprintf(stderr, "WebCpp Error: Could not read entire file %s\n", file_path);
        fclose(file);
        free(buffer);
        return NULL;
    }

    buffer[length] = '\0';
    fclose(file);
    return buffer;
}

// Helper function to escape HTML content for safe embedding in a JS string literal
// This is a very basic escaper and would need to be more robust for production.
static char* webcpp_escape_html_for_js_injection(const char* html_content) {
    if (!html_content) return NULL;
    
    size_t original_len = strlen(html_content);
    // Estimate new size. Worst case: every char needs escaping (e.g., \n -> \\n, ' -> \')
    // A more robust solution would calculate exact length or use dynamic resizing.
    size_t new_len_estimate = original_len * 2 + 1; 
    char* escaped_str = (char*)malloc(new_len_estimate);
    if (!escaped_str) return NULL;

    char* p_out = escaped_str;
    const char* p_in = html_content;
    while (*p_in) {
        switch (*p_in) {
            case '\'': *(p_out++) = '\\'; *(p_out++) = '\''; break;
            case '"':  *(p_out++) = '\\'; *(p_out++) = '"';  break; // If using double quotes in JS
            case '\\': *(p_out++) = '\\'; *(p_out++) = '\\'; break;
            case '\n': *(p_out++) = '\\'; *(p_out++) = 'n';  break;
            case '\r': *(p_out++) = '\\'; *(p_out++) = 'r';  break;
            // Add more escapes as needed (e.g., for backticks if using template literals)
            default:   *(p_out++) = *p_in; break;
        }
        if (p_out >= escaped_str + new_len_estimate - 2) { // -2 for potential two-char escape + null terminator
            // Buffer too small, realloc or handle error
            // For simplicity, we'll assume initial estimate is enough here.
            // In a real scenario, this needs robust handling.
            fprintf(stderr, "WebCpp Warning: Escaped string buffer might be too small for HTML content.\n");
            // To prevent buffer overflow, we stop here. A real implementation might realloc.
            *p_out = '\0'; // Null-terminate what we have
            // Potentially free(escaped_str) and return NULL if this is considered a fatal error.
            // For now, we'll proceed with the potentially truncated (but safe) string.
            break; 
        }
        p_in++;
    }
    *p_out = '\0';
    return escaped_str;
}

bool webcpp_load_html_file(webcpp_context_t* context, const char* file_path) {
    if (!context || !file_path) {
        return false; // Invalid arguments
    }

    char* html_content = webcpp_read_entire_file(file_path);
    if (!html_content) {
        return false; // Failed to read file
    }

    // The paths assume that UI_ASSETS_OUTPUT_DIR
    // is served from the root as /ui_assets/ by your WebCpp environment or system shell.
    // Adjust this path if your serving mechanism is different.
    const char* theme_css_tag = "<link rel='stylesheet' type='text/css' href='/ui_assets/css/openace_theme.css'>";
    const char* acegui_css_tag = "<link rel='stylesheet' type='text/css' href='/ui_assets/css/acegui.css'>";
    const char* acegui_js_tag = "<script type='text/javascript' src='/ui_assets/js/acegui.js' defer></script>";

    // Find the end of the <head> tag to insert our tags.
    // A more robust solution would use a proper HTML parser, but string manipulation can work for well-formed simple HTML.
    char* head_end_tag_pos = strcasestr(html_content, "</head>");
    char* final_html_content;

    if (head_end_tag_pos) {
        size_t head_len = head_end_tag_pos - html_content;
        size_t total_len = strlen(html_content) + strlen(theme_css_tag) + strlen(acegui_css_tag) + strlen(acegui_js_tag) + 1;
        final_html_content = (char*)malloc(total_len);
        if (!final_html_content) {
            fprintf(stderr, "WebCpp Error: Failed to allocate memory for modified HTML content.\n");
            free(html_content);
            return false;
        }
        // Copy content before </head>
        strncpy(final_html_content, html_content, head_len);
        final_html_content[head_len] = '\0'; // Null-terminate
        // Append our tags
        strcat(final_html_content, theme_css_tag);
        strcat(final_html_content, acegui_css_tag);
        strcat(final_html_content, acegui_js_tag);
        // Append the rest of the original HTML (including </head> and <body> etc.)
        strcat(final_html_content, head_end_tag_pos);
    } else {
        // Fallback: if no </head> tag, prepend to the whole content. This is less ideal.
        fprintf(stderr, "WebCpp Warning: No </head> tag found in %s. Prepending framework assets.\n", file_path);
        size_t total_len = strlen(html_content) + strlen(theme_css_tag) + strlen(acegui_css_tag) + strlen(acegui_js_tag) + 1;
        final_html_content = (char*)malloc(total_len);
         if (!final_html_content) {
            fprintf(stderr, "WebCpp Error: Failed to allocate memory for modified HTML content (fallback).\n");
            free(html_content);
            return false;
        }
        strcpy(final_html_content, theme_css_tag);
        strcat(final_html_content, acegui_css_tag);
        strcat(final_html_content, acegui_js_tag);
        strcat(final_html_content, html_content);
    }
    free(html_content); // Free original content

    char* escaped_html = webcpp_escape_html_for_js_injection(final_html_content);
    free(final_html_content); // Free the combined HTML string

    if (!escaped_html) {
        fprintf(stderr, "WebCpp Error: Failed to escape HTML content for JS injection.\n");
        return false;
    }

    // Construct the JavaScript command to replace the document's content.
    // Using document.documentElement.innerHTML is a common way.
    // Max command length: "document.documentElement.innerHTML = '';" + content + safety margin
    size_t js_command_len = strlen("document.documentElement.innerHTML = '';") + strlen(escaped_html) + 50;
    char* js_command = (char*)malloc(js_command_len);
    
    if (!js_command) {
        fprintf(stderr, "WebCpp Error: Failed to allocate memory for JS command.\n");
        free(escaped_html);
        return false;
    }

    sprintf(js_command, "document.documentElement.innerHTML = '%s';", escaped_html);
    
    free(escaped_html); // Free escaped content

    bool success = webcpp_eval_js(context, js_command);
    free(js_command); // Free the JS command string

    if (!success) {
        fprintf(stderr, "WebCpp Error: webcpp_eval_js failed to load HTML content from %s.\n", file_path);
    }
    return success;
}

// Basic Base64 encoding (simplified, no line breaks)
static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char* webcpp_base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {
    *output_length = 4 * ((input_length + 2) / 3);
    char *encoded_data = (char*)malloc(*output_length + 1); // +1 for null terminator
    if (encoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = base64_chars[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = base64_chars[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = base64_chars[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = base64_chars[(triple >> 0 * 6) & 0x3F];
    }

    // Pad with '='
    size_t mod_table[] = {0, 2, 1};
    for (size_t i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    encoded_data[*output_length] = '\0';
    return encoded_data;
}

// Basic Base64 decoding
static unsigned char* webcpp_base64_decode(const char *data, size_t input_length, size_t *output_length) {
    if (input_length % 4 != 0) return NULL; // Invalid Base64 input

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = (unsigned char*)malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : strchr(base64_chars, data[i++]) - base64_chars;
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : strchr(base64_chars, data[i++]) - base64_chars;
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : strchr(base64_chars, data[i++]) - base64_chars;
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : strchr(base64_chars, data[i++]) - base64_chars;

        uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }
    return decoded_data;
}

bool webcpp_save_image_from_data_url(webcpp_context_t* context, const char* full_file_path, const char* base64_data_url) {
    if (!context || !full_file_path || !base64_data_url) return false;

    const char* base64_marker = ";base64,";
    const char* base64_content_start = strstr(base64_data_url, base64_marker);

    if (!base64_content_start) {
        fprintf(stderr, "WebCpp Error: Invalid data URL format. Missing ';base64,'.\n");
        return false;
    }
    base64_content_start += strlen(base64_marker); // Move pointer past the marker

    size_t decoded_length;
    unsigned char* image_bytes = webcpp_base64_decode(base64_content_start, strlen(base64_content_start), &decoded_length);

    if (!image_bytes) {
        fprintf(stderr, "WebCpp Error: Failed to decode Base64 image data for %s.\n", full_file_path);
        return false;
    }

    FILE* outfile = fopen(full_file_path, "wb");
    if (!outfile) {
        fprintf(stderr, "WebCpp Error: Could not open file %s for writing.\n", full_file_path);
        free(image_bytes);
        return false;
    }

    size_t written = fwrite(image_bytes, 1, decoded_length, outfile);
    fclose(outfile);
    free(image_bytes);

    if (written != decoded_length) {
        fprintf(stderr, "WebCpp Error: Failed to write all image bytes to %s.\n", full_file_path);
        return false;
    }
    return true;
}

bool webcpp_list_directory_items(webcpp_context_t* context, const char* directory_path, char** result_json) {
    if (!context || !directory_path || !result_json) return false;

    DIR *dir;
    struct dirent *entry;
    char buffer[2048]; // Buffer for constructing JSON string
    strcpy(buffer, "[");
    bool first_item = true;

    if ((dir = opendir(directory_path)) == NULL) {
        perror("opendir() error");
        *result_json = strdup("[]"); // Return empty JSON array on error
        return false;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // Skip . and ..
        }

        if (!first_item) {
            strcat(buffer, ",");
        }
        first_item = false;

        char item_path[1024];
        snprintf(item_path, sizeof(item_path), "%s/%s", directory_path, entry->d_name);

        // Basic escaping for JSON string values (name and path)
        // A robust solution would handle all special characters.
        char escaped_name[256];
        char escaped_path[1024];
        // Simple escape: just an example, not comprehensive
        strncpy(escaped_name, entry->d_name, sizeof(escaped_name)-1); escaped_name[sizeof(escaped_name)-1] = '\0';
        strncpy(escaped_path, item_path, sizeof(escaped_path)-1); escaped_path[sizeof(escaped_path)-1] = '\0';
        // TODO: Implement proper JSON string escaping for name and path

        char item_json[1500];
        snprintf(item_json, sizeof(item_json),
                 "{\"name\":\"%s\",\"type\":\"%s\",\"path\":\"%s\"}",
                 escaped_name,
                 (entry->d_type == DT_DIR) ? "dir" : "file",
                 escaped_path);
        strcat(buffer, item_json);
    }
    closedir(dir);
    strcat(buffer, "]");

    *result_json = strdup(buffer);
    if (!*result_json) return false; // strdup failed

    return true;
}

bool webcpp_get_image_data_url(webcpp_context_t* context, const char* image_path, char** data_url_string) {
    if (!context || !image_path || !data_url_string) return false;

    FILE* image_file = fopen(image_path, "rb");
    if (!image_file) {
        fprintf(stderr, "WebCpp Error: Could not open image file %s\n", image_path);
        return false;
    }

    fseek(image_file, 0, SEEK_END);
    long file_size = ftell(image_file);
    fseek(image_file, 0, SEEK_SET);

    unsigned char* image_data = (unsigned char*)malloc(file_size);
    if (!image_data) {
        fprintf(stderr, "WebCpp Error: Could not allocate memory for image data.\n");
        fclose(image_file);
        return false;
    }

    if (fread(image_data, 1, file_size, image_file) != (size_t)file_size) {
        fprintf(stderr, "WebCpp Error: Could not read image file %s\n", image_path);
        fclose(image_file);
        free(image_data);
        return false;
    }
    fclose(image_file);

    size_t base64_len;
    char* base64_data = webcpp_base64_encode(image_data, file_size, &base64_len);
    free(image_data);

    if (!base64_data) {
        fprintf(stderr, "WebCpp Error: Base64 encoding failed for %s\n", image_path);
        return false;
    }

    // Determine MIME type (very basic, based on extension)
    const char* mime_type = "image/jpeg"; // Default
    const char* ext = strrchr(image_path, '.');
    if (ext) {
        if (strcmp(ext, ".png") == 0) mime_type = "image/png";
        else if (strcmp(ext, ".gif") == 0) mime_type = "image/gif";
        // Add more types as needed
    }

    // Format: "data:[<mime_type>];base64,<data>"
    size_t data_url_len = strlen("data:") + strlen(mime_type) + strlen(";base64,") + base64_len + 1;
    *data_url_string = (char*)malloc(data_url_len);
    if (!*data_url_string) {
        fprintf(stderr, "WebCpp Error: Could not allocate memory for data URL string.\n");
        free(base64_data);
        return false;
    }

    sprintf(*data_url_string, "data:%s;base64,%s", mime_type, base64_data);
    free(base64_data);

    return true;
}

bool webcpp_call_js_function(webcpp_context_t* context, const char* function_name, const char* args_json, char** result_json) {
    if (!context || !function_name) return false;
    
    // TODO: Call JS function
    // This will involve:
    // 1. Parsing arguments
    // 2. Calling function
    // 3. Converting result
    
    return true;
}

void* webcpp_get_element(webcpp_context_t* context, const char* selector) {
    if (!context || !selector) return NULL;
    
    // TODO: Get DOM element
    // This will involve:
    // 1. Parsing selector
    // 2. Finding element
    // 3. Creating wrapper
    
    return NULL;
}

bool webcpp_set_property(webcpp_context_t* context, void* element, const char* property, const char* value) {
    if (!context || !element || !property) return false;
    
    // TODO: Set element property
    // This will involve:
    // 1. Validating property
    // 2. Setting value
    // 3. Updating DOM
    
    return true;
}

bool webcpp_get_property(webcpp_context_t* context, void* element, const char* property, char** value) {
    if (!context || !element || !property || !value) return false;
    
    // TODO: Get element property
    // This will involve:
    // 1. Validating property
    // 2. Getting value
    // 3. Converting to string
    
    return true;
}

void webcpp_destroy_context(webcpp_context_t* context) {
    if (!context) return;
    
    // TODO: Cleanup context
    // This will involve:
    // 1. Destroying JS context
    // 2. Cleaning up DOM
    // 3. Releasing renderer
    if (context->js_context) {
        webcpp_v8_engine_context_t* v8_ctx = (webcpp_v8_engine_context_t*)context->js_context;
        // Important: Need to manage cleanup of V8CallbackData structs created in webcpp_register_function
        // This is non-trivial and often involves tracking them or using V8's weak persistent handles.
        // For this example, that cleanup is omitted for brevity.

        v8_ctx->global_object.Reset();
        v8_ctx->context.Reset();
        // Do NOT dispose the isolate here if it's shared or managed globally.
        // If each context has its own isolate, then:
        // v8_ctx->isolate->Dispose(); 
        // delete v8_ctx->isolate; // If isolate was created with new
        free(v8_ctx);
    }
    free(context);
}

void webcpp_shutdown(void) {
    if (!g_initialized) return;
    
    if (g_config.enable_gpu) gpu_cleanup();
    if (g_config.enable_threading) threading_cleanup();
    
    if (strcmp(g_config.js_engine, "v8") == 0) {
        // Note: Isolate disposal should happen after all contexts using it are gone.
        // If isolates are per-context, they are disposed in webcpp_destroy_context.
        // If there's a single global isolate, it might be disposed here, or not at all if V8 manages it.
        v8::V8::Dispose();
        // v8::V8::ShutdownPlatform(); // Deprecated, use DisposePlatform
        if (g_v8_platform) {
            v8::V8::DisposePlatform(); // Replaces ShutdownPlatform
            delete g_v8_platform; // If platform was created with new DefaultPlatform().release()
            g_v8_platform = nullptr;
        }
        printf("WebCpp: V8 shutdown.\n");
    }
    g_initialized = false;
}

// TabBar and Tab widget creation functions
void* create_tabbar_widget_js(webcpp_context_t* ctx, const char* id) {
    if (!ctx || !id) return NULL;
    
    // Create TabBar widget using the C widget system
    widget_t* tabbar = widget_create_tabbar(id);
    if (tabbar) {
        printf("WebCpp: Created TabBar widget with id: %s\n", id);
    }
    return tabbar;
}

void* create_tab_widget_js(webcpp_context_t* ctx, const char* id, const char* title, void* content_widget) {
    if (!ctx || !id || !title) return NULL;
    
    // Create Tab widget using the C widget system
    widget_t* tab = widget_create_tab(id, title, (widget_t*)content_widget);
    if (tab) {
        printf("WebCpp: Created Tab widget with id: %s, title: %s\n", id, title);
    }
    return tab;
}

bool add_tab_to_tabbar_js(webcpp_context_t* ctx, void* tabbar_widget, void* tab_widget) {
    if (!ctx || !tabbar_widget || !tab_widget) return false;
    
    // Add tab to tabbar using the C widget system
    bool success = widget_add_child((widget_t*)tabbar_widget, (widget_t*)tab_widget);
    if (success) {
        printf("WebCpp: Added tab to TabBar successfully\n");
    }
    return success;
} 