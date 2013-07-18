#ifndef __ORDER_BOOK_HPP__
#define __ORDER_BOOK_HPP__

#include <map>
#include <unordered_map>
#include <functional>
#include <string>

#include "Order.hpp"
#include "PriceLevelMap.hpp"
#include "OrderList.hpp"
#include "ErrorSummary.hpp"

namespace RgmInterview {
	namespace OrderBook {
		class OrderBook
		{
		public:
			typedef PriceLevelMap < std::greater<uint32_t> > BuyPriceLevelMap;
			typedef PriceLevelMap < std::less<uint32_t> > SellPriceLevelMap;

			OrderBook ( ErrorSummary & error_summary,
						uint32_t target_size );
			~OrderBook();

			bool add ( Order_ptr const & order,
					   std::string const & order_id,
					   OrderSide::Side side,
					   std::string const & time,
					   std::ostream &os ) ;
			void reduce ( std::string const & order_id,
						  uint32_t volume,
						  std::string const & time,
						  std::ostream &os ) ;

			BuyPriceLevelMap const & buys() const
			{
				return m_buys;
			}

			SellPriceLevelMap const & sells() const
			{
				return m_sells;
			}
		private:
			typedef std::unordered_map < std::string, OrderNode_list::iterator > OrderDict;

			ErrorSummary & m_error_summary;
			uint32_t m_target_size;
			BuyPriceLevelMap m_buys;
			SellPriceLevelMap m_sells;
			OrderDict m_all_orders;

			/*
			* When we need to operate on an (Buy/Sell)OrderMap, we just use these bound functions.
			* They are indexed by order type, and we don't have to supply the map or comparison operator anymore.
			*/
			typedef std::function<OrderNode_list::iterator ( Order_ptr const & ) > Add_functor;
			typedef std::function<void ( std::string const &, OrderNode_list::iterator &, uint32_t ) > Reduce_functor;
			typedef std::function<void ( std::string const &, std::ostream & ) > Check_functor;
			Add_functor m_add_functors[2];
			Reduce_functor m_reduce_functors[2];
			Check_functor m_check_functors[2];
			uint32_t m_last_values[2];

			template <class T>
			OrderNode_list::iterator add ( T & map,
										   Order_ptr const & order );

			template <class T>
			inline void reduce ( T & map,
								 std::string const & order_id,
								 OrderNode_list::iterator & order,
								 uint32_t volume );

			template <class T>
			void check ( T & map,
						 OrderSide::Side side,
						 std::string const & time,
						 std::ostream &os );
		};

		typedef OrderBook * OrderBook_ptr;
	}
}


#endif