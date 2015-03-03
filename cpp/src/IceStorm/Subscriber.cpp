// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceStorm/Subscriber.h>
#include <IceStorm/Instance.h>
#include <IceStorm/TraceLevels.h>
#include <IceStorm/NodeI.h>
#include <IceStorm/Util.h>
#include <Ice/LoggerUtil.h>
#include <iterator>

using namespace std;
using namespace IceStorm;
using namespace IceStormElection;

//
// Per Subscriber object.
//
namespace
{

class PerSubscriberPublisherI : public Ice::BlobjectArray
{
public:

    PerSubscriberPublisherI(const InstancePtr& instance) :
        _instance(instance)
    {
    }

    ~PerSubscriberPublisherI()
    {
    }

    void
    setSubscriber(const SubscriberPtr& subscriber)
    {
        _subscriber = subscriber;
    }

    virtual bool
    ice_invoke(const pair<const Ice::Byte*, const Ice::Byte*>& inParams,
               vector<Ice::Byte>&,
               const Ice::Current& current)
    {
        // Use cached reads.
        CachedReadHelper unlock(_instance->node(), __FILE__, __LINE__);

        EventDataPtr event = new EventData(
            current.operation,
            current.mode,
            Ice::ByteSeq(),
            current.ctx);

        //
        // COMPILERBUG: gcc 4.0.1 doesn't like this.
        //
        //event->data.swap(Ice::ByteSeq(inParams.first, inParams.second));
        Ice::ByteSeq data(inParams.first, inParams.second);
        event->data.swap(data);

        EventDataSeq e;
        e.push_back(event);
        _subscriber->queue(false, e);
        return true;
    }

private:

    const InstancePtr _instance;
    /*const*/ SubscriberPtr _subscriber;
};
typedef IceUtil::Handle<PerSubscriberPublisherI> PerSubscriberPublisherIPtr;

}

// Each of the various Subscriber types.
namespace
{

class SubscriberBatch : public Subscriber
{
public:

    SubscriberBatch(const InstancePtr&, const SubscriberRecord&, const Ice::ObjectPrx&, int, const Ice::ObjectPrx&);
    ~SubscriberBatch();

    virtual void flush();

    void doFlush();

private:

    const Ice::ObjectPrx _obj;
    const IceUtil::Time _interval;
};
typedef IceUtil::Handle<SubscriberBatch> SubscriberBatchPtr;

class SubscriberOneway : public Subscriber
{
public:

    SubscriberOneway(const InstancePtr&, const SubscriberRecord&, const Ice::ObjectPrx&, int, const Ice::ObjectPrx&);
    ~SubscriberOneway();

    virtual void flush();

    void sent();

private:

    const Ice::ObjectPrx _obj;
};
typedef IceUtil::Handle<SubscriberOneway> SubscriberOnewayPtr;

class SubscriberTwoway : public Subscriber
{
public:

    SubscriberTwoway(const InstancePtr&, const SubscriberRecord&, const Ice::ObjectPrx&, int, int,
                     const Ice::ObjectPrx&);

    virtual void flush();

private:

    const Ice::ObjectPrx _obj;
};

class SubscriberLink : public Subscriber
{
public:

    SubscriberLink(const InstancePtr&, const SubscriberRecord&);

    virtual void flush();

private:

    const TopicLinkPrx _obj;
};

class OnewayIceInvokeI : public Ice::AMI_Object_ice_invoke, public Ice::AMISentCallback
{
public:

    OnewayIceInvokeI(const SubscriberOnewayPtr& subscriber) :
        _subscriber(subscriber)
    {
    }

    virtual void
    ice_response(bool, const std::vector<Ice::Byte>&)
    {
        assert(false);
    }

    virtual void
    ice_sent()
    {
        _subscriber->sent();
    }

    virtual void
    ice_exception(const Ice::Exception& e)
    {
        _subscriber->error(true, e);
    }

private:

    const SubscriberOnewayPtr _subscriber;
};

class IceInvokeI : public Ice::AMI_Object_ice_invoke
{
public:

    IceInvokeI(const SubscriberPtr& subscriber) :
        _subscriber(subscriber)
    {
    }

    virtual void
    ice_response(bool, const std::vector<Ice::Byte>&)
    {
        _subscriber->response();
    }

    virtual void
    ice_exception(const Ice::Exception& e)
    {
        _subscriber->error(true, e);
    }

private:

    const SubscriberPtr _subscriber;
};

class FlushBatchI : public Ice::AMI_Object_ice_flushBatchRequests
{
public:

    FlushBatchI(const SubscriberPtr& subscriber) :
        _subscriber(subscriber)
    {
    }

    virtual void
    ice_exception(const Ice::Exception& e)
    {
        _subscriber->error(false, e);
    }

private:

    const SubscriberPtr _subscriber;
};

class FlushTimerTask : public IceUtil::TimerTask
{
public:

    FlushTimerTask(const SubscriberBatchPtr& subscriber) :
        _subscriber(subscriber)
    {
    }

    virtual void
    runTimerTask()
    {
        _subscriber->doFlush();
    }

private:

    const SubscriberBatchPtr _subscriber;
};

}

SubscriberBatch::SubscriberBatch(
    const InstancePtr& instance,
    const SubscriberRecord& rec,
    const Ice::ObjectPrx& proxy,
    int retryCount,
    const Ice::ObjectPrx& obj) :
    Subscriber(instance, rec, proxy, retryCount, 1),
    _obj(obj),
    _interval(instance->flushInterval())
{
    assert(retryCount == 0);
}

SubscriberBatch::~SubscriberBatch()
{
}

void
SubscriberBatch::flush()
{
    if(_outstanding == 0)
    {
        ++_outstanding;
        _instance->batchFlusher()->schedule(new FlushTimerTask(this), _interval);
    }
}

void
SubscriberBatch::doFlush()
{
    EventDataSeq v;
    {
        IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);
        
        --_outstanding;
        assert(_outstanding == 0);
        
        if(_shutdown)
        {
            _lock.notify();
        }

        //
        // If the subscriber isn't online we're done.
        //
        if(_state != SubscriberStateOnline)
        {
            return;
        }
        v.swap(_events);
        assert(!v.empty());
    }

    try
    {
        vector<Ice::Byte> dummy;
        for(EventDataSeq::const_iterator p = v.begin(); p != v.end(); ++p)
        {
            _obj->ice_invoke((*p)->op, (*p)->mode, (*p)->data, dummy, (*p)->context);
        }
    }
    catch(const Ice::Exception& ex)
    {
        error(false, ex);
        return;
    }

    _obj->ice_flushBatchRequests_async(new FlushBatchI(this));

    // This is significantly faster than the async version, but it can
    // block the calling thread. Bad news!

    //_obj->ice_flushBatchRequests();
}

SubscriberOneway::SubscriberOneway(
    const InstancePtr& instance,
    const SubscriberRecord& rec,
    const Ice::ObjectPrx& proxy,
    int retryCount,
    const Ice::ObjectPrx& obj) :
    Subscriber(instance, rec, proxy, retryCount, 5),
    _obj(obj)
{
    assert(retryCount == 0);
}

SubscriberOneway::~SubscriberOneway()
{
}

void
SubscriberOneway::flush()
{
    IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);
    
    //
    // If the subscriber isn't online we're done.
    //
    if(_state != SubscriberStateOnline || _events.empty())
    {
        return;
    }

    // Send up to _maxOutstanding pending events.
    while(_outstanding < _maxOutstanding && !_events.empty())
    {
        //
        // Dequeue the head event, count one more outstanding AMI
        // request.
        //
        EventDataPtr e = _events.front();
        _events.erase(_events.begin());
        try
        {
            if(!_obj->ice_invoke_async(new OnewayIceInvokeI(this), e->op, e->mode, e->data, e->context))
            {
                ++_outstanding;
            }
        }
        catch(const Ice::Exception& ex)
        {
            error(true, ex);
            return;
        }
    }

    if(_events.empty() && _outstanding == 0 && _shutdown)
    {
        _lock.notify();
    }
}

void
SubscriberOneway::sent()
{
    IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);
        
    // Decrement the _outstanding count.
    --_outstanding;
    assert(_outstanding >= 0 && _outstanding < _maxOutstanding);

    if(_events.empty() && _outstanding == 0 && _shutdown)
    {
        _lock.notify();
    }
    else if(_outstanding <= 0 && !_events.empty())
    {
        flush();
    }
}

SubscriberTwoway::SubscriberTwoway(
    const InstancePtr& instance,
    const SubscriberRecord& rec,
    const Ice::ObjectPrx& proxy,
    int retryCount,
    int maxOutstanding,
    const Ice::ObjectPrx& obj) :
    Subscriber(instance, rec, proxy, retryCount, maxOutstanding),
    _obj(obj)
{
}

void
SubscriberTwoway::flush()
{
    IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);

    //
    // If the subscriber isn't online we're done.
    //
    if(_state != SubscriberStateOnline || _events.empty())
    {
        return;
    }

    // Send up to _maxOutstanding pending events.
    while(_outstanding < _maxOutstanding && !_events.empty())
    {
        //
        // Dequeue the head event, count one more outstanding AMI
        // request.
        //
        EventDataPtr e = _events.front();
        _events.erase(_events.begin());
        ++_outstanding;
          
        try
        {
            _obj->ice_invoke_async(new IceInvokeI(this), e->op, e->mode, e->data, e->context);
        }
        catch(const Ice::Exception& ex)
        {
            error(true, ex);
            return;
        }
    }
}

namespace
{

class Topiclink_forwardI : public IceStorm::AMI_TopicLink_forward
{
public:

    Topiclink_forwardI(const SubscriberPtr& subscriber) :
        _subscriber(subscriber)
    {
    }

    virtual void
    ice_response()
    {
        _subscriber->response();
    }

    virtual void
    ice_exception(const Ice::Exception& e)
    {
        _subscriber->error(true, e);
    }

private:

    const SubscriberPtr _subscriber;
};

}

SubscriberLink::SubscriberLink(
    const InstancePtr& instance,
    const SubscriberRecord& rec) :
    Subscriber(instance, rec, 0, -1, 1),
    _obj(TopicLinkPrx::uncheckedCast(rec.obj->ice_collocationOptimized(false)->ice_timeout(instance->sendTimeout())))
{
}

void
SubscriberLink::flush()
{
    IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);
    
    if(_state != SubscriberStateOnline || _outstanding > 0)
    {
        return;
    }

    EventDataSeq v;
    v.swap(_events);

    EventDataSeq::iterator p = v.begin();
    while(p != v.end())
    {
        if(_rec.cost != 0)
        {
            int cost = 0;
            Ice::Context::const_iterator q = (*p)->context.find("cost");
            if(q != (*p)->context.end())
            {
                cost = atoi(q->second.c_str());
            }
            if(cost > _rec.cost)
            {
                p = v.erase(p);
                continue;
            }
        }
        ++p;
    }

    if(!v.empty())
    {
        try
        {
            ++_outstanding;
            _obj->forward_async(new Topiclink_forwardI(this), v);
        }
        catch(const Ice::Exception& ex)
        {
            error(true, ex);
        }
    }
}

SubscriberPtr
Subscriber::create(
    const InstancePtr& instance,
    const SubscriberRecord& rec)
{
    if(rec.link)
    {
        return new SubscriberLink(instance, rec);
    }
    else
    {
        PerSubscriberPublisherIPtr per = new PerSubscriberPublisherI(instance);
        Ice::Identity perId;
        perId.category = instance->instanceName();
        perId.name = "topic." + rec.topicName + ".publish." +
                     instance->communicator()->identityToString(rec.obj->ice_getIdentity());
        Ice::ObjectPrx proxy = instance->publishAdapter()->add(per, perId);
        TraceLevelsPtr traceLevels = instance->traceLevels();
        SubscriberPtr subscriber;

        try
        {
            int retryCount = 0;
            QoS::const_iterator p = rec.theQoS.find("retryCount");
            if(p != rec.theQoS.end())
            {
                retryCount = atoi(p->second.c_str());
            }

            string reliability;
            p = rec.theQoS.find("reliability");
            if(p != rec.theQoS.end())
            {
                reliability = p->second;
            }
            if(!reliability.empty() && reliability != "ordered")
            {
                throw BadQoS("invalid reliability: " + reliability);
            }

            //
            // Override the timeout.
            //
            Ice::ObjectPrx newObj;
            try
            {
                newObj = rec.obj->ice_timeout(instance->sendTimeout());
            }
            catch(const Ice::FixedProxyException&)
            {
                //
                // In the event IceStorm is collocated this could be a
                // fixed proxy in which case its not possible to set the
                // timeout.
                //
                newObj = rec.obj;
            }
            if(reliability == "ordered")
            {
                if(!newObj->ice_isTwoway())
                {
                    throw BadQoS("ordered reliability requires a twoway proxy");
                }
                subscriber = new SubscriberTwoway(instance, rec, proxy, retryCount, 1, newObj);
            }
            else if(newObj->ice_isOneway() || newObj->ice_isDatagram())
            {
                if(retryCount > 0)
                {
                    throw BadQoS("non-zero retryCount QoS requires a twoway proxy");
                }
                subscriber = new SubscriberOneway(instance, rec, proxy, retryCount, newObj);
            }
            else if(newObj->ice_isBatchOneway() || newObj->ice_isBatchDatagram())
            {
                if(retryCount > 0)
                {
                    throw BadQoS("non-zero retryCount QoS requires a twoway proxy");
                }
                subscriber = new SubscriberBatch(instance, rec, proxy, retryCount, newObj);
            }
            else //if(newObj->ice_isTwoway())
            {
                assert(newObj->ice_isTwoway());
                subscriber = new SubscriberTwoway(instance, rec, proxy, retryCount, 5, newObj);
            }
            per->setSubscriber(subscriber);
        }
        catch(const Ice::Exception&)
        {
            instance->publishAdapter()->remove(proxy->ice_getIdentity());
            throw;
        }

        return subscriber;
    }
}

Subscriber::~Subscriber()
{
    //cout << "~Subscriber" << endl;
}

Ice::ObjectPrx
Subscriber::proxy() const
{
    return _proxyReplica;
}

Ice::Identity
Subscriber::id() const
{
    return _rec.id;
}

SubscriberRecord
Subscriber::record() const
{
    return _rec;
}

bool
Subscriber::queue(bool forwarded, const EventDataSeq& events)
{
    IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);

    // If this is a link subscriber if the set of events were
    // forwarded from another IceStorm instance then do not queue the
    // events.
    if(forwarded && _rec.link)
    {
        return true;
    }

    switch(_state)
    {
    case SubscriberStateOffline:
    {
        if(IceUtil::Time::now(IceUtil::Time::Monotonic) < _next)
        {
            break;
        }
        
        //
        // State transition to online.
        //
        setState(SubscriberStateOnline);
        // fall through
    }
    
    case SubscriberStateOnline:
        copy(events.begin(), events.end(), back_inserter(_events));
        flush();
        break;

    case SubscriberStateError:
        return false;

    case SubscriberStateReaped:
        break;
    }

    return true;
}

bool
Subscriber::reap()
{
    IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);
    assert(_state >= SubscriberStateError);
    if(_state == SubscriberStateError)
    {
        setState(SubscriberStateReaped);
        return true;
    }
    return false;
}

void
Subscriber::resetIfReaped()
{
    IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);
    if(_state == SubscriberStateReaped)
    {
        setState(SubscriberStateError);
    }
}

bool
Subscriber::errored() const
{
    IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);
    return _state >= SubscriberStateError;
}

void
Subscriber::destroy()
{
    //
    // Clear the per-subscriber object if it exists.
    //
    if(_proxy)
    {
        try
        {
            _instance->publishAdapter()->remove(_proxy->ice_getIdentity());
        }
        catch(const Ice::NotRegisteredException&)
        {
            // Ignore
        }
        catch(const Ice::ObjectAdapterDeactivatedException&)
        {
            // Ignore
        }
    }
}

void
Subscriber::error(bool dec, const Ice::Exception& e)
{
    IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);

    if(dec)
    {
        // Decrement the _outstanding count.
        --_outstanding;
        assert(_outstanding >= 0 && _outstanding < _maxOutstanding);
    }

    // A hard error is an ObjectNotExistException or
    // NotRegisteredException.
    bool hardError = dynamic_cast<const Ice::ObjectNotExistException*>(&e) ||
                     dynamic_cast<const Ice::NotRegisteredException*>(&e);

    //
    // A twoway subscriber can queue multiple send events and
    // therefore its possible to get multiple error'd replies. Ignore
    // replies if we're retrying and its not yet time to process the
    // next request.
    //
    IceUtil::Time now = IceUtil::Time::now(IceUtil::Time::Monotonic);
    if(!hardError && _state == SubscriberStateOffline && now < _next)
    {
        return;
    }

    //
    // If we're in our retry limits and the error isn't a hard failure
    // (that is ObjectNotExistException or NotRegisteredException)
    // then we transition to an offline state.
    //
    if(!hardError && (_retryCount == -1 || _currentRetry < _retryCount))
    {
        assert(_state < SubscriberStateError);

        TraceLevelsPtr traceLevels = _instance->traceLevels();
        if(_currentRetry == 0)
        {
            Ice::Warning warn(traceLevels->logger);
            warn << traceLevels->subscriberCat << ":" << _instance->communicator()->identityToString(_rec.id);
            if(traceLevels->subscriber > 1)
            {
                warn << " endpoints: " << IceStormInternal::describeEndpoints(_rec.obj);
            }
            warn << " subscriber offline: " << e
                 << " discarding events: " << _instance->discardInterval() << "s retryCount: " << _retryCount;
        }
        else
        {
            if(traceLevels->subscriber > 0)
            {
                Ice::Trace out(traceLevels->logger, traceLevels->subscriberCat);
                out << this << " ";
                out << _instance->communicator()->identityToString(_rec.id);
                if(traceLevels->subscriber > 1)
                {
                    out << " endpoints: " << IceStormInternal::describeEndpoints(_rec.obj);
                }
                out << " subscriber offline: " << e
                    << " discarding events: " << _instance->discardInterval() << "s retry: "
                    << _currentRetry << "/" << _retryCount;
            }
        }

        // Transition to offline state, increment the retry count and
        // clear all queued events.
        _next = now + _instance->discardInterval();
        ++_currentRetry;
        _events.clear();
        setState(SubscriberStateOffline);
    }
   // Errored out.
    else if(_state < SubscriberStateError)
    {
        _events.clear();
        setState(SubscriberStateError);
        
        TraceLevelsPtr traceLevels = _instance->traceLevels();
        if(traceLevels->subscriber > 0)
        {
            Ice::Trace out(traceLevels->logger, traceLevels->subscriberCat);
            out << this << " ";
            out << _instance->communicator()->identityToString(_rec.id);
            if(traceLevels->subscriber > 1)
            {
                out << " endpoints: " << IceStormInternal::describeEndpoints(_rec.obj);
            }
            out << " subscriber errored out: " << e
                << " retry: " << _currentRetry << "/" << _retryCount;
        }
    }

    if(_shutdown && _events.empty())
    {
        _lock.notify();
    }
}

void
Subscriber::response()
{
    IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);
        
    // Decrement the _outstanding count.
    --_outstanding;
    assert(_outstanding >= 0 && _outstanding < _maxOutstanding);

    //
    // A successful response means we're no longer retrying, we're
    // back active.
    //
    _currentRetry = 0;

    if(_events.empty() && _outstanding == 0 && _shutdown)
    {
        _lock.notify();
    }
    else
    {
        flush();
    }
}

void
Subscriber::shutdown()
{
    IceUtil::Monitor<IceUtil::RecMutex>::Lock sync(_lock);

    _shutdown = true;
    while(_outstanding > 0 && !_events.empty())
    {
        _lock.wait();
    }
}

Subscriber::Subscriber(
    const InstancePtr& instance,
    const SubscriberRecord& rec,
    const Ice::ObjectPrx& proxy,
    int retryCount,
    int maxOutstanding) :
    _instance(instance),
    _rec(rec),
    _retryCount(retryCount),
    _maxOutstanding(maxOutstanding),
    _proxy(proxy),
    _proxyReplica(proxy),
    _shutdown(false),
    _state(SubscriberStateOnline),
    _outstanding(0),
    _currentRetry(0)
{
    if(_proxy && _instance->publisherReplicaProxy())
    {
        const_cast<Ice::ObjectPrx&>(_proxyReplica) =
            _instance->publisherReplicaProxy()->ice_identity(_proxy->ice_getIdentity());
    }
}

namespace
{

string
stateToString(Subscriber::SubscriberState state)
{
    switch(state)
    {
    case Subscriber::SubscriberStateOnline:
        return "online";
    case Subscriber::SubscriberStateOffline:
        return "offline";
    case Subscriber::SubscriberStateError:
        return "error";
    case Subscriber::SubscriberStateReaped:
        return "reaped";
    default:
        return "???";
    }
}

}

void
Subscriber::setState(Subscriber::SubscriberState state)
{
    if(state != _state)
    {
        TraceLevelsPtr traceLevels = _instance->traceLevels();
        if(traceLevels->subscriber > 1)
        {
            Ice::Trace out(traceLevels->logger, traceLevels->subscriberCat);
            out << this << " endpoints: " << IceStormInternal::describeEndpoints(_rec.obj) 
                << " transition from: " << stateToString(_state) << " to: " << stateToString(state);
        }
        _state = state;
    }
}

bool
IceStorm::operator==(const SubscriberPtr& subscriber, const Ice::Identity& id)
{
    return subscriber->id() == id;
}

bool
IceStorm::operator==(const Subscriber& s1, const Subscriber& s2)
{
    return &s1 == &s2;
}

bool
IceStorm::operator!=(const Subscriber& s1, const Subscriber& s2)
{
    return &s1 != &s2;
}

bool
IceStorm::operator<(const Subscriber& s1, const Subscriber& s2)
{
    return &s1 < &s2;
}
