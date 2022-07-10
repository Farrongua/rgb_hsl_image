#pragma once

namespace graphics {
	class HSLAPixel {
	public:
		double h, s, l, a;

		HSLAPixel() : h(255), s(0), l(0), a(0) {};

		HSLAPixel(double h, double s, double l, double a)
			: h(h), s(s), l(l), a(a) {};
	};
}