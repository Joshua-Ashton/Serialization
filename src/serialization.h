#pragma once
//
//
//  Created by ZK on 14-7-4.
//
//

#include <cassert>
#include <cmath>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <utility>
#include <set>

#ifdef WIN32
#include <type_traits>
#include <random>
#else
#include <type_traits>
#endif

using namespace std;

class ISerializable
{
public:
	virtual ostream& serialize(ostream& ostream_) = 0;
	virtual istream& deSerialize(istream& istream_) = 0;
};

template<typename T>
static void ZeroMem(T& t)
{
	memset(&t, 0, sizeof (T));
}

template<typename T>
static istream& DeSerialize(istream& istream_, T&  t_)
{
	istream_.read((char*)&t_, sizeof (T));
	return istream_;
}

template<typename T>
static ostream& Serialize(ostream& ostream_, T&  t_)
{
 
	ostream_.write((char*)&t_, sizeof (T));
	return ostream_;
}


static istream& DeSerialize(istream& istream_, ISerializable* t_)
{
	return t_->deSerialize(istream_);

}


static ostream& Serialize(ostream& ostream_, ISerializable* t_)
{
	return t_->serialize(ostream_);
}
 

static  ostream&  Serialize(ostream& ostream_,const std::string& string_)
{
	size_t size = string_.size();
	Serialize(ostream_, size);
	ostream_.write(string_.c_str(), string_.size());
	return ostream_;
}

static  ostream&  Serialize(ostream& ostream_, std::string& string_)
{
	size_t size = string_.size();
	Serialize(ostream_,size);
	ostream_.write(string_.c_str(), string_.size());
	return ostream_;
}


static  istream&  DeSerialize(istream& istream_, std::string& string_)
{
	int size = 0;
	DeSerialize(istream_,size);
	char* buffer = new char[size + 1];
	memset(buffer, 0, size + 1);
	istream_.read(buffer, size);
	string_.assign(buffer);
	delete[] buffer;
	return  istream_;
}


static  ostream&  Serialize(ostream& ostream_, const char* str)
{
	size_t size = strlen(str);
	Serialize(ostream_, size);
	ostream_.write(str, size);
	return ostream_;
}
 

static  istream&  DeSerialize(istream& istream_, char* str)
{
	int size = 0;
	DeSerialize(istream_, size);
	istream_.read(str, size);
	return  istream_;
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
		container.insert(container.begin(), t);
	}
	assert(istream_.good());
	return istream_;
}

template <class T >
static ostream&  Serialize(ostream& ostream_, vector<T*>& container)
{

	size_t size = container.size();
	ostream_.write((char*)&size, sizeof (size_t));
	int index = 0;
	for (auto ite = container.begin(); ite != container.end(); ite++)
	{
		if ((*ite) != NULL)
		{

			Serialize(ostream_, index);
			ISerializable* i = dynamic_cast<ISerializable*>(*ite);
			Serialize(ostream_, i);
		}
		else
		{
			int index2 = -1;
			Serialize(ostream_, index2);
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
		int index = -1;
		DeSerialize(istream_, index);
		if (index != -1)
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
		container.insert( t);
	}
	assert(istream_.good());
	return istream_;
}

template <class T >
static ostream&  Serialize(ostream& ostream_, set<T*>& container)
{

	size_t size = container.size();
	 Serialize(ostream_,size);
	int index = 0;
	for (auto ite = container.begin(); ite != container.end(); ite++)
	{
		if ((*ite) != NULL)
		{
			bool noNULL=true;
			Serialize(ostream_, noNULL);
			ISerializable* i = dynamic_cast<ISerializable*>(*ite);
			Serialize(ostream_, i);
		}
		else
		{
			bool noNULL = false;
			Serialize(ostream_, noNULL);
		}
		index++;
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
		bool notNULL=false;
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

