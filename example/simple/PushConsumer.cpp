// PullConsumer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <set>

#include <DefaultMQPushConsumer.h>
#include <MessageListener.h>
#include <Message.h>
#include <MessageExt.h>
#include <MessageQueue.h>
#include <PullResult.h>
#include <MQClientException.h>

long long str2ll( const char *str )
{
#ifdef WIN32
	return _atoi64(str);
#else
	return atoll(str);
#endif

}

class MsgListener : public MessageListenerConcurrently
{
public:
	MsgListener()
	{
		consumeTimes = 0;
	}

	~MsgListener()
	{

	}

	ConsumeConcurrentlyStatus consumeMessage(std::list<MessageExt*>& msgs,
											ConsumeConcurrentlyContext& context)
	{
		MessageExt* msg = msgs.front();
		long long offset = msg->getQueueOffset();
		std::string maxOffset = msg->getProperty(Message::PROPERTY_MAX_OFFSET);

		long long diff = str2ll(maxOffset.c_str()) - offset;

		if (diff > 100000)
		{
			// TODO ��Ϣ�ѻ���������⴦��
			// return ConsumeConcurrentlyStatus.CONSUME_SUCCESS;
		}

		std::list<MessageExt*>::iterator it = msgs.begin();

		for (;it != msgs.end();it++)
		{
			MessageExt* me = *it;
			std::string str;
			str.assign(me->getBody(),me->getBodyLen());
			std::cout<<str<<std::endl;
		}

		consumeTimes++;
		if ((consumeTimes % 2) == 0)
		{
			return RECONSUME_LATER;
		}
		else if ((consumeTimes % 3) == 0)
		{
			context.delayLevelWhenNextConsume = 5;
			return RECONSUME_LATER;
		}

		return CONSUME_SUCCESS;
	}

	int consumeTimes;
};

int main(int argc, char* argv[])
{
	if (argc<2)
	{
		printf("Usage:%s ip:port\n",argv[0]);
		return 0;
	}

	DefaultMQPushConsumer consumer("please_rename_unique_group_name");
	//consumer.setNamesrvAddr(argv[1]);

	/**
	* ����ָ��topic��������Ϣ
	*/
	// consumer.subscribe("TopicTest", "*");

	/**
	* ����ָ��topic��tags�ֱ����TagA��TagC��TagD
	*/
	consumer.subscribe("TopicTest", "TagA || TagC || TagD");

	MsgListener* listener = new MsgListener();

	consumer.registerMessageListener(listener);
	consumer.start();
	
	delete listener;

	return 0;
}