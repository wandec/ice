// **********************************************************************
//
// Copyright (c) 2003-present ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceSSL/OpenSSLUtil.h>

#include <openssl/err.h>
//
// Avoid old style cast warnings from OpenSSL macros
//
#if defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

using namespace std;

namespace
{

#ifndef OPENSSL_NO_DH

//
// The following arrays are predefined Diffie Hellman group parameters.
// It is highly recommeded that applications provide their own Diffie
// Hellman parameters.
//

static DH*
convertDH(const unsigned char* buf, int len)
{
    return d2i_DHparams(0, &buf, len);
}

unsigned char dh512[] =
{
  0x30, 0x46, 0x02, 0x41, 0x00, 0xa0, 0xf5, 0x3d, 0x77, 0xbd, 0x03, 0x3e,
  0xa2, 0x45, 0xa6, 0xa5, 0xe7, 0x5a, 0xfd, 0xad, 0x05, 0x1e, 0x8b, 0x07,
  0x63, 0x2a, 0x6a, 0xe7, 0xab, 0x91, 0xec, 0xaf, 0x2a, 0x48, 0x96, 0xda,
  0xc5, 0x29, 0x47, 0x8e, 0xf7, 0x29, 0x0b, 0x9c, 0x1d, 0x0e, 0x2d, 0xdb,
  0x1e, 0xff, 0x92, 0x09, 0xdf, 0x9f, 0xf7, 0x28, 0xcd, 0xe3, 0x86, 0x06,
  0x78, 0x9d, 0xa6, 0xe2, 0x32, 0x43, 0x2d, 0x8d, 0xeb, 0x02, 0x01, 0x02
};

unsigned char dh1024[] =
{
  0x30, 0x81, 0x87, 0x02, 0x81, 0x81, 0x00, 0xc4, 0x74, 0xb0, 0x83, 0x04,
  0x83, 0x17, 0x7b, 0x41, 0xb1, 0xff, 0xe8, 0xd7, 0x3e, 0x0e, 0xa4, 0xd9,
  0xdb, 0x44, 0x47, 0xc9, 0xf7, 0xe7, 0x46, 0x75, 0xcd, 0x0d, 0x05, 0x8a,
  0xde, 0x9f, 0x35, 0x67, 0xa3, 0x46, 0x95, 0xa2, 0x54, 0x59, 0xd4, 0x04,
  0xeb, 0x5a, 0x30, 0x74, 0xab, 0xc2, 0x3c, 0x6d, 0xaf, 0x62, 0x70, 0x97,
  0xf2, 0x2a, 0xd3, 0xff, 0xe3, 0x93, 0xf1, 0x4c, 0x51, 0x41, 0x48, 0x5b,
  0x9e, 0x72, 0xf9, 0x2c, 0xe2, 0x3c, 0xd8, 0x44, 0x99, 0xec, 0xf6, 0x90,
  0x31, 0x9f, 0xe5, 0x15, 0x86, 0xd3, 0x96, 0x87, 0x96, 0x1e, 0x16, 0xc1,
  0x71, 0xd1, 0x31, 0xa0, 0x3a, 0xcb, 0xf0, 0xab, 0xe5, 0x8b, 0x81, 0x93,
  0x98, 0x6d, 0xfc, 0x4e, 0x38, 0x68, 0xf5, 0x2c, 0x90, 0x13, 0x80, 0xa4,
  0x23, 0xe1, 0x6b, 0xb9, 0x02, 0x7e, 0x77, 0x45, 0x24, 0xdf, 0x89, 0x81,
  0xb3, 0xae, 0x1b, 0x02, 0x01, 0x02
};

unsigned char dh2048[] =
{
  0x30, 0x82, 0x01, 0x08, 0x02, 0x82, 0x01, 0x01, 0x00, 0x81, 0x36, 0x57,
  0x4d, 0x5b, 0xbe, 0xe6, 0x70, 0x6d, 0xd7, 0x1f, 0x69, 0x85, 0x29, 0x4a,
  0x3c, 0xe2, 0x60, 0x73, 0x61, 0x82, 0x57, 0xd0, 0xd0, 0x13, 0x93, 0xf4,
  0xeb, 0x29, 0x9c, 0xfb, 0x35, 0x55, 0x39, 0x47, 0x8d, 0x26, 0xa7, 0x6b,
  0x2c, 0x40, 0xc1, 0xd0, 0x69, 0xf6, 0xc0, 0x0e, 0x0e, 0x5c, 0x6e, 0xae,
  0xab, 0x40, 0x4d, 0xdf, 0x3a, 0xfb, 0x21, 0x47, 0x04, 0xbb, 0x10, 0x40,
  0x51, 0xaf, 0x7d, 0x6a, 0xc7, 0x1d, 0x1f, 0x0d, 0xbd, 0x34, 0xa7, 0x48,
  0xff, 0x9e, 0xc2, 0x70, 0x3a, 0x00, 0x68, 0x0f, 0x61, 0xd8, 0x80, 0xeb,
  0x73, 0x4b, 0xe3, 0x02, 0x9b, 0xbb, 0x9c, 0x29, 0x23, 0x44, 0xb2, 0xaa,
  0x73, 0xea, 0x56, 0xee, 0x46, 0xa3, 0x70, 0xe1, 0xd6, 0x8b, 0x6c, 0x5f,
  0xe1, 0xce, 0x57, 0x21, 0xe3, 0xd3, 0x35, 0x3d, 0xa9, 0xe1, 0x89, 0x64,
  0x52, 0x02, 0xbb, 0x44, 0x52, 0x04, 0xba, 0x22, 0x70, 0x56, 0x52, 0x6b,
  0x95, 0x85, 0xa8, 0x3e, 0x21, 0x6a, 0xbf, 0x5b, 0x22, 0x1c, 0x6c, 0x49,
  0xbb, 0xf5, 0x97, 0x38, 0x33, 0xef, 0x7e, 0xf8, 0x37, 0x73, 0xd8, 0x4a,
  0xd5, 0xfe, 0xfd, 0xb7, 0x12, 0x62, 0xe1, 0xa9, 0xdd, 0x98, 0xc0, 0xa0,
  0x0f, 0xf8, 0x52, 0x7a, 0xfe, 0x50, 0xd5, 0xa6, 0x18, 0xf1, 0x93, 0x90,
  0xa4, 0x4d, 0x48, 0x65, 0xac, 0x93, 0xbf, 0xeb, 0xaa, 0xbf, 0x46, 0xa2,
  0x2f, 0xc6, 0xe7, 0xef, 0x0d, 0xec, 0x07, 0x39, 0x71, 0x98, 0xc7, 0xe3,
  0x24, 0xac, 0x81, 0x0f, 0xe2, 0x9a, 0xaf, 0x7e, 0x51, 0x54, 0x15, 0x09,
  0xed, 0x47, 0x64, 0xfe, 0x0b, 0xaf, 0x9a, 0x5f, 0x36, 0xa2, 0x0c, 0x36,
  0x2b, 0x59, 0x58, 0x88, 0xa2, 0xf6, 0xa0, 0x5c, 0x4a, 0x9f, 0x58, 0xf2,
  0x51, 0x47, 0x39, 0xdd, 0xc8, 0xd1, 0x77, 0xd3, 0x5f, 0x3f, 0x73, 0x1b,
  0xa3, 0x02, 0x01, 0x02
};

unsigned char dh4096[] =
{
  0x30, 0x82, 0x02, 0x08, 0x02, 0x82, 0x02, 0x01, 0x00, 0x9e, 0x50, 0x2b,
  0x53, 0x42, 0x4d, 0x05, 0xa8, 0xbf, 0xd0, 0x22, 0x4d, 0xd3, 0xb1, 0x54,
  0x01, 0x0e, 0xab, 0xe6, 0x6e, 0x65, 0xf4, 0xb2, 0x1f, 0x95, 0x6a, 0xc9,
  0xe7, 0xf5, 0x9c, 0x14, 0xe9, 0x43, 0xa3, 0x96, 0x5f, 0x2e, 0x3a, 0x5e,
  0x85, 0x4d, 0x2a, 0xaa, 0xe3, 0xf9, 0x4a, 0x46, 0x14, 0xab, 0xc6, 0x6e,
  0x15, 0x6b, 0x63, 0x1d, 0xed, 0x34, 0x73, 0x54, 0xf3, 0xa6, 0x79, 0xcb,
  0xb8, 0x67, 0x9e, 0xf8, 0xb2, 0x9a, 0x27, 0x6e, 0x61, 0x51, 0xaf, 0x37,
  0xf4, 0x67, 0xad, 0x14, 0x79, 0x53, 0x62, 0x0c, 0xf4, 0x26, 0x14, 0x92,
  0xf8, 0x30, 0x32, 0xe2, 0x36, 0x85, 0x4e, 0x80, 0x49, 0xc2, 0x95, 0x4f,
  0x92, 0x18, 0x04, 0x8c, 0x20, 0xa3, 0xf0, 0x3a, 0x04, 0xc5, 0x6c, 0xf0,
  0xc7, 0xd4, 0xb8, 0xd9, 0xaf, 0xeb, 0x46, 0xe2, 0x1d, 0xbc, 0xf7, 0xa5,
  0x49, 0xf6, 0xa0, 0x33, 0xf9, 0x0d, 0x6a, 0xcc, 0x4d, 0xec, 0xee, 0xc6,
  0xd5, 0xd7, 0xf3, 0x97, 0x05, 0x8c, 0x56, 0x65, 0xfa, 0x73, 0xae, 0x47,
  0x02, 0xcd, 0x89, 0xc1, 0xef, 0xe4, 0x15, 0x61, 0x6e, 0x8e, 0x49, 0xfa,
  0x5f, 0xa4, 0x3c, 0x07, 0x06, 0xb8, 0x5f, 0x16, 0xa5, 0x99, 0xa7, 0x0e,
  0xa2, 0xb1, 0x6a, 0xa6, 0x57, 0x96, 0x52, 0xe2, 0x23, 0x4b, 0x06, 0xb5,
  0xe1, 0x8c, 0x98, 0x4f, 0x19, 0xb4, 0x2b, 0x48, 0xdf, 0xb8, 0xb0, 0xcd,
  0xa2, 0x13, 0xba, 0xc7, 0xbf, 0x79, 0xf3, 0x5e, 0xe0, 0x49, 0xb8, 0x8e,
  0x28, 0x4b, 0x10, 0xd9, 0x6c, 0x87, 0x84, 0xf8, 0xde, 0x70, 0x47, 0x82,
  0x61, 0x0c, 0x1c, 0xb0, 0xf8, 0x55, 0xde, 0xd4, 0xb8, 0xe8, 0x96, 0xa9,
  0x2c, 0x0d, 0xcd, 0x29, 0x19, 0x4e, 0x15, 0x4a, 0xaa, 0xa0, 0x58, 0xfb,
  0xd6, 0x20, 0xbd, 0x24, 0x52, 0x2c, 0x3f, 0x18, 0xab, 0x9d, 0x39, 0x21,
  0xf5, 0x4c, 0xa5, 0x47, 0x72, 0x10, 0xce, 0x7b, 0x27, 0x94, 0x0f, 0x58,
  0xe7, 0x3a, 0x2c, 0x8a, 0xf0, 0x8c, 0x1e, 0x56, 0xdc, 0x36, 0xa7, 0x40,
  0x17, 0xab, 0x8e, 0x8c, 0x34, 0xe2, 0x5d, 0xb2, 0xde, 0x95, 0x5e, 0x90,
  0x03, 0x1c, 0xc8, 0x12, 0x0a, 0x83, 0x25, 0x25, 0x8b, 0xdd, 0x49, 0x3a,
  0x6b, 0x57, 0x12, 0x54, 0xe1, 0x3c, 0xe8, 0x7e, 0x23, 0x8c, 0x4c, 0x5f,
  0x2d, 0x4c, 0xe8, 0x6e, 0x31, 0x8f, 0x9d, 0xb8, 0x2e, 0x30, 0xf4, 0x7f,
  0x8a, 0x1f, 0xc9, 0x1d, 0x12, 0xe0, 0x86, 0x5d, 0x6c, 0x3d, 0xcf, 0x35,
  0x30, 0x2f, 0xcb, 0xac, 0x86, 0xaf, 0x5c, 0x00, 0x72, 0xac, 0x88, 0xe8,
  0x52, 0x9a, 0x0b, 0x93, 0xe7, 0x66, 0xae, 0x6e, 0x49, 0x82, 0x2c, 0x92,
  0x18, 0x9c, 0x91, 0x86, 0x4f, 0xc9, 0x3c, 0x23, 0x2e, 0xff, 0xff, 0xe5,
  0xec, 0x1a, 0x8f, 0x9a, 0x54, 0x5b, 0x42, 0xb4, 0x1e, 0x63, 0x63, 0xed,
  0x99, 0x5c, 0x51, 0xe2, 0x02, 0xed, 0x18, 0x81, 0x25, 0xc6, 0xc6, 0xaf,
  0x62, 0xf6, 0x09, 0x9b, 0xd4, 0x36, 0x34, 0x42, 0x0d, 0x11, 0xe9, 0x95,
  0x31, 0x0c, 0x71, 0x43, 0xd8, 0x24, 0xf2, 0xe2, 0x1e, 0x17, 0xc1, 0xae,
  0x06, 0x13, 0x1d, 0xde, 0xbf, 0xe6, 0x62, 0x1a, 0xc2, 0x0f, 0x2d, 0xf5,
  0x5c, 0xf3, 0x09, 0x62, 0x6f, 0x64, 0x3b, 0xad, 0x33, 0xce, 0xd3, 0xbd,
  0x4c, 0x2c, 0x44, 0x1e, 0x16, 0x06, 0x1e, 0xdb, 0x01, 0xa1, 0xd7, 0x82,
  0x83, 0xa3, 0x39, 0x0b, 0xc7, 0x61, 0x6c, 0xa6, 0x23, 0xfb, 0x40, 0xa0,
  0x3b, 0xdf, 0x35, 0xab, 0x4f, 0xed, 0x04, 0x16, 0x42, 0x6c, 0x8f, 0x5b,
  0x9a, 0xcd, 0x07, 0x03, 0xca, 0x35, 0x42, 0x58, 0xef, 0xca, 0xcd, 0x6d,
  0x8b, 0xd9, 0x96, 0x43, 0xa2, 0x13, 0x18, 0xc1, 0x21, 0x50, 0xc2, 0xf5,
  0xab, 0x6b, 0xdc, 0xf4, 0x5b, 0x02, 0x01, 0x02
};

}

IceSSL::OpenSSL::DHParams::DHParams() :
    _dh512(0), _dh1024(0), _dh2048(0), _dh4096(0)
{
}

IceSSL::OpenSSL::DHParams::~DHParams()
{
    for(ParamList::iterator p = _params.begin(); p != _params.end(); ++p)
    {
        DH_free(p->second);
    }
    DH_free(_dh512);
    DH_free(_dh1024);
    DH_free(_dh2048);
    DH_free(_dh4096);
}

bool
IceSSL::OpenSSL::DHParams::add(int keyLength, const string& file)
{
    BIO* bio = BIO_new(BIO_s_file());
    if(BIO_read_filename(bio, file.c_str()) <= 0)
    {
        BIO_free(bio);
        return false;
    }
    DH* dh = PEM_read_bio_DHparams(bio, 0, 0, 0);
    BIO_free(bio);
    if(!dh)
    {
        return false;
    }
    ParamList::iterator p = _params.begin();
    while(p != _params.end() && keyLength > p->first)
    {
        ++p;
    }
    _params.insert(p, KeyParamPair(keyLength, dh));
    return true;
}

DH*
IceSSL::OpenSSL::DHParams::get(int keyLength)
{
    //
    // First check the set of parameters specified by the user.
    // Return the first set whose key length is at least keyLength.
    //
    for(ParamList::iterator p = _params.begin(); p != _params.end(); ++p)
    {
        if(p->first >= keyLength)
        {
            return p->second;
        }
    }

    //
    // No match found. Use one of the predefined parameter sets instead.
    //
    IceUtil::Mutex::Lock sync(*this);
    if(keyLength >= 4096)
    {
        if(!_dh4096)
        {
            _dh4096 = convertDH(dh4096, (int) sizeof(dh4096));
        }
        return _dh4096;
    }
    else if(keyLength >= 2048)
    {
        if(!_dh2048)
        {
            _dh2048 = convertDH(dh2048, (int) sizeof(dh2048));
        }
        return _dh2048;
    }
    else if(keyLength >= 1024)
    {
        if(!_dh1024)
        {
            _dh1024 = convertDH(dh1024, (int) sizeof(dh1024));
        }
        return _dh1024;
    }
    else
    {
        if(!_dh512)
        {
            _dh512 = convertDH(dh512, (int) sizeof(dh512));
        }
        return _dh512;
    }
}

#endif

string
IceSSL::OpenSSL::getSslErrors(bool verbose)
{
    ostringstream ostr;

    const char* file;
    const char* data;
    int line;
    int flags;
    unsigned long err;
    int count = 0;
    while((err = ERR_get_error_line_data(&file, &line, &data, &flags)) != 0)
    {
        if(count > 0)
        {
            ostr << endl;
        }

        if(verbose)
        {
            if(count > 0)
            {
                ostr << endl;
            }

            char buf[200];
            ERR_error_string_n(err, buf, sizeof(buf));

            ostr << "error # = " << err << endl;
            ostr << "message = " << buf << endl;
            ostr << "location = " << file << ", " << line;
            if(flags & ERR_TXT_STRING)
            {
                ostr << endl;
                ostr << "data = " << data;
            }
        }
        else
        {
            const char* reason = ERR_reason_error_string(err);
            ostr << (reason == ICE_NULLPTR ? "unknown reason" : reason);
            if(flags & ERR_TXT_STRING)
            {
                ostr << ": " << data;
            }
        }

        ++count;
    }

    ERR_clear_error();

    return ostr.str();
}
