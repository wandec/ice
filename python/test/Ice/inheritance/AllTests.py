# **********************************************************************
#
# Copyright (c) 2003-present ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import Ice, Test, sys

def test(b):
    if not b:
        raise RuntimeError('test assertion failed')

def allTests(helper, communicator):
    sys.stdout.write("testing stringToProxy... ")
    sys.stdout.flush()
    ref = "initial:{0}".format(helper.getTestEndpoint())
    base = communicator.stringToProxy(ref)
    test(base)
    print("ok")

    sys.stdout.write("testing checked cast... ")
    sys.stdout.flush()
    initial = Test.InitialPrx.checkedCast(base)
    test(initial)
    test(initial == base)
    print("ok")

    sys.stdout.write("getting proxies for class hierarchy... ")
    sys.stdout.flush()
    ca = initial.caop()
    cb = initial.cbop()
    cc = initial.ccop()
    cd = initial.cdop()
    test(ca != cb)
    test(ca != cc)
    test(ca != cd)
    test(cb != cc)
    test(cb != cd)
    test(cc != cd)
    print("ok")

    sys.stdout.write("getting proxies for interface hierarchy... ")
    sys.stdout.flush()
    ia = initial.iaop()
    ib1 = initial.ib1op()
    ib2 = initial.ib2op()
    ic = initial.icop()
    test(ia != ib1)
    test(ia != ib2)
    test(ia != ic)
    test(ib1 != ic)
    test(ib2 != ic)
    print("ok")

    sys.stdout.write("invoking proxy operations on class hierarchy... ")
    sys.stdout.flush()
    cao = ca.caop(ca)
    test(cao == ca)
    cao = ca.caop(cb)
    test(cao == cb)
    cao = ca.caop(cc)
    test(cao == cc)
    cao = cb.caop(ca)
    test(cao == ca)
    cao = cb.caop(cb)
    test(cao == cb)
    cao = cb.caop(cc)
    test(cao == cc)
    cao = cc.caop(ca)
    test(cao == ca)
    cao = cc.caop(cb)
    test(cao == cb)
    cao = cc.caop(cc)
    test(cao == cc)

    cao = cb.cbop(cb)
    test(cao == cb)
    cbo = cb.cbop(cb)
    test(cbo == cb)
    cao = cb.cbop(cc)
    test(cao == cc)
    cbo = cb.cbop(cc)
    test(cbo == cc)
    cao = cc.cbop(cb)
    test(cao == cb)
    cbo = cc.cbop(cb)
    test(cbo == cb)
    cao = cc.cbop(cc)
    test(cao == cc)
    cbo = cc.cbop(cc)
    test(cbo == cc)

    cao = cc.ccop(cc)
    test(cao == cc)
    cbo = cc.ccop(cc)
    test(cbo == cc)
    cco = cc.ccop(cc)
    test(cco == cc)
    print("ok")

    sys.stdout.write("ditto, but for interface hierarchy... ")
    sys.stdout.flush()
    iao = ia.iaop(ia)
    test(iao == ia)
    iao = ia.iaop(ib1)
    test(iao == ib1)
    iao = ia.iaop(ib2)
    test(iao == ib2)
    iao = ia.iaop(ic)
    test(iao == ic)
    iao = ib1.iaop(ia)
    test(iao == ia)
    iao = ib1.iaop(ib1)
    test(iao == ib1)
    iao = ib1.iaop(ib2)
    test(iao == ib2)
    iao = ib1.iaop(ic)
    test(iao == ic)
    iao = ib2.iaop(ia)
    test(iao == ia)
    iao = ib2.iaop(ib1)
    test(iao == ib1)
    iao = ib2.iaop(ib2)
    test(iao == ib2)
    iao = ib2.iaop(ic)
    test(iao == ic)
    iao = ic.iaop(ia)
    test(iao == ia)
    iao = ic.iaop(ib1)
    test(iao == ib1)
    iao = ic.iaop(ib2)
    test(iao == ib2)
    iao = ic.iaop(ic)
    test(iao == ic)

    iao = ib1.ib1op(ib1)
    test(iao == ib1)
    ib1o = ib1.ib1op(ib1)
    test(ib1o == ib1)
    iao = ib1.ib1op(ic)
    test(iao == ic)
    ib1o = ib1.ib1op(ic)
    test(ib1o == ic)
    iao = ic.ib1op(ib1)
    test(iao == ib1)
    ib1o = ic.ib1op(ib1)
    test(ib1o == ib1)
    iao = ic.ib1op(ic)
    test(iao == ic)
    ib1o = ic.ib1op(ic)
    test(ib1o == ic)

    iao = ib2.ib2op(ib2)
    test(iao == ib2)
    ib2o = ib2.ib2op(ib2)
    test(ib2o == ib2)
    iao = ib2.ib2op(ic)
    test(iao == ic)
    ib2o = ib2.ib2op(ic)
    test(ib2o == ic)
    iao = ic.ib2op(ib2)
    test(iao == ib2)
    ib2o = ic.ib2op(ib2)
    test(ib2o == ib2)
    iao = ic.ib2op(ic)
    test(iao == ic)
    ib2o = ic.ib2op(ic)
    test(ib2o == ic)

    iao = ic.icop(ic)
    test(iao == ic)
    ib1o = ic.icop(ic)
    test(ib1o == ic)
    ib2o = ic.icop(ic)
    test(ib2o == ic)
    ico = ic.icop(ic)
    test(ico == ic)

    print("ok")

    sys.stdout.write("ditto, but for class implementing interfaces... ")
    sys.stdout.flush()
    cao = cd.caop(cd)
    test(cao == cd)
    cbo = cd.cbop(cd)
    test(cbo == cd)
    cco = cd.ccop(cd)
    test(cco == cd)

    iao = cd.iaop(cd)
    test(iao == cd)
    ib1o = cd.ib1op(cd)
    test(ib1o == cd)
    ib2o = cd.ib2op(cd)
    test(ib2o == cd)

    cao = cd.cdop(cd)
    test(cao == cd)
    cbo = cd.cdop(cd)
    test(cbo == cd)
    cco = cd.cdop(cd)
    test(cco == cd)

    iao = cd.cdop(cd)
    test(iao == cd)
    ib1o = cd.cdop(cd)
    test(ib1o == cd)
    ib2o = cd.cdop(cd)
    test(ib2o == cd)

    print("ok")

    return initial
