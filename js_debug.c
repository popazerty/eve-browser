#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <JavaScriptCore/JavaScript.h>

int JSValueToString(JSContextRef ctx, JSValueRef argument, JSValueRef *exception, char ** string)
{
    JSType type = JSValueGetType(ctx, argument);    
    if(type != kJSTypeString)
        return -1;

    JSStringRef s;
    s = JSValueToStringCopy(ctx, argument, exception);

    unsigned int len = JSStringGetLength(s);
    *string = (char * )malloc(len+1);
    JSStringGetUTF8CString(s, *string, len+1);

    JSStringRelease(s);
    return strlen(*string);
}


void printJSStringRef(JSStringRef string)
{
    unsigned int len = JSStringGetLength(string);
    char * buffer = (char * )malloc(len+1);
    JSStringGetUTF8CString(string, buffer, len+1);
    printf("%s\n", buffer);
    free(buffer);
}

void printJSObjectRef(JSContextRef ctx, JSObjectRef argument)
{

    JSPropertyNameArrayRef names = JSObjectCopyPropertyNames(ctx, argument);
    unsigned int propertySize = JSPropertyNameArrayGetCount(names);
    printf("%s > (propertySize=%d)\n", __func__, propertySize);
    for(unsigned int i = 0; i < propertySize; i++)
    {
        JSStringRef name = JSPropertyNameArrayGetNameAtIndex(names, i);
        printf("[%2d] ", i); printJSStringRef(name); printf("\n");
    }
    printf("%s < \n", __func__);
}

void printJSValueRef(JSContextRef ctx, JSValueRef argument, JSValueRef *exception)
{
    printf("%s > \n", __func__);
    JSType type = JSValueGetType(ctx, argument);
    switch(type)
    {
    case kJSTypeUndefined: printf("kJSTypeUndefined\n"); break;
    case kJSTypeNull: printf("kJSTypeNull\n"); break;
    case kJSTypeBoolean: 
        printf("kJSTypeBoolean\n"); 
        printf("%s\n", JSValueToBoolean(ctx, argument)?"True":"False"); 
        break;
    case kJSTypeNumber: 
        printf("kJSTypeNumber\n"); 
        printf("%f\n", JSValueToNumber(ctx, argument, exception)); 
        break;

    case kJSTypeString: 
        printf("kJSTypeString\n"); 
        JSStringRef s;
        s = JSValueToStringCopy(ctx, argument, exception);
        printJSStringRef(s); 
        JSStringRelease(s);
        break;

    case kJSTypeObject: 
        printf("kJSTypeObject\n"); 
        JSObjectRef o = JSValueToObject(ctx, argument, exception);
        printJSObjectRef(ctx, o); 
        break;

    }
    printf("%s < \n", __func__);
}
