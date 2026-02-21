#pragma once

namespace Nullity
{
	class Time
	{
	public:
        float deltaTime = 0.0f;
        int fps;
        float msPerFrame;
        float lastFrame = 0.0f;
	};
}
