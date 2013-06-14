/* 
 * Copyright (c) 2013 Soluciones Tecnológicas de Calidad S.L. <info@stcsl.es> and
 *                    María Ten Rodríguez <m.ten@stcsl.es>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once;

#include <stdlib.h>

#include <array>
#include <deque>
#include <exception>
#include <forward_list>
#include <istream>
#include <limits>
#include <list>
#include <map>
#include <ostream>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Seza
{
    inline wchar_t convertToWChar(const char& c);
    inline char convertToChar(const wchar_t& c);
    inline std::string convertToString(const std::wstring& str);
    inline std::wstring convertToWString(const std::string& str);

    class Serializer;
    class Deserializer;
    class Serializable;
    class SerializableSTLContainer;

    /* -- MACROS TO REGISTER A CLASS AS A SERIALIZABLE -- */

    /** Macro for clases with no members serializables **/
#define NO_MEMBERS ;

    /** Macro for serializable member **/
#define ADD_MEMBER(name, type) \
        _members[#name] = new RefMember<type>(instance.name);

    /** Macro to define a class as serializable **/
#define REGISTER_SERIALIZABLE(className, members) \
    template<> \
    class SerializableClass<className> : public Serializable \
    { \
    public: \
        SerializableClass(className& instance) : \
            Serializable(), \
            _instance(instance) \
        { \
            members \
        } \
        SerializableClass(const className& instance) : \
            Serializable(), \
            _instance(const_cast<className&>(instance)) \
        { \
            members \
        } \
        char const *getClassName() const { return #className; } \
    protected: \
        className& _instance; \
    };


    /* -- EXCEPTIONS -- */

    /** This exception is thrown when a iterator is out of range in a serializable STL container **/
    class OutOfRangeException : public std::exception 
    {
    public:
      const char* what() const { return "Out of range in STL container!\n"; }
    };

    /* -- SERIALIZABLE STL CONTAINER CLASS -- */

    /** Adapter for STL containers without public iterators**/
    template <class Container>
    class Adapter : public Container 
    {
    public:
        typedef typename Container::container_type container_type;
        container_type &getContainer() { return this->c; }
    };

    /** Serializable STL container**/
    class SerializableSTLContainer
    {
    public:
        SerializableSTLContainer(const std::string& name) : _name(name) {}

        /** Returns the size of the container **/
        virtual size_t size() const = 0;
        /** Sets the internal iterator of the container to the begin **/
        virtual void begin() const = 0;
        /** Advances the iterator **/
        virtual void next() const = 0;
        /** Checks if the iterator is set to the begin **/
        virtual bool isBegin() const = 0;
        /** Checks if the iterator is set to the end **/
        virtual bool isEnd() const = 0;

        /** Returns the name of the STL container **/
        virtual const std::string& getClassName() const { return _name; }
        /** Serializes the element of the container pointed by the iterator **/
        virtual void serializeElem(Serializer* sez, std::ostream& os) const = 0;
        /** Serializes the element of the container pointed by the iterator **/
        virtual void serializeElem(Serializer* sez, std::wostream& os) const = 0;
        /** Deserializes the element of the container pointed by the iterator **/
        virtual void deserializeElem(Deserializer* dez, std::istream& is) const = 0;
        /** Deserializes the element of the container pointed by the iterator **/
        virtual void deserializeElem(Deserializer* dez, std::wistream& is) const = 0;

    protected:
        std::string _name;
    };

    /** Serializable STL pair**/
    template<typename K, typename T>
    class SerializableSTLPair : public SerializableSTLContainer
    {
    public:
        SerializableSTLPair(std::pair<K, T>& instance, const std::string& name) : 
            _instance(instance), 
            _it(0),
            _pos(0), 
            SerializableSTLContainer(name)
        {
        }
        virtual size_t size() const { return 2; }
        virtual void begin() const { _it = 0; }
        virtual void next() const { ++_it; }
        virtual bool isBegin() const { return (_it == 0); }
        virtual bool isEnd() const { return (_it == 2); }

        virtual void serializeElem(Serializer* sez, std::ostream& os) const 
        {
            if(_it == 0)
                sez->write(os, _instance.first); 
            else if(_it == 1)
                sez->write(os, _instance.second);
            else
                throw OutOfRangeException();
        }
        virtual void serializeElem(Serializer* sez, std::wostream& os) const
        {
            if(_it == 0)
                sez->write(os, _instance.first); 
            else if(_it == 1)
                sez->write(os, _instance.second);
            else
                throw OutOfRangeException();
        }
        virtual void deserializeElem(Deserializer* dez, std::istream& is) const 
        { 
            if(_pos == 0)
                dez->read(is, _instance.first);
            else if(_pos == 1)
                dez->read(is, _instance.second);
            else
                throw OutOfRangeException();
            _pos++;
        }
        virtual void deserializeElem(Deserializer* dez, std::wistream& is) const 
        { 
            if(_pos == 0)
                dez->read(is, _instance.first);
            else if(_pos == 1)
                dez->read(is, _instance.second);
            else
                throw OutOfRangeException();
            _pos++;
        }
    protected:
        std::pair<K, T>& _instance;
        mutable unsigned int _it;
        mutable size_t _pos;
    };

    /** Serializable deque and list **/
    template<typename C, typename T>
    class SerializableSTLList : public SerializableSTLContainer
    {
    public:
        SerializableSTLList(C& instance, const std::string& name) : 
            _instance(instance), 
            _it(_instance.begin()), 
            SerializableSTLContainer(name)
        {
        }
        virtual size_t size() const { return _instance.size(); }
        virtual void begin() const { _it = _instance.begin(); }
        virtual void next() const { ++_it; }
        virtual bool isBegin() const { return (_it == _instance.begin()); }
        virtual bool isEnd() const { return (_it == _instance.end()); }

        virtual void serializeElem(Serializer* sez, std::ostream& os) const { sez->write(os, *_it); }
        virtual void serializeElem(Serializer* sez, std::wostream& os) const { sez->write(os, *_it); }
        virtual void deserializeElem(Deserializer* dez, std::istream& is) const 
        { 
            T tmp;
            dez->read(is, tmp);
            _instance.push_back(tmp);
        }
        virtual void deserializeElem(Deserializer* dez, std::wistream& is) const 
        { 
            T tmp;
            dez->read(is, tmp);
            _instance.push_back(tmp);
        }
    protected:
        C& _instance;
        mutable typename C::const_iterator _it;
    };

    /** Serializable array **/
    template<typename T, size_t N>
    class SerializableSTLList<std::array<T, N>, T> : public SerializableSTLContainer
    {
    public:
        SerializableSTLList(std::array<T, N>& instance, const std::string& name) : 
            _instance(instance), 
            _it(_instance.begin()), 
            _pos(0), 
            SerializableSTLContainer(name)
        {
        }
        virtual size_t size() const { return _instance.size(); }
        virtual void begin() const { _it = _instance.begin(); }
        virtual void next() const { ++_it; }
        virtual bool isBegin() const { return (_it == _instance.begin()); }
        virtual bool isEnd() const { return (_it == _instance.end()); }

        virtual void serializeElem(Serializer* sez, std::ostream& os) const { sez->write(os, *_it); }
        virtual void serializeElem(Serializer* sez, std::wostream& os) const { sez->write(os, *_it); }
        virtual void deserializeElem(Deserializer* dez, std::istream& is) const 
        { 
            if(_pos > _instance.size())
                throw OutOfRangeException();
            T tmp;
            dez->read(is, tmp);
            _instance[_pos] = tmp;
            _pos++;
        }
        virtual void deserializeElem(Deserializer* dez, std::wistream& is) const 
        { 
            if(_pos > _instance.size())
                throw OutOfRangeException();
            T tmp;
            dez->read(is, tmp);
            _instance[_pos] = tmp;
            _pos++;
        }
    protected:
        void nextInsertPos() { _pos++; }
        std::array<T, N>& _instance;
        mutable typename std::array<T, N>::const_iterator _it;
        mutable size_t _pos;
    };

    /** Serializable forward list **/
    template<typename T>
    class SerializableSTLList<std::forward_list<T>, T > : public SerializableSTLContainer
    {
    public:
        SerializableSTLList(std::forward_list<T>& instance, const std::string& name) : 
            _instance(instance), 
            _it(_instance.begin()), 
            SerializableSTLContainer(name)
        {
        }
        virtual size_t size() const { return _instance.max_size(); }
        virtual void begin() const { _it = _instance.begin(); }
        virtual void next() const { ++_it; }
        virtual bool isBegin() const { return (_it == _instance.begin()); }
        virtual bool isEnd() const { return (_it == _instance.end()); }

        virtual void serializeElem(Serializer* sez, std::ostream& os) const { sez->write(os, *_it); }
        virtual void serializeElem(Serializer* sez, std::wostream& os) const { sez->write(os, *_it); }
        virtual void deserializeElem(Deserializer* dez, std::istream& is) const 
        { 
            T tmp;
            dez->read(is, tmp);
            _instance.push_front(tmp);
        }
        virtual void deserializeElem(Deserializer* dez, std::wistream& is) const 
        { 
            T tmp;
            dez->read(is, tmp);
            _instance.push_front(tmp);
        }
    protected:
        std::forward_list<T>& _instance;
        mutable typename std::forward_list<T>::const_iterator _it;
    };

    /** Serializable map **/
    template<typename C, typename K, typename T>
    class SerializableSTLMap : public SerializableSTLContainer
    {
    public:
        SerializableSTLMap(C& instance, const std::string& name) : 
            _instance(instance), 
            _it(_instance.begin()), 
            SerializableSTLContainer(name)
        {
        }
        virtual size_t size() const { return _instance.size(); }
        virtual void begin() const { _it = _instance.begin(); }
        virtual void next() const { ++_it; }
        virtual bool isBegin() const { return (_it == _instance.begin()); }
        virtual bool isEnd() const { return (_it == _instance.end()); }

        virtual void serializeElem(Serializer* sez, std::ostream& os) const 
        { 
            std::pair<K, T> tmp = (*_it);
            sez->write(os, tmp); 
        }
        virtual void serializeElem(Serializer* sez, std::wostream& os) const
        { 
            std::pair<K, T> tmp = (*_it);
            sez->write(os, tmp); 
        }
        virtual void deserializeElem(Deserializer* dez, std::istream& is) const 
        { 
            std::pair<K, T> tmp;
            dez->read(is, tmp);
            _instance.insert(tmp);
        }
        virtual void deserializeElem(Deserializer* dez, std::wistream& is) const 
        { 
            std::pair<K, T> tmp;
            dez->read(is, tmp);
            _instance.insert(tmp);
        }
    protected:
        C& _instance;
        mutable typename C::const_iterator _it;
    };

    /** Serializable set and multiset **/
    template<typename C, typename T>
    class SerializableSTLSet : public SerializableSTLContainer
    {
    public:
        SerializableSTLSet(C& instance, const std::string& name) : 
            _instance(instance), 
            _it(_instance.begin()), 
            SerializableSTLContainer(name)
        {
        }
        virtual size_t size() const { return _instance.size(); }
        virtual void begin() const { _it = _instance.begin(); }
        virtual void next() const { ++_it; }
        virtual bool isBegin() const { return (_it == _instance.begin()); }
        virtual bool isEnd() const { return (_it == _instance.end()); }

        virtual void serializeElem(Serializer* sez, std::ostream& os) const { sez->write(os, *_it); }
        virtual void serializeElem(Serializer* sez, std::wostream& os) const { sez->write(os, *_it); }
        virtual void deserializeElem(Deserializer* dez, std::istream& is) const 
        { 
            T tmp;
            dez->read(is, tmp);
            _instance.insert(tmp);
        }
        virtual void deserializeElem(Deserializer* dez, std::wistream& is) const 
        { 
            T tmp;
            dez->read(is, tmp);
            _instance.insert(tmp);
        }
    protected:
        C& _instance;
        mutable typename C::const_iterator _it;
    };

    /** Serializable stack, queue and priority queue **/
    template<typename C, typename T>
    class SerializableSTLQueue : public SerializableSTLContainer
    {
    public:
        SerializableSTLQueue(C& instance, const std::string& name) : 
            _instance(instance), 
            SerializableSTLContainer(name)
        {
             _adapter = reinterpret_cast<Adapter<C> *>(&instance);
        }
        virtual size_t size() const { return _instance.size(); }
        virtual void begin() const { _it = _adapter->getContainer().begin(); }
        virtual void next() const { ++_it; }
        virtual bool isBegin() const { return (_it == _adapter->getContainer().begin()); }
        virtual bool isEnd() const { return (_it == _adapter->getContainer().end()); }

        virtual void serializeElem(Serializer* sez, std::ostream& os) const { sez->write(os, *_it); }
        virtual void serializeElem(Serializer* sez, std::wostream& os) const { sez->write(os, *_it); }
        virtual void deserializeElem(Deserializer* dez, std::istream& is) const 
        { 
            T tmp;
            dez->read(is, tmp);
            _instance.push(tmp);
        }
        virtual void deserializeElem(Deserializer* dez, std::wistream& is) const 
        { 
            T tmp;
            dez->read(is, tmp);
            _instance.push(tmp);
        }
    protected:
        C& _instance;
        mutable Adapter<C> *_adapter; // to access underlying container
        mutable typename Adapter<C>::container_type::const_iterator _it;
    };

    /* -- SERIALIZABLE CLASS -- */

    /** Serializable class member **/
    struct RefMemberBase
    {
        virtual void serializeElem(Serializer* sez, std::ostream& os) const = 0;
        virtual void serializeElem(Serializer* sez, std::wostream& os) const = 0;
        virtual void deserializeElem(Deserializer* dez, std::istream& is) const = 0;
        virtual void deserializeElem(Deserializer* dez, std::wistream& is) const = 0;
    };

    /** Specialization fo a serializable class member **/
    template<typename T>
    class RefMember : public RefMemberBase
    {
    public:
        RefMember(T& ref) : _ref(ref) {}
        RefMember(const T& ref) : _ref(const_cast<T&>(ref)) {}
        ~RefMember() { _ref = nullptr; }

        virtual void serializeElem(Serializer* sez, std::ostream& os) const { sez->write(os, _ref); }
        virtual void serializeElem(Serializer* sez, std::wostream& os) const { sez->write(os, _ref); }
        virtual void deserializeElem(Deserializer* dez, std::istream& is) const { dez->read(is, _ref); }
        virtual void deserializeElem(Deserializer* dez, std::wistream& is) const { dez->read(is, _ref); }

    protected:
        T& _ref;
    };

    /** Serializable class**/
    class Serializable
    {
    public:
        Serializable()
        {
        }
        ~Serializable()
        {
            for(std::map<std::string, RefMemberBase*>::iterator it = _members.begin();
                it != _members.end(); ++it)
            {
                delete it->second;
            }
            _members.clear();
        }

        /** Returns the count of the serializable class members **/
        virtual size_t membersCount() const { return _members.size(); }
        /** Sets the internal iterator of the container to the begin **/
        virtual void begin() const { _it = _members.begin(); }
        /** Advances the iterator **/
        virtual void next() const { ++_it; }
        /** Checks if the iterator is set to the begin **/
        virtual bool isBegin() const { return (_it == _members.begin()); }
        /** Checks if the iterator is set to the end **/
        virtual bool isEnd() const { return (_it == _members.end()); }

        /** Returns the name of the serializable class **/
        virtual const char *getClassName() const = 0;
        /** Serializes the member name of the container pointed by the iterator **/
        virtual void serializeElemName(Serializer* sez, std::ostream& os) const;
        /** Serializes the member name of the container pointed by the iterator **/
        virtual void serializeElemName(Serializer* sez, std::wostream& os) const;
        /** Serializes the member value of the container pointed by the iterator **/
        virtual void serializeElemValue(Serializer* sez, std::ostream& os) const;
        /** Serializes the member value of the container pointed by the iterator **/
        virtual void serializeElemValue(Serializer* sez, std::wostream& os) const;
        /** Returns true if the member name deserializated exists in the class **/
        virtual bool deserializeElemName(Deserializer* dez, std::istream& is) const;
        /** Returns true if the member name deserializated exists in the class **/
        virtual bool deserializeElemName(Deserializer* dez, std::wistream& is) const;
        /** Deserializes the member value of the container pointed by the iterator **/
        virtual void deserializeElemValue(Deserializer* dez, std::istream& is) const;
        /** Deserializes the member value of the container pointed by the iterator **/
        virtual void deserializeElemValue(Deserializer* dez, std::wistream& is) const;

    protected:
        std::map<std::string, RefMemberBase*> _members;
        mutable std::map<std::string, RefMemberBase*>::const_iterator _it;
    };

    /** Specialization fo a serializable class **/
    template<class C>
    class SerializableClass : public Serializable
    {
    public:
        SerializableClass(C& instance) : 
            Serializable(),
            _instance(instance) 
        {
        }

        SerializableClass(const C& instance) : 
            Serializable(),
            _instance(const_cast<C&>(instance)) 
        {
        }

    protected:
        C& _instance;
    };

    /* -- SERIALIZER INTERFACE -- */
    class Serializer
    {
    public:
        /** Null values **/
        virtual void write(std::ostream& os) = 0;
        virtual void write(std::wostream& os) = 0;
        /** Basic types **/
        virtual void write(std::ostream& os, const bool& value) = 0;
        virtual void write(std::ostream& os, const char& value) = 0;
        virtual void write(std::ostream& os, const unsigned char& value) = 0;
        virtual void write(std::ostream& os, const wchar_t& value) = 0;
        virtual void write(std::ostream& os, const short& value) = 0;
        virtual void write(std::ostream& os, const unsigned short& value) = 0;
        virtual void write(std::ostream& os, const int& value) = 0;
        virtual void write(std::ostream& os, const unsigned int& value) = 0;
        virtual void write(std::ostream& os, const long& value) = 0;
        virtual void write(std::ostream& os, const unsigned long& value) = 0;
        virtual void write(std::ostream& os, const long long& value) = 0;
        virtual void write(std::ostream& os, const unsigned long long& value) = 0;
        virtual void write(std::ostream& os, const float& value) = 0;
        virtual void write(std::ostream& os, const double& value) = 0;
        virtual void write(std::ostream& os, const long double& value) = 0;
        virtual void write(std::ostream& os, bool* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, char* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, unsigned char* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, wchar_t* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, short* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, unsigned short* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, int* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, unsigned int* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, long* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, unsigned long* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, long long* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, unsigned long long* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, float* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, double* value) { this->write(os, *value); }
        virtual void write(std::ostream& os, long double* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, const bool& value) = 0;
        virtual void write(std::wostream& os, const char& value) = 0;
        virtual void write(std::wostream& os, const unsigned char& value) = 0;
        virtual void write(std::wostream& os, const wchar_t& value) = 0;
        virtual void write(std::wostream& os, const short& value) = 0;
        virtual void write(std::wostream& os, const unsigned short& value) = 0;
        virtual void write(std::wostream& os, const int& value) = 0;
        virtual void write(std::wostream& os, const unsigned int& value) = 0;
        virtual void write(std::wostream& os, const long& value) = 0;
        virtual void write(std::wostream& os, const unsigned long& value) = 0;
        virtual void write(std::wostream& os, const long long& value) = 0;
        virtual void write(std::wostream& os, const unsigned long long& value) = 0;
        virtual void write(std::wostream& os, const float& value) = 0;
        virtual void write(std::wostream& os, const double& value) = 0;
        virtual void write(std::wostream& os, const long double& value) = 0;
        virtual void write(std::wostream& os, bool* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, char* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, unsigned char* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, wchar_t* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, short* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, unsigned short* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, int* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, unsigned int* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, long* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, unsigned long* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, long long* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, unsigned long long* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, float* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, double* value) { this->write(os, *value); }
        virtual void write(std::wostream& os, long double* value) { this->write(os, *value); }
        /** Arrays of basic types **/
        virtual void write(std::ostream& os, const bool* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const char* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const unsigned char* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const wchar_t* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const short* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const unsigned short* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const int* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const unsigned int* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const long* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const unsigned long* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const long long* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const unsigned long long* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const float* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const double* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const long double* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const bool* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const char* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const unsigned char* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const wchar_t* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const short* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const unsigned short* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const int* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const unsigned int* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const long* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const unsigned long* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const long long* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const unsigned long long* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const float* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const double* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const long double* vector, const size_t& size) = 0;
        /** Strings **/
        virtual void write(std::ostream& os, const std::string& string) = 0;
        virtual void write(std::ostream& os, const std::wstring& string) = 0;
        virtual void write(std::wostream& os, const std::string& string) = 0;
        virtual void write(std::wostream& os, const std::wstring& string) = 0;
        /** Arrays of strings **/
        virtual void write(std::ostream& os, const std::string* vector, const size_t& size) = 0;
        virtual void write(std::ostream& os, const std::wstring* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const std::string* vector, const size_t& size) = 0;
        virtual void write(std::wostream& os, const std::wstring* vector, const size_t& size) = 0;
        /** Serializable STL container **/
        virtual void write(std::ostream& os, const SerializableSTLContainer& container) = 0;
        virtual void write(std::wostream& os, const SerializableSTLContainer& container) = 0;
        /** Automatic serializators for STL containers **/
        template<typename K, typename T>
        void write(std::ostream& os, std::pair<K, T>& container)
        {
            SerializableSTLPair<K, T> tmp(container, "std::pair");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T, std::size_t N>
        void write(std::ostream& os, std::array<T, N>& container)
        {
            SerializableSTLList<std::array<T, N>, T> tmp(const_cast<std::array<T, N>&>(container), "std::array");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::ostream& os, std::deque<T>& container)
        {
            SerializableSTLList<std::deque<T>, T> tmp(container, "std::deque");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::ostream& os, std::forward_list<T>& container)
        {
            SerializableSTLList<std::forward_list<T>, T> tmp(container, "std::forward_list");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::ostream& os, std::list<T>& container)
        {
            SerializableSTLList<std::list<T>, T> tmp(container, "std::list");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void write(std::ostream& os, std::map<K, T>& container)
        {
            SerializableSTLMap<std::map<K, T>, K, T> tmp(container, "std::map");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void write(std::ostream& os, std::multimap<K, T>& container)
        {
            SerializableSTLMap<std::multimap<K, T>, K, T> tmp(container, "std::multimap");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::ostream& os, std::multiset<T>& container)
        {
            SerializableSTLSet<std::multiset<T>, T> tmp(container, "std::multiset");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::ostream& os, std::priority_queue<T>& container)
        {
            SerializableSTLQueue<std::priority_queue<T>, T> tmp(container, "std::priority_queue");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::ostream& os, std::queue<T>& container)
        {
            SerializableSTLQueue<std::queue<T>, T> tmp(container, "std::queue");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::ostream& os, std::set<T>& container)
        {
            SerializableSTLSet<std::set<T>, T> tmp(container, "std::set");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::ostream& os, std::stack<T>& container)
        {
            SerializableSTLQueue<std::stack<T>, T> tmp(container, "std::stack");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void write(std::ostream& os, std::unordered_map<K, T>& container)
        {
            SerializableSTLMap<std::unordered_map<K, T>, K, T> tmp(container, "std::unordered_map");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void write(std::ostream& os, std::unordered_multimap<K, T>& container)
        {
            SerializableSTLMap<std::unordered_multimap<K, T>, K, T> tmp(container, "std::unordered_multimap");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::ostream& os, std::unordered_multiset<T>& container)
        {
            SerializableSTLSet<std::unordered_multiset<T>, T> tmp(container, "std::unordered_multiset");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::ostream& os, std::unordered_set<T>& container)
        {
            SerializableSTLSet<std::unordered_set<T>, T> tmp(container, "std::unordered_set");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::ostream& os, std::vector<T>& container)
        {
            SerializableSTLList<std::vector<T>, T> tmp(container, "std::vector");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void write(std::wostream& os, std::pair<K, T>& container)
        {
            SerializableSTLPair<K, T> tmp(container, "std::pair");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T, std::size_t N>
        void write(std::wostream& os, std::array<T, N>& container)
        {
            SerializableSTLList<std::array<T, N>, T> tmp(container, "std::array");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::wostream& os, std::deque<T>& container)
        {
            SerializableSTLList<std::deque<T>, T> tmp(container, "std::deque");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::wostream& os, std::forward_list<T>& container)
        {
            SerializableSTLList<std::forward_list<T>, T> tmp(container, "std::forward_list");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::wostream& os, std::list<T>& container)
        {
            SerializableSTLList<std::list<T>, T> tmp(container, "std::list");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void write(std::wostream& os, std::map<K, T>& container)
        {
            SerializableSTLMap<std::map<K, T>, K, T> tmp(container, "std::map");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void write(std::wostream& os, std::multimap<K, T>& container)
        {
            SerializableSTLMap<std::multimap<K, T>, K, T> tmp(container, "std::multimap");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::wostream& os, std::multiset<T>& container)
        {
            SerializableSTLSet<std::multiset<T>, T> tmp(container, "std::multiset");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::wostream& os, std::priority_queue<T>& container)
        {
            SerializableSTLQueue<std::priority_queue<T>, T> tmp(container, "std::priority_queue");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::wostream& os, std::queue<T>& container)
        {
            SerializableSTLQueue<std::queue<T>, T> tmp(container, "std::queue");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::wostream& os, std::set<T>& container)
        {
            SerializableSTLSet<std::set<T>, T> tmp(container, "std::set");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::wostream& os, std::stack<T>& container)
        {
            SerializableSTLQueue<std::stack<T>, T> tmp(container, "std::stack");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void write(std::wostream& os, std::unordered_map<K, T>& container)
        {
            SerializableSTLMap<std::unordered_map<K, T>, K, T> tmp(container, "std::unordered_map");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void write(std::wostream& os, std::unordered_multimap<K, T>& container)
        {
            SerializableSTLMap<std::unordered_multimap<K, T>, K, T> tmp(container, "std::unordered_multimap");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::wostream& os, std::unordered_multiset<T>& container)
        {
            SerializableSTLSet<std::unordered_multiset<T>, T> tmp(container, "std::unordered_multiset");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::wostream& os, std::unordered_set<T>& container)
        {
            SerializableSTLSet<std::unordered_set<T>, T> tmp(container, "std::unordered_set");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void write(std::wostream& os, std::vector<T>& container)
        {
            SerializableSTLList<std::vector<T>, T> tmp(container, "std::vector");
            this->write(os, (const SerializableSTLContainer&) tmp);
        }
        /** Serializable classes **/
        virtual void write(std::ostream& os, const Serializable& object) = 0;
        virtual void write(std::wostream& os, const Serializable& object) = 0;
        /** Automatic serializators for serializable classes **/
        template<typename C>
        void write(std::ostream& os, const C& object, typename std::enable_if<!(std::is_abstract<SerializableClass<C> >::value) >::type* = 0)
        {
            SerializableClass<C> tmp(object);
            this->write(os, (const Serializable&) tmp);
        }
        template<typename C>
        void write(std::wostream& os, const C& object, typename std::enable_if<!(std::is_abstract<SerializableClass<C> >::value) >::type* = 0)
        {
            SerializableClass<C> tmp(object);
            this->write(os, (const Serializable&) tmp);
        }
        /** Automatic serializators for non serializable classes **/
        template<typename C>
        void write(std::ostream& os, const C& object, typename std::enable_if<(!(std::is_enum<C>::value)&&(std::is_abstract<SerializableClass<C> >::value)) >::type* = 0)
        {
            this->write(os);
        }
        template<typename C>
        void write(std::wostream& os, const C& object, typename std::enable_if<(!(std::is_enum<C>::value)&&(std::is_abstract<SerializableClass<C> >::value)) >::type* = 0)
        {
            this->write(os);
        }
        /** Automatic serializators for enums. Enums are converted to int **/
        template<typename C>
        void write(std::ostream& os, const C& object, typename std::enable_if<((std::is_enum<C>::value)&&(std::is_abstract<SerializableClass<C> >::value)) >::type* = 0)
        {
            this->write(os, (int)object);
        }
        template<typename C>
        void write(std::wostream& os, const C& object, typename std::enable_if<((std::is_enum<C>::value)&&(std::is_abstract<SerializableClass<C> >::value)) >::type* = 0)
        {
            this->write(os, (int)object);
        }
    };

    /* -- SERIALIZER INTERFACE -- */
    class Deserializer
    {
    public:
        /** Null values **/
        virtual void read(std::istream& is) = 0;
        virtual void read(std::wistream& is) = 0;
        /** Basic types **/
        virtual void read(std::istream& is, bool& value) = 0;
        virtual void read(std::istream& is, char& value) = 0;
        virtual void read(std::istream& is, unsigned char& value) = 0;
        virtual void read(std::istream& is, wchar_t& value) = 0;
        virtual void read(std::istream& is, short& value) = 0;
        virtual void read(std::istream& is, unsigned short& value) = 0;
        virtual void read(std::istream& is, int& value) = 0;
        virtual void read(std::istream& is, unsigned int& value) = 0;
        virtual void read(std::istream& is, long& value) = 0;
        virtual void read(std::istream& is, unsigned long& value) = 0;
        virtual void read(std::istream& is, long long& value) = 0;
        virtual void read(std::istream& is, unsigned long long& value) = 0;
        virtual void read(std::istream& is, float& value) = 0;
        virtual void read(std::istream& is, double& value) = 0;
        virtual void read(std::istream& is, long double& value) = 0;
        virtual void read(std::istream& is, bool* value) { this->read(is, *value); }
        virtual void read(std::istream& is, char* value) { this->read(is, *value); }
        virtual void read(std::istream& is, unsigned char* value) { this->read(is, *value); }
        virtual void read(std::istream& is, wchar_t* value) { this->read(is, *value); }
        virtual void read(std::istream& is, short* value) { this->read(is, *value); }
        virtual void read(std::istream& is, unsigned short* value) { this->read(is, *value); }
        virtual void read(std::istream& is, int* value) { this->read(is, *value); }
        virtual void read(std::istream& is, unsigned int* value) { this->read(is, *value); }
        virtual void read(std::istream& is, long* value) { this->read(is, *value); }
        virtual void read(std::istream& is, unsigned long* value) { this->read(is, *value); }
        virtual void read(std::istream& is, long long* value) { this->read(is, *value); }
        virtual void read(std::istream& is, unsigned long long* value) { this->read(is, *value); }
        virtual void read(std::istream& is, float* value) { this->read(is, *value); }
        virtual void read(std::istream& is, double* value) { this->read(is, *value); }
        virtual void read(std::istream& is, long double* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, bool& value) = 0;
        virtual void read(std::wistream& is, char& value) = 0;
        virtual void read(std::wistream& is, unsigned char& value) = 0;
        virtual void read(std::wistream& is, wchar_t& value) = 0;
        virtual void read(std::wistream& is, short& value) = 0;
        virtual void read(std::wistream& is, unsigned short& value) = 0;
        virtual void read(std::wistream& is, int& value) = 0;
        virtual void read(std::wistream& is, unsigned int& value) = 0;
        virtual void read(std::wistream& is, long& value) = 0;
        virtual void read(std::wistream& is, unsigned long& value) = 0;
        virtual void read(std::wistream& is, long long& value) = 0;
        virtual void read(std::wistream& is, unsigned long long& value) = 0;
        virtual void read(std::wistream& is, float& value) = 0;
        virtual void read(std::wistream& is, double& value) = 0;
        virtual void read(std::wistream& is, long double& value) = 0;
        virtual void read(std::wistream& is, bool* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, char* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, unsigned char* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, wchar_t* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, short* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, unsigned short* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, int* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, unsigned int* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, long* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, unsigned long* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, long long* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, unsigned long long* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, float* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, double* value) { this->read(is, *value); }
        virtual void read(std::wistream& is, long double* value) { this->read(is, *value); }
        /** Arrays of basic types **/
        virtual size_t read(std::istream& is, bool* value, const size_t& size) = 0;
        virtual size_t read(std::istream& is, char* value, const size_t& size) = 0;
        virtual size_t read(std::istream& is, unsigned char* value, const size_t& size) = 0;
        virtual size_t read(std::istream& is, wchar_t* value, const size_t& size) = 0;
        virtual size_t read(std::istream& is, short* vector, const size_t& size) = 0;
        virtual size_t read(std::istream& is, unsigned short* vector, const size_t& size) = 0;
        virtual size_t read(std::istream& is, int* vector, const size_t& size) = 0;
        virtual size_t read(std::istream& is, unsigned int* vector, const size_t& size) = 0;
        virtual size_t read(std::istream& is, long* vector, const size_t& size) = 0;
        virtual size_t read(std::istream& is, unsigned long* vector, const size_t& size) = 0;
        virtual size_t read(std::istream& is, long long* vector, const size_t& size) = 0;
        virtual size_t read(std::istream& is, unsigned long long* vector, const size_t& size) = 0;
        virtual size_t read(std::istream& is, float* vector, const size_t& size) = 0;
        virtual size_t read(std::istream& is, double* vector, const size_t& size) = 0;
        virtual size_t read(std::istream& is, long double* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, bool* value, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, char* value, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, unsigned char* value, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, wchar_t* value, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, short* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, unsigned short* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, int* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, unsigned int* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, long* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, unsigned long* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, long long* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, unsigned long long* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, float* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, double* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, long double* vector, const size_t& size) = 0;
        /** Strings **/
        virtual void read(std::istream& is, std::string& string) = 0;
        virtual void read(std::istream& is, std::wstring& string) = 0;
        virtual void read(std::wistream& is, std::string& string) = 0;
        virtual void read(std::wistream& is, std::wstring& string) = 0;
        /** Arrays of strings **/
        virtual size_t read(std::istream& is, std::string* vector, const size_t& size) = 0;
        virtual size_t read(std::istream& is, std::wstring* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, std::string* vector, const size_t& size) = 0;
        virtual size_t read(std::wistream& is, std::wstring* vector, const size_t& size) = 0;
        /** Serializable STL container **/
        virtual void read(std::istream& is, SerializableSTLContainer& container) = 0;
        virtual void read(std::wistream& is, SerializableSTLContainer& container) = 0;
        /** Automatic serializators for STL containers **/
        template<typename K, typename T>
        void read(std::istream& is, std::pair<K, T>& container)
        {
            SerializableSTLPair<K, T> tmp(container, "std::pair");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T, std::size_t N>
        void read(std::istream& is, std::array<T, N>& container)
        {
            SerializableSTLList<std::array<T, N>, T> tmp(container, "std::array");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::istream& is, std::deque<T>& container)
        {
            SerializableSTLList<std::deque<T>, T> tmp(container, "std::deque");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::istream& is, std::forward_list<T>& container)
        {
            SerializableSTLList<std::forward_list<T>, T> tmp(container, "std::forward_list");
            this->read(is, (SerializableSTLContainer&) tmp);
            container.reverse(); // Trick because the forward list only has push_front
        }
        template<typename T>
        void read(std::istream& is, std::list<T>& container)
        {
            SerializableSTLList<std::list<T>, T> tmp(container, "std::list");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void read(std::istream& is, std::map<K, T>& container)
        {
            SerializableSTLMap<std::map<K, T>, K, T> tmp(container, "std::map");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void read(std::istream& is, std::multimap<K, T>& container)
        {
            SerializableSTLMap<std::multimap<K, T>, K, T> tmp(container, "std::multimap");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::istream& is, std::multiset<T>& container)
        {
            SerializableSTLSet<std::multiset<T>, T> tmp(container, "std::multiset");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::istream& is, std::priority_queue<T>& container)
        {
            SerializableSTLQueue<std::priority_queue<T>, T> tmp(container, "std::priority_queue");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::istream& is, std::queue<T>& container)
        {
            SerializableSTLQueue<std::queue<T>, T> tmp(container, "std::queue");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::istream& is, std::set<T>& container)
        {
            SerializableSTLSet<std::set<T>, T> tmp(container, "std::set");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::istream& is, std::stack<T>& container)
        {
            SerializableSTLQueue<std::stack<T>, T> tmp(container, "std::stack");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void read(std::istream& is, std::unordered_map<K, T>& container)
        {
            SerializableSTLMap<std::unordered_map<K, T>, K, T> tmp(container, "std::unordered_map");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void read(std::istream& is, std::unordered_multimap<K, T>& container)
        {
            SerializableSTLMap<std::unordered_multimap<K, T>, K, T> tmp(container, "std::unordered_multimap");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::istream& is, std::unordered_multiset<T>& container)
        {
            SerializableSTLSet<std::unordered_multiset<T>, T> tmp(container, "std::unordered_multiset");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::istream& is, std::unordered_set<T>& container)
        {
            SerializableSTLSet<std::unordered_set<T>, T> tmp(container, "std::unordered_set");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::istream& is, std::vector<T>& container)
        {
            SerializableSTLList<std::vector<T>, T> tmp(container, "std::vector");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void read(std::wistream& is, std::pair<K, T>& container)
        {
            SerializableSTLPair<K, T> tmp(container, "std::pair");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T, std::size_t N>
        void read(std::wistream& is, std::array<T, N>& container)
        {
            SerializableSTLList<std::array<T, N>, T> tmp(container, "std::array");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::wistream& is, std::deque<T>& container)
        {
            SerializableSTLList<std::deque<T>, T> tmp(container, "std::deque");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::wistream& is, std::forward_list<T>& container)
        {
            SerializableSTLList<std::forward_list<T>, T> tmp(container, "std::forward_list");
            this->read(is, (SerializableSTLContainer&) tmp);
            container.reverse(); // Trick because the forward list only has push_front
        }
        template<typename T>
        void read(std::wistream& is, std::list<T>& container)
        {
            SerializableSTLList<std::list<T>, T> tmp(container, "std::list");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void read(std::wistream& is, std::map<K, T>& container)
        {
            SerializableSTLMap<std::map<K, T>, K, T> tmp(container, "std::map");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void read(std::wistream& is, std::multimap<K, T>& container)
        {
            SerializableSTLMap<std::multimap<K, T>, K, T> tmp(container, "std::multimap");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::wistream& is, std::multiset<T>& container)
        {
            SerializableSTLSet<std::multiset<T>, T> tmp(container, "std::multiset");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::wistream& is, std::priority_queue<T>& container)
        {
            SerializableSTLQueue<std::priority_queue<T>, T> tmp(container, "std::priority_queue");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::wistream& is, std::queue<T>& container)
        {
            SerializableSTLQueue<std::queue<T>, T> tmp(container, "std::queue");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::wistream& is, std::set<T>& container)
        {
            SerializableSTLSet<std::set<T>, T> tmp(container, "std::set");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::wistream& is, std::stack<T>& container)
        {
            SerializableSTLQueue<std::stack<T>, T> tmp(container, "std::stack");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void read(std::wistream& is, std::unordered_map<K, T>& container)
        {
            SerializableSTLMap<std::unordered_map<K, T>, K, T> tmp(container, "std::unordered_map");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename K, typename T>
        void read(std::wistream& is, std::unordered_multimap<K, T>& container)
        {
            SerializableSTLMap<std::unordered_multimap<K, T>, K, T> tmp(container, "std::unordered_multimap");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::wistream& is, std::unordered_multiset<T>& container)
        {
            SerializableSTLSet<std::unordered_multiset<T>, T> tmp(container, "std::unordered_multiset");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::wistream& is, std::unordered_set<T>& container)
        {
            SerializableSTLSet<std::unordered_set<T>, T> tmp(container, "std::unordered_set");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        template<typename T>
        void read(std::wistream& is, std::vector<T>& container)
        {
            SerializableSTLList<std::vector<T>, T> tmp(container, "std::vector");
            this->read(is, (SerializableSTLContainer&) tmp);
        }
        /** Serializable classes **/
        virtual void read(std::istream& is, Serializable& object) = 0;
        virtual void read(std::wistream& is, Serializable& object) = 0;
        /** Automatic serializators for serializable classes **/
        template<typename C>
        void read(std::istream& is, C& object, typename std::enable_if<!(std::is_abstract<SerializableClass<C> >::value) >::type* = 0)
        {
            SerializableClass<C> tmp(object);
            this->read(is, (Serializable&) tmp);
        }
        template<typename C>
        void read(std::wistream& is, C& object, typename std::enable_if<!(std::is_abstract<SerializableClass<C> >::value) >::type* = 0)
        {
            SerializableClass<C> tmp(object);
            this->read(is, (Serializable&) tmp);
        }
        /** Automatic serializators for non serializable classes **/
        template<typename C>
        void read(std::istream& is, C& object, typename std::enable_if<(!(std::is_enum<C>::value)&&(std::is_abstract<SerializableClass<C> >::value)) >::type* = 0)
        {
            this->read(is);
        }
        template<typename C>
        void read(std::wistream& is, C& object, typename std::enable_if<(!(std::is_enum<C>::value)&&(std::is_abstract<SerializableClass<C> >::value)) >::type* = 0)
        {
            this->read(is);
        }
        /** Automatic serializators for enums. Enums are converted to int **/
        template<typename C>
        void read(std::istream& is, C& object, typename std::enable_if<((std::is_enum<C>::value)&&(std::is_abstract<SerializableClass<C> >::value)) >::type* = 0)
        {
            int tmp;
            this->read(is, tmp);
            object = static_cast<C>(tmp);
        }
        template<typename C>
        void read(std::wistream& is, C& object, typename std::enable_if<((std::is_enum<C>::value)&&(std::is_abstract<SerializableClass<C> >::value)) >::type* = 0)
        {
            int tmp;
            this->read(is, tmp);
            object = static_cast<C>(tmp);
        }
    };

    /* -- SERIALIZER IMPLEMENTATION -- */
    template<class C>
    class SerializerImpl : public Serializer
    {
    public:
        /** Null values **/
        virtual void write(std::ostream& os) { static_cast<C*>(this)->writeNull(os); }
        virtual void write(std::wostream& os) { static_cast<C*>(this)->writeNull(os); }
        /** Basic types **/
        virtual void write(std::ostream& os, const bool& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const char& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const unsigned char& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const wchar_t& value) { static_cast<C*>(this)->writeValue(os, convertToChar(value)); }
        virtual void write(std::ostream& os, const short& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const unsigned short& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const int& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const unsigned int& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const long& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const unsigned long& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const long long& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const unsigned long long& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const float& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const double& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::ostream& os, const long double& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const bool& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const char& value) { static_cast<C*>(this)->writeValue(os, convertToWChar(value)); }
        virtual void write(std::wostream& os, const unsigned char& value) { static_cast<C*>(this)->writeValue(os, convertToWChar((char)value)); }
        virtual void write(std::wostream& os, const wchar_t& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const short& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const unsigned short& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const int& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const unsigned int& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const long& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const unsigned long& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const long long& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const unsigned long long& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const float& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const double& value) { static_cast<C*>(this)->writeValue(os, value); }
        virtual void write(std::wostream& os, const long double& value) { static_cast<C*>(this)->writeValue(os, value); }
        /** Arrays of basic types **/
        virtual void write(std::ostream& os, const bool* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const char* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const unsigned char* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const wchar_t* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const short* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const unsigned short* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const int* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const unsigned int* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const long* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const unsigned long* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const long long* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const unsigned long long* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const float* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const double* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const long double* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const bool* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const char* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const unsigned char* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const wchar_t* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const short* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const unsigned short* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const int* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const unsigned int* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const long* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const unsigned long* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const long long* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const unsigned long long* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const float* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const double* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const long double* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        /** Strings **/
        virtual void write(std::ostream& os, const std::string& string) { static_cast<C*>(this)->writeString(os, string); }
        virtual void write(std::ostream& os, const std::wstring& string) { static_cast<C*>(this)->writeString(os, convertToString(string)); }
        virtual void write(std::wostream& os, const std::string& string) { static_cast<C*>(this)->writeString(os, convertToWString(string)); }
        virtual void write(std::wostream& os, const std::wstring& string) { static_cast<C*>(this)->writeString(os, string); }
        /** Arrays of strings **/
        virtual void write(std::ostream& os, const std::string* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::ostream& os, const std::wstring* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const std::string* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        virtual void write(std::wostream& os, const std::wstring* vector, const size_t& size) { static_cast<C*>(this)->writeArray(os, vector, size); }
        /** Serializable STL container **/
        virtual void write(std::ostream& os, const SerializableSTLContainer& container) { static_cast<C*>(this)->writeSTLContainer(os, container); }
        virtual void write(std::wostream& os, const SerializableSTLContainer& container) { static_cast<C*>(this)->writeSTLContainer(os, container); }
        /** Serializable classes **/
        virtual void write(std::ostream& os, const Serializable& object) { static_cast<C*>(this)->writeSerializable(os, object); }
        virtual void write(std::wostream& os, const Serializable& object) { static_cast<C*>(this)->writeSerializable(os, object); }
    };

    /* -- DESERIALIZER IMPLEMENTATION -- */
    template<class C>
    class DeserializerImpl : public Deserializer
    {
    public:
        /** Null values **/
        virtual void read(std::istream& is) { static_cast<C*>(this)->readNull(is); }
        virtual void read(std::wistream& is) { static_cast<C*>(this)->readNull(is); }
        /** Basic types **/
        virtual void read(std::istream& is, bool& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, char& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, unsigned char& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, wchar_t& value) 
        {
            char tmp;
            static_cast<C*>(this)->readValue(is, tmp); 
            value = convertToWChar(tmp);
        }
        virtual void read(std::istream& is, short& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, unsigned short& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, int& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, unsigned int& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, long& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, unsigned long& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, long long& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, unsigned long long& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, float& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, double& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::istream& is, long double& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, bool& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, char& value) 
        {
            wchar_t tmp;
            static_cast<C*>(this)->readValue(is, tmp); 
            value = convertToChar(tmp);
        }
        virtual void read(std::wistream& is, unsigned char& value) 
        { 
            wchar_t tmp;
            static_cast<C*>(this)->readValue(is, tmp);
            value = (unsigned char)convertToChar(tmp);
        }
        virtual void read(std::wistream& is, wchar_t& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, short& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, unsigned short& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, int& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, unsigned int& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, long& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, unsigned long& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, long long& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, unsigned long long& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, float& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, double& value) { static_cast<C*>(this)->readValue(is, value); }
        virtual void read(std::wistream& is, long double& value) { static_cast<C*>(this)->readValue(is, value); }
        /** Arrays of basic types **/
        virtual size_t read(std::istream& is, bool* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, char* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, unsigned char* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, wchar_t* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, short* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, unsigned short* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, int* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, unsigned int* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, long* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, unsigned long* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, long long* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, unsigned long long* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, float* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, double* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, long double* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, bool* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, char* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, unsigned char* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, wchar_t* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, short* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, unsigned short* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, int* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, unsigned int* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, long* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, unsigned long* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, long long* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, unsigned long long* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, float* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, double* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, long double* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        /** Strings **/
        virtual void read(std::istream& is, std::string& string) { static_cast<C*>(this)->readString(is, string); }
        virtual void read(std::istream& is, std::wstring& string)
        { 
            std::string tmp;
            static_cast<C*>(this)->readString(is, tmp);
            string = convertToWString(tmp);
        }
        virtual void read(std::wistream& is, std::string& string)
        { 
            std::wstring tmp;
            static_cast<C*>(this)->readString(is, tmp); 
            string = convertToString(tmp);
        }
        virtual void read(std::wistream& is, std::wstring& string) { static_cast<C*>(this)->readString(is, string); }
        /** Arrays of strings **/
        virtual size_t read(std::istream& is, std::string* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::istream& is, std::wstring* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, std::string* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        virtual size_t read(std::wistream& is, std::wstring* vector, const size_t& size) { return static_cast<C*>(this)->readArray(is, vector, size); }
        /** Serializable STL container **/
        virtual void read(std::istream& is, SerializableSTLContainer& container) { return static_cast<C*>(this)->readSTLContainer(is, container); }
        virtual void read(std::wistream& is, SerializableSTLContainer& container) { return static_cast<C*>(this)->readSTLContainer(is, container); }
        /** Serializable classes **/
        virtual void read(std::istream& is, Serializable& object) { static_cast<C*>(this)->readSerializable(is, object); }
        virtual void read(std::wistream& is, Serializable& object) { static_cast<C*>(this)->readSerializable(is, object); }
    };

    /* -- CHARACTER CONVERSION UTILS -- */

    /** This function convert a char to a wchar_t **/
    inline wchar_t convertToWChar(const char& c)
    {
        wchar_t tmp;
        mbstowcs(&tmp, &c, 1);
        return tmp;
    }

    /** This function convert a wchar_t to a char. 
    Notice out of range wchar_t are represented as MAX_CHAR **/
    inline char convertToChar(const wchar_t& c)
    {
        char tmp;
        int length = wctomb(&tmp, c);
        if(length == -1)
            return std::numeric_limits<char>::max();
        return tmp; 
    }

    /** This function convert a string into a wstring **/
    inline std::wstring convertToWString(const std::string& str)
    {
        std::wstring tmp(str.begin(), str.end());
        return tmp;
    }

    /** This function convert a Wstring into a string **/
    inline std::string convertToString(const std::wstring& str)
    {
        std::string tmp(str.begin(), str.end());
        return tmp;
    }

    /** -- SOME SERIALIZE CLASS METHODS -- */
    inline void Serializable::serializeElemName(Serializer* sez, std::ostream& os) const 
    { 
        sez->write(os, _it->first); 
    }

    inline void Serializable::serializeElemName(Serializer* sez, std::wostream& os) const 
    { 
        sez->write(os, _it->first); 
    }

    inline void Serializable::serializeElemValue(Serializer* sez, std::ostream& os) const 
    { 
        _it->second->serializeElem(sez, os); 
    }

    inline void Serializable::serializeElemValue(Serializer* sez, std::wostream& os) const 
    { 
        _it->second->serializeElem(sez, os); 
    }

    inline bool Serializable::deserializeElemName(Deserializer* dez, std::istream& is) const
    {
        std::string memberName;
        dez->read(is, memberName);

        if((_it = _members.find(memberName)) != _members.end())
            return true;
    
        return false;
    }

    inline bool Serializable::deserializeElemName(Deserializer* dez, std::wistream& is) const
    {
        std::string memberName;
        dez->read(is, memberName);

        if((_it = _members.find(memberName)) != _members.end())
            return true;
    
        return false;
    }

    inline void Serializable::deserializeElemValue(Deserializer* dez, std::istream& is) const
    {
        _it->second->deserializeElem(dez, is);
    }

    inline void Serializable::deserializeElemValue(Deserializer* dez, std::wistream& is) const
    {
        _it->second->deserializeElem(dez, is);
    }
}
