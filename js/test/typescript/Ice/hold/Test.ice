// **********************************************************************
//
// Copyright (c) 2003-present ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#pragma once

[["js:es6-module"]]

module Test
{

interface Hold
{
    void putOnHold(int seconds);
    void waitForHold();
    void setOneway(int value, int expected);
    int set(int value, int delay);
    void shutdown();
}

}
