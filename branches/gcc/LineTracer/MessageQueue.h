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
