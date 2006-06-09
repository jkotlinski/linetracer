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
