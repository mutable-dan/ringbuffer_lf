#include "rb_generic.h"
#include <iostream>
#include <cstdint>
#include <cstring>

using namespace std;

namespace collections
{
    
    //  lock free single reade, single writer 
    ///////////////////////////////////////////
    CRingbuffer::CRingbuffer( uint64_t a_ulQueueItemCount ) :
        m_ulQueueItemCount( a_ulQueueItemCount ), 
        m_ulHead( 0 ), 
        m_ulTail( 0 ), 
        m_ulMask( a_ulQueueItemCount-1 )
    {
        m_nPageSize = getpagesize();
        if( 0 != posix_memalign( (void**)&m_pQueue, m_nPageSize, m_ulQueueItemCount*sizeof(uint8_t) ) )
        {
            cerr << "failed to get queue memory" << endl;
            m_pQueue = nullptr;
        }
    }
    
    CRingbuffer::~CRingbuffer()
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
    bool CRingbuffer::push( uint8_t* a_pData )
    {
      // full if head = tail+#elements  
      // not full then head < tail + #elements
      // want to wait if full !(head == tail + elements) or head != tail + #elements
        
      if( (m_ulTail + m_ulQueueItemCount) == m_ulHead )
      {
         return false;
      }
        
      *(m_pQueue + ( m_ulHead++ & m_ulMask )*sizeof(uint8_t) ) = *a_pData;
      return true;
    }

    
    /**
     * @brief pop at the tail
     *
     * @return uint8_t*
     **/
    uint8_t* CRingbuffer::pop( uint8_t* a_pData )
    {
      // empty if head == tail
      // wait if empty !(head == tail) or head != tail

      if( m_ulTail == m_ulHead )
      {
         return nullptr;
      }
        
      *a_pData = *(m_pQueue + (m_ulTail++ & m_ulMask)*sizeof(uint8_t) );
      return a_pData;
    }
       
}
