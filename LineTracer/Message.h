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

namespace Messaging {

	enum message_ids {
		MSG_INVALID_LOWER, 

		//from GUI to image processor
		MSG_INVALIDATE_FROM_LAYER,
		MSG_PROJECT_SETTINGS,
		MSG_STOP_PROCESSING,
		MSG_START_PROCESSING,
		MSG_KILL_THREAD,

		//------
		MSG_INVALID_UPPER
	};

	class Message
	{
	public:
		Message(int a_message_id, unsigned int * a_w_param, int a_l_param);
		~Message(void);

		int GetId();
		unsigned int * GetWParam();
		int GetLParam();
	private:
		int m_message_id;
		unsigned int * m_w_param;
		int m_l_param;
	};

}
