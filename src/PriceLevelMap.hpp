#ifndef __ORDER_MAP_HPP__
#define __ORDER_MAP_HPP__

#include <assert.h>
#include <map>
#include <unordered_map>
#include <limits>

#include "OrderList.hpp"

namespace RgmInterview {
	namespace OrderBook {

		/*
		* A map+table that has constant time lookups, but still O(logN) only when we create a new price level
		*/
		template <class T>
		class PriceLevelMap
		{
		public:
			uint32_t total_volume;
			typedef typename std::map < uint32_t, OrderList_ptr, T > LevelsTree;

			PriceLevelMap ( uint32_t target_volume ) : total_volume ( 0 ),
				m_cached_total_value ( std::numeric_limits<uint32_t>::max() ),
				m_last_considered_level ( std::numeric_limits<uint32_t>::max() ),
				m_target_volume ( target_volume )
			{
			}

			/* Add( O(1) ) or Find ( O(logN) ) the price level in the map */
			OrderList_ptr & add ( uint32_t price )
			{
				// this resets the cached value
				if ( m_last_considered_level != std::numeric_limits<uint32_t>::max() &&
						T() ( price, m_last_considered_level ) )
				{
					m_last_considered_level =  std::numeric_limits<uint32_t>::max();
					m_cached_total_value = std::numeric_limits<uint32_t>::max();
				}
				typename LevelsTable::iterator iter ( m_table.find ( price ) );
				if ( iter != m_table.end() )
					return iter->second->second;
				else
				{
					OrderList_ptr node_list = std::make_shared < OrderList > ();
					typename LevelsTree::iterator iter = m_tree.insert ( std::make_pair ( price, node_list ) ).first;
					m_table.insert ( std::make_pair ( price, iter ) );
					assert ( iter->second->total_volume == 0 );
					return iter->second;
				}
			}

			/* Returns true if this takes out the whole order, false otherwise */
			bool reduce ( OrderNode_list::iterator & order_iter,
						  uint32_t volume )
			{
				Order_ptr order ( ( *order_iter ) );
				OrderList_ptr & price_level ( add ( order->price() ) );
				// this resets the cached value
				if ( m_last_considered_level != std::numeric_limits<uint32_t>::max() &&
						( order->price() ==  m_last_considered_level ||
						  T() ( order->price(), m_last_considered_level ) ) )
				{
					m_last_considered_level =  std::numeric_limits<uint32_t>::max();
					m_cached_total_value = std::numeric_limits<uint32_t>::max();
				}
				if ( order->volume() <= volume )
				{
					assert ( price_level->total_volume > 0 );
					volume = order->volume();
					price_level->remove ( order_iter );
					price_level->total_volume -= volume;
					if ( price_level->empty() )
						remove ( order->price() );
					delete ( order );
					total_volume -= volume;
					return true;
				}
				else
				{
					order->reduce ( volume );
					price_level->total_volume -= volume;
					total_volume -= volume;
					return false;
				}
			}

			uint32_t get_total_value ( )
			{
				uint32_t target_volume ( m_target_volume );
				uint32_t total_value ( std::numeric_limits<uint32_t>::max() );
				if ( total_volume >= target_volume )
				{
					if ( m_cached_total_value != std::numeric_limits<uint32_t>::max() )
						return m_cached_total_value;
					total_value = 0;
					for ( auto iter = begin();
							target_volume != 0 && iter != end();
							iter++ )
					{
						uint32_t price ( iter->first );
						OrderList_ptr const & list ( iter->second );
						uint32_t volume_traded_at_level ( std::min ( target_volume, list->total_volume ) );
						total_value += price * volume_traded_at_level;
						target_volume -= volume_traded_at_level;
						m_last_considered_level = price;
					}
					m_cached_total_value = total_value;
					assert ( target_volume == 0 );
				}
				return total_value;
			}

			bool empty() const
			{
				assert ( m_tree.empty() == m_table.empty() );
				assert ( !m_tree.empty() || total_volume == 0 );
				return m_tree.empty();
			}

			size_t size() const
			{
				assert ( m_tree.size() == m_table.size() );
				return m_tree.size();
			}

			void clear()
			{
				m_table.clear();
				m_tree.clear();
			}

			typename LevelsTree::const_iterator begin() const
			{
				return m_tree.begin();
			}

			typename LevelsTree::const_iterator end() const
			{
				return m_tree.end();
			}

		private:
			typedef typename std::unordered_map < uint32_t, typename LevelsTree::iterator > LevelsTable;
			LevelsTree m_tree;
			LevelsTable m_table;
			uint32_t m_cached_total_value;
			uint32_t m_last_considered_level;
			uint32_t m_target_volume;

			/* Remove ( O(1) ) the price level from the map */
			void remove ( uint32_t price )
			{
				typename LevelsTable::const_iterator iter ( m_table.find ( price ) );
				assert ( iter != m_table.end() );
				assert ( iter->second->second->total_volume == 0 );
				assert ( iter->second->second->empty() );
				m_tree.erase ( iter->second );
				m_table.erase ( iter );
			}
		};
	}
}

#endif