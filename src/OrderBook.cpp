#include <algorithm>
#include <assert.h>
#include <functional>

#include "Constants.hpp"
#include "OrderBook.hpp"

namespace RgmInterview {
	namespace OrderBook {

		OrderBook::OrderBook ( ErrorSummary & error_summary,
							   uint32_t target_size ) :
			m_error_summary ( error_summary ),
			m_target_size ( target_size )
		{
			m_add_functors[ OrderSide::BUY ] = std::bind ( &OrderBook::add<BuyPriceLevelMap>, this, std::ref ( m_buys ), std::placeholders::_1 );
			m_add_functors[ OrderSide::SELL ] = std::bind ( &OrderBook::add<SellPriceLevelMap>, this, std::ref ( m_sells ), std::placeholders::_1 );
			m_reduce_functors [ OrderSide::BUY ] = std::bind ( &OrderBook::reduce<BuyPriceLevelMap>, this, std::ref ( m_buys ), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
			m_reduce_functors [ OrderSide::SELL ] = std::bind ( &OrderBook::reduce<SellPriceLevelMap>, this, std::ref ( m_sells ), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
			m_check_functors  [ OrderSide::BUY ] = std::bind ( &OrderBook::check<BuyPriceLevelMap>, this, std::ref ( m_buys ), OrderSide::BUY, std::placeholders::_1, std::placeholders::_2 );
			m_check_functors  [ OrderSide::SELL ] = std::bind ( &OrderBook::check<SellPriceLevelMap>, this, std::ref ( m_sells ), OrderSide::SELL, std::placeholders::_1, std::placeholders::_2 );
			m_last_values [ OrderSide::BUY ] = std::numeric_limits<uint32_t>::max();
			m_last_values [ OrderSide::SELL ] = std::numeric_limits<uint32_t>::max();
		}

		/*
		* The orderbook knows all about our orders, so should dealloc them here
		*/
		OrderBook::~OrderBook()
		{
			m_buys.clear();
			m_sells.clear();
		}

		/*
		* Create a new 'price level' if we have to,
		* and add the order to it.
		* Returns true if succesful, false if the order already exists
		*/
		bool OrderBook::add ( Order_ptr const & order,
							  std::string const & order_id,
							  OrderSide::Side side,
							  std::string const & time,
							  std::ostream &os )
		{
			assert ( order->price() > 0 );
			OrderDict::iterator iter ( m_all_orders.find ( order_id ) );
			if ( iter == m_all_orders.end() )
			{
				m_all_orders.insert ( std::make_pair ( order_id, m_add_functors [ side ] ( order ) ) );
				m_check_functors [ side ] ( time, os );
				return true;
			}
			else
			{
				m_error_summary.duplicate_order_id++;
				return false;
			}
		}

		template <class T>
		OrderNode_list::iterator OrderBook::add ( T & map, Order_ptr const & order )
		{
			OrderList_ptr & list ( map.add ( order->price() ) );
			assert ( list->total_volume >= 0 );
			map.total_volume += order->volume();
			list->total_volume += order->volume();
			assert ( list->total_volume > 0 );
			OrderNode_list::iterator return_iter = list->add ( order );
			assert ( ( *return_iter ) == order );
			return return_iter;
		}

		void OrderBook::reduce ( std::string const & order_id,
								 uint32_t volume,
								 std::string const & time,
								 std::ostream &os )
		{
			OrderDict::iterator iter ( m_all_orders.find ( order_id ) );
			if ( iter != m_all_orders.end() )
			{
				Order_ptr const & order ( ( *iter->second ) );
				OrderSide::Side side ( order->side() );
				m_reduce_functors [ side ] ( order_id, iter->second, volume );
				m_check_functors [ side ] ( time, os );
			}
			else
			{
				m_error_summary.order_modify_on_order_i_dont_know ++;
			}
		}

		template <class T>
		void OrderBook::reduce ( T & map,
								 std::string const & order_id,
								 OrderNode_list::iterator & order_iter,
								 uint32_t volume )
		{
			if ( map.reduce ( order_iter, volume ) )
				m_all_orders.erase ( order_id );
		}

		template <class T>
		void OrderBook::check ( T  & map,
								OrderSide::Side side,
								std::string const & time,
								std::ostream &os )
		{
			uint32_t new_value ( map.get_total_value ( m_target_size ) );
			if ( m_last_values [ side ] != new_value )
			{
				m_last_values [ side ] = new_value;
				if ( new_value != std::numeric_limits<uint32_t>::max() )
				{
					double val ( new_value / Constants::round_size );
					printf ( "%s %c %0.2f\n", time.c_str(), ( side == OrderSide::BUY ? 'S' : 'B' ), val );
				}
				else
					printf ( "%s %c NA\n", time.c_str(), ( side == OrderSide::BUY ? 'S' : 'B' ) );
			}
		}
	}
}
