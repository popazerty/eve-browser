#ifndef JS_DEBUG_H_
#define JS_DEBUG_H_

void printJSStringRef(JSStringRef string);

void printJSObjectRef(JSContextRef ctx, JSObjectRef argument);

void printJSValueRef(JSContextRef ctx, JSValueRef argument, JSValueRef *exception);

int JSValueToString(JSContextRef ctx, JSValueRef argument, JSValueRef *exception, char ** string);

#endif
