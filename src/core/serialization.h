#ifndef STL_SERIALIZATION_H
#define STL_SERIALIZATION_H
//
//
//  Created by ZK on 14-7-4.
//  Updated by Joshua Ashton on 18-3-28
//

#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <type_traits>
#include <vector>

class serialize {
public:
	class I {
	public:
		virtual std::ostream &write(std::ostream &ostream_) = 0;
		virtual std::istream &read(std::istream &istream_) = 0;
	};

	static inline bool LE() {

		const static int n = 1;
		const static bool le = (*(char *)&n == 1);
		return le;
	}

	static inline bool ReadEndian(std::istream &istream_) {
		bool littleEndian = false;
		istream_.read((char *)&littleEndian, sizeof(littleEndian));
		return littleEndian;
	}

	static inline void WriteEndian(std::ostream &ostream_) {
		bool littleEndian = LE();

		ostream_.write((char *)&littleEndian, sizeof(littleEndian));
	}

	static inline std::istream &read_internal(std::istream &istream_, char *p,
		uint32_t size) {

		if (!LE()) {
			// read as little endian
			for (int i = size - 1; i >= 0; --i) {
				istream_.read(p + i, 1);
			}
		}
		else
			istream_.read(p, size);
		return istream_;
	}

	static inline std::ostream &write_internal(std::ostream &ostream_, const char *p,
		uint32_t size) {

		if (!LE()) {
			// if big endian .write as little endian
			for (int i = size - 1; i >= 0; --i) {
				ostream_.write(p + i, 1);
			}
		}
		else
			ostream_.write(p, size);
		return ostream_;
	}

	static inline std::istream &read(std::istream &istream_, I *t_) {
		return t_->read(istream_);
	}

	static inline std::istream &read(std::istream &istream_, std::string &string_) {
		int size = 0;
		read(istream_, size);
		string_.resize(size);
		read_internal(istream_, const_cast<char *>(string_.c_str()), size);
		return istream_;
	}

	static inline std::istream &read(std::istream &istream_, char *str) {
		int size = 0;
		read(istream_, size);
		read_internal(istream_, str, size);
		return istream_;
	}

	static inline std::istream &read(std::istream &istream_, std::vector<bool> &container) {
		if (!istream_.good() || istream_.eof())
			return istream_;

		int size;
		container.clear();
		read(istream_, size);
		for (int i = 0; i < size; ++i) {
			bool t;
			read(istream_, t);
			container.push_back(t);
		}
		assert(istream_.good());
		return istream_;
	}

	static inline std::ostream &write(std::ostream &ostream_, I *t_) {
		return t_->write(ostream_);
	}

	static inline std::ostream &write(std::ostream &ostream_, const std::string &string_) {
		uint32_t size = string_.size();
		write(ostream_, size);
		write_internal(ostream_, string_.c_str(), string_.size());
		return ostream_;
	}

	static inline std::ostream &write(std::ostream &ostream_, std::string &string_) {
		uint32_t size = string_.size();
		write(ostream_, size);
		write_internal(ostream_, string_.c_str(), string_.size());
		return ostream_;
	}

	static inline std::ostream &write(std::ostream &ostream_, const char *str) {
		uint32_t size = strlen(str);
		write(ostream_, size);
		write_internal(ostream_, str, size);
		return ostream_;
	}

	static inline std::ostream &write(std::ostream &ostream_, std::vector<bool> &container) {
		uint32_t size = container.size();
		write(ostream_, size);

		for (auto ite : container) {
			bool c = ite;
			write(ostream_, c);
		}

		return ostream_;
	}

	template <typename T> static inline void ZeroMem(T &t) {
		memstd::set(&t, 0, sizeof(T));
	}

	template <typename T> static inline std::istream &read(std::istream &istream_, T &t_) {
		if (std::is_fundamental<T>::value) {
			return read_internal(istream_, (char *)&t_, sizeof(t_));
		}
		else {

			if (std::is_trivial<T>::value)
				return read_internal(istream_, (char *)&t_, sizeof(t_));
			else
				return read(istream_, (serialize::I *)&t_);
		}
		assert(0);
		return istream_;
	}

	template <typename T> static inline std::ostream &write(std::ostream &ostream_, T &t_) {
		if (std::is_fundamental<T>::value) {
			return write_internal(ostream_, (const char *)&t_, sizeof(t_));
		}
		else {

			if (std::is_trivial<T>::value)
				return write_internal(ostream_, (const char *)&t_, sizeof(t_));
			else
				return write(ostream_, (serialize::I *)&t_);
		}
	}

	/////////////std::vector//////////////////////////////////
	template <class T>
	static inline std::ostream &write(std::ostream &ostream_, std::vector<T> &container) {
		uint32_t size = container.size();
		write(ostream_, size);
		for (auto &ite : container) {
			write(ostream_, ite);
		}
		return ostream_;
	}

	template <class T>
	static inline std::istream &read(std::istream &istream_, std::vector<T> &container) {
		if (!istream_.good() || istream_.eof())
			return istream_;

		int size;
		container.clear();
		read(istream_, size);
		for (int i = 0; i < size; ++i) {
			T t;
			read(istream_, t);
			container.push_back(t);
		}
		assert(istream_.good());
		return istream_;
	}

	template <class T>
	static inline std::ostream &write(std::ostream &ostream_, std::vector<T *> &container) {

		uint32_t size = container.size();

		write_internal(ostream_, (char *)&size, sizeof(uint32_t));
		int index = 0;
		for (auto ite = container.begin(); ite != container.end(); ite++) {

			if ((*ite) != NULL) {
				bool notNULL = true;
				write(ostream_, notNULL);

				I *i = dynamic_cast<I *>(*ite);
				write(ostream_, i);
			}
			else {
				bool notNULL = false;
				write(ostream_, notNULL);
			}
			index++;
		}
		return ostream_;
	}

	template <class T>
	static inline std::istream &read(std::istream &istream_, std::vector<T *> &container) {
		if (!istream_.good() || istream_.eof())
			return istream_;
		int size;
		container.clear();

		read(istream_, size);
		container.resize(size);
		for (int i = 0; i < size; ++i) {
			bool notNULL = false;
			read(istream_, notNULL);

			if (notNULL) {
				T *object = new T;
				read(istream_, dynamic_cast<I *>(object));
				container[i] = object;
			}
			else {
				container[i] = NULL;
			}
		}
		return istream_;
	}

	////////////////std::map/////////////////////

	template <class K, class V>
	static inline std::ostream &write(std::ostream &ostream_, std::map<K, V> &container) {

		uint32_t size = container.size();
		write(ostream_, size);

		for (auto p : container) {
			write(ostream_, p.first);
			write(ostream_, p.second);
		}
		return ostream_;
	}

	template <class K, class V>
	static inline std::istream &read(std::istream &istream_, std::map<K, V> &container) {
		if (!istream_.good() || istream_.eof())
			return istream_;
		int size;
		container.clear();
		read(istream_, size);

		for (int i = 0; i < size; ++i) {
			K key;
			V value;
			read(istream_, key);

			read(istream_, value);
			container[key] = value;
		}
		return istream_;
	}

	template <class K, class V>
	static inline std::ostream &write(std::ostream &ostream_, std::map<K, V *> &container) {

		uint32_t size = container.size();
		write(ostream_, size);
		int index = 0;
		for (auto p : container) {
			write(ostream_, p.first);

			if ((p.second) != NULL) {
				bool notNULL = true;
				write(ostream_, notNULL);
				I *i = dynamic_cast<I *>(p.second);
				write(ostream_, i);
			}
			else {
				bool notNULL = false;
				write(ostream_, notNULL);
			}

			index++;
		}
		return ostream_;
	}

	template <class K, class V>
	static inline std::istream &read(std::istream &istream_, std::map<K, V *> &container) {
		if (!istream_.good() || istream_.eof())
			return istream_;
		int size;
		container.clear();
		read(istream_, size);

		for (int i = 0; i < size; ++i) {
			K key;
			read(istream_, key);
			bool notNULL;
			read(istream_, notNULL);
			if (notNULL) {
				V *object = new V;
				read(istream_, dynamic_cast<I *>(object));
				container[key] = (V *)object;
			}
			else {
				container[key] = NULL;
			}
		}
		return istream_;
	}

	////////////////std::unordered_map/////////////////////

	template <class K, class V>
	static inline std::ostream &write(std::ostream &ostream_, std::unordered_map<K, V> &container) {

		uint32_t size = container.size();
		write(ostream_, size);

		for (auto p : container) {
			write(ostream_, p.first);
			write(ostream_, p.second);
		}
		return ostream_;
	}

	template <class K, class V>
	static inline std::istream &read(std::istream &istream_, std::unordered_map<K, V> &container) {
		if (!istream_.good() || istream_.eof())
			return istream_;
		int size;
		container.clear();
		read(istream_, size);

		for (int i = 0; i < size; ++i) {
			K key;
			V value;
			read(istream_, key);

			read(istream_, value);
			container[key] = value;
		}
		return istream_;
	}

	template <class K, class V>
	static inline std::ostream &write(std::ostream &ostream_, std::unordered_map<K, V *> &container) {

		uint32_t size = container.size();
		write(ostream_, size);
		int index = 0;
		for (auto p : container) {
			write(ostream_, p.first);

			if ((p.second) != NULL) {
				bool notNULL = true;
				write(ostream_, notNULL);
				I *i = dynamic_cast<I *>(p.second);
				write(ostream_, i);
			}
			else {
				bool notNULL = false;
				write(ostream_, notNULL);
			}

			index++;
		}
		return ostream_;
	}

	template <class K, class V>
	static inline std::istream &read(std::istream &istream_, std::unordered_map<K, V *> &container) {
		if (!istream_.good() || istream_.eof())
			return istream_;
		int size;
		container.clear();
		read(istream_, size);

		for (int i = 0; i < size; ++i) {
			K key;
			read(istream_, key);
			bool notNULL;
			read(istream_, notNULL);
			if (notNULL) {
				V *object = new V;
				read(istream_, dynamic_cast<I *>(object));
				container[key] = (V *)object;
			}
			else {
				container[key] = NULL;
			}
		}
		return istream_;
	}

	////////////////std::set/////////////////////

	template <class T>
	static inline std::ostream &write(std::ostream &ostream_, std::set<T> &container) {
		uint32_t size = container.size();
		write(ostream_, size);
		for (auto &ite : container) {
			write(ostream_, ite);
		}
		return ostream_;
	}

	template <class T>
	static inline std::istream &read(std::istream &istream_, std::set<T> &container) {
		if (!istream_.good() || istream_.eof())
			return istream_;

		int size;
		container.clear();
		read(istream_, size);
		for (int i = 0; i < size; ++i) {
			T t;
			read(istream_, t);
			container.insert(t);
		}
		assert(istream_.good());
		return istream_;
	}

	template <class T>
	static inline std::ostream &write(std::ostream &ostream_, std::set<T *> &container) {

		uint32_t size = container.size();
		write(ostream_, size);

		for (auto ite = container.begin(); ite != container.end(); ite++) {
			if ((*ite) != NULL) {
				bool noNULL = true;
				write(ostream_, noNULL);
				I *i = dynamic_cast<I *>(*ite);
				write(ostream_, i);
			}
			else {
				bool noNULL = false;
				write(ostream_, noNULL);
			}
		}
		return ostream_;
	}

	template <class T>
	static inline std::istream &read(std::istream &istream_, std::set<T *> &container) {
		if (!istream_.good() || istream_.eof())
			return istream_;
		int size;
		container.clear();

		read(istream_, size);
		container.resize(size);
		for (int i = 0; i < size; ++i) {
			bool notNULL = false;
			read(istream_, notNULL);
			if (notNULL) {
				T *object = new T;
				read(istream_, dynamic_cast<I *>(object));
				container.insert(object);
			}
			else {
				container.insert(NULL);
			}
		}
		return istream_;
	}

	////////////////std::list/////////////////////

	template <class T>
	static inline std::ostream &write(std::ostream &ostream_, std::list<T> &container) {
		uint32_t size = container.size();
		write(ostream_, size);
		for (auto &ite : container) {
			write(ostream_, ite);
		}
		return ostream_;
	}

	template <class T>
	static inline std::istream &read(std::istream &istream_, std::list<T> &container) {
		if (!istream_.good() || istream_.eof())
			return istream_;

		int size;
		container.clear();
		read(istream_, size);
		for (int i = 0; i < size; ++i) {
			T t;
			read(istream_, t);
			container.push_back(t);
		}
		assert(istream_.good());
		return istream_;
	}

	template <class T>
	static inline std::ostream &write(std::ostream &ostream_, std::list<T *> &container) {

		uint32_t size = container.size();
		write(ostream_, size);

		for (auto ite = container.begin(); ite != container.end(); ite++) {
			if ((*ite) != NULL) {
				bool noNULL = true;
				write(ostream_, noNULL);
				I *i = dynamic_cast<I *>(*ite);
				write(ostream_, i);
			}
			else {
				bool noNULL = false;
				write(ostream_, noNULL);
			}
		}
		return ostream_;
	}

	template <class T>
	static inline std::istream &read(std::istream &istream_, std::list<T *> &container) {
		if (!istream_.good() || istream_.eof())
			return istream_;
		int size;
		container.clear();

		read(istream_, size);

		for (int i = 0; i < size; ++i) {
			bool notNULL = false;
			read(istream_, notNULL);
			if (notNULL) {
				T *object = new T;
				read(istream_, dynamic_cast<I *>(object));
				container.push_back(object);
			}
			else {
				container.push_back(NULL);
			}
		}
		return istream_;
	}
};

#endif