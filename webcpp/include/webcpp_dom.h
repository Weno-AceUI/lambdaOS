#ifndef WEBCPP_DOM_H
#define WEBCPP_DOM_H

#include <stdint.h>
#include <stdbool.h>

// DOM node types
typedef enum {
    DOM_NODE_ELEMENT,
    DOM_NODE_TEXT,
    DOM_NODE_COMMENT,
    DOM_NODE_DOCUMENT
} dom_node_type_t;

// DOM element
typedef struct {
    dom_node_type_t type;
    char* tag_name;
    char* id;
    char* class_name;
    void* attributes;        // Map of attributes
    void* style;            // Computed style
    void* children;         // Child nodes
    void* parent;           // Parent node
    void* next_sibling;     // Next sibling
    void* prev_sibling;     // Previous sibling
} dom_node_t;

// DOM document
typedef struct {
    dom_node_t* root;       // Root element
    void* head;             // Head element
    void* body;             // Body element
    void* style_sheets;     // Style sheets
} dom_document_t;

// Initialize DOM system
bool dom_init(void);

// Create new document
dom_document_t* dom_create_document(void);

// Create element
dom_node_t* dom_create_element(dom_document_t* doc, const char* tag_name);

// Create text node
dom_node_t* dom_create_text_node(dom_document_t* doc, const char* text);

// Append child
bool dom_append_child(dom_node_t* parent, dom_node_t* child);

// Remove child
bool dom_remove_child(dom_node_t* parent, dom_node_t* child);

// Get element by ID
dom_node_t* dom_get_element_by_id(dom_document_t* doc, const char* id);

// Get elements by class name
dom_node_t** dom_get_elements_by_class_name(dom_document_t* doc, const char* class_name, uint32_t* count);

// Get elements by tag name
dom_node_t** dom_get_elements_by_tag_name(dom_document_t* doc, const char* tag_name, uint32_t* count);

// Query selector
dom_node_t* dom_query_selector(dom_document_t* doc, const char* selector);

// Query selector all
dom_node_t** dom_query_selector_all(dom_document_t* doc, const char* selector, uint32_t* count);

// Set attribute
bool dom_set_attribute(dom_node_t* element, const char* name, const char* value);

// Get attribute
const char* dom_get_attribute(dom_node_t* element, const char* name);

// Remove attribute
bool dom_remove_attribute(dom_node_t* element, const char* name);

// Set style property
bool dom_set_style(dom_node_t* element, const char* property, const char* value);

// Get style property
const char* dom_get_style(dom_node_t* element, const char* property);

// Add event listener
bool dom_add_event_listener(dom_node_t* element, const char* event_name, void* callback);

// Remove event listener
bool dom_remove_event_listener(dom_node_t* element, const char* event_name, void* callback);

// Cleanup DOM document
void dom_destroy_document(dom_document_t* doc);

// Shutdown DOM system
void dom_shutdown(void);

#endif // WEBCPP_DOM_H 