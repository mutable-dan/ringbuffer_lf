#include "cqueue.h"
#include "rb_generic.h"
#include <iostream>
#include <unistd.h>
#include <thread>
#include <cstdlib>
#include <functional>
#include <chrono>

using namespace std;

#ifdef DEBUG
    const uint64_t cunNumberOfElementsToWrite = 10;
    const uint32_t cunQueueElementCount       =  4;             // must be powers of 2
#else
    const uint64_t cunNumberOfElementsToWrite = 50000000;
    const uint32_t cunQueueElementCount       = 1024;           // must be powers of 2
#endif


void reader_test( collections::CQueueLockFreeSingle& a_queue  );
void writer_test( collections::CQueueLockFreeSingle& a_queue );


int main(int , char **) 
{
    collections::CQueueLockFreeSingle lockedqueue_single( cunQueueElementCount, sizeof(uint32_t) );

    std::cout << "Test lock circular buffer - single reader/writer lockfree locking" << std::endl;
    std::cout << "cores:" << std::thread::hardware_concurrency() << std::endl;

    std::chrono::time_point<std::chrono::system_clock> start_test, end_test;
    start_test = std::chrono::system_clock::now();
    std::thread thdreader1_2( reader_test, std::ref(lockedqueue_single) );
    std::thread thdWriter1_2( writer_test, std::ref(lockedqueue_single) );
    thdWriter1_2.join();
    thdreader1_2.join();
    end_test = std::chrono::system_clock::now();

    std::cout << "execution time:" << std::chrono::duration_cast<std::chrono::seconds>     ( end_test - start_test ).count() << "  s" << std::endl;
    std::cout << "execution time:" << std::chrono::duration_cast<std::chrono::milliseconds>( end_test - start_test ).count() << " ms" << std::endl;
    std::cout << "execution time:" << std::chrono::duration_cast<std::chrono::nanoseconds> ( end_test - start_test ).count() << " ns" << std::endl;
    
    int32_t abuf[100];
    collections::generic::CRingbuffer<int32_t> rb( 10 );
    rb.push( &abuf[0] );
    int32_t nval;
    rb.pop( &nval );
    return 0;
}



///
/// \brief push values onto queue while there is space, if full, it will spin
/// \param a_queue
///
void writer_test( collections::CQueueLockFreeSingle& a_queue )
{
    int32_t nData;
    for( uint64_t unIndex = 0; unIndex < cunNumberOfElementsToWrite; ++unIndex )
    {
        nData =  unIndex;// rand() % 255;
        #ifdef DEBUG
            cout << "push: " << nData << endl;
        #endif
        while( a_queue.push( static_cast<void*>( &nData ) ) == false )
        {
            // full spin
            #ifdef DEBUG
                cout << "push: full" << endl;
            #endif
            this_thread::yield();
        }
    }
}


///
/// \brief reader_test
/// \param a_queue
///
void reader_test( collections::CQueueLockFreeSingle& a_queue  )
{
    uint32_t unData;
    uint32_t unCount = 0;
    while( unCount++ < cunNumberOfElementsToWrite )
    {
        while( false == a_queue.pop( &unData ) )
        {
            #ifdef DEBUG
                cout << "pop: empty" << endl;
            #endif
            pthread_yield();
        }
        #ifdef DEBUG
            cout << "pop:" << unData << endl;
        #endif
    }
    #ifdef DEBUG
        cout << "popped:" << unCount-1 << endl;
    #endif
}
