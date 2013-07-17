# My solution to 
http://www.rgmadvisors.com/problems/orderbook/

Processes messages such as 
28800538 A b S 44.26 100
28800562 A c B 44.10 100
28800744 R b 100
28800758 A d B 44.18 157
28800773 A e S 44.38 100
28800796 R d 157
28800812 A f B 44.18 157
28800974 A g S 44.27 100
28800975 R e 100

Which deal with adding orders to an order book and reducing volume as well, possibly removing the 
orders completely.

# Questions
* How did you choose your implementation language?

I like C++. It's fast, efficient and looks pretty elegant.

* What is the time complexity for processing an Add Order message?

O(1) if you're adding to an existing level, O(logN) otherwise because we will create
a new 'price level' and add the order there. This involves finding the right spot in a binary
tree which is that O(logN)

* What is the time complexity for processing a Reduce Order message?

O(1). Finding and reducing the order is done through hash tables, which are constant time. 

* Time complexity for finding the total expense

We need to do this every time our expected value could have changed:
- There's enough volume in the total book
- We've changed a relevant price ( added/removed up to the last considered level )

If this is the case, it will take O(N) time because we just might to consider every >> price level <<.
Note that we do this on a price level 'level', not per order.

* If your implementation were put into production and found to be too slow, what ideas would you try out to improve its performance? (Other than reimplementing it in a different language such as C or C++.) 

I'm already caching values; if we add/remove on a level that isn't relevant to the total value, we 
don't need to recalculate. We can make a couple of small improvements here to calculate even less.

