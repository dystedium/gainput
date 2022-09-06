
#include <gainput/gainput.h>

#ifdef GAINPUT_PLATFORM_MAC

#import <AppKit/AppKit.h>

namespace gainput
{

static_assert(sizeof(GainputNSUInteger) == sizeof(NSUInteger));

bool MacIsApplicationKey()
{
    return [[NSApplication sharedApplication] keyWindow ] != nil;
}

GainputNSUInteger GetNSEventType(void *event)
{
	return static_cast<NSEvent*>(event).type;
}

}

#endif
