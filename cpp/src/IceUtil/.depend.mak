ArgVector$(OBJEXT): ArgVector.cpp "$(includedir)/IceUtil/ArgVector.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/DisableWarnings.h"
Cond$(OBJEXT): Cond.cpp "$(includedir)/IceUtil/Cond.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Time.h" "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Exception.h"
ConvertUTF$(OBJEXT): ConvertUTF.cpp ../IceUtil/ConvertUTF.h "$(includedir)/IceUtil/Unicode.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Exception.h"
CountDownLatch$(OBJEXT): CountDownLatch.cpp "$(includedir)/IceUtil/CountDownLatch.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Time.h"
CtrlCHandler$(OBJEXT): CtrlCHandler.cpp "$(includedir)/IceUtil/CtrlCHandler.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/MutexPtrLock.h" "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Time.h" "$(includedir)/IceUtil/Mutex.h" "$(includedir)/IceUtil/Lock.h" "$(includedir)/IceUtil/MutexProtocol.h"
Exception$(OBJEXT): Exception.cpp "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/MutexPtrLock.h" "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Time.h" "$(includedir)/IceUtil/Mutex.h" "$(includedir)/IceUtil/Lock.h" "$(includedir)/IceUtil/MutexProtocol.h" "$(includedir)/IceUtil/StringUtil.h"
FileUtil$(OBJEXT): FileUtil.cpp "$(includedir)/IceUtil/DisableWarnings.h" "$(includedir)/IceUtil/FileUtil.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Shared.h" "$(includedir)/IceUtil/Handle.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Unicode.h"
InputUtil$(OBJEXT): InputUtil.cpp "$(includedir)/IceUtil/InputUtil.h" "$(includedir)/IceUtil/Config.h"
Options$(OBJEXT): Options.cpp "$(includedir)/IceUtil/Options.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/RecMutex.h" "$(includedir)/IceUtil/Lock.h" "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Time.h" "$(includedir)/IceUtil/MutexProtocol.h" "$(includedir)/IceUtil/Shared.h" "$(includedir)/IceUtil/Handle.h" "$(includedir)/IceUtil/StringUtil.h"
OutputUtil$(OBJEXT): OutputUtil.cpp "$(includedir)/IceUtil/OutputUtil.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/FileUtil.h" "$(includedir)/IceUtil/Shared.h" "$(includedir)/IceUtil/Handle.h" "$(includedir)/IceUtil/Exception.h"
Random$(OBJEXT): Random.cpp "$(includedir)/IceUtil/Random.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Mutex.h" "$(includedir)/IceUtil/Lock.h" "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Time.h" "$(includedir)/IceUtil/MutexProtocol.h" "$(includedir)/IceUtil/MutexPtrLock.h"
RWRecMutex$(OBJEXT): RWRecMutex.cpp "$(includedir)/IceUtil/RWRecMutex.h" "$(includedir)/IceUtil/Mutex.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Lock.h" "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Time.h" "$(includedir)/IceUtil/MutexProtocol.h" "$(includedir)/IceUtil/Cond.h" "$(includedir)/IceUtil/Thread.h" "$(includedir)/IceUtil/Shared.h" "$(includedir)/IceUtil/Handle.h"
RecMutex$(OBJEXT): RecMutex.cpp "$(includedir)/IceUtil/RecMutex.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Lock.h" "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Time.h" "$(includedir)/IceUtil/MutexProtocol.h"
StaticMutex$(OBJEXT): StaticMutex.cpp "$(includedir)/IceUtil/StaticMutex.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Lock.h" "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Time.h"
Shared$(OBJEXT): Shared.cpp "$(includedir)/IceUtil/Shared.h" "$(includedir)/IceUtil/Config.h"
StringUtil$(OBJEXT): StringUtil.cpp "$(includedir)/IceUtil/StringUtil.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Unicode.h" "$(includedir)/IceUtil/Exception.h"
Thread$(OBJEXT): Thread.cpp "$(includedir)/IceUtil/Thread.h" "$(includedir)/IceUtil/Shared.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Handle.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Mutex.h" "$(includedir)/IceUtil/Lock.h" "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Time.h" "$(includedir)/IceUtil/MutexProtocol.h"
ThreadException$(OBJEXT): ThreadException.cpp "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Time.h"
Time$(OBJEXT): Time.cpp "$(includedir)/IceUtil/DisableWarnings.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Time.h"
Timer$(OBJEXT): Timer.cpp "$(includedir)/IceUtil/Timer.h" "$(includedir)/IceUtil/Shared.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Thread.h" "$(includedir)/IceUtil/Handle.h" "$(includedir)/IceUtil/Exception.h" "$(includedir)/IceUtil/Mutex.h" "$(includedir)/IceUtil/Lock.h" "$(includedir)/IceUtil/ThreadException.h" "$(includedir)/IceUtil/Time.h" "$(includedir)/IceUtil/MutexProtocol.h" "$(includedir)/IceUtil/Monitor.h" "$(includedir)/IceUtil/Cond.h"
UUID$(OBJEXT): UUID.cpp "$(includedir)/IceUtil/UUID.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Random.h" "$(includedir)/IceUtil/Exception.h"
Unicode$(OBJEXT): Unicode.cpp "$(includedir)/IceUtil/Unicode.h" "$(includedir)/IceUtil/Config.h" "$(includedir)/IceUtil/Exception.h" ../IceUtil/ConvertUTF.h
MutexProtocol$(OBJEXT): MutexProtocol.cpp "$(includedir)/IceUtil/MutexProtocol.h" "$(includedir)/IceUtil/Config.h"
