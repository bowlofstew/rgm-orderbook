#ifndef __ORDER_LIST_HPP__
#define __ORDER_LIST_HPP__

#include <stdint.h>
#include <list>
#include <memory>

#include "Order.hpp"
#include "PoolAllocator.hpp"


namespace RgmInterview {
	namespace OrderBook {

		typedef std::list < Order_ptr > OrderNode_list;

		class OrderList
		{
		public:
			uint32_t total_volume;
			OrderList();
			~OrderList();
			OrderNode_list::iterator add ( Order_ptr const & order );
			void remove ( OrderNode_list::iterator const & order_iter );
			bool empty() const;
			size_t size() const;
			OrderNode_list::iterator begin();
			OrderNode_list::iterator end();
			static inline void* operator new ( std::size_t sz )
			{
				return PoolAllocator<OrderList>::instance().allocate ( sz ) ;
			}

			static inline void operator delete ( void* p )
			{
				PoolAllocator<OrderList>::instance().deallocate ( static_cast< OrderList * > ( p ), sizeof ( OrderList ) ) ;
			}
		private:
			;
			OrderList ( OrderList const & rhs ) {}
			OrderNode_list m_list;
		};
		typedef std::shared_ptr < OrderList > OrderList_ptr;
	}
}

#endif
