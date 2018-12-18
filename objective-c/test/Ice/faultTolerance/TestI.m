// **********************************************************************
//
// Copyright (c) 2003-present ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <objc/Ice.h>
#import <faultTolerance/TestI.h>

#include <unistd.h>

@implementation TestI
-(void) shutdown:(ICECurrent*)current
{
    [[current.adapter getCommunicator] shutdown];
}

-(void) abort:(ICECurrent*)__unused current
{
    exit(0);
}

-(void) idempotentAbort:(ICECurrent*)__unused current
{
    exit(0);
}

-(ICEInt) pid:(ICECurrent*)__unused current
{
    return getpid();
}
@end
