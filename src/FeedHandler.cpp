#include <algorithm>
#include <iostream>
#include <limits>
#include <assert.h>
#include <stdlib.h>
#include <stdexcept>
#include <cmath>

#include "FeedHandler.hpp"

namespace RgmInterview {
	namespace OrderBook {

		// valid order actions (A,R)
		const char FeedHandler::f_add ( 'A' );
		const char FeedHandler::f_reduce ( 'R' );

		// valid sides are (B,S)
		const char FeedHandler::f_buy ( 'B' );
		const char FeedHandler::f_sell ( 'S' );

		// fields seperated by ( )
		const char FeedHandler::f_whitespace ( ' ' );

		// also, allow dos style formatting .. where our lines still have a \r at the end
		const char FeedHandler::f_return ( '\r' );

		FeedHandler::FeedHandler ( uint32_t target_size ) :
			m_target_size ( target_size ),
			m_book ( m_error_summary, target_size )
		{
		}

		FeedHandler::~FeedHandler()
		{
		}

		void FeedHandler::processMessage ( const std::string &line, std::ostream &os )
		{
			try
			{
				size_t timestamp_begin ( 0 );
				size_t timestamp_end ( line.find ( f_whitespace ) );
				size_t action_begin ( timestamp_end + 1 );
				size_t action_end ( line.find ( f_whitespace, action_begin ) );
				size_t order_id_begin ( action_end + 1 );
				size_t order_id_end ( line.find ( f_whitespace, order_id_begin ) );
				if ( timestamp_end != std::string::npos &&
						action_end == action_begin + 1 &&
						order_id_end != std::string::npos )
				{
					char action ( line[action_begin] );
					std::string time ( line.substr ( timestamp_begin, timestamp_end ) );
					switch ( action )
					{
					case f_add:
					{
						size_t side_begin ( order_id_end + 1 );
						size_t side_end ( line.find ( f_whitespace, side_begin ) );
						size_t price_begin ( side_end + 1 );
						size_t price_end ( line.find ( f_whitespace, price_begin ) );
						size_t size_begin ( price_end + 1 );
						size_t size_end ( line.size() );
						uint32_t size;
						double price;
						if ( price_end != std::string::npos &&
								side_end == side_begin + 1 &&
								( line[side_begin] == f_buy || line[side_begin] == f_sell ) &&
								tryParse ( &line[price_begin], price_end - price_begin, price ) &&
								tryParse ( &line[size_begin], size_end - size_begin, size ) )
							processAddOrderMessage ( line.substr ( order_id_begin, order_id_end - order_id_begin ),
													 ( line[side_begin] == f_buy ? OrderSide::BUY : OrderSide::SELL ),
													 size,
													 price,
													 time,
													 os );
						else
							m_error_summary.corrupted_messages++;
						break;
					}
					case f_reduce:
					{
						uint32_t size;
						size_t size_begin ( order_id_end + 1 );
						size_t size_end ( line.size() );
						if ( tryParse ( &line[size_begin], size_end - size_begin, size ) )
							processReduceOrderMessage ( line.substr ( order_id_begin, order_id_end - order_id_begin ),
														size,
														time,
														os );
						else
						{
							m_error_summary.out_of_bounds_or_weird_numbers++;
						}
						break;
					}
					default:
						m_error_summary.corrupted_messages++;
						return;
					}
				}
				else
				{
					m_error_summary.corrupted_messages++;
					return;
				}
			} catch ( std::runtime_error & )
			{
				// ouch - I really shouldn't get here
				m_error_summary.unexpected_exception++;
			}
		}

		void FeedHandler::processAddOrderMessage ( std::string const & order_id,
				OrderSide::Side side,
				uint32_t size,
				double price,
				std::string const & time,
				std::ostream & os )
		{
			Order_ptr order ( new Order  (
								  side,
								  size,
								  static_cast < uint32_t > ( std::floor ( price * Constants::round_size ) ) ) );
			// if we can't add this order, we have to dispose it ourselves
			if ( !m_book.add ( order, order_id, side, time, os ) )
				delete ( order );
		}

		void FeedHandler::processReduceOrderMessage ( std::string const & order_id,
				uint32_t size,
				std::string const & time,
				std::ostream & os )
		{
			m_book.reduce ( order_id,
							size,
							time,
							os );
		}

		void FeedHandler::printErrorSummary ( std::ostream & os ) const
		{
			os << "Errors:" << std::endl;
			os << m_error_summary;
		}

		OrderBook const & FeedHandler::book() const
		{
			return m_book;
		}

		ErrorSummary const & FeedHandler::errors() const
		{
			return m_error_summary;
		}

		bool FeedHandler::tryParse ( const char * input, size_t len, double & out )
		{
			char* endptr;
			out = strtod ( input, &endptr );
			// success if we processed exactly the number of characters we expected
			return ( endptr == input + len && out > 0 );
		}

		bool FeedHandler::tryParse ( const char * input, size_t len, uint32_t & out )
		{
			char * endptr;
			out = strtoul ( input, &endptr, 10 );
			// success if we processed exactly the number of characters we expected and there's no '-' in there
			return ( std::find ( input, input + len, '-' ) == input + len &&
					 endptr == input + len &&
					 ( len < 10 || !isUIntOverflow ( input, len ) ) );
		}

		/*
		* A not so quick check to see if the value's bigger than uint32_t::max
		*/
		bool FeedHandler::isUIntOverflow ( const char * input, size_t len )
		{
			static const char * max_size ( "4294967295" );
			if ( len > 10 )
				return true;
			for ( size_t i = 0; i < len && input[i] >= max_size[i] ; i++ )
			{
				if ( input[i] > max_size[i] )
					return true;
			}
			return false;
		}

		CE double FeedHandler::maxPrice()
		{
			return std::floor ( std::numeric_limits<uint32_t>::max() / Constants::round_size );
		}
	}
}
