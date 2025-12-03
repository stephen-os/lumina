#pragma once

#include <functional>

#include <uiohook.h>

#include "Base.h"
#include "Event.h"

namespace Lumina::Core
{
	struct CaptureSpecifications
	{	
		bool Keyboard = true;			// Capture keyboard input
		bool Mouse = true;				// Capture mouse input
		bool MousePosition = true;		// Capture mouse position
	};

	class Capture
	{
	public:
		Capture(const CaptureSpecifications& specifications = CaptureSpecifications());
		~Capture();

		void Start();
		void Stop();

		bool IsCapturing() const { return m_Capturing; }

		void ApplySpecifications(const CaptureSpecifications& specifications);
		CaptureSpecifications& GetSpecifications() { return m_Specifications; }

		void SetEventCallback(const std::function<void(Event&)>& callback) { m_CaptureEventCallback = callback; }

	private:
		friend void dispatch_proc(uiohook_event* const event, void* user_data); 

		std::function<void(class Event&)> m_CaptureEventCallback;
		CaptureSpecifications m_Specifications;
		bool m_Capturing = false;

		struct PlatformData;
		Scope<PlatformData> m_PlatformData;
	};
}