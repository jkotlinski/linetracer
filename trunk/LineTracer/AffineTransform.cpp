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
#include ".\affinetransform.h"

AffineTransform::AffineTransform(void)
: m_translationX(0)
, m_translationY(0)
, m_scale(1)
{
}

AffineTransform::~AffineTransform(void)
{
}

void AffineTransform::TranslateBy(float a_x, float a_y)
{
	m_translationX += a_x;
	m_translationY += a_y;
}

void AffineTransform::ScaleBy(float a_scale)
{
	ASSERT( a_scale > 0 );
	m_scale *= a_scale;
}

void AffineTransform::Invert(void)
{
	ASSERT ( m_scale>0 );
	m_scale = 1/m_scale;
	m_translationX *= m_scale;
	m_translationY *= m_scale;
	m_translationX = -m_translationX;
	m_translationY = -m_translationY;
}

Matrix *AffineTransform::GetMatrix(void)
{
	Matrix *l_matrix = new Matrix ( 
		m_scale, 0.0f, 
		0.0f, m_scale, 
		m_translationX, 
		m_translationY );
	return l_matrix;
}

//return true if image should be movable
bool AffineTransform::Validate(const CSize & a_imageSize, const CSize & a_viewSize)
{
    CSize l_transformedImageSize ( a_imageSize );
	TransformSize ( l_transformedImageSize );
    
	PointF l_topLeft ( 0.0f, 0.0f );
	PointF l_bottomRight ( 
		REAL( a_imageSize.cx ), 
		REAL( a_imageSize.cy ) );
	TransformPoint ( l_topLeft );
	TransformPoint ( l_bottomRight );
    
    bool l_isMovable = false;
    
    //validate width
    if( l_transformedImageSize.cx >= a_viewSize.cx ) 
    {
        l_isMovable = true;

        //image is shown larger than view: make sure there's no empty space around picture!
        if(l_topLeft.X > 0) {
			TranslateBy ( -l_topLeft.X, 0.0f );
        }
        if(l_bottomRight.X < a_viewSize.cx ) {
            TranslateBy ( a_viewSize.cx - l_bottomRight.X, 0.0f );
        }
    } 
    else 
    {
        //image is shown smaller than view: center image
        float l_imageCenter = (l_topLeft.X + l_bottomRight.X) / 2.0f;
        float l_viewCenter = a_viewSize.cx / 2.0f;
        TranslateBy ( l_viewCenter - l_imageCenter, 0.0f );
    }
    
    //validate height
    if( l_transformedImageSize.cy >= a_viewSize.cy ) 
    {
        l_isMovable = true;

        //image is shown larger than view: make sure there's no empty space around picture!
        if(l_topLeft.Y > 0) {
			TranslateBy ( 0.0f, -l_topLeft.Y );
        }
        if(l_bottomRight.Y < a_viewSize.cy ) {
            TranslateBy ( 0.0f, a_viewSize.cy - l_bottomRight.Y );
        }
    } 
    else 
    {
        //image is shown smaller than view: center image
        float l_imageCenter = (l_bottomRight.Y + l_topLeft.Y) / 2.0f;
        float l_viewCenter = a_viewSize.cy / 2.0f;
        TranslateBy ( 0.0f, l_viewCenter - l_imageCenter );
    }
    
	return l_isMovable;
}

void AffineTransform::TransformSize(CSize & a_size) const
{
	a_size.cx = static_cast<LONG>(a_size.cx * m_scale);
	a_size.cy = static_cast<LONG>(a_size.cy * m_scale);
}

void AffineTransform::TransformPoint(PointF & a_point) const
{
	a_point.X *= m_scale;
	a_point.Y *= m_scale;
	a_point.X += m_translationX;
	a_point.Y += m_translationY;
}

float AffineTransform::GetTranslationX(void) const
{
	return m_translationX;
}

float AffineTransform::GetTranslationY(void) const
{
	return m_translationY;
}
