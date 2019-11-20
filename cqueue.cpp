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
        m_ulQueueSize( a_ulQueueItemCount ),
        m_ulDataSize ( a_ulDataSize ),
        m_ulMask     ( a_ulQueueItemCount-1 )
    {
        m_nPageSize = getpagesize();
        if( 0 != posix_memalign( &m_pQueue, m_nPageSize, m_ulQueueSize*m_ulDataSize ) )
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
      // full if head = tail+#elements  
      // not full then head < tail + #elements
      // want to wait if full !(head == tail + elements) or head != tail + #elements
        
      if( (m_ulTail + m_ulQueueSize) == m_ulHead )
      {
         return false;
      }
        
      // datasize is m_ulDataSize
      // cast void as char, thus an int32 would have a size of 4 in x86
      memcpy( ( static_cast<char*>(m_pQueue) + ( m_ulHead++ & m_ulMask )*m_ulDataSize), a_pData, m_ulDataSize );
      return true;
    }

    
    /**
     * @brief pop at the tail
     *
     * @return void*
     **/
    bool CQueueLockFreeSingle::pop( void* a_pData )
    {
      // empty if head == tail
      // wait if empty !(head == tail) or head != tail

      if( m_ulTail == m_ulHead )
      {
          // empty
          return false;
      }
        
      memcpy( a_pData, ( static_cast<char*>(m_pQueue) + (m_ulTail++ & m_ulMask)*m_ulDataSize), m_ulDataSize );
      return true;
    }
    
    
}
