#ifndef _WIN32
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE RgmBookTests

#include <algorithm>
#include <limits>
#include <iomanip>

#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>

#ifdef PROFILE
#include <gperftools/profiler.h>
#endif

#include "OrderList.hpp"
#include "OrderBook.hpp"
#include "FeedHandler.hpp"

using namespace RgmInterview::OrderBook;

BOOST_AUTO_TEST_CASE ( processCorrectLines )
{
	FeedHandler handler ( 200 );
	OrderBook::BuyPriceLevelMap buys ( handler.book().buys() );
	OrderBook::SellPriceLevelMap sells ( handler.book().sells() );
	ErrorSummary const & errors ( handler.errors() );
	std::ostringstream os;
	handler.processMessage ( "28800538 A b S 44.26 100", os );
	handler.processMessage ( "28800562 A c B 44.10 100", os );
	handler.processMessage ( "28800744 R b 100", os );
	handler.processMessage ( "28800758 A d B 44.18 157", os );
	handler.processMessage ( "28800773 A e S 44.38 100", os );
	handler.processMessage ( "28800796 R d 157", os );
	handler.processMessage ( "28800812 A f B 44.18 157", os );
	handler.processMessage ( "28800974 A g S 44.27 100", os );
	handler.processMessage ( "28800975 R e 100", os );
	handler.processMessage ( "28812071 R f 100", os );
	handler.processMessage ( "28813129 A h B 43.68 50", os );
	handler.processMessage ( "28813300 R f 57", os );
	handler.processMessage ( "28813830 A i S 44.18 100", os );
	handler.processMessage ( "28814087 A j S 44.18 1000", os );
	handler.processMessage ( "28814834 R c 100", os );
	handler.processMessage ( "28814864 A k B 44.09 100", os );
	handler.processMessage ( "28815774 R k 100", os );
	handler.processMessage ( "28815804 A l B 44.07 175", os );
	handler.processMessage ( "28815937 R j 1000", os );
	handler.processMessage ( "28816245 A m S 44.22 100", os );
	BOOST_CHECK ( errors.empty() );
}

// we don't like too many spaces, unexpected action (not A/R) or side ( not B/S) or prices
BOOST_AUTO_TEST_CASE ( processCorruptedLines )
{
	FeedHandler handler ( 200 );
	ErrorSummary const & errors ( handler.errors() );
	std::ostringstream os;
	handler.processMessage ( "28800538 *  b S 44.26 100", os );
	BOOST_CHECK_EQUAL ( errors.corrupted_messages, ( size_t ) 1 );
	handler.processMessage ( "28800538 A  b T 44.26 100", os );
	BOOST_CHECK_EQUAL ( errors.corrupted_messages, ( size_t ) 2 );
	handler.processMessage ( "28800538 A      b B AA.26 100", os );
	BOOST_CHECK_EQUAL ( errors.corrupted_messages, ( size_t ) 3 );
	handler.processMessage ( "28800538 A b S 44.26   100", os );
	BOOST_CHECK_EQUAL ( errors.corrupted_messages, ( size_t ) 3 );
	handler.processMessage ( "28800538 A b S 44.26 A", os );
	BOOST_CHECK_EQUAL ( errors.corrupted_messages, ( size_t ) 4 );
}

BOOST_AUTO_TEST_CASE ( reduceUnknownOrder )
{
	FeedHandler handler ( 200 );
	OrderBook::BuyPriceLevelMap buys ( handler.book().buys() );
	OrderBook::SellPriceLevelMap sells ( handler.book().sells() );
	ErrorSummary const & errors ( handler.errors() );
	std::ostringstream os;
	handler.processMessage ( "28800744 R b 100", os );
	BOOST_CHECK_EQUAL ( errors.order_modify_on_order_i_dont_know, ( size_t ) 1 );
}

BOOST_AUTO_TEST_CASE ( addOrderTwice )
{
	FeedHandler handler ( 200 );
	OrderBook::BuyPriceLevelMap buys ( handler.book().buys() );
	OrderBook::SellPriceLevelMap sells ( handler.book().sells() );
	ErrorSummary const & errors ( handler.errors() );
	std::ostringstream os;
	handler.processMessage ( "28800538 A b S 44.26 100", os );
	handler.processMessage ( "28800538 A b S 44.26 100", os );
	BOOST_CHECK_EQUAL ( errors.duplicate_order_id, ( size_t ) 1 );
}

BOOST_AUTO_TEST_CASE ( reduceNegativeVolume )
{
	FeedHandler handler ( 200 );
	OrderBook::BuyPriceLevelMap buys ( handler.book().buys() );
	OrderBook::SellPriceLevelMap sells ( handler.book().sells() );
	ErrorSummary const & errors ( handler.errors() );
	std::ostringstream os;
	handler.processMessage ( "28800538 A b S 44.26 100", os );
	handler.processMessage ( "28800744 R b -100", os );
	BOOST_CHECK_EQUAL ( errors.out_of_bounds_or_weird_numbers, ( size_t ) 1 );
}
