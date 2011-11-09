/*
  ==============================================================================

   This file is part of Prim
   Copyright 2007-2011 by William Andrew Burnson

  ------------------------------------------------------------------------------

   Prim can be redistributed and/or modified under the terms of
   the GNU Lesser General Public License, as published by the
   Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   Prim is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with Prim; if not, visit www.gnu.org/licenses or write:
   
   Free Software Foundation, Inc.
   59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ==============================================================================
*/

#ifndef PRIM_THREADS_H
#define PRIM_THREADS_H
 
/*The default is to use std::thread, but this is a C++11 feature and may not be
available for all platforms. Alternatives are pthread.h (GCC) and windows.h
(MSVC). Available options:
1) C++11 (default): Requires C++ std::thread
2) GCC:             Use #define PRIM_THREADS_USE_PTHREAD
3) Windows:         Use #define PRIM_THREADS_USE_WINDOWS
*/

//Convert defines into numerical form to allow the use of #if and #elif.
#ifndef PRIM_THREADS_USE_PTHREAD
  #define PRIM_THREADS_USE_PTHREAD 0
#else
  #undef  PRIM_THREADS_USE_PTHREAD
  #define PRIM_THREADS_USE_PTHREAD 1
#endif

#ifndef PRIM_THREADS_USE_WINDOWS
  #define PRIM_THREADS_USE_WINDOWS 0
#else
  #undef  PRIM_THREADS_USE_WINDOWS
  #define PRIM_THREADS_USE_WINDOWS 1
#endif

namespace prim
{
  ///Recursive thread mutex maintaining a lock and unlock count.
  class Mutex
  {
    //The mutex object -- (void*) to avoid including thread headers in this file. 
    void* RecursiveMutex;
    
    public:
    
    ///Initializes the mutex with a lock count of zero.
    Mutex();
    
    ///Releases the mutex. The lock count must be zero at this point.
    ~Mutex();
    
    ///Blocks until a lock is established, incrementing the lock count.
    void Lock(void);
    
    ///Attempts to establish a lock but returns false if unsuccessful.
    bool TryLock(void);
    
    /**Decrements the lock count. If the lock count is greater than the unlock
    count, then the lock is still in effect (since it is a recursive mutex).*/
    void Unlock(void);
  };

  ///Locks a given mutex for the lifetime of the object. 
  class ScopedLock
  {
    Mutex& ScopedMutex;
    
    public:
    ///Locks the given mutex. 
    ScopedLock(Mutex& ScopedMutex) : ScopedMutex(ScopedMutex)
    {
      ScopedMutex.Lock();
    }
    
    ///Deleted assignment operator.
    ScopedLock& operator = (const ScopedLock&);
    
    ///Unlocks the mutex.
    ~ScopedLock()
    {
      ScopedMutex.Unlock();
    }
  };

  class Thread
  {
    //The platform thread object
    void* ThreadObject; //(void*) avoids including thread headers in this file. 
    
    //Thread state signal
    Mutex InternalMutex;
    
    //Thread state flags
    bool Running;
    bool Ending;
    
  #if PRIM_THREADS_USE_PTHREAD
    static void* BeginThread(void* VoidThreadPointer);
  #elif PRIM_THREADS_USE_WINDOWS
    static unsigned long __stdcall BeginThread(void* VoidThreadPointer);
  #else
    static void BeginThread(Thread* ThreadPointer);
  #endif
    
    public:
    
    /**Overload and place thread contents inside. If the thread can be
    interrupted, then make sure to call IsEnding() frequently to see whether or
    not the method should return. You can signal a thread to end by calling
    SignalEnd().*/
    virtual void Run(void) = 0;

    ///Initializes but does not begin the thread.
    Thread();

    /**Releases the thread. WaitToEnd() must be called before this time, or
    else a crash may occur due to members in the subclass not being available.*/
    virtual ~Thread();

    /**Begins the thread. If the thread is already running, then it will wait
    for the current thread instance to end before running the new instance.*/
    bool Begin(void);

    /**Determines whether the end-state was signaled by SignalEnd(). A thread
    that receives true from this method should clean-up and end Run() as quickly
    as possible.*/
    bool IsEnding(void)
    {
      ScopedLock Lock(InternalMutex);
      return Ending;
    }
    
    /**Determines whether or not the thread is still running. Note that a thread
    is considered to be running until WaitToEnd() is called. Even if the Run()
    method has already returned, the thread is still considered active.*/
    bool IsRunning(void)
    {
      ScopedLock Lock(InternalMutex);
      return Running;
    }

    /**Indicates to the thread that it should end as soon as possible. This is
    achieved by periodically checking IsEnding() in the Run() method and exiting
    if necessary.*/
    void SignalEnd(void)
    {
      ScopedLock Lock(InternalMutex);
      Ending = true;
    }

    /**The proper way to clean up a thread. It automatically signals the thread
    to exit using SignalEnd(), and will block the caller thread until it
    exits.*/
    void WaitToEnd(void);
  };
}

#ifdef PRIM_COMPILE_INLINE

#if PRIM_THREADS_USE_PTHREAD
#include <pthread.h> //pthread
#elif PRIM_THREADS_USE_WINDOWS
#include <windows.h> //Windows threads
#else
#include <thread> //std::thread
#endif

namespace prim
{
  Mutex::Mutex()
  {
#if PRIM_THREADS_USE_PTHREAD
    //Create the mutex attributes object for a recursive mutex.
    pthread_mutexattr_t MutexAttributes;
    pthread_mutexattr_init(&MutexAttributes);
    pthread_mutexattr_settype(&MutexAttributes, PTHREAD_MUTEX_RECURSIVE);
    
    //Initialize the mutex using the attributes object.
    RecursiveMutex = (void*)new pthread_mutex_t;
    pthread_mutex_init((pthread_mutex_t*)RecursiveMutex, &MutexAttributes);
#elif PRIM_THREADS_USE_WINDOWS
    RecursiveMutex = (void*)new HANDLE;
    *((HANDLE*)RecursiveMutex) = CreateMutex(0, FALSE, 0);
#else
    RecursiveMutex = (void*)new std::recursive_mutex;
#endif
  }
  
  Mutex::~Mutex()
  {
#if PRIM_THREADS_USE_PTHREAD
    delete (pthread_mutex_t*)RecursiveMutex;
#elif PRIM_THREADS_USE_WINDOWS
    CloseHandle(*((HANDLE*)RecursiveMutex));
#else
    delete (std::recursive_mutex*)RecursiveMutex;
#endif
  }
  
  void Mutex::Lock(void)
  {
#if PRIM_THREADS_USE_PTHREAD
    pthread_mutex_lock((pthread_mutex_t*)RecursiveMutex);
#elif PRIM_THREADS_USE_WINDOWS
    WaitForSingleObject(*((HANDLE*)RecursiveMutex), INFINITE);
#else
    ((std::recursive_mutex*)RecursiveMutex)->lock();
#endif
  }
  
  bool Mutex::TryLock(void)
  {
#if PRIM_THREADS_USE_PTHREAD
    return pthread_mutex_trylock((pthread_mutex_t*)RecursiveMutex) == 0;
#elif PRIM_THREADS_USE_WINDOWS
    return WaitForSingleObject(*((HANDLE*)RecursiveMutex), 0) == WAIT_OBJECT_0;
#else
    return ((std::recursive_mutex*)RecursiveMutex)->try_lock();
#endif
  }
  
  void Mutex::Unlock(void)
  {
#if PRIM_THREADS_USE_PTHREAD
    pthread_mutex_unlock((pthread_mutex_t*)RecursiveMutex);
#elif PRIM_THREADS_USE_WINDOWS
    ReleaseMutex(*((HANDLE*)RecursiveMutex));
#else
    ((std::recursive_mutex*)RecursiveMutex)->unlock();
#endif
  }

#if PRIM_THREADS_USE_PTHREAD
  void* Thread::BeginThread(void* VoidThreadPointer)
  {
    Thread* ThreadPointer = (Thread*)VoidThreadPointer;
    {ScopedLock(ThreadPointer->InternalMutex);} //Wait for initialization
    ThreadPointer->Run();
    return 0;
  }
#elif PRIM_THREADS_USE_WINDOWS
  unsigned long __stdcall Thread::BeginThread(void* VoidThreadPointer)
  {
    Thread* ThreadPointer = (Thread*)VoidThreadPointer;
    {ScopedLock(ThreadPointer->InternalMutex);} //Wait for initialization
    ThreadPointer->Run();
    return 0;
  }
#else
  void Thread::BeginThread(Thread* ThreadPointer)
  {
    {ScopedLock(ThreadPointer->InternalMutex);} //Wait for initialization
    ThreadPointer->Run();
  }
#endif

  Thread::Thread() : Running(false), Ending(false)
  {
#if PRIM_THREADS_USE_PTHREAD
    ThreadObject = (void*)new pthread_t;
#elif PRIM_THREADS_USE_WINDOWS
    ThreadObject = (void*)new HANDLE;
#else
    ThreadObject = (void*)new std::thread;
#endif
  }
  
  Thread::~Thread()
  {
    /*This is to prevent a crash, but all threads should be ended before they
    go out of scope or are deleted. It is really to late here to end the thread
    since the subclass has had member data deleted already.*/
    WaitToEnd();
#if PRIM_THREADS_USE_PTHREAD
    delete (pthread_t*)ThreadObject;
#elif PRIM_THREADS_USE_WINDOWS
    delete (HANDLE*)ThreadObject;
#else
    delete (std::thread*)ThreadObject;
#endif
  }
  
  bool Thread::Begin(void)
  {
    WaitToEnd();
    {
      ScopedLock Lock(InternalMutex);
      Running = true;
#if PRIM_THREADS_USE_PTHREAD
      pthread_attr_t ThreadAttributes;
      pthread_attr_init(&ThreadAttributes);
      pthread_attr_setdetachstate(&ThreadAttributes, PTHREAD_CREATE_JOINABLE);
      pthread_create((pthread_t*)ThreadObject, &ThreadAttributes, BeginThread,
        (void*)this);
#elif PRIM_THREADS_USE_WINDOWS
      DWORD ThreadID = 0;
      *((HANDLE*)ThreadObject) = CreateThread(NULL, 0,
      (LPTHREAD_START_ROUTINE)Thread::BeginThread, (LPVOID)this, 0, &ThreadID);
#else
     *((std::thread*)ThreadObject) = std::thread(BeginThread, this);
#endif
      return true;
    }
  }

  void Thread::WaitToEnd(void)
  {
    if(!IsRunning())
      return;
    SignalEnd();
#if PRIM_THREADS_USE_PTHREAD
    pthread_join(*(pthread_t*)ThreadObject, 0);
#elif PRIM_THREADS_USE_WINDOWS
    WaitForSingleObject(*((HANDLE*)ThreadObject), INFINITE);
#else
    ((std::thread*)ThreadObject)->join();
#endif
    {
      ScopedLock Lock(InternalMutex);
      Ending = false;
      Running = false;
    }
  }
}
#endif
#endif

