#include "cqueue.h"
#include "rb_generic.h"
#include <iostream>
#include <unistd.h>
#include <thread>
#include <cstdlib>
#include <functional>
#include <chrono>



const uint64_t cunNumberOfElementsToWrite = 50000000;
const uint32_t cunQueueElementCount       = 1024;        // must be mult of 2
//const uint32_t cunQueueElementCount       = 8;        // must be mult of 2

void reader_test2( collections::CQueueLockFreeSingle& a_queue  );
void writer_test2( collections::CQueueLockFreeSingle& a_queue );


int main(int , char **) 
{
   std::cout << "-----------" << sizeof( int64_t ) << std::endl;
   std::chrono::time_point<std::chrono::system_clock> start_test1, end_test1;
   std::chrono::time_point<std::chrono::system_clock> start_test2, end_test2;
   (void)start_test1;
   (void)end_test1;


   // single reader writer
   std::cout << "Test lock circular buffer - single reader/writer lockfree locking" << std::endl;
   std::cout << "Theads supported by hardware:" << std::thread::hardware_concurrency() << std::endl;
   collections::CQueueLockFreeSingle lockedqueue_single( cunQueueElementCount, sizeof(uint32_t) );

   start_test2 = std::chrono::system_clock::now();
      std::thread thdWriter1_2( writer_test2, std::ref(lockedqueue_single) );
      std::thread thdreader1_2( reader_test2, std::ref(lockedqueue_single) );
      thdWriter1_2.join();
      thdreader1_2.join();
   end_test2 = std::chrono::system_clock::now();

   std::cout << std::endl << "test2" << std::endl;
   std::cout << "execution time:" << std::chrono::duration_cast<std::chrono::seconds>( end_test2 - start_test2 ).count()      << "  s" << std::endl;
   std::cout << "execution time:" << std::chrono::duration_cast<std::chrono::milliseconds>( end_test2 - start_test2 ).count() << " ms" << std::endl;
   std::cout << "execution time:" << std::chrono::duration_cast<std::chrono::nanoseconds>( end_test2 - start_test2 ).count()  << " ns" << std::endl;
    
   int32_t abuf[100];
   collections::generic::CRingbuffer<int32_t> rb( 10 );
   rb.push( &abuf[0] );
   int32_t nval;
   rb.pop( &nval );
   return 0;
}




void writer_test2( collections::CQueueLockFreeSingle& a_queue )
{
   int32_t nData;
   for( uint64_t unIndex = 0; unIndex < cunNumberOfElementsToWrite; ++unIndex )
   {
      nData = rand() % 255;  // populate array
      while( a_queue.push( static_cast<void*>( &nData ) ) == false )
      {
         pthread_yield();
      }
   }
}

void reader_test2( collections::CQueueLockFreeSingle& a_queue  )
{
    uint32_t punData;
    uint32_t unCount = 0;
    void* pRes = nullptr;
    while( unCount++ < cunNumberOfElementsToWrite )
    {
      pRes  = a_queue.pop( &punData );
      while( nullptr == pRes ) 
      {
         pRes  = a_queue.pop( &punData );
         pthread_yield();
      } 

    }
    //std::cout << "done RD" << std::endl;
}
