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

class JsonSerializer : public Seza::SerializerImpl<JsonSerializer>
{
protected:

    friend class Seza::SerializerImpl<JsonSerializer>;

    // null
    template<typename Stream>
    void writeNull(Stream& os)
    {
        os << "null";
    }

    // Values
    template<typename Stream, typename Type> 
    void writeValue(Stream& os, const Type& value)
    {
        os << std::boolalpha << std::setprecision(20) << value;
    }

    // Strings
    template<typename Stream, typename Type> 
    void writeString(Stream& os, const Type& value)
    {
        os << JSON::quotationMark <<value.c_str() << JSON::quotationMark;
    }

    // Arrays
    template<typename Stream, typename Type> 
    void writeArray(Stream& os, const Type* vector, const size_t& size)
    {
        os << JSON::beginArray;
        
        for(size_t i=0; i<size; ++i)
        {
            if(i > 0) 
                os << JSON::elementSeparator;

            this->write(os, vector[i]);
        }

        os << JSON::endArray;
    }

    // STL conatiners
    template<typename Stream>
    void writeSTLContainer(Stream& os, const Seza::SerializableSTLContainer& container)
    {
        os << JSON::beginArray;

        for(container.begin(); !container.isEnd(); container.next())
        {
            if(!container.isBegin()) 
                os << JSON::elementSeparator;

            container.serializeElem(this, os);
        }

        os << JSON::endArray;
    }

    // Serializable class
    template<typename Stream>
    void writeSerializable(Stream& os, const Seza::Serializable& object)
    {
        os << JSON::beginObject;
        writeString(os, std::string("_className_"));
        os << JSON::valueSeparator;
        writeString(os, std::string(object.getClassName()));

        for(object.begin(); !object.isEnd(); object.next())
        {
            os << JSON::elementSeparator;
            object.serializeElemName(this, os);
            os << JSON::valueSeparator;
            object.serializeElemValue(this, os);
        }

        os << JSON::endObject;
    }
};
