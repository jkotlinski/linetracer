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

#pragma once

class CLogger
{
public:
	static CLogger* Instance(void);
	static void Inactivate(void);
	static void Activate(void);
	void Log(LPCSTR pszFormat, ...);
private:
	//lint -e{1704} singleton
	CLogger(void);
	~CLogger(void);
	FILE* m_outFile;
	bool m_active;
};
