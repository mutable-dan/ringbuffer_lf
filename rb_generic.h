#pragma once

#include <stdint.h>
#include <unistd.h>
#include <cstdlib>
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

/*
 * sample ringbuffer that stores pointers t uchars
 * sample generic ringbuffer that store pointers to typename
 * 
 */


namespace collections
{

    const int64_t g_lCachLine = 64;
   
    // this pushes pointers to uint8_t a_pData
    class CRingbuffer final
    {
        private:
                    std::atomic<uint64_t>       m_ulQueueItemCount;
                    uint8_t                     m_padding_0[g_lCachLine-sizeof(int64_t)];
                    std::atomic<uint64_t>       m_ulHead    = { 0 };
                    uint8_t                     m_padding_1[g_lCachLine-sizeof(int64_t)];
                    std::atomic<uint64_t>       m_ulTail    = { 0 };
                    uint8_t                     m_padding_2[g_lCachLine-sizeof(int64_t)];
            const   uint64_t                    m_ulMask;
                    uint8_t*                    m_pQueue;
            
                    // diagnotics
                    int                         m_nPageSize = 0;

        public:
            CRingbuffer( uint64_t a_ulQueueItemCount );
            CRingbuffer( const CRingbuffer& ) = delete;
            ~CRingbuffer();
            
            CRingbuffer&   operator= ( const CRingbuffer& other ) = delete;
            bool           operator==(const CRingbuffer& other)   = delete;
            
            uint8_t* pop ( uint8_t* a_pData );
            bool     push( uint8_t* a_pData );
    };   
    
    
    namespace generic
    {
      template<typename T> class CRingbuffer final
      {
         private:
                     std::atomic<uint64_t>       m_ulQueueItemCount;
                     uint8_t                     m_padding_0[g_lCachLine-sizeof(int64_t)];
                     std::atomic<uint64_t>       m_ulHead;
                     uint8_t                     m_padding_1[g_lCachLine-sizeof(int64_t)];
                     std::atomic<uint64_t>       m_ulTail;
                     uint8_t                     m_padding_2[g_lCachLine-sizeof(int64_t)];
               const uint64_t                    m_ulMask;
                     T*                          m_pQueue;
               
               // diagnotics
               int         m_nPageSize;

         public:
               CRingbuffer( uint64_t a_ulQueueItemCount ) :
                  m_ulQueueItemCount( a_ulQueueItemCount ), 
                  m_ulMask( a_ulQueueItemCount-1 )
               {
                  m_nPageSize = getpagesize();
                  if( 0 != posix_memalign( (void**)&m_pQueue, m_nPageSize, m_ulQueueItemCount*sizeof(T) ) )
                  {
                        m_pQueue = nullptr;
                  }
               }
               
               
               ~CRingbuffer()
               {
                  if( nullptr != m_pQueue )
                  {
                        free( m_pQueue );
                        m_pQueue = nullptr;
                  }
               }
               
               CRingbuffer( const CRingbuffer& ) = delete;
               
               CRingbuffer&   operator= ( const CRingbuffer& other ) = delete;
               bool           operator==(const CRingbuffer& other)   = delete;
               
               bool     push( T* a_pData )
               {
                  // full if head = tail+#elements  
                  // not full then head < tail + #elements
                  // want to wait if full !(head == tail + elements) or head != tail + #elements
                  
                  if( (m_ulTail + m_ulQueueItemCount) == m_ulHead )
                  {
                     return false;
                  }
                  
                  *(m_pQueue + ( m_ulHead++ & m_ulMask )*sizeof(T) ) = *a_pData;
                  return true;
               }
               
               T* pop ( T* a_pData )
               {
                  // empty if head == tail
                  // wait if empty !(head == tail) or head != tail

                  if( m_ulTail == m_ulHead )
                  {
                     return nullptr;
                  }
                  
                  *a_pData = *(m_pQueue + (m_ulTail++ & m_ulMask)*sizeof(T) );
                  return a_pData;
               }
               
      };   
    }
    
    
}
