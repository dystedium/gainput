

#include <gainput/gainput.h>
#include "GainputInputDeltaState.h"

#if defined(GAINPUT_PLATFORM_WIN)
#include "GainputInputDeviceKeyboardWin.h"
#include "GainputInputDeviceMouseWin.h"
#elif defined(GAINPUT_PLATFORM_ANDROID)
#include "GainputInputDeviceTouchAndroid.h"
#include <android_native_app_glue.h>
#endif


namespace gainput
{

InputManager::InputManager(Allocator& allocator) :
		allocator_(allocator),
		devices_(allocator_),
		nextDeviceId_(0),
		listeners_(allocator_),
		deltaState_(allocator_.New<InputDeltaState>(allocator_))
{

}

InputManager::~InputManager()
{
	allocator_.Delete(deltaState_);
}

void
InputManager::Update()
{
	InputDeltaState* ds = listeners_.empty() ? 0 : deltaState_;

	for (DeviceMap::iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		it->second->Update(ds);
	}

	if (ds)
	{
		ds->NotifyListeners(listeners_);
		ds->Clear();
	}
}

unsigned
InputManager::GetDeviceCountByType(InputDevice::DeviceType type) const
{
	unsigned count = 0;
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (it->second->GetType() == type)
		{
			++count;
		}
	}
	return count;
}


#if defined(GAINPUT_PLATFORM_WIN)
void
InputManager::HandleMessage(const MSG& msg)
{
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (it->second->GetType() == InputDevice::DT_KEYBOARD)
		{
			InputDeviceKeyboard* keyboard = static_cast<InputDeviceKeyboard*>(it->second);
			InputDeviceKeyboardImpl* keyboardImpl = keyboard->GetPimpl();
			GAINPUT_ASSERT(keyboardImpl);
			keyboardImpl->HandleMessage(msg);
		}
		else if (it->second->GetType() == InputDevice::DT_MOUSE)
		{
			InputDeviceMouse* mouse = static_cast<InputDeviceMouse*>(it->second);
			InputDeviceMouseImpl* mouseImpl = mouse->GetPimpl();
			GAINPUT_ASSERT(mouseImpl);
			mouseImpl->HandleMessage(msg);
		}
	}
}
#endif

#if defined(GAINPUT_PLATFORM_ANDROID)
int32_t
InputManager::HandleInput(AInputEvent* event)
{
	int handled = 0;
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (it->second->GetType() == InputDevice::DT_TOUCH)
		{
			InputDeviceTouch* touch = static_cast<InputDeviceTouch*>(it->second);
			InputDeviceTouchImpl* touchImpl = touch->GetPimpl();
			GAINPUT_ASSERT(touchImpl);
			handled |= touchImpl->HandleInput(event);
		}
	}
	return handled;
}
#endif

}

