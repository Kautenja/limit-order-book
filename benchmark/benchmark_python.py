"""Measure public Python calls with identical workloads across binding builds."""
import argparse
import gc
import json
import platform
import statistics
import sys
import time
from pathlib import Path


def queries(book, loops):
    """Read eight scalar results from a populated book per iteration."""
    for _ in range(loops):
        book.best_sell()
        book.best_buy()
        book.best(True)
        book.has(1)
        book.volume()
        book.volume_sell(101)
        book.count()
        book.count_at(99)


def existing_limit(book, loops):
    """Add and cancel at a price with a permanent resting order."""
    for _ in range(loops):
        book.limit_sell(3, 10, 101)
        book.cancel(3)


def new_limit(book, loops):
    """Create and remove a price level beside the resting book."""
    for _ in range(loops):
        book.limit_buy(3, 10, 98)
        book.cancel(3)


def crossing_limits(book, loops):
    """Partially and then completely fill a crossing limit order."""
    for _ in range(loops):
        book.limit_sell(3, 10, 100)
        book.limit_buy(4, 4, 100)
        book.limit_buy(5, 6, 100)


def market_sweep(book, loops):
    """Sweep three new price levels, including a partial final fill."""
    for _ in range(loops):
        book.limit_sell(3, 10, 100)
        book.limit_sell(4, 10, 101)
        book.limit_sell(5, 10, 102)
        book.market_buy(6, 25)
        book.market_buy(7, 5)


def mixed_events(book, loops):
    """Mix side-dispatched entry, queries, matching, and cancellation."""
    for _ in range(loops):
        book.limit(False, 3, 10, 100)
        book.limit(True, 4, 4, 100)
        book.best(False)
        book.volume(100)
        book.has(3)
        book.market(True, 5, 3)
        book.count_at(100)
        book.cancel(3)
        book.limit(True, 6, 10, 98)
        book.volume_buy(98)
        book.cancel(6)
        book.count()


WORKLOADS = (
    ('queries', queries, 8),
    ('existing_limit', existing_limit, 2),
    ('new_limit', new_limit, 2),
    ('crossing_limits', crossing_limits, 3),
    ('market_sweep', market_sweep, 5),
    ('mixed_events', mixed_events, 12),
)


def measure(book_type, workload, loops, calls):
    """Time only the Python event loop; verify the resulting state afterward."""
    book = book_type()
    # The sweep consumes its entire book. Other workloads retain two anchors.
    if workload is not market_sweep:
        book.limit_sell(1, 100, 101)
        book.limit_buy(2, 100, 99)
    collecting = gc.isenabled()
    gc.disable()
    try:
        start = time.perf_counter_ns()
        workload(book, loops)
        elapsed = time.perf_counter_ns() - start
    finally:
        if collecting:
            gc.enable()
    if workload is market_sweep:
        assert book.count() == book.volume() == 0
    else:
        assert book.count() == 2 and book.volume() == 200
        assert book.has(1) and book.has(2)
        assert book.best_sell() == 101 and book.best_buy() == 99
    return elapsed / (loops * calls)


def main():
    """Print per-call timing distributions and optionally compare a baseline."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--loops', type=int, default=100000)
    parser.add_argument('--repeats', type=int, default=9)
    parser.add_argument('--warmups', type=int, default=2)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--compare', type=Path)
    parser.add_argument('--label', default='current')
    parser.add_argument('--package-root', type=Path)
    args = parser.parse_args()
    if args.loops < 1 or args.repeats < 2 or args.warmups < 1:
        parser.error('use positive loops/warmups and at least two repeats')
    # A saved baseline package can be measured by the very same runner.
    root = args.package_root or Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root.resolve()))
    from limit_order_book import LimitOrderBook

    result = {
        'label': args.label,
        'python': sys.version,
        'platform': platform.platform(),
        'machine': platform.machine(),
        'loops': args.loops,
        'repeats': args.repeats,
        'warmups': args.warmups,
        'workloads': {},
    }
    for name, workload, calls in WORKLOADS:
        for _ in range(args.warmups):
            measure(LimitOrderBook, workload, args.loops, calls)
        samples = [
            measure(LimitOrderBook, workload, args.loops, calls)
            for _ in range(args.repeats)
        ]
        result['workloads'][name] = {
            'calls_per_loop': calls,
            'ns_per_call': samples,
            'median': statistics.median(samples),
            'min': min(samples),
            'max': max(samples),
        }
        print(f'{name:20} {statistics.median(samples):9.1f} ns/call '
              f'[{min(samples):.1f}, {max(samples):.1f}]')
    if args.output:
        args.output.write_text(json.dumps(result, indent=2) + '\n')
    if args.compare:
        baseline = json.loads(args.compare.read_text())
        for key in ('python', 'platform', 'machine', 'loops', 'repeats', 'warmups'):
            if baseline[key] != result[key]:
                parser.error(f'baseline differs in {key}; comparisons need matched runs')
        for name, current in result['workloads'].items():
            previous = baseline['workloads'][name]
            assert previous['calls_per_loop'] == current['calls_per_loop']
            speedup = previous['median'] / current['median']
            separated = current['max'] < previous['min']
            print(f'{name:20} {speedup:6.2f}x median speedup; '
                  f'ranges {"separate" if separated else "overlap"}')


if __name__ == '__main__':
    main()
