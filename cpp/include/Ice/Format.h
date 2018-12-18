// **********************************************************************
//
// Copyright (c) 2003-present ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_FORMAT_H
#define ICE_FORMAT_H

#include <Ice/Config.h>

namespace Ice
{

/**
 * Describes the possible formats for classes and exceptions.
 */
#ifdef ICE_CPP11_MAPPING
enum class FormatType : unsigned char
#else
enum FormatType
#endif
{
    /**
     * Indicates that no preference was specified.
     */
    DefaultFormat,
    /**
     * A minimal format that eliminates the possibility for slicing unrecognized types.
     */
    CompactFormat,
    /**
     * Allow slicing and preserve slices for unknown types.
     */
    SlicedFormat
};

}

#endif
