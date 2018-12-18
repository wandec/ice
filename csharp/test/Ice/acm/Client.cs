// **********************************************************************
//
// Copyright (c) 2003-present ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using Test;

namespace Ice
{
    namespace acm
    {
        public class Client : TestHelper
        {
            override public void run(string[] args)
            {
                var properties = createTestProperties(ref args);
                properties.setProperty("Ice.Warn.Connections", "0");
                using(var communicator = initialize(properties))
                {
                    AllTests.allTests(this);
                }
            }

            public static int Main(string[] args)
            {
                return TestDriver.runTest<Client>(args);
            }
        }
    }
}
