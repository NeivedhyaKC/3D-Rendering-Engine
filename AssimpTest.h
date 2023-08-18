#pragma once
#include "TestObject.h"

class AssimpTest : public TestObject<AssimpTest>
{
public:
    AssimpTest(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
	    float scale);
};

