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
#include ".\distancemapbuilder.h"

DistanceMapBuilder::DistanceMapBuilder(void)
: m_distanceMap(NULL)
{
}

DistanceMapBuilder::~DistanceMapBuilder(void)
{
	delete m_distanceMap;
}

DistanceMapBuilder* DistanceMapBuilder::Instance(void)
{
	static DistanceMapBuilder inst;
	return &inst;
}

void DistanceMapBuilder::CalcDistanceMap(const CRawImage<bool>* img)
{
	delete m_distanceMap;
	m_distanceMap = new CRawImage<int>(img->GetWidth(),img->GetHeight());
	m_distanceMap->Clear();

	//pass 1... from nw to se
	{
		for(int y=1; y<img->GetHeight()-1; y++) {
			for(int x=1; x<img->GetWidth()-1; x++) {
				if(!img->GetPixel(x,y)) {
					int l_val = m_distanceMap->GetPixel(x-1,y)+3;
					l_val = min(l_val, m_distanceMap->GetPixel(x,y-1)+3);
					l_val = min(l_val, m_distanceMap->GetPixel(x-1,y-1)+4);
					l_val = min(l_val, m_distanceMap->GetPixel(x+1,y-1)+4);
					m_distanceMap->SetPixel(x, y, l_val);
				}
			}
		}
	}
	//pass 2... from se to nw
	{
		for(int y=img->GetHeight()-2; y>0; y--) {
			for(int x=img->GetWidth()-2; x>0; x--) {
				if(!img->GetPixel(x,y)) {
					int l_val = m_distanceMap->GetPixel(x,y);
					l_val = min(l_val,m_distanceMap->GetPixel(x+1,y)+3);
					l_val = min(l_val,m_distanceMap->GetPixel(x,y+1)+3);
					l_val = min(l_val,m_distanceMap->GetPixel(x+1,y+1)+4);
					l_val = min(l_val, m_distanceMap->GetPixel(x-1,y+1)+4);

					m_distanceMap->SetPixel(x,y,l_val);
				}
			}
		}
	}
}

const CRawImage<int>* DistanceMapBuilder::GetDistanceMap() const {
	return m_distanceMap;
}
