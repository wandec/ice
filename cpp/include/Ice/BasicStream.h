// **********************************************************************
//
// Copyright (c) 2001
// MutableRealms, Inc.
// Huntsville, AL, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef ICE_STREAM_H
#define ICE_STREAM_H

#include <Ice/InstanceF.h>
#include <Ice/ObjectF.h>
#include <Ice/ProxyF.h>
#include <Ice/Buffer.h>

//#define ICE_ACTIVE_STRING_INDIRECTION

namespace Ice
{

class UserException;

}

namespace IceInternal
{

class ICE_API BasicStream : public Buffer
{
public:

    BasicStream(const InstancePtr&);

    InstancePtr instance() const;

    void swap(BasicStream&);

    void resize(int);
    void reserve(int);

    void startWriteEncaps();
    void endWriteEncaps();
    void startReadEncaps();
    void endReadEncaps();
    void checkReadEncaps();
    Ice::Int getReadEncapsSize();
    void skipEncaps();

    void writeBlob(const std::vector<Ice::Byte>&);
    void readBlob(std::vector<Ice::Byte>&, Ice::Int);

    void write(Ice::Byte v) { b.push_back(v); }
    void write(const std::vector<Ice::Byte>&);
    void read(Ice::Byte&);
    void read(std::vector<Ice::Byte>&);

    void write(bool v) { b.push_back(static_cast<Ice::Byte>(v)); }
    void write(const std::vector<bool>&);
    void read(bool&);
    void read(std::vector<bool>&);

    void write(Ice::Short);
    void write(const std::vector<Ice::Short>&);
    void read(Ice::Short&);
    void read(std::vector<Ice::Short>&);

    void write(Ice::Int);
    void write(const std::vector<Ice::Int>&);
    void read(Ice::Int&);
    void read(std::vector<Ice::Int>&);

    void write(Ice::Long);
    void write(const std::vector<Ice::Long>&);
    void read(Ice::Long&);
    void read(std::vector<Ice::Long>&);

    void write(Ice::Float);
    void write(const std::vector<Ice::Float>&);
    void read(Ice::Float&);
    void read(std::vector<Ice::Float>&);

    void write(Ice::Double);
    void write(const std::vector<Ice::Double>&);
    void read(Ice::Double&);
    void read(std::vector<Ice::Double>&);

    void write(const std::string&);
    void write(const std::vector<std::string>&);
    void read(std::string&);
    void read(std::vector<std::string>&);

    void write(const std::wstring&);
    void write(const wchar_t*);
    void write(const std::vector<std::wstring>&);
    void read(std::wstring&);
    void read(std::vector<std::wstring>&);

    void write(const Ice::ObjectPrx&);
    void read(Ice::ObjectPrx&);

    void write(const Ice::ObjectPtr&);
    bool read(const std::string&, Ice::ObjectPtr&);
    void read(const Ice::ObjectPtr&);

    void write(const Ice::UserException&);
    ::Ice::Int throwException(const std::string*, const std::string*);

private:

    //
    // Optimization. The instance may not be deleted while a
    // stack-allocated Incoming still holds it.
    //
    const InstancePtr& _instance;

    struct ReadEncaps
    {
	Container::size_type start;
	Ice::Byte encoding;
	std::vector<const char*> stringsRead;
	std::vector<const wchar_t*> wstringsRead;
	std::vector<Ice::Object*> objectsRead;
    };

    struct WriteEncaps
    {
	Container::size_type start;
#ifdef ICE_ACTIVE_STRING_INDIRECTION
	std::map<const char*, Ice::Int> stringsWritten;
	std::map<const wchar_t*, Ice::Int> wstringsWritten;
#endif
	std::map<Ice::Object*, Ice::Int> objectsWritten;
    };

    std::vector<ReadEncaps> _readEncapsStack;
    std::vector<WriteEncaps> _writeEncapsStack;
    ReadEncaps* _currentReadEncaps;
    WriteEncaps* _currentWriteEncaps;
};

}

#endif
