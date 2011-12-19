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

#include "./Message.h"
#include <queue>
#include <afxmt.h>

namespace Messaging {

class MessageQueue
{
private:
	std::queue<Message> m_queue;
	CEvent m_got_message_event;
	CCriticalSection m_critical_section;

public:
	MessageQueue(void);
	~MessageQueue(void);

	void PostMsg ( Message & a_message );

	Messaging::Message GetMsg ( );
	bool IsEmpty ( );
	void Clear ( );
};

}
