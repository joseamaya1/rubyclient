/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef INPUTMESSAGE_H
#define INPUTMESSAGE_H

#include "declarations.h"
#include <framework/luaengine/luaobject.h>

// @bindclass
class InputMessage : public LuaObject
{
public:
    enum {
        BUFFER_MAXSIZE = 65536,
        MAX_HEADER_SIZE = 8
    };

    InputMessage();

    void setBuffer(const std::string& buffer);
    std::string getBuffer() { return std::string((char*)m_buffer + m_headerPos, m_messageSize); }

    void skipBytes(uint16_t bytes) { m_readPos += bytes; }
    void setReadPos(uint16_t readPos) { m_readPos = readPos; }

    std::string getString();
    double getDouble();

    uint8_t getByte() {
		if (!canRead(1)) {
			return 0;
		}

        uint8_t v = m_buffer[m_readPos];
        m_readPos += 1;
        return v;
	}

    template<typename T>
	T get() {
		if (!canRead(sizeof(T))) {
			return 0;
		}

		T v;
		memcpy(&v, m_buffer + m_readPos, sizeof(T));
		m_readPos += sizeof(T);
		return v;
	}

    uint8_t peekByte() { uint8_t v = get<uint8_t>(); m_readPos-=1; return v; }
    uint16_t peekU16() { uint16_t v = get<uint16_t>(); m_readPos-=2; return v; }
    uint32_t peekU32() { uint32_t v = get<uint32_t>(); m_readPos-=4; return v; }
    uint64_t peekU64() { uint64_t v = get<uint64_t>(); m_readPos-=8; return v; }

    bool decryptRsa(int size);

    int getReadSize() { return m_readPos - m_headerPos; }
    int getReadPos() { return m_readPos; }
    int getUnreadSize() { return m_messageSize - (m_readPos - m_headerPos); }
    uint16_t getMessageSize() { return m_messageSize; }

    bool eof() { return (m_readPos - m_headerPos) >= m_messageSize; }

protected:
    void reset();
    void fillBuffer(uint8_t *buffer, uint16_t size);

    void setHeaderSize(uint16_t size);
    void setMessageSize(uint16_t size) { m_messageSize = size; }

    uint8_t* getReadBuffer() { return m_buffer + m_readPos; }
    uint8_t* getHeaderBuffer() { return m_buffer + m_headerPos; }
    uint8_t* getDataBuffer() { return m_buffer + MAX_HEADER_SIZE; }
    uint16_t getHeaderSize() { return (MAX_HEADER_SIZE - m_headerPos); }

    uint16_t readSize() { return get<uint16_t>(); }
    bool readChecksum();

    friend class Protocol;

private:
    bool canRead(int bytes);
    void checkRead(int bytes);
    void checkWrite(int bytes);

    uint16_t m_headerPos;
    uint16_t m_readPos;
    uint16_t m_messageSize;
    uint8_t m_buffer[BUFFER_MAXSIZE];
};

#endif
