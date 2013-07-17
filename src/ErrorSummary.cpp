#include "ErrorSummary.hpp"

namespace RgmInterview {
	namespace OrderBook {

		ErrorSummary::ErrorSummary() :
			corrupted_messages ( 0 ),
			out_of_bounds_or_weird_numbers ( 0 ),
			order_modify_on_order_i_dont_know ( 0 ),
			duplicate_order_id ( 0 ),
			unexpected_exception ( 0 )
		{
		}

		std::ostream& operator<< ( std::ostream& os, const ErrorSummary& sum )
		{
			os << "[ GLOBAL] Corrupted messages: " << sum.corrupted_messages << std::endl;
			os << "[ GLOBAL] Out of bounds or otherwise weird data: " << sum.out_of_bounds_or_weird_numbers << std::endl;
			os << "[  ORDER] Modify without corresponding order: " << sum.order_modify_on_order_i_dont_know << std::endl;
			os << "[  ORDER] Duplicate order id: " << sum.duplicate_order_id << std::endl;
			os << "[SERIOUS] Unexpected exception: " << sum.unexpected_exception << std::endl;
			return os;
		}

		bool ErrorSummary::empty() const
		{
			return !corrupted_messages &&
				   !duplicate_order_id &&
				   !out_of_bounds_or_weird_numbers &&
				   !order_modify_on_order_i_dont_know &&
				   !unexpected_exception;
		}
	}
}