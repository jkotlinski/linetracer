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
#include ".\message.h"

#include <assert.h>

namespace Messaging {

	Message::Message(int a_message, unsigned int * a_w_param, int a_l_param)
	: m_message_id ( a_message )
	, m_w_param ( a_w_param )
	, m_l_param ( a_l_param )
	{
		assert ( m_message_id > MSG_INVALID_LOWER );
		assert ( m_message_id < MSG_INVALID_UPPER );
	}

	Message::~Message(void)
	{
	}

	int Message::GetId() {
		return m_message_id;
	}

	unsigned int * Message::GetWParam() {
		return m_w_param;
	}

	int Message::GetLParam() {
		return m_l_param;
	}

}
