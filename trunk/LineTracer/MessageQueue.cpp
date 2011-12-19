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
#include ".\messagequeue.h"

namespace Messaging {

	MessageQueue::MessageQueue(void)
	{
	}

	MessageQueue::~MessageQueue(void)
	{
	}

	//------------

	void MessageQueue::PostMsg ( Message & a_message ) {
		//TRACE("PostMsg");
		CSingleLock l_lock(&m_critical_section);
		l_lock.Lock();

		m_queue.push(a_message);
		m_got_message_event.SetEvent();
	}

	Messaging::Message MessageQueue::GetMsg ( ) {
		//TRACE("GetMsg");
		CSingleLock l_lock(&m_critical_section);
		l_lock.Lock();

		if ( m_queue.empty() ) {
			//TRACE("waiting in GetMsg()...");
			m_got_message_event.ResetEvent();

			l_lock.Unlock();

			::WaitForSingleObject ( 
				m_got_message_event.m_hObject, 
				INFINITE );
			//TRACE("GetMsg() restarted");
		}
		Messaging::Message l_message = m_queue.front();
		m_queue.pop();
		
		return l_message;
	}

	bool MessageQueue::IsEmpty ( ) {
		return m_queue.empty();
	}

	void MessageQueue::Clear ( ) {
		CSingleLock l_lock(&m_critical_section);
		l_lock.Lock();
		while ( !m_queue.empty() ) {
			Message l_message = m_queue.front();
			delete ( l_message.GetWParam() );
			m_queue.pop();
		}
	}
}
