#pragma once

#include <cstdint>

namespace Lumina
{
	class UUID
	{
	public:
		[[nodiscard]] static uint64_t Generate();
	};
}
