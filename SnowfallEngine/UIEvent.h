#pragma once
#include <functional>

template<class T>
using UIEventHandler = void(*)(T&);

template<class T>
class UIEvent
{
public:
	UIEvent() {}
	void Fire(T& args) const;
	void Subscribe(UIEventHandler<T> handler);
	void Unsubscribe(UIEventHandler<T> handler);
	int GetSubscriberCount();
private:
	std::vector<UIEventHandler<T>> m_handlers;
};

template<class T>
inline void UIEvent<T>::Fire(T & args) const
{
	for (UIEventHandler<T> t : m_handlers)
		t(args);
}

template<class T>
inline void UIEvent<T>::Subscribe(UIEventHandler<T> handler)
{
	m_handlers.push_back(handler);
}

template<class T>
inline void UIEvent<T>::Unsubscribe(UIEventHandler<T> handler)
{
	m_handlers.erase(std::find(m_handlers.begin(), m_handlers.end(), handler));
}

template<class T>
inline int UIEvent<T>::GetSubscriberCount()
{
	return m_handlers.size();
}
