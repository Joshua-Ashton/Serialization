#pragma once
//
//
//  Created by ZK on 14-7-4.
//
//

#include <cassert>
#include <map>
#include <vector>
#include <iostream>
#include <set>
#include <list>
 
#include "ISerializable.h"
using namespace std;

#define  CHECK_ENDIAN 0

// little endian if true
static bool LE()
{
	const static int n = 1;
	return (*(char *)&n == 1);
}
static bool ReadEndian(istream &istream_)
{

	bool littleEndian = false;
	istream_.read((char*)&littleEndian, sizeof (littleEndian));
	return littleEndian;
}
static void WriteEndian(ostream &ostream_)
{
	bool littleEndian = LE();
 
	ostream_.write((char*)&littleEndian, sizeof (littleEndian));
}



template<typename T>
static void ZeroMem(T& t)
{
	memset(&t, 0, sizeof (T));
}


istream&  read_internal(istream& istream_, char* p, size_t size)
{
static bool endian_checked=false;
if (!endian_checked && CHECK_ENDIAN)
	{assert(LE()==ReadEndian(istream_));
		endian_checked=true;
	}
	istream_.read(p, size);
	return  istream_ ;
}

ostream& write_internal(ostream& ostream_, const char* p, size_t size)
{
	static bool endian_checked = false;
	if (!endian_checked && CHECK_ENDIAN)
	{
		WriteEndian(ostream_);
		endian_checked = true;
	}
	ostream_.write(p, size);
	return  ostream_ ;
}

template<typename T>
static istream& DeSerialize(istream& istream_, T&  t_)
{
	
	return read_internal(istream_, (char*)&t_, sizeof (t_));
}

template<typename T>
static ostream& Serialize(ostream& ostream_, T&  t_)
{
	int size = sizeof(t_);
	return write_internal(ostream_, (const char*)&t_, sizeof(t_));
}

 

static istream& DeSerialize(istream& istream_, ISerializable* t_)
{
	return t_->deSerialize(istream_);

}
 
static ostream& Serialize(ostream& ostream_, ISerializable* t_)
{
	return t_->serialize(ostream_);
}


static  ostream&  Serialize(ostream& ostream_, const std::string& string_)
{
	size_t size = string_.size();
	Serialize(ostream_, size);
	write_internal(ostream_, string_.c_str(), string_.size());
	return ostream_;
}

static  ostream&  Serialize(ostream& ostream_, std::string& string_)
{
	size_t size = string_.size();
	Serialize(ostream_, size);
	write_internal(ostream_, string_.c_str(), string_.size());
	return ostream_;
}


static  istream&  DeSerialize(istream& istream_, std::string& string_)
{
	int size = 0;
	DeSerialize(istream_, size);
	char* buffer = new char[size + 1];
	memset(buffer, 0, size + 1);
	read_internal(istream_, buffer, size);
	string_.assign(buffer);
	delete[] buffer;
	return  istream_;
}


static  ostream&  Serialize(ostream& ostream_, const char* str)
{
	size_t size = strlen(str);
	Serialize(ostream_, size);
	write_internal(ostream_, str, size);
	return ostream_;
}


static  istream&  DeSerialize(istream& istream_, char* str)
{
	int size = 0;
	DeSerialize(istream_, size);
	read_internal(istream_,str,size);
	return  istream_;
}


/////////////vector<bool>//////////////////////////////////
//why specialize this ? //http://stackoverflow.com/questions/15809157/why-is-the-size-of-stdvectorbool-16-byte
 
static ostream& Serialize(ostream& ostream_, vector<bool>& container)
{
	size_t size = container.size();
	Serialize(ostream_, size);
 
	for (auto  ite : container)
	{
		bool c=ite;
		Serialize(ostream_, c);
	}
 
	return ostream_;
}
 
static  istream& DeSerialize(istream& istream_, vector<bool>&container)
{
	if (!istream_.good() || istream_.eof())return istream_;

	int size;
	container.clear();
	DeSerialize(istream_, size);
	for (int i = 0; i < size; ++i)
	{
		bool t;
		DeSerialize(istream_, t);
		container.push_back(t);
	}
	assert(istream_.good());
	return istream_;
}


/////////////vector//////////////////////////////////
template <class T >
static ostream& Serialize(ostream& ostream_, vector<T>& container)
{
	size_t size = container.size();
	Serialize(ostream_, size);
	for (auto& ite : container)
	{
		Serialize(ostream_, ite);
	}
	return ostream_;
}

template <class T >
static  istream& DeSerialize(istream& istream_, vector<T>&container)
{
	if (!istream_.good() || istream_.eof())return istream_;

	int size;
	container.clear();
	DeSerialize(istream_, size);
	for (int i = 0; i < size; ++i)
	{
		T t;
		DeSerialize(istream_, t);
		container.push_back(  t);
	}
	assert(istream_.good());
	return istream_;
}

template <class T >
static ostream&  Serialize(ostream& ostream_, vector<T*>& container)
{

	size_t size = container.size();
	 
	write_internal(ostream_, (char*)&size, sizeof (size_t));
	int index = 0;
	for (auto ite = container.begin(); ite != container.end(); ite++)
	{

		if ((*ite) != NULL)
		{
			bool  notNULL = true;
			Serialize(ostream_, notNULL);
 
			ISerializable* i = dynamic_cast<ISerializable*>(*ite);
			Serialize(ostream_, i);
		}
		else
		{
			bool  notNULL = false;
			Serialize(ostream_, notNULL);
		}
		index++;
	}
	return ostream_;
}

template <class T >
static  istream& DeSerialize(istream& istream_, vector<T*>& container)
{
	if (!istream_.good() || istream_.eof())return istream_;
	int size;
	container.clear();

	DeSerialize(istream_, size);
	container.resize(size);
	for (int i = 0; i < size; ++i)
	{
		bool  notNULL = false;
		DeSerialize(istream_, notNULL);

		if (notNULL)
		{
			T* object = new T;
			DeSerialize(istream_, dynamic_cast<ISerializable*>(object));
			container[i] = object;
		}
		else
		{
			container[i] = NULL;
		}
	}
	return istream_;
}


////////////////map/////////////////////

template <class K, class V>
static ostream&  Serialize(ostream& ostream_, map<K, V>& container)
{

	size_t size = container.size();
	Serialize(ostream_, size);

	for (auto p : container)
	{
		Serialize(ostream_, p.first);
		Serialize(ostream_, p.second);


	}
	return ostream_;
}

template <class K, class V>
static  istream& DeSerialize(istream& istream_, map<K, V>& container)
{
	if (!istream_.good() || istream_.eof())return istream_;
	int size;
	container.clear();
	DeSerialize(istream_, size);

	for (int i = 0; i < size; ++i)
	{
		K key;
		V value;
		DeSerialize(istream_, key);

		DeSerialize(istream_, value);
		container[key] = value;

	}
	return istream_;
}


template <class K, class V>
static ostream&  Serialize(ostream& ostream_, map<K, V*>& container)
{

	size_t size = container.size();
	Serialize(ostream_, size);
	int index = 0;
	for (auto p : container)
	{
		Serialize(ostream_, p.first);

		if ((p.second) != NULL)
		{
			bool notNULL = true;
			Serialize(ostream_, notNULL);
			ISerializable* i = dynamic_cast<ISerializable*>(p.second);
			Serialize(ostream_, i);
		}
		else
		{
			bool notNULL = false;
			Serialize(ostream_, notNULL);
		}


		index++;
	}
	return ostream_;
}

template <class K, class V>
static  istream& DeSerialize(istream& istream_, map<K, V*>& container)
{
	if (!istream_.good() || istream_.eof())return istream_;
	int size;
	container.clear();
	DeSerialize(istream_, size);

	for (int i = 0; i < size; ++i)
	{
		K key;
		DeSerialize(istream_, key);
		bool  notNULL;
		DeSerialize(istream_, notNULL);
		if (notNULL)
		{
			V* object = new V;
			DeSerialize(istream_, dynamic_cast<ISerializable*>(object));
			container[key] = (V*)object;
		}
		else
		{
			container[key] = NULL;
		}
	}
	return istream_;
}


////////////////set/////////////////////

template <class T >
static ostream& Serialize(ostream& ostream_, set<T>& container)
{
	size_t size = container.size();
	Serialize(ostream_, size);
	for (auto& ite : container)
	{
		Serialize(ostream_, ite);
	}
	return ostream_;
}

template <class T >
static  istream& DeSerialize(istream& istream_, set<T>&container)
{
	if (!istream_.good() || istream_.eof())return istream_;

	int size;
	container.clear();
	DeSerialize(istream_, size);
	for (int i = 0; i < size; ++i)
	{
		T t;
		DeSerialize(istream_, t);
		container.insert(t);
	}
	assert(istream_.good());
	return istream_;
}

template <class T >
static ostream&  Serialize(ostream& ostream_, set<T*>& container)
{

	size_t size = container.size();
	Serialize(ostream_, size);

	for (auto ite = container.begin(); ite != container.end(); ite++)
	{
		if ((*ite) != NULL)
		{
			bool noNULL = true;
			Serialize(ostream_, noNULL);
			ISerializable* i = dynamic_cast<ISerializable*>(*ite);
			Serialize(ostream_, i);
		}
		else
		{
			bool noNULL = false;
			Serialize(ostream_, noNULL);
		}

	}
	return ostream_;
}

template <class T >
static  istream& DeSerialize(istream& istream_, set<T*>& container)
{
	if (!istream_.good() || istream_.eof())return istream_;
	int size;
	container.clear();

	DeSerialize(istream_, size);
	container.resize(size);
	for (int i = 0; i < size; ++i)
	{
		bool notNULL = false;
		DeSerialize(istream_, notNULL);
		if (notNULL)
		{
			T* object = new T;
			DeSerialize(istream_, dynamic_cast<ISerializable*>(object));
			container.insert(object);
		}
		else
		{
			container.insert(NULL);
		}
	}
	return istream_;
}



////////////////list/////////////////////

template <class T >
static ostream& Serialize(ostream& ostream_, list<T>& container)
{
	size_t size = container.size();
	Serialize(ostream_, size);
	for (auto& ite : container)
	{
		Serialize(ostream_, ite);
	}
	return ostream_;
}

template <class T >
static  istream& DeSerialize(istream& istream_, list<T>&container)
{
	if (!istream_.good() || istream_.eof())return istream_;

	int size;
	container.clear();
	DeSerialize(istream_, size);
	for (int i = 0; i < size; ++i)
	{
		T t;
		DeSerialize(istream_, t);
		container.push_back(t);
	}
	assert(istream_.good());
	return istream_;
}

template <class T >
static ostream&  Serialize(ostream& ostream_, list<T*>& container)
{

	size_t size = container.size();
	Serialize(ostream_, size);

	for (auto ite = container.begin(); ite != container.end(); ite++)
	{
		if ((*ite) != NULL)
		{
			bool noNULL = true;
			Serialize(ostream_, noNULL);
			ISerializable* i = dynamic_cast<ISerializable*>(*ite);
			Serialize(ostream_, i);
		}
		else
		{
			bool noNULL = false;
			Serialize(ostream_, noNULL);
		}

	}
	return ostream_;
}



template <class T >
static  istream& DeSerialize(istream& istream_, list<T*>& container)
{
	if (!istream_.good() || istream_.eof())return istream_;
	int size;
	container.clear();

	DeSerialize(istream_, size);

	for (int i = 0; i < size; ++i)
	{
		bool notNULL = false;
		DeSerialize(istream_, notNULL);
		if (notNULL)
		{
			T* object = new T;
			DeSerialize(istream_, dynamic_cast<ISerializable*>(object));
			container.push_back(object);
		}
		else
		{
			container.push_back(NULL);
		}
	}
	return istream_;
}
