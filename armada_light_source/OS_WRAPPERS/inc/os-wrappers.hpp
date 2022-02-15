#ifndef OS_WRAPPERS_HPP_
#define OS_WRAPPERS_HPP_

class CritialSection
{
public:
	~CritialSection();
	void lock();
	void unlock();
	bool isLocked();

private:
	bool m_isLocked = false;
};




#endif

