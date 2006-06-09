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
