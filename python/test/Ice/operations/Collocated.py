#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-present ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

from TestHelper import TestHelper
TestHelper.loadSlice("Test.ice")
import Ice
import TestI
import AllTests


class Collocated(TestHelper):

    def run(self, args):
        properties = self.createTestProperties(args)
        properties.setProperty("Ice.BatchAutoFlushSize", "100")
        with self.initialize(properties=properties) as communicator:
            communicator.getProperties().setProperty("TestAdapter.Endpoints", self.getTestEndpoint())
            adapter = communicator.createObjectAdapter("TestAdapter")
            prx = adapter.add(TestI.MyDerivedClassI(), Ice.stringToIdentity("test"))
            # adapter.activate() // Don't activate OA to ensure collocation is used.
            if prx.ice_getConnection():
                raise RuntimeError("collocation doesn't work")
            AllTests.allTests(self, communicator)
