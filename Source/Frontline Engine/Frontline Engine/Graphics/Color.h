#pragma once
class Color
{
	float r, g, b;
public:
	Color(float nR, float nG, float nB) : r(nR), g(nG), b(nB) {}
	char to8Bit()
	{
		char lb_retval = 0;
		if (r < .33) {
			if (g < .33) {
				if (b < .33) {
					lb_retval = 0x00;
				}
				else if (b < .67) {
					lb_retval = 0x01;
				}
				else {
					lb_retval = 0x09;
				}
			}
			else if (g < .67) {
				if (b < .33) {
					lb_retval = 0x02;
				}
				else if (b < .67) {
					lb_retval = 0x03;
				}
				else {
					lb_retval = 0x03;
				}
			}
			else {
				if (b < .33) {
					lb_retval = 0x0A;
				}
				else if (b < .67) {
					lb_retval = 0x03;
				}
				else {
					lb_retval = 0x0B;
				}
			}
		}
		else if (r < .67) {
			if (g < .33) {
				if (b < .33) {
					lb_retval = 0x04;
				}
				else if (b < .67) {
					lb_retval = 0x05;
				}
				else {
					lb_retval = 0x05;
				}
			}
			else if (g < .67) {
				if (b < .33) {
					lb_retval = 0x06;
				}
				else if (b < .67) {
					lb_retval = 0x08;
				}
				else {
					lb_retval = 0x03;
				}
			}
			else {
				if (b < .33) {
					lb_retval = 0x0A;
				}
				else if (b < .67) {
					lb_retval = 0x0A;
				}
				else {
					lb_retval = 0x0B;
				}
			}
		}
		else {
			if (g < .33) {
				if (b < .33) {
					lb_retval = 0x0C;
				}
				else if (b < .67) {
					lb_retval = 0x0D;
				}
				else {
					lb_retval = 0x0D;
				}
			}
			else if (g < .67) {
				if (b < .33) {
					lb_retval = 0x06;
				}
				else if (b < .67) {
					lb_retval = 0x0D;
				}
				else {
					lb_retval = 0x0D;
				}
			}
			else {
				if (b < .33) {
					lb_retval = 0x0E;
				}
				else if (b < .67) {
					lb_retval = 0x0E;
				}
				else {
					lb_retval = 0x0F;
				}
			}
		}
		return lb_retval;
	}
	~Color() {}
};

