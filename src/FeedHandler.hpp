#ifndef __FEED_HANDLER_HPP
#define __FEED_HANDLER_HPP

#ifdef _WIN32
#define CE
#else
#define CE constexpr
#endif

#include "Constants.hpp"
#include "Order.hpp"
#include "OrderBook.hpp"
#include "ErrorSummary.hpp"

namespace RgmInterview {
	namespace OrderBook {

		class FeedHandler
		{
		public:
			FeedHandler ( uint32_t target_size );
			~FeedHandler();
			void processMessage ( const std::string &line, std::ostream &os );
			void printErrorSummary ( std::ostream & os ) const;
			OrderBook const & book() const;
			ErrorSummary const & errors() const;
		private:
			static const char f_add ;
			static const char f_reduce;

			static const char f_buy;
			static const char f_sell;
			static const char f_whitespace;

			static const char f_return;
			FeedHandler ( FeedHandler const & rhs ) : m_book ( m_error_summary, rhs.m_target_size ) {}

			void processAddOrderMessage ( std::string const & order_id,
										  OrderSide::Side side,
										  uint32_t size,
										  double price,
										  std::string const & time,
										  std::ostream & os );

			void processReduceOrderMessage ( std::string const & order_id,
											 uint32_t size,
											 std::string const & time,
											 std::ostream & os );

			static bool tryParse ( const char * input, size_t len, double & out );
			static bool tryParse ( const char * input, size_t len, uint32_t & out );
			static bool isUIntOverflow ( const char * input, size_t len );

			static CE double maxPrice();

			ErrorSummary m_error_summary;
			uint32_t m_target_size;
			OrderBook m_book;
		};
	}
}

#endif