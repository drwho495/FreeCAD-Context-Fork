/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef BASE_STREAM_H
#define BASE_STREAM_H

#ifdef __GNUC__
#include <cstdint>
#endif

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Swap.h"
#include "FileInfo.h"


class QByteArray;
class QIODevice;
class QBuffer;
using PyObject = struct _object;

namespace Base
{

class BaseExport Stream
{
public:
    enum ByteOrder
    {
        BigEndian,
        LittleEndian
    };

    ByteOrder byteOrder() const;
    void setByteOrder(ByteOrder);

protected:
    Stream();
    virtual ~Stream();

    Stream(const Stream&) = default;
    Stream(Stream&&) = default;
    Stream& operator=(const Stream&) = default;
    Stream& operator=(Stream&&) = default;

    bool isSwapped() const
    {
        return _swap;
    };

private:
    bool _swap {false};
};

/**
 * The OutputStream class provides writing of binary data to an ostream.
 * @author Werner Mayer
 */
class BaseExport OutputStream: public Stream
{
public:
    /** Constructor
     * @param rout: output downstream
     * @param binary: whether to output as text or not
     */
    explicit OutputStream(std::ostream &rout, bool binary=true);
    ~OutputStream() override;

    OutputStream& operator<<(bool b)
    {
        if(_binary) {
            _out.write((const char*)&b, sizeof(bool));
        }
        else {
            _out << b << '\n';
        }
        return *this;
    }

    OutputStream& operator<<(int8_t ch)
    {
        if(_binary) {
            _out.write((const char*)&ch, sizeof(int8_t));
        }
        else {
            _out << ch << '\n';
        }
        return *this;
    }

    OutputStream& operator<<(uint8_t uch)
    {
        if(_binary) {
            _out.write((const char*)&uch, sizeof(uint8_t));
        }
        else {
            _out << uch << '\n';
        }
        return *this;
    }

    OutputStream& operator<<(int16_t s)
    {
        if(_binary) {
            if (isSwapped()) SwapEndian<int16_t>(s);
            _out.write((const char*)&s, sizeof(int16_t));
        }
        else {
            _out << s << '\n';
        }
        return *this;
    }

    OutputStream& operator<<(uint16_t us)
    {
        if(_binary) {
            if (isSwapped()) {
                SwapEndian<uint16_t>(us);
            }
            _out.write((const char*)&us, sizeof(uint16_t));
        }
        else  {
            _out << us << '\n';
        }
        return *this;
    }

    OutputStream& operator<<(int32_t i)
    {
        if(_binary) {
            if (isSwapped()) {
                SwapEndian<int32_t>(i);
            }
            _out.write((const char*)&i, sizeof(int32_t));
        }
        else {
            _out << i << '\n';
        }
        return *this;
    }

    OutputStream& operator<<(uint32_t ui)
    {
        if(_binary) {
            if (isSwapped()) {
                SwapEndian<uint32_t>(ui);
            }
            _out.write((const char*)&ui, sizeof(uint32_t));
        }
        else {
            _out << ui << '\n';
        }
        return *this;
    }

    OutputStream& operator<<(int64_t l)
    {
        if(_binary) {
            if (isSwapped()) {
                SwapEndian<int64_t>(l);
            }
            _out.write((const char*)&l, sizeof(int64_t));
        }
        else {
            _out << l << '\n';
        }
        return *this;
    }

    OutputStream& operator<<(uint64_t ul)
    {
        if(_binary) {
            if (isSwapped()) {
                SwapEndian<uint64_t>(ul);
            }
            _out.write((const char*)&ul, sizeof(uint64_t));
        }
        else {
            _out << ul << '\n';
        }
        return *this;
    }

    OutputStream& operator<<(float f)
    {
        if(_binary) {
            if (isSwapped()) {
                SwapEndian<float>(f);
            }
            _out.write((const char*)&f, sizeof(float));
        }
        else {
            _out << f << '\n';
        }
        return *this;
    }

    OutputStream& operator<<(double d)
    {
        if(_binary) {
            if (isSwapped()) {
                SwapEndian<double>(d);
            }
            _out.write((const char*)&d, sizeof(double));
        }
        else {
            _out << d << '\n';
        }
        return *this;
    }

    OutputStream& operator<<(const char *s);

    OutputStream& operator<<(const std::string &s)
    {
        return (*this) << s.c_str();
    }

    OutputStream& operator<<(char c)
    {
        _out.put(c);
        return *this;
    }


    bool isBinary() const
    {
        return _binary;
    }

    OutputStream(const OutputStream&) = delete;
    OutputStream(OutputStream&&) = delete;
    void operator=(const OutputStream&) = delete;
    void operator=(OutputStream&&) = delete;

private:
    std::ostream& _out;
    bool _binary {false};
};

/**
 * The InputStream class provides reading of binary data from an istream.
 * @author Werner Mayer
 */
class BaseExport InputStream: public Stream
{
public:
    /** Constructor
     * @param rin: upstream input
     * @param binary: whether read the stream as text or not
     */
    explicit InputStream(std::istream &rin, bool binary=true);
    ~InputStream() override;

    InputStream& operator>>(bool& b) {
        if(_binary) {
            _in.read((char*)&b, sizeof(bool));
        }
        else {
            _in >> b;
        }
        return *this;
    }

    InputStream& operator>>(int8_t& ch) {
        if(_binary) {
            _in.read((char*)&ch, sizeof(int8_t));
        }
        else {
            int i;
            _in >> i;
            ch = (int8_t)i;
        }
        return *this;
    }

    InputStream& operator>>(uint8_t& uch) {
        if(_binary) {
            _in.read((char*)&uch, sizeof(uint8_t));
        }
        else {
            unsigned u;
            _in >> u;
            uch = (uint8_t)u;
        }
        return *this;
    }

    InputStream& operator>>(int16_t& s)
    {
        if(_binary) {
            _in.read((char*)&s, sizeof(int16_t));
            if (isSwapped()) {
                SwapEndian<int16_t>(s);
            }
        }
        else {
            _in >> s;
        }
        return *this;
    }

    InputStream& operator>>(uint16_t& us)
    {
        if(_binary) {
            _in.read((char*)&us, sizeof(uint16_t));
            if (isSwapped()) {
                SwapEndian<uint16_t>(us);
            }
        }
        else {
            _in >> us;
        }
        return *this;
    }

    InputStream& operator>>(int32_t& i)
    {
        if(_binary) {
            _in.read((char*)&i, sizeof(int32_t));
            if (isSwapped()) {
                SwapEndian<int32_t>(i);
            }
        }
        else {
            _in >> i;
        }
        return *this;
    }

    InputStream& operator>>(uint32_t& ui)
    {
        if(_binary) {
            _in.read((char*)&ui, sizeof(uint32_t));
            if (isSwapped()) {
                SwapEndian<uint32_t>(ui);
            }
        }
        else {
            _in >> ui;
        }
        return *this;
    }

    InputStream& operator>>(int64_t& l)
    {
        if(_binary) {
            _in.read((char*)&l, sizeof(int64_t));
            if (isSwapped()) {
                SwapEndian<int64_t>(l);
            }
        }
        else {
            _in >> l;
        }
        return *this;
    }

    InputStream& operator>>(uint64_t& ul)
    {
        if(_binary) {
            _in.read((char*)&ul, sizeof(uint64_t));
            if (isSwapped()) {
                SwapEndian<uint64_t>(ul);
            }
        }
        else {
            _in >> ul;
        }
        return *this;
    }

    InputStream& operator>>(float& f)
    {
        if(_binary) {
            _in.read((char*)&f, sizeof(float));
            if (isSwapped()) {
                SwapEndian<float>(f);
            }
        }
        else {
            _in >> f;
        }
        return *this;
    }

    InputStream& operator>>(double& d)
    {
        if(_binary) {
            _in.read((char*)&d, sizeof(double));
            if (isSwapped()) {
                SwapEndian<double>(d);
            }
        }
        else {
            _in >> d;
        }
        return *this;
    }

    InputStream& operator>>(std::string &s);

    InputStream& operator>>(char &c)
    {
        c = (char)_in.get();
        return *this;
    }

    explicit operator bool() const
    {
        // test if _Ipfx succeeded
        return !_in.eof();
    }

    bool isBinary() const
    {
        return _binary;
    }

    InputStream(const InputStream&) = delete;
    InputStream(InputStream&&) = delete;
    void operator=(const InputStream&) = delete;
    void operator=(InputStream&&) = delete;

private:
    std::istream& _in;
    std::ostringstream _ss;
    bool _binary {false};
};

// ----------------------------------------------------------------------------

/**
 * This class implements the streambuf interface to write data to a QByteArray.
 * This class can only be used for writing but not for reading purposes.
 * @author Werner Mayer
 */
class BaseExport ByteArrayOStreambuf: public std::streambuf
{
public:
    explicit ByteArrayOStreambuf(QByteArray& ba);
    ~ByteArrayOStreambuf() override;

protected:
    int_type overflow(std::streambuf::int_type c) override;
    std::streamsize xsputn(const char* s, std::streamsize num) override;
    pos_type seekoff(std::streambuf::off_type off,
                     std::ios_base::seekdir way,
                     std::ios_base::openmode which = std::ios::in | std::ios::out) override;
    pos_type seekpos(std::streambuf::pos_type pos,
                     std::ios_base::openmode which = std::ios::in | std::ios::out) override;

public:
    ByteArrayOStreambuf(const ByteArrayOStreambuf&) = delete;
    ByteArrayOStreambuf(ByteArrayOStreambuf&&) = delete;
    ByteArrayOStreambuf& operator=(const ByteArrayOStreambuf&) = delete;
    ByteArrayOStreambuf& operator=(ByteArrayOStreambuf&&) = delete;

private:
    QBuffer* _buffer;
};

/**
 * This class implements the streambuf interface to read data from a QByteArray.
 * This class can only be used for reading but not for writing purposes.
 * @author Werner Mayer
 */
class BaseExport ByteArrayIStreambuf: public std::streambuf
{
public:
    explicit ByteArrayIStreambuf(const QByteArray& data);
    ~ByteArrayIStreambuf() override;

protected:
    int_type uflow() override;
    int_type underflow() override;
    int_type pbackfail(int_type ch) override;
    std::streamsize showmanyc() override;
    pos_type seekoff(std::streambuf::off_type off,
                     std::ios_base::seekdir way,
                     std::ios_base::openmode which = std::ios::in | std::ios::out) override;
    pos_type seekpos(std::streambuf::pos_type pos,
                     std::ios_base::openmode which = std::ios::in | std::ios::out) override;

public:
    ByteArrayIStreambuf(const ByteArrayIStreambuf&) = delete;
    ByteArrayIStreambuf(ByteArrayIStreambuf&&) = delete;
    ByteArrayIStreambuf& operator=(const ByteArrayIStreambuf&) = delete;
    ByteArrayIStreambuf& operator=(ByteArrayIStreambuf&&) = delete;

private:
    const QByteArray& _buffer;
    int _beg, _end, _cur;
};

/**
 * Simple class to write data directly into Qt's QIODevice.
 * This class can only be used for writing but not reading purposes.
 * @author Werner Mayer
 */
class BaseExport IODeviceOStreambuf: public std::streambuf
{
public:
    explicit IODeviceOStreambuf(QIODevice* dev);
    ~IODeviceOStreambuf() override;

protected:
    int_type overflow(std::streambuf::int_type c) override;
    std::streamsize xsputn(const char* s, std::streamsize num) override;
    pos_type seekoff(std::streambuf::off_type off,
                     std::ios_base::seekdir way,
                     std::ios_base::openmode which = std::ios::in | std::ios::out) override;
    pos_type seekpos(std::streambuf::pos_type pos,
                     std::ios_base::openmode which = std::ios::in | std::ios::out) override;

public:
    IODeviceOStreambuf(const IODeviceOStreambuf&) = delete;
    IODeviceOStreambuf(IODeviceOStreambuf&&) = delete;
    IODeviceOStreambuf& operator=(const IODeviceOStreambuf&) = delete;
    IODeviceOStreambuf& operator=(IODeviceOStreambuf&&) = delete;

private:
    QIODevice* device;
};

/**
 * Simple class to read data directly from Qt's QIODevice.
 * This class can only be used for readihg but not writing purposes.
 * @author Werner Mayer
 */
class BaseExport IODeviceIStreambuf: public std::streambuf
{
public:
    explicit IODeviceIStreambuf(QIODevice* dev);
    ~IODeviceIStreambuf() override;

protected:
    int_type underflow() override;
    pos_type seekoff(std::streambuf::off_type off,
                     std::ios_base::seekdir way,
                     std::ios_base::openmode which = std::ios::in | std::ios::out) override;
    pos_type seekpos(std::streambuf::pos_type pos,
                     std::ios_base::openmode which = std::ios::in | std::ios::out) override;

public:
    IODeviceIStreambuf(const IODeviceIStreambuf&) = delete;
    IODeviceIStreambuf(IODeviceIStreambuf&&) = delete;
    IODeviceIStreambuf& operator=(const IODeviceIStreambuf&) = delete;
    IODeviceIStreambuf& operator=(IODeviceIStreambuf&&) = delete;

private:
    QIODevice* device;
    /* data buffer:
     * - at most, pbSize characters in putback area plus
     * - at most, bufSize characters in ordinary read buffer
     */
    static const int pbSize = 4;       // size of putback area
    static const int bufSize = 1024;   // size of the data buffer
    char buffer[bufSize + pbSize] {};  // data buffer
};

class BaseExport PyStreambuf: public std::streambuf
{
    using int_type = std::streambuf::int_type;
    using pos_type = std::streambuf::pos_type;
    using off_type = std::streambuf::off_type;
    using seekdir = std::ios::seekdir;
    using openmode = std::ios::openmode;

public:
    enum Type
    {
        StringIO,
        BytesIO,
        Unknown
    };

    explicit PyStreambuf(PyObject* o, std::size_t buf_size = 256, std::size_t put_back = 8);
    ~PyStreambuf() override;
    void setType(Type t)
    {
        type = t;
    }

protected:
    int_type underflow() override;
    int_type overflow(int_type c = EOF) override;
    std::streamsize xsputn(const char* s, std::streamsize num) override;
    int sync() override;
    pos_type seekoff(off_type offset, seekdir dir, openmode mode) override;
    pos_type seekpos(pos_type offset, openmode mode) override;

private:
    bool flushBuffer();
    bool writeStr(const char* s, std::streamsize num);

public:
    PyStreambuf(const PyStreambuf&) = delete;
    PyStreambuf(PyStreambuf&&) = delete;
    PyStreambuf& operator=(const PyStreambuf&) = delete;
    PyStreambuf& operator=(PyStreambuf&&) = delete;

private:
    PyObject* inp;
    Type type {Unknown};
    const std::size_t put_back;
    std::vector<char> buffer;
};

class BaseExport Streambuf: public std::streambuf
{
public:
    explicit Streambuf(const std::string& data);
    ~Streambuf() override;

protected:
    int_type uflow() override;
    int_type underflow() override;
    int_type pbackfail(int_type ch) override;
    std::streamsize showmanyc() override;
    pos_type seekoff(std::streambuf::off_type off,
                     std::ios_base::seekdir way,
                     std::ios_base::openmode which = std::ios::in | std::ios::out) override;
    pos_type seekpos(std::streambuf::pos_type pos,
                     std::ios_base::openmode which = std::ios::in | std::ios::out) override;

public:
    Streambuf(const Streambuf&) = delete;
    Streambuf(Streambuf&&) = delete;
    Streambuf& operator=(const Streambuf&) = delete;
    Streambuf& operator=(Streambuf&&) = delete;

private:
    std::string::const_iterator _beg;
    std::string::const_iterator _end;
    std::string::const_iterator _cur;
};

// ----------------------------------------------------------------------------

class FileInfo;

/**
 * The ofstream class is provided for convenience. On Windows
 * platforms it opens a stream with UCS-2 encoded file name
 * while on Linux platforms the file name is UTF-8 encoded.
 * @author Werner Mayer
 */
class ofstream: public std::ofstream
{
public:
    ofstream() = default;
    ofstream(const ofstream&) = delete;
    ofstream(ofstream&&) = delete;
    explicit ofstream(const FileInfo& fi, ios_base::openmode mode = std::ios::out | std::ios::trunc)
#ifdef _MSC_VER
        : std::ofstream(fi.toStdWString().c_str(), mode) {}
#else
        : std::ofstream(fi.filePath().c_str(), mode)
    {}
#endif
        ~ofstream() override = default;
    void open(const FileInfo& fi, ios_base::openmode mode = std::ios::out | std::ios::trunc)
    {
#ifdef _MSC_VER
        std::ofstream::open(fi.toStdWString().c_str(), mode);
#else
        std::ofstream::open(fi.filePath().c_str(), mode);
#endif
    }

    ofstream& operator=(const ofstream&) = delete;
    ofstream& operator=(ofstream&&) = delete;
};

/**
 * The ofstream class is provided for convenience. On Windows
 * platforms it opens a stream with UCS-2 encoded file name
 * while on Linux platforms the file name is UTF-8 encoded.
 * @author Werner Mayer
 */
class ifstream: public std::ifstream
{
public:
    ifstream() = default;
    ifstream(const ifstream&) = delete;
    ifstream(ifstream&&) = delete;
    explicit ifstream(const FileInfo& fi, ios_base::openmode mode = std::ios::in)
#ifdef _MSC_VER
        : std::ifstream(fi.toStdWString().c_str(), mode) {}
#else
        : std::ifstream(fi.filePath().c_str(), mode)
    {}
#endif
        ~ifstream() override = default;
    void open(const FileInfo& fi, ios_base::openmode mode = std::ios::in)
    {
#ifdef _MSC_VER
        std::ifstream::open(fi.toStdWString().c_str(), mode);
#else
        std::ifstream::open(fi.filePath().c_str(), mode);
#endif
    }

    ifstream& operator=(const ifstream&) = delete;
    ifstream& operator=(ifstream&&) = delete;
};

}  // namespace Base

#endif  // BASE_STREAM_H
