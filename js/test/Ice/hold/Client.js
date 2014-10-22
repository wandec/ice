// **********************************************************************
//
// Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, require, exports)
{
    var Ice = require("icejs").Ice;
    var Test = require("Test").Test;
    var Promise = Ice.Promise;

    function loop(fn, repetitions, condition)
    {
        var i = 0;
        var next = function()
        {
            while(i++ < repetitions && (!condition || condition.value))
            {
                var r = fn.call(i);
                if(r)
                {
                    return r.then(next);
                }
            }
        };
        return next();
    }

    var allTests = function(out, communicator)
    {
        var failCB = function() { test(false); };
        var hold, holdOneway, holdSerialized, holdSerializedOneway;
        var condition = { value: true };
        var value = 0;

        var p = new Promise();
        var test = function(b)
        {
            if(!b)
            {
                try
                {
                    throw new Error("test failed");
                }
                catch(err)
                {
                    p.fail(err);
                    throw err;
                }
            }
        };

        var seq;
        Promise.try(
            function()
            {
                var ref = "hold:default -p 12010";
                return Test.HoldPrx.checkedCast(communicator.stringToProxy(ref));
            }
        ).then(
            function(obj)
            {
                test(obj !== null);
                hold = obj;
                holdOneway = Test.HoldPrx.uncheckedCast(hold.ice_oneway());

                var refSerialized = "hold:default -p 12011";
                return Test.HoldPrx.checkedCast(communicator.stringToProxy(refSerialized));
            }
        ).then(
            function(obj)
            {
                test(obj !== null);
                holdSerialized = obj;
                holdSerializedOneway = Test.HoldPrx.uncheckedCast(holdSerialized.ice_oneway());

                out.write("changing state between active and hold rapidly... ");
                
                var i;
                var r = new Ice.Promise().succeed();
                for(i = 0; i < 1; ++i)
                {
                    r = r.then(function() { return hold.putOnHold(0); });
                }
                for(i = 0; i < 1; ++i)
                {
                    r = r.then(function() { return holdOneway.putOnHold(0); });
                }
                for(i = 0; i < 1; ++i)
                {
                    r = r.then(function() { return holdSerialized.putOnHold(0); });
                }
                for(i = 0; i < 1; ++i)
                {
                    r = r.then(function() { return holdSerializedOneway.putOnHold(0); });
                }
                return r;
            }
        ).then(
            function()
            {
                out.writeLine("ok");

                out.write("testing without serialize mode... ");
                var result = null;
                condition.value = true;
                return loop(function()
                            {
                                var expected = value;
                                var result = hold.set(value + 1, 3).then(function(v)
                                                                         {
                                                                             condition.value = (v == expected);
                                                                         });
                                ++value;
                                if(value % 100 === 0)
                                {
                                    return result;
                                }
                                return null;
                            }, 100000, condition);
            }
        ).then(
            function()
            {
                test(!condition.value);
                out.writeLine("ok");

                out.write("testing with serialize mode... ");

                condition.value = true;
                var result;
                return loop(
                    function()
                    {
                        var expected = value;
                        result = holdSerialized.set(value + 1, 1).then(function(v)
                                                                       {
                                                                           condition.value = (v == expected);
                                                                       });
                        ++value;
                        if(value % 100 === 0)
                        {
                            return result;
                        }
                        return null;
                    }, 1000, condition);
            }
        ).then(
            function()
            {
                test(condition.value);
                return loop(function()
                            {
                                holdSerializedOneway.setOneway(value + 1, value);
                                ++value;
                                if((value % 100) === 0)
                                {
                                    holdSerializedOneway.putOnHold(1);
                                }
                            }, 3000);
            }
        ).then(
            function()
            {
                out.writeLine("ok");

                out.write("testing serialization... ");

                condition.value = true;
                value = 0;
                return holdSerialized.set(value, 0);
            }
        ).then(
            function()
            {
                return loop(
                    function()
                    {
                        // Create a new proxy for each request
                        var result = holdSerialized.ice_oneway().setOneway(value + 1, value);
                        ++value;
                        if((value % 100) === 0)
                        {
                            return result.then(function()
                                               {
                                                   return holdSerialized.ice_getConnection().then(
                                                       function(con)
                                                       {
                                                           return con.close(false);
                                                       });
                                               });
                        }
                        return null;
                    }, 1000);
            }
        ).then(
            function()
            {
                out.writeLine("ok");

                out.write("testing waitForHold... ");

                return hold.waitForHold().then(
                    function()
                    {
                        return hold.waitForHold();
                    }
                ).then(
                    function()
                    {
                        return loop(function(i)
                                    {
                                        var r = hold.ice_oneway().ice_ping();
                                        if((i % 20) == 0)
                                        {
                                            r = r.then(function() { return hold.putOnHold(0); })
                                        }
                                        return r;
                                    }, 100);
                    }
                ).then(
                    function()
                    {
                        return hold.putOnHold(-1);
                    }
                ).then(
                    function()
                    {
                        return hold.ice_ping();
                    }
                ).then(
                    function()
                    {
                        return hold.putOnHold(-1);
                    }
                ).then(
                    function()
                    {
                        return hold.ice_ping();
                    }
                );
            }
        ).then(
            function()
            {
                out.writeLine("ok");

                out.write("changing state to hold and shutting down server... ");
                return hold.shutdown();
            }
        ).then(
            function()
            {
                out.writeLine("ok");
                p.succeed();
            },
            function(ex)
            {
                out.writeLine("failed!");
                p.fail(ex);
            });
        return p;
    };

    var run = function(out, id)
    {
        //
        // For this test, we want to disable retries.
        //
        id.properties.setProperty("Ice.RetryIntervals", "-1");

        //
        // We don't want connection warnings because of the timeout
        //
        id.properties.setProperty("Ice.Warn.Connections", "0");

        //
        // We need to send messages large enough to cause the transport
        // buffers to fill up.
        //
        id.properties.setProperty("Ice.MessageSizeMax", "10000");

        id.properties.setProperty("Ice.RetryIntervals", "-1");

        var c = Ice.initialize(id);
        return Promise.try(
            function()
            {
                return allTests(out, c);
            }
        ).finally(
            function()
            {
                return c.destroy();
            }
        );
    };
    exports.__test__ = run;
    exports.__runServer__ = true;
}
(typeof(global) !== "undefined" && typeof(global.process) !== "undefined" ? module : undefined,
 typeof(global) !== "undefined" && typeof(global.process) !== "undefined" ? require : window.Ice.__require,
 typeof(global) !== "undefined" && typeof(global.process) !== "undefined" ? exports : window));
