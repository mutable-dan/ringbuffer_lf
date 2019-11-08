#include "cqueue.h"
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <unistd.h>
#include <cstring>

using namespace std;

namespace collections
{
    
    //  lock free single reade, single writer 
    ///////////////////////////////////////////
    
    
    CQueueLockFreeSingle::CQueueLockFreeSingle( uint64_t a_ulQueueItemCount, uint64_t a_ulDataSize ) :
        m_ulQueueItemCount( a_ulQueueItemCount ), 
        m_ulHead( 0 ), 
        m_ulTail( 0 ), 
        m_ulDataSize( a_ulDataSize ), 
        m_ulMask( a_ulQueueItemCount-1 )
    {
        m_nPageSize = getpagesize();
        if( 0 != posix_memalign( &m_pQueue, m_nPageSize, m_ulQueueItemCount*m_ulDataSize ) )
        {
            cerr << "failed to get queue memory" << endl;
            m_pQueue = nullptr;
        }
    }
    
    CQueueLockFreeSingle::~CQueueLockFreeSingle()
    {
        if( nullptr != m_pQueue )
        {
            free( m_pQueue );
            m_pQueue = nullptr;
        }
    }

    
    
    /**
     * @brief push at the tail
     *
     * @param a_pData 
     * @return bool
     **/
    bool CQueueLockFreeSingle::push( void* a_pData )
    {
      /////std::unique_lock<std::mutex> lock( m_mtx );
      
      // full if head = tail+#elements  
      // not full then head < tail + #elements
      // want to wait if full !(head == tail + elements) or head != tail + #elements
        
      if( (m_ulTail + m_ulQueueItemCount) == m_ulHead )
      {
         return false;
      }
        
      memcpy( (m_pQueue + ( m_ulHead++ & m_ulMask )*m_ulDataSize), a_pData, m_ulDataSize ); // dest, src, size
      return true;
    }

    
    /**
     * @brief pop at the tail
     *
     * @return void*
     **/
    void* CQueueLockFreeSingle::pop( void* a_pData )
    {
      // empty if head == tail
      // wait if empty !(head == tail) or head != tail

      if( m_ulTail == m_ulHead )
      {
         return nullptr;
      }
        
      memcpy( a_pData, (m_pQueue + (m_ulTail++ & m_ulMask)*m_ulDataSize), m_ulDataSize );
      return a_pData;
    }
    
    
}
