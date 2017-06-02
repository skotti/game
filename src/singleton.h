#ifndef SINGLETON_H
#define SINGLETON_H


template <typename T>
class Singleton {
private:
	static T* m_self;
	
protected:
	Singleton() {}
	virtual ~Singleton() {}

public:
	static T* instance() {
		if (!m_self) {
			m_self = new T();
		}
		return m_self;
	}
	
	static void destroy() {
		if (m_self) {
			delete m_self;
			m_self = nullptr;
		}
	}
	
};

template <typename T>
T* Singleton<T>::m_self = nullptr;

#endif