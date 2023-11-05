#pragma once

class NiColorA {
public:
	NiColorA(float fr, float fg, float fb, float fa) : r(fr), g(fg), b(fb), a(fa) {}
	NiColorA() /* advc: */ : r(0), g(0), b(0), a(0) {}

    float r, g, b, a;
};
