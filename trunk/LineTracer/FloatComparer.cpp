/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "StdAfx.h"
#include "floatcomparer.h"

bool CFloatComparer::FloatsDiffer(double a_val1, double a_val2) {
	static const double K_LARGEST_DIFF_FOR_EQUALITY = 0.1;
	double l_diff = Abs ( a_val1 - a_val2 );
	if ( l_diff > K_LARGEST_DIFF_FOR_EQUALITY ) {
		return true;
	}
	return false;
}

double CFloatComparer::Abs(double a_val)
{
	return ( a_val < 0 ) ? -a_val : a_val;
}
