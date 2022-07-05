#pragma once

#include <cmath>

namespace graphics {
	
	typedef struct {
		unsigned char r, g, b, a;
	} rgbaColor;

	typedef struct {
		double h; //Hue defines the color itself, for example, 
				  //red in distinction to blue or yellow.
				  //The values for the hue axis run from 0–360°
		double s; //Saturation indicates the degree to which 
				  //the hue differs from a neutral gray. The values run from
				  //0 %, which is no color saturation, to 100 %
		double l; //Luminance indicates the level of illumination. The values run as percentages; 0% appears black
				  //(no light) while 100 % is full illumination, which washes out the color(it appears white).
		double a;
	} hslaColor;

	static hslaColor RGB_TO_HSL(rgbaColor rgb) {
		double r, g, b;
		r = rgb.r / 255.;
		g = rgb.g / 255.;
		b = rgb.b / 255.;


		//formula from https://en.wikipedia.org/wiki/HSL_and_HSV
		double min, max;
		min = (r < g) ? r : g;
		min = (min < b) ? min : b;

		max = (r > g) ? r : g;
		max = (max > b) ? max : b;

		double chroma;
		chroma = max - min;

		hslaColor hsl;
		// Check for black, white, and shades of gray, where H is undefined,
		// S is always 0, and L controls the shade of gray.  Mathematically, this
		// is true when chroma == 0, but we'll use a near-zero value to account
		// for floating point errors.
		if (chroma < 0.001 || max < 0.001) {
			hsl.h = hsl.s = 0;
			return hsl;
		}

		//Compute H
		if (max == r) { hsl.h = 0 + (g - b) / chroma; }
		else if (max == g) { hsl.h = 2 + (b - r) / chroma; }
		else if (max == b) { hsl.h = 4 + (r - g) / chroma; }
		hsl.h *= 60;
		if (hsl.h < 0) { hsl.h += 360; }

		//Computr L
		hsl.l = (max + min) * 0.5;
		//Compute S
		hsl.s = chroma / (1 - fabs(2 * hsl.l - 1));
		//Compute A
		hsl.a = rgb.a / 255.;

		return hsl;
	}

	static rgbaColor HSL_TO_RBG(hslaColor hsl){
		rgbaColor rgb;

		if (hsl.s <= 0.001) {
			rgb.r = rgb.g = rgb.b = round(hsl.l * 255);
		}
		else {
			double c = hsl.s * (1 - fabs((2 * hsl.l) - 1));
			double h = hsl.h / 60;
			double x = c * (1 - fabs(fmod(h, 2) - 1));

			double r, g, b;
			//Compute R`, G`, B`
			if (h <= 1) { r = c; g = x; b = 0; }
			else if (h <= 2) { r = x; g = c; b = 0; }
			else if (h <= 3) { r = 0; g = c; b = x; }
			else if (h <= 4) { r = 0; g = x; b = c; }
			else if (h <= 5) { r = x; g = 0; b = c; }
			else			 { r = c; g = 0; b = x; }

			double m = hsl.l - (c * 0.5);
			rgb.r = round((r + m) * 255);
			rgb.g = round((g + m) * 255);
			rgb.b = round((b + m) * 255);
		}

		rgb.a = round(hsl.a * 255);

		return rgb;
	}

}
