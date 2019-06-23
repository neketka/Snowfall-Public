#pragma once
#include <functional>

template<class T>
using UIEventHandler = std::function<void(T&)>;

template<class T1, class T2>
using BindableHandler = void(T1::*)(T2&);

class EventArgs {};

template<class T>
class UIEvent
{
public:
	UIEvent() {}
	void Fire(T& args) const
	{
		for (UIEventHandler<T> t : m_handlers)
			t(args);
	}

	template<class T1>
	void Subscribe(BindableHandler<T1, T> handler, T1 *_this)
	{
		m_handlers.push_back(std::bind(handler, _this, std::placeholders::_1));
	}

	int GetSubscriberCount() 
	{
		return m_handlers.size();
	}
private:
	std::vector<UIEventHandler<T>> m_handlers;
};

