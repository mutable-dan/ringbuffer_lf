#pragma once

#include <stdint.h>
#include <atomic>


/*
 * size of queue, n: 4
 * 00
 * 01
 * 10
 * 11
 * mask: n(100) -1 = 11
 * 
 * pos  mask    val
 * 00   11      00
 * 01   11      01
 * 10   11      10
 * 11   11      11
 * 
 * push -> from head
 * pop  -> from tail
 * 
 * 3
 * 2
 * 1
 * 0 <-- head=0, tail=0 (empty)
 * 
 * push[1]
 * 3
 * 2
 * 1 <-- head=1
 * 0 <-- tail=0
 * 
 * pop[0]
 * 3
 * 2
 * 1 <--head=1, tail=1  (empty)
 * 0
 * 
 * push[1]
 * 3
 * 2 <-- head=2
 * 1 <-- tail=1
 * 0
 * 
 * push[2]
 * 3 <-- head=3
 * 2
 * 1 <-- tail=1
 * 0
 * 
 * push[3]
 * 3
 * 2
 * 1 <-- tail=1 
 * 0 <-- head(4)=0
 * 
 * push[4]
 * 3
 * 2
 * 1 <-- head(5)=1 tail=1 (full)
 * 0
 * 
 * pop[3]
 * 3
 * 2 <-- tail=2
 * 1 <-- head(5)=1
 * 0
 * 
 * pop[2]
 * 3 <-- tail=3
 * 2
 * 1 <-- head(5)=1
 * 0
 * 
 * pop[1]
 * 3
 * 2
 * 1 <-- head(5)=1
 * 0 <-- tail(4)=0
 * 
 * pop[0]
 * 3
 * 2
 * 1 <-- head(5)=1 tail(5)=1 (empty)
 * 0
 * 
 * empty    head = tail
 * full     head&mask = tail&mask  or head = tail+#elements
 * not full tail < head  or head < tail + #elements
 * 
 * 
 * tail + qsize-1 > head -> not full
 * tail < head           -> not full
 * tail == head          -> empty
 * 
 * push - wait if full
 * 
 * 
 */

namespace collections
{
   const int64_t lCachLine = 64;
   
    // note:  to push a class, this will need to be modified to do placement new
    // allocates a ringbuffer of n*size of data and pushes data onto queue and copies data off
    class CQueueLockFreeSingle final
    {
        private:
                    std::atomic<uint64_t>       m_ulQueueItemCount;
                    uint8_t                     m_padding_0[lCachLine-sizeof(int64_t)];
                    std::atomic<uint64_t>       m_ulHead;
                    uint8_t                     m_padding_1[lCachLine-sizeof(int64_t)];
                    std::atomic<uint64_t>       m_ulTail;
                    uint8_t                     m_padding_2[lCachLine-sizeof(int64_t)];
            const   uint64_t                m_ulDataSize;
            const   uint64_t                m_ulMask;
                    void*                   m_pQueue;
            
            // diagnotics
            int         m_nPageSize;

        public:
            CQueueLockFreeSingle( uint64_t a_ulQueueItemCount, uint64_t a_ulDataSize );
            CQueueLockFreeSingle( const CQueueLockFreeSingle& ) = delete;
            ~CQueueLockFreeSingle();
            
            CQueueLockFreeSingle& operator= ( const CQueueLockFreeSingle& other ) = delete;
            bool    operator==(const CQueueLockFreeSingle& other)   = delete;
            
            void* pop(  void* a_pData );
            bool  push( void* a_pData );
    };
    
    
    
    
}
