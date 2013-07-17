#include <assert.h>
#include <algorithm>

#include "OrderList.hpp"

namespace RgmInterview {
	namespace OrderBook {

		OrderList::OrderList() : total_volume ( 0 )
		{
			assert ( total_volume == 0 );
		}

		OrderList::~OrderList()
		{
			for ( OrderNode_list::iterator iter = m_list.begin();
					iter != m_list.end();
					iter++ )
				delete ( ( *iter ) );
			total_volume = 0;
		}

		OrderNode_list::iterator OrderList::begin()
		{
			return m_list.begin();
		}

		OrderNode_list::iterator OrderList::end()
		{
			return m_list.end();
		}

		bool OrderList::empty() const
		{
			return m_list.empty();
		}

		size_t OrderList::size() const
		{
			return m_list.size();
		}

		OrderNode_list::iterator OrderList::add ( Order_ptr const & order )
		{
			m_list.push_back ( order );
			OrderNode_list::iterator last ( m_list.end() );
			return --last;
		}

		void OrderList::remove ( OrderNode_list::iterator order_iter )
		{
			OrderNode_list::iterator c_iter ( m_list.begin() );
			m_list.erase ( order_iter );
		}
	}
}
