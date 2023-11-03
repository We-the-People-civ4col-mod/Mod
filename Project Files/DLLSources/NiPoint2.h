#pragma once

class NiPoint2
{
public:
	NiPoint2() /* advc: */ : x(-1), y(-1) {}
	NiPoint2(float fx, float fy) : x(fx),y(fy) {}

	float x, y;
};
