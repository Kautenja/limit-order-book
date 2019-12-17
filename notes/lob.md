[source](https://web.archive.org/web/20110219155647/http://howtohft.wordpress.com/author/howtohft/)

# Building a Trading System – General Considerations

One of the biggest lessons I’ve learned is the important of separating the trading models–the mathematical or algorithmic logic that analyzes data and decides how to trade–from the the trading system–the general framework which provides interfaces for receiving data and interacting with the market to the trading models.  The nature of high frequency trading is such that it is critical to be able to rapidly iterate to adapt models to market conditions.  If a trading system has what is truly model logic strewn throughout, it can become a nightmare to modify model behavior; new, fundamentally different, models may have to be foregone because implementing them will take too much time.

The above point, I’m sure the reader is thinking, sounds obvious: of course concerns should be separated.  The problem is that it is often very difficult in practice to distinguish what should belong to the model from what should belong to the trading system before it has become an issue.  Let me give an example:

Suppose we have built a model that takes one tick of market data (either an execution or another message altering the limit order book), does some calculations, and then decides whether or not to place an order.  Prima facie, this looks like a reasonable general model, but it is not: in some cases it will leave us making trading decisions based on a false view of the market.  The reason for this is that while a stream of market data is serial, the events the data represents are not: some of the events are truly simultaneous.  For example, a large market order that sweeps the book will generate a set of executions at the range of limits the order filled at, all of which happened at the time the market order was placed.  A model that trades off of the first of these ticks will have an incorrect belief about the current price.  A correct implementation would allow a model to detect simultaneous events and delay a trading decision until they have all been processed.

A general trading model should be the source for every behavior that we could conceivably want to control or vary, and its interface with the trading system should be very carefully defined and limited so that it can be easily tested.  A trading system, on the other hand, should be everything that is static, primarily the connectivity with markets, databases, and other computers, an order management system, and interfaces for the models to access these resources.  If I was starting a HFT system from scratch, I would treat the development of the trading system as if I was writing an API–as opposed to a focused system for a specific task–to allow myself maximum flexibility during the process of model development.

The order management system (OMS), mentioned previously, is one of the most important parts of the trading system and, like the trading model interface, it is one where the most obvious implementations will lead to headaches down the road.  The primary mistake made in designing an OMS that I’ve seen, and not just ones used for HFT, is in coupling the intention to place an order with the actual placement of an order.  This can happen because the system was designed without an explicit OMS, so models generate orders which are sent directly to the market, with no intermediary object that has information about all open orders.  It can also happen when the OMS is designed as a simple conduit for reaching the market, so logic that requires orders to be in a certain state must block waiting for a response.  There are two main problems with these OMS designs: one, a blocked model will be unable to react to changing market conditions and will likely build up a backlog of stale market data while it waits for trading confirmations and two, these designs preclude the possibility of optimizing the trading behavior across all of the models that are using the OMS (An example of such an optimization might be two models placing orders in opposite directions for the same security.  An intelligent OMS might combine them into a single net order depending on their limit prices and whether it expects positive or negative net commissions).

In general, the best implementation I’ve found for an OMS is separate the intentions of a trading model–such as “place limit orders at $10.15 and $10.10″ or “buy 300 shares of INTC immediately”–from the resulting actual positions.  With this design, a trading model can, asynchronous from actual trading, update its intentions and the OMS can intelligently work to implement them, notifying the model when they are fulfilled.  This leaves the model free to continuously process data and potentially change intentions before they are fulfilled.  An extremely “intelligent” OMS, however, might be better thought of as a special kind of model that gathers and transforms the trading requests of all of the other models, since it is likely that an “intelligent” OMS would need to be tuned and otherwise parameterized.  Further discussion of implementing a high performance OMS will be the subject of future posts.

Before beginning the task of modeling financial markets, it is important to understand the capabilities and limitations of the trading system that will drive the models.  Oftentimes, a model that is extremely profitable in theory is impossible to implement because of things like system latency or the fundamental nature of limit order books.  Discussion of these capabilities and limitations will be the subject of future posts, especially those covering the implementation of synthetic option models.

This post attempted to touch on several important considerations to keep in mind when designing a trading system.  Future posts will go into more depth on implementation details.

# How to Build a Fast Limit Order Book

> To give some idea of the data volumes, the Nasdaq TotalView ITCH feed, which is every event in every instrument traded on the Nasdaq, can have data rates of 20+ gigabytes/day with spikes of 3 megabytes/second or more.  The individual messages average about 20 bytes each so this means handling 100,000-200,000 messages per second during high volume periods.

three primary functions:

1.  add
    -   places order at the end of a list of orders to be execute at a limit price
2.  cancel
    -   removes and arbitrary order
3.  execute
    -   removes an order from the inside of the book
        -   "inside" is oldest buy at highest price and oldest sell at lowest price

> The goal is to implement these operations in $O(1)$ time while making it possible for the trading model to efficiently ask questions like “what are the best bid and offer?”, “how much volume is there between prices $a$ and $b$?” or “what is order $x$’s current position in the book?”

> The vast majority of the activity in a book is usually made up of add and cancel operations as market makers jockey for position, with executions a distant third (in fact I would argue that the bulk of the useful information on many stocks, particularly in the morning, is in the pattern of adds and cancels, not executions, but that is a topic for another post).

> Each of these operations is keyed off an id number (`Order.idNumber` in the pseudo-code below), making a hash table a natural structure for tracking them.

```
Order
  int idNumber;
  bool buyOrSell;
  int shares;
  int limit;
  int entryTime;
  int eventTime;
  Order *nextOrder;
  Order *prevOrder;
  Limit *parentLimit;

Limit  // representing a single limit price
  int limitPrice;
  int size;
  int totalVolume;
  Limit *parent;
  Limit *leftChild;
  Limit *rightChild;
  Order *headOrder;
  Order *tailOrder;

Book
  Limit *buyTree;
  Limit *sellTree;
  Limit *lowestSell;
  Limit *highestBuy;
```

> The idea is to have a binary tree of Limit objects sorted by `limitPrice`, each of which is itself a doubly linked list of Order objects.  Each side of the book, the buy Limits and the sell Limits, should be in separate trees so that the inside of the book corresponds to the end and beginning of the buy Limit tree and sell Limit tree, respectively.  Each order is also an entry in a map keyed off `idNumber`, and each Limit is also an entry in a map keyed off `limitPrice`.

## Time Complexity

Table: Time Complexity Analysis

+------------------+---------------------------------------------------------------+
| Operation        | Complexity                                                    |
+------------------+---------------------------------------------------------------+
| Add              | - $O(log M)$ for the first order at a limit                   |
|                  | - $O(1)$ for all others                                       |
+------------------+---------------------------------------------------------------+
| Cancel           | $O(1)$                                                        |
+------------------+---------------------------------------------------------------+
| Execute          | $O(1)$                                                        |
+------------------+---------------------------------------------------------------+
| GetVolumeAtLimit | $O(1)$                                                        |
+------------------+---------------------------------------------------------------+
| GetBestBid/Offer | $O(1)$                                                        |
+------------------+---------------------------------------------------------------+

where:

-   $N$ is the number of orders
-   $M$ is the number of price limits (typically $<< N$)

> Some strategy for keeping the limit tree balanced should be used because the nature of markets is such that orders will be being removed from one side of the tree as they’re being added to the other.  Keep in mind, though, that it is important to be able to update `Book.lowestSell/highestBuy` in $O(1)$ time when a limit is deleted (which is why each Limit has a Limit `*parent`) so that `GetBestBid/Offer` can remain $O(1)$.

# Clocks and Latency

Update:  I should make clear that the “trading clock” I talk about will have an expectation that is the market time minus the network latency and an error that is a function of the variability of the network latency.  Whether this is good enough of course depends on the usage.  Synchronizing the system clock to an external source, like a GPS clock, may be a better option as long as the machine configuration is such that small adjustments to the clock throughout the day don’t result in bad interval measurements.

Update 2: tr8dr in the comments makes the important point that accessing the system clock has a cost that should be considered, and also highlights a couple more points I should address.  One is that data feeds from multiple venues will have different latencies, so obtaining a picture of the current total market state across all venues is a not a trivial one, and a topic for another post.  Another is that, generally speaking, trading models are going to be responding to market ticks directly, not the values read off a clock.  When trying to get a “jump” on particular events that happen at a certain time, like market open or perhaps another pattern that has shown up in analysis, then an absolute measure of time is most useful.  tr8dr also has an interesting post on his blog about order book simulation, something I’d planned to talk about myself.

Standard computer clocks are generally terrible at maintaining an accurate time over long periods.  This is because a clock on a computer is just a counter, and the counter needs to be updated at exact time intervals to be accurate; computers are incapable of doing this, however, because they only perform operations at discrete times defined by their chip clock frequency, the period of which will never be an exact multiple of the required time interval.  [Here is a link](https://web.archive.org/web/20110417205914/http://www.eecis.udel.edu/~ntp/ntpfaq/NTP-s-sw-clocks.htm) to a more complete description of this and related issues.

One way to deal with this is to ignore the system clock and use timestamps from a data feed as the “clock.”  Of course, the timestamps will be irregular and when the data is sparse achieving a required precision may be impossible.

Recognizing the fact that while computers are bad at maintaining clock accuracy over large time periods because of accumulating error, they are good at measuring time over short intervals, the best way I’ve found to build a trading clock is to measure time as the last timestamp from a data feed plus the time delta since that timestamp was received, measured by the system clock.  This can be implemented by simply recording the data feed timestamp and the system clock at once, with the only tricky part being to make sure that these two values are written in a single atomic operation so that calls to access the trading clock don’t need to be synchronized.  Also note that the timestamp/system time pair does not need to be updated with every market tick; how often it needs to be updated is a function of the system clock drift, which should definitely be measured on any machine that trades.

Keeping a log of the timestamp/system time pairs is a good idea, as well, because the difference between these times should grow at the system clock drift rate.  If it does not, this usually indicates a backlog in the data feed and should be investigated.  When handling multiple data feeds, comparing the timestamps and drift rates between the feeds will give even more information about potential problems.  In fact maintaining a matrix of the different measures of time at different points in the system is a very good way to monitor the health of the system.

Data latency, the time it takes for data to get to the trading system from the market, is one of the primary latencies of a HFT system that needs to be minimized, and the other is trading latency.  For models that are tick driven (models that trade as fast as possible after receiving a particular data event), I measure trading latency as the timestamp of the resulting event on the data feed minus the timestamp of the tick that sparked the trade.  This is a measure of the true system responsiveness.  Note that no call to the system clock is involved in the calculation, and it also does not include the data latency.

When trading directly with the market, the reference number required to identify an order sent by the trading system on the market data feed will of course be known.  If trading through an intermediary brokerage, probably via FIX, the order reference number can usually be provided as a field in the FIX execution reports. Either way it’s very important to have this information.

One last point I want to make regarding latency is that the dynamics of latency for similarly capable market participants sets up a kind of sub-”poker game”, separate from the “poker game” of the larger price movements.  When a trading system has sent off an order in response to the (delayed) state of the market, it knows before anyone else the “card” it is going to play and so has some knowledge of the future state of the market.  A good trading model will work to anticipate other’s responses to this “card” as well as anticipate the “cards” of others.  Assuming competitors have similar latencies, the “cards” of others will show up on the data feed within one round trip time (data latency plus trading latency) and their responses to the original “card” will show up within two round trip times.

# Message Queues

In an earlier post I talked about the need to make the implementation of a limit order book as fast as possible because of the very high data rate typical of market data feeds. Another important set of structures to make correct are the queues the trading system will use to pass messages between threads. Since a market data feed can burst to rates of 200,000+ messages/sec, the performance of the mechanisms that relay these messages is key.

Using a standard blocking queue implementation may not meet performance requirements because of the synchronization that will happen every time a message is enqueued, though using an implementation that allows blocks of messages to be enqueued or dequeued at one time can help. An alternative is to build lock-free implementation of the required queues using atomic operations. A very good, clear explanation of how to implement a lock-free single reader/single writer queue was written by Herb Sutter here. This implementation can be modified to handle multiple writers by using an atomic swap operation in the enqueue method.

One problem with using lock-free queues, however, is that the reading thread may spin waiting for the next element(s) to be added, wasting cpu time. For queues that are being constantly filled this may still be efficient, but for queues written to in bursts, a good implementation can be to let the reading thread toggle the queue behavior, switching from lock-free to blocking after a set number of read attempts return nothing, and switching back to lock-free mode after a blocking read succeeds. Special care must be taken in designing the mechanism for switching queue modes, however, as a poor implementation can leave the reader blocked while messages sit in the queue.

Another problem with lock-free queues is that their performance tends to degrade relative to blocking queues as the number of writers increases so, as with limit order books, which implementation is best very much depends on the context.

Regardless of the specific queueing implementation, it is important to be able to enqueue and dequeue blocks of multiple messages at once because of a point I mentioned in an earlier post: market events arrive serially but some are actually simultaneous, so it is important for the correctness of a trading model that simultaneous events appear in the model’s message queue all at the same time.

# NOTES

## Binary Search Tree

http://www.algolist.net/Data_structures/Binary_search_tree/Removal
