#pragma once

class CFPoint
{
public:
	explicit CFPoint(double ix=0,double iy=0);
	virtual ~CFPoint(void);
	const CFPoint operator-(void) const;
	const CFPoint Unit(void) const;
	const CFPoint operator+=(const CFPoint &point);
	const double Distance(const CFPoint &point) const;
	const double GetX(void) const;
	const double GetY(void) const;
	void SetX(const double a_val);
	void SetY(const double a_val);
	const unsigned int HashValue() const;
private:
	double m_x;
	double m_y;
};

bool operator<(const CFPoint& p1,const CFPoint& p2);
const CFPoint operator*(const CFPoint& point, const double val);
double operator*(const CFPoint& point1, const CFPoint& point2);
const CFPoint operator-(const CFPoint& original, const CFPoint& subtractor);
const CFPoint operator+(const CFPoint& point1, const CFPoint& point2);
const bool operator==(const CFPoint& a_p1, const CFPoint& a_p2);
const bool operator!=(const CFPoint& a_p1, const CFPoint& a_p2);

inline const double CFPoint::GetX(void) const {
	return m_x;
}
inline const double CFPoint::GetY(void) const {
	return m_y;
}
