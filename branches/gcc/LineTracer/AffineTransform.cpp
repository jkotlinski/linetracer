#include "AffineTransform.h"

#include <wx/graphics.h>

AffineTransform::AffineTransform(void)
: m_translationX(0)
, m_translationY(0)
, m_scale(1)
{
}

void AffineTransform::TranslateBy(float a_x, float a_y)
{
	m_translationX += a_x;
	m_translationY += a_y;
}

void AffineTransform::ScaleBy(float a_scale)
{
	wxASSERT( a_scale > 0 );
	m_scale *= a_scale;
}

void AffineTransform::Invert(void)
{
	wxASSERT ( m_scale>0 );
	m_scale = 1/m_scale;
	m_translationX *= m_scale;
	m_translationY *= m_scale;
	m_translationX = -m_translationX;
	m_translationY = -m_translationY;
}

wxGraphicsMatrix* AffineTransform::CreateMatrix(void)
{
	wxGraphicsMatrix* matrix = new wxGraphicsMatrix();
	matrix->Set(
		m_scale, 0.0f, 
		0.0f, m_scale, 
		m_translationX, 
		m_translationY );
	return matrix;
}

//return true if image should be movable
bool AffineTransform::Validate(const wxSize& a_imageSize, const wxSize& a_viewSize)
{
    wxSize l_transformedImageSize ( a_imageSize );
	TransformSize ( l_transformedImageSize );
    
	wxRealPoint l_topLeft ( 0.0f, 0.0f );
	wxRealPoint l_bottomRight ( 
		a_imageSize.GetWidth(), 
		a_imageSize.GetHeight() );
	TransformPoint ( l_topLeft );
	TransformPoint ( l_bottomRight );
    
    bool l_isMovable = false;
    
    //validate width
    if( l_transformedImageSize.GetWidth() >= a_viewSize.GetWidth() ) 
    {
        l_isMovable = true;

        //image is shown larger than view: make sure there's no empty space around picture!
        if(l_topLeft.x > 0) {
			TranslateBy ( -l_topLeft.x, 0.0f );
        }
        if(l_bottomRight.x < a_viewSize.GetWidth() ) {
            TranslateBy ( a_viewSize.GetWidth() - l_bottomRight.x, 0.0f );
        }
    } 
    else 
    {
        //image is shown smaller than view: center image
        float l_imageCenter = (l_topLeft.x + l_bottomRight.x) / 2.0f;
        float l_viewCenter = a_viewSize.GetWidth() / 2.0f;
        TranslateBy ( l_viewCenter - l_imageCenter, 0.0f );
    }
    
    //validate height
    if( l_transformedImageSize.GetHeight() >= a_viewSize.GetHeight() ) 
    {
        l_isMovable = true;

        //image is shown larger than view: make sure there's no empty space around picture!
        if(l_topLeft.y > 0) {
			TranslateBy ( 0.0f, -l_topLeft.y );
        }
        if(l_bottomRight.y < a_viewSize.GetHeight() ) {
            TranslateBy ( 0.0f, a_viewSize.GetHeight() - l_bottomRight.y );
        }
    } 
    else 
    {
        //image is shown smaller than view: center image
        float l_imageCenter = (l_bottomRight.y + l_topLeft.y) / 2.0f;
        float l_viewCenter = a_viewSize.GetHeight() / 2.0f;
        TranslateBy ( 0.0f, l_viewCenter - l_imageCenter );
    }
    
	return l_isMovable;
}

void AffineTransform::TransformSize(wxSize& a_size) const
{
	a_size.SetWidth(a_size.GetWidth() * m_scale);
	a_size.SetHeight(a_size.GetHeight() * m_scale);
}

void AffineTransform::TransformPoint(wxRealPoint& a_point) const
{
	a_point.x *= m_scale;
	a_point.y *= m_scale;
	a_point.x += m_translationX;
	a_point.y += m_translationY;
}

float AffineTransform::GetTranslationX(void) const
{
	return m_translationX;
}

float AffineTransform::GetTranslationY(void) const
{
	return m_translationY;
}
