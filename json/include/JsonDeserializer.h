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

#include <iomanip>

#include "Seza.h"
#include "JsonDefinitions.h"

class JsonException : std::exception 
{
public:
  const char* what() const { return "Invalid JSON format!\n"; }
};

class JsonDeserializer : public Seza::DeserializerImpl<JsonDeserializer>
{
protected:
    friend class Seza::DeserializerImpl<JsonDeserializer>;

    // null
    void readNull(std::istream& is)
    {
        char value[5];
        is.read(value, 4);
        value[4] = '\0';

        if(std::string(value) != "null")
            throw JsonException();
    }

    void readNull(std::wistream& is)
    {
        wchar_t value[5];
        is.read(value, 4);
        value[4] = '\0';

        if(std::wstring(value) != L"null")
            throw JsonException();
    }

    // Values
    template<typename S, typename T> 
    void readValue(S& is, T& value)
    {
        is >> std::boolalpha >> value;
    }

    // String
    template<typename T> 
    void readString(std::istream& is, T& value)
    {
        while(is.peek() != JSON::quotationMark) // Search begin string
            is.ignore(1);
        is.ignore(1);
        
        std::getline(is, value, JSON::quotationMark); // Copy until quotation mark
    }

    template<typename T> 
    void readString(std::wistream& is, T& value)
    {
        while(is.peek() != JSON::quotationMark) // Search begin string
            is.ignore(1);
        is.ignore(1);
        
        std::getline(is, value, Seza::convertToWChar(JSON::quotationMark)); // Copy until quotation mark
    }

    // Arrays
    template<typename T> 
    size_t readArray(std::istream& is, T* vector, const size_t& size)
    {
        size_t length;
        char c;

        is >> c;
        if(c != JSON::beginArray)
            throw new JsonException();

        c = JSON::elementSeparator;
        
        for(length = 0; ((length < size) && (c != JSON::endArray)); ++length)
        {
            if(c != JSON::elementSeparator)
                throw new JsonException();

            this->read(is, vector[length]);
            is >> c;
        }

        if(c != JSON::endArray)
            throw new JsonException();

        return length;
    }

    template<typename T> 
    size_t readArray(std::wistream& is, T* vector, const size_t& size)
    {
        size_t length;
        wchar_t c;

        is >> c;
        if(c != JSON::beginArray)
            throw new JsonException();

        c = JSON::elementSeparator;
        
        for(length = 0; ((length < size) && (c != JSON::endArray)); ++length)
        {
            if(c != JSON::elementSeparator)
                throw new JsonException();

            this->read(is, vector[length]);
            is >> c;
        }

        if(c != JSON::endArray)
            throw new JsonException();

        return length;
    }
    
    // STL containers
    void readSTLContainer(std::istream& is, Seza::SerializableSTLContainer& container)
    {
        char c;
        is >> c;

        if(c != JSON::beginArray)
            throw new JsonException();

        c = JSON::elementSeparator;

        while((c != JSON::endArray) && (c != EOF))
        {
            if(c != JSON::elementSeparator)
                throw new JsonException();

            container.deserializeElem(this, is);
            is >> c;
        }

        if(c != JSON::endArray)
            throw new JsonException();
    }

    void readSTLContainer(std::wistream& is, Seza::SerializableSTLContainer& container)
    {
        wchar_t c;
        is >> c;

        if(c != JSON::beginArray)
            throw new JsonException();

        c = JSON::elementSeparator;

        while((c != JSON::endArray) && (c != EOF))
        {
            if(c != JSON::elementSeparator)
                throw new JsonException();

            container.deserializeElem(this, is);
            is >> c;
        }

        if(c != JSON::endArray)
            throw new JsonException();
    }
    // Serializable class
    void readSerializable(std::istream& is, const Seza::Serializable& object)
    {
        char c;
        is >> c;

        if(c != JSON::beginObject)
            throw new JsonException();

        std::string className;
        readString(is, className);

        if(className != "_className_")
            throw new JsonException();

        is >> c;
        if(c != JSON::valueSeparator)
            throw new JsonException();

        readString(is, className);

        if(className != object.getClassName())
            throw new JsonException();

        c = JSON::elementSeparator;
        is >> c;

        while((c != JSON::endObject) && (c != EOF))
        {
            if(c != JSON::elementSeparator)
                throw new JsonException();

            if(!object.deserializeElemName(this, is))
                throw new JsonException();

            is >> c;
            if(c != JSON::valueSeparator)
                throw new JsonException();

            object.deserializeElemValue(this, is);
            is >> c;
        }

        if(c != JSON::endObject)
            throw new JsonException();
    }

    void readSerializable(std::wistream& is, const Seza::Serializable& object)
    {
        wchar_t c;
        is >> c;

        if(c != JSON::beginObject)
            throw new JsonException();

        std::wstring className;
        readString(is, className);

        if(className != L"_className_")
            throw new JsonException();

        is >> c;
        if(c != JSON::valueSeparator)
            throw new JsonException();

        readString(is, className);

        if(className != Seza::convertToWString(object.getClassName()))
            throw new JsonException();

        c = JSON::elementSeparator;
        is >> c;

        while((c != JSON::endObject) && (c != EOF))
        {
            if(c != JSON::elementSeparator)
                throw new JsonException();

            if(!object.deserializeElemName(this, is))
                throw new JsonException();

            is >> c;
            if(c != JSON::valueSeparator)
                throw new JsonException();

            object.deserializeElemValue(this, is);
            is >> c;
        }

        if(c != JSON::endObject)
            throw new JsonException();
    }
};
