#pragma once

template<class T>
class RefPtr
{
public:
	RefPtr() 
		: _ptr(nullptr)
	{
	}

	explicit RefPtr(T *ptr) 
		: _ptr(ptr)
	{
		_ptr->add_ref();
	}

	~RefPtr()
	{
		if (_ptr)
			_ptr->release();
	}

	void attach(T *ptr)
	{
		if (_ptr)
			_ptr->release();
		_ptr = ptr;

	}

	void operator=(T *ptr)
	{
		if (_ptr)
			_ptr->release();
		if (ptr)
			ptr->add_ref();
		_ptr = ptr;
	}

	T *get()
	{
		return _ptr;
	}

	T *operator->()
	{
		return _ptr;
	}

	T& operator*()
	{
		return *_ptr;
	}

	T** operator&()
	{
		return &_ptr;
	}

private:
	T *_ptr;
};
