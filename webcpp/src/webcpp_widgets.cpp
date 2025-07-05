#include "../include/webcpp.h"
#include "../../ui/include/aceui_widget.h"
#include <stdio.h>

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

// Function to register all widget functions with WebCpp
bool webcpp_register_widget_functions(webcpp_context_t* context) {
    if (!context) return false;
    
    // Register TabBar and Tab functions
    if (!webcpp_register_function(context, "WidgetAPI_createTabBar", (void*)create_tabbar_widget_js)) {
        fprintf(stderr, "WebCpp Error: Failed to register WidgetAPI_createTabBar\n");
        return false;
    }
    
    if (!webcpp_register_function(context, "WidgetAPI_createTab", (void*)create_tab_widget_js)) {
        fprintf(stderr, "WebCpp Error: Failed to register WidgetAPI_createTab\n");
        return false;
    }
    
    if (!webcpp_register_function(context, "WidgetAPI_addTabToTabBar", (void*)add_tab_to_tabbar_js)) {
        fprintf(stderr, "WebCpp Error: Failed to register WidgetAPI_addTabToTabBar\n");
        return false;
    }
    
    printf("WebCpp: Successfully registered all widget functions\n");
    return true;
} 