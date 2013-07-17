#include <assert.h>
#include <cmath>
#include <sstream>

#include "Constants.hpp"
#include "Order.hpp"

namespace RgmInterview {
	namespace OrderBook {

		/*
		 * I am converting the price from double into a uint32_t. This is to make comparisons further down the easier.
		 * Otherwise, I would have to result to ' std::abs(a-b) < std::numeric_limits<double>::epsilon() ' for just about
		 * every operation involving doubles.
		 *
		 * We obviously have to pick Constants::round_size properly.
		 */
		Order::Order (
			OrderSide::Side side,
			uint32_t volume,
			uint32_t price ) :
			m_side ( side ),
			m_volume ( volume ),
			m_price ( price )
		{
			assert ( m_volume > 0 );
			assert ( m_price > 0 );
		}

		OrderSide::Side Order::side() const
		{
			return m_side;
		}

		uint32_t Order::volume() const  {
			return m_volume;
		}

		uint32_t Order::price() const  {
			return m_price;
		}

		void Order::reduce ( uint32_t volume )
		{
			assert ( m_volume > volume );
			m_volume -= volume;
		}
	}
}

