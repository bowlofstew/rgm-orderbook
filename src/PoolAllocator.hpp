#ifndef __POOL_ALLOCATOR_HPP__
#define __POOL_ALLOCATOR_HPP__

#include <assert.h>
#include <stack>
#include <memory>

namespace RgmInterview {
	namespace OrderBook {

		template <class T>
		class PoolAllocator : public std::allocator<T>
		{
		public:
			/* The pool allocator cuts down on the number of mallocs we have to do. On the flip-side, it is less memory efficient.  */
			PoolAllocator<T> ( )
			{
				for ( size_t i = 0 ; i < 200; i++ )
					m_stack.push ( &tArr[i] );
			}

			static PoolAllocator<T> & instance()
			{
				static PoolAllocator<T> instance;
				return instance;
			}

			~PoolAllocator<T>()
			{
				while ( !m_stack.empty() )
				{
					T* top = m_stack.top();
					assert ( top );
					if ( top < tArr || top >= tArr + 200 * sizeof ( T ) )
						std::allocator<T>::deallocate ( top, sizeof ( T ) );
					m_stack.pop();
				}
			}

			T* allocate ( size_t n, const void * hint = 0 )
			{
				if ( m_stack.empty() )
					return std::allocator<T>::allocate ( n, hint );
				T* t = m_stack.top();
				m_stack.pop();
				return t;
			}

			void deallocate ( T* t, size_t n )
			{
				static const size_t max_size ( 1000 );
				assert ( t );
				if ( m_stack.size() < max_size )
					m_stack.push ( t );
				else if ( t < tArr || t >= tArr + 200 * sizeof ( T ) )
					std::allocator<T>::deallocate ( t, n );
			}

		private:
			std::stack<T*> m_stack;
			PoolAllocator<T> ( PoolAllocator<T> const & rhs ) {}
			T tArr[200];
		};
	}
}

#endif
