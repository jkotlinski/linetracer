#include "StdAfx.h"
#include "epswriter.h"

#include "LayerManager.h"
#include "PolyLineIterator.h"

#include <set>
#include <vector>
#include ".\epswriter.h"
#include <boost/shared_ptr.hpp>

using namespace std;

void WriteLines(CLineImage & a_line_image, CStdioFile & a_out);

CEpsWriter::CEpsWriter(void)
{
	//LOG("init epswriter\n");
}

CEpsWriter::~CEpsWriter(void)
{
}

CEpsWriter* CEpsWriter::Instance() {
    static CEpsWriter inst;
    return &inst;
}

void CEpsWriter::Write(const CString &FileName)
{
	CStdioFile out(FileName,CFile::modeCreate|CFile::typeText|CFile::modeWrite);

	CLayerManager *lm = CLayerManager::Instance();

	CLineImage *lineImage = dynamic_cast<CLineImage*> (lm->GetLastLayer()->GetSketchImage());

	ASSERT ( lineImage != NULL );
	int width=lineImage->GetWidth();
	int height=lineImage->GetHeight();

	out.WriteString("%!PS-Adobe-3.0 EPSF-3.0\n");
	out.WriteString("%%BoundingBox: ");
	CString str;
	str.Format("%i %i %i %i\n",0,0,width,height);
	out.WriteString(str);
	//use round pen
	out.WriteString("1 setlinecap\n");
	out.WriteString("1 setlinejoin\n");

	out.WriteString("1.0 setlinewidth\n");

	WriteLines(*lineImage, out);

	out.Close();
}

typedef multimap<pair<int,int>,CPolyLine*> coord_line_map_type;
typedef set<pair<int,int> > coord_set_type;

void AddPointToMap (
	const CSketchPoint & a_point,
	coord_line_map_type & a_map,
	coord_set_type & a_coords,
	CPolyLine * a_line
	) {
	int x = static_cast<int>(a_point.GetX()+0.5);
	int y = static_cast<int>(a_point.GetY()+0.5);
	pair<int,int> l_coord (x,y);
	a_coords.insert(l_coord);
	a_map.insert(coord_line_map_type::value_type(l_coord, a_line));
}

void WriteLines ( CLineImage & a_line_image, CStdioFile & a_out )
{
	set<CPolyLine*> l_handled_lines;
	coord_line_map_type l_coord_line_map;
	coord_set_type l_coords;
	CLineImage l_my_image;
	for ( unsigned int l_line_index=0; 
		l_line_index < a_line_image.Size(); 
		l_line_index++ ) {
		CPolyLine * l_line = a_line_image.GetLine(l_line_index);
		AddPointToMap( **(l_line->Begin()), l_coord_line_map, l_coords, l_line );
		AddPointToMap( **(l_line->End()-1), l_coord_line_map, l_coords, l_line );
	}

	typedef set<pair<CPolyLine*,CPolyLine*> > neighbor_set_type;
	neighbor_set_type l_neighbor_set;
	int l_curr_id = 1;

	for( coord_set_type::iterator l_coord_iter = l_coords.begin(); 
		l_coord_iter != l_coords.end();
		l_coord_iter++
		)
	{
		pair<int,int> l_point = *l_coord_iter;
		bool l_point_is_in_two_lines = ( 2 == l_coord_line_map.count(l_point) );
		if ( l_point_is_in_two_lines ) {
			coord_line_map_type::iterator l_clm_iter = l_coord_line_map.lower_bound(l_point);
			CPolyLine * l_line_1 = l_clm_iter->second;
			l_clm_iter++;
			CPolyLine * l_line_2 = l_clm_iter->second;

			TRACE("new line neighbors: %x %x\n", l_line_1, l_line_2);

			l_neighbor_set.insert ( neighbor_set_type::value_type(l_line_1, l_line_2) );
			l_neighbor_set.insert ( neighbor_set_type::value_type(l_line_2, l_line_1) );
		}
	}

	for( coord_set_type::iterator l_coord_iter = l_coords.begin(); 
		l_coord_iter != l_coords.end();
		l_coord_iter++
		)
	{
		pair<int,int> l_point = *l_coord_iter;
		bool l_point_is_in_two_lines = ( 2 == l_coord_line_map.count(l_point) );
		if ( false == l_point_is_in_two_lines ) {
			//ok... let's write out lines that start in point
			TRACE("examine point %i %i\n", l_point.first, l_point.second);
			coord_line_map_type::iterator l_clm_iter = l_coord_line_map.lower_bound ( l_point );
			coord_line_map_type::iterator l_upper = l_coord_line_map.upper_bound ( l_point );

			for ( ; l_clm_iter != l_upper; l_clm_iter++ ) {
				coord_line_map_type::value_type l_coord_line = *l_clm_iter;
				pair<int,int> l_current_coord = l_clm_iter->first;
				CPolyLine * l_line = l_clm_iter->second;
				if ( l_handled_lines.count(l_line) > 0 ) continue;
				bool l_status = false;
				bool l_first_go = true;

				for (;;) {
					TRACE("write line %x\n", l_line);

					l_handled_lines.insert(l_line);
					l_line->Trace();

					PolyLineIterator l_pli (l_line, CFPoint(l_current_coord.first, l_current_coord.second), l_status);
					assert ( l_status );
					CSketchPoint *l_first_point = l_pli.Next(l_status);
					assert ( l_status );
					CSketchPoint *l_second_point = l_pli.Next(l_status);
					assert ( l_status );

					if ( l_first_go ) {
						CString str;
						l_first_go = false;
						str.Format("%f %f moveto\n",
							l_first_point->GetX(),
							a_line_image.GetHeight() - l_first_point->GetY());
						a_out.WriteString(str);
					}
					{
						//write curve
						CString str;
						CFPoint p;

						p = l_first_point->GetControlPoint();
						str.Format("%f %f ",p.GetX(),
							a_line_image.GetHeight() - p.GetY());
						a_out.WriteString(str);

						p = l_second_point->GetControlPoint();
						str.Format("%f %f ",p.GetX(),a_line_image.GetHeight()-p.GetY());
						a_out.WriteString(str);

						//endpoint
						p = l_second_point->GetCoords();
						str.Format("%f %f curveto\n",p.GetX(),a_line_image.GetHeight()-p.GetY());
						a_out.WriteString(str);
					}

					bool l_found = false;
					neighbor_set_type::iterator l_neighbor_iter = l_neighbor_set.begin();
					TRACE("look for neighbors to %x\n", l_line);
					while ( l_neighbor_iter != l_neighbor_set.end() ) {
						if ( l_line == (*l_neighbor_iter).first ) {
							l_found = true;
							break;
						}
						l_neighbor_iter++;
					}
					if ( !l_found ) break;
					CPolyLine * l_neighbor_line = l_neighbor_iter->second;
					assert ( l_line != l_neighbor_line );
					TRACE("erase %x %x\n", l_line, l_neighbor_line);
					l_neighbor_set.erase(neighbor_set_type::value_type(l_line,l_neighbor_line));
					l_neighbor_set.erase(neighbor_set_type::value_type(l_neighbor_line,l_line));
					l_line = l_neighbor_line;
					l_current_coord.first = static_cast<int>(l_second_point->GetX()+0.5);
					l_current_coord.second = static_cast<int>(l_second_point->GetY()+0.5);
				}
				TRACE("end of line\n");
				a_out.WriteString("stroke\n");
			}
		}
	}

// handle circles

	for ( unsigned int l_line_index=0; 
		l_line_index < a_line_image.Size(); 
		l_line_index++ ) {
		
		CPolyLine * l_line = a_line_image.GetLine(l_line_index);

		if ( l_handled_lines.count(l_line) > 0 ) continue;

		CSketchPoint *l_first_point = l_line->GetHeadPoint();
		CSketchPoint *l_second_point = l_line->GetTailPoint();

		CString str;
		str.Format("%f %f moveto\n",
			l_first_point->GetX(),
			a_line_image.GetHeight() - l_first_point->GetY());
		a_out.WriteString(str);

		//write curve
		CFPoint p = l_first_point->GetControlPoint();
		str.Format("%f %f ",p.GetX(),
			a_line_image.GetHeight() - p.GetY());
		a_out.WriteString(str);

		p = l_second_point->GetControlPoint();
		str.Format("%f %f ",p.GetX(),a_line_image.GetHeight()-p.GetY());
		a_out.WriteString(str);

		//endpoint
		p = l_second_point->GetCoords();
		str.Format("%f %f curveto\n",p.GetX(),a_line_image.GetHeight()-p.GetY());
		a_out.WriteString(str);

		a_out.WriteString("stroke\n");
	}
}
