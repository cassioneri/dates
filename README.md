# Date algorithms

## TL;DR

Contains low-level date algorithms that can be used to implement the following C++20's functions:

    std::chrono::year::is_leap()                                 // a.k.a. is_leap_year
    std::chrono::year_month_day_last::day()                      // a.k.a. last_day_of_month
    std::chrono::year_month_day::operator sys_days()             // a.k.a. to_rata_die
    std::chrono::year_month_day::year_month_day(const sys_days&) // a.k.a. to_date

This work is similar to those by Peter Baum [[1]](#baum) and Howard Hinnant [[2]](#hinnant).
Benchmark results suggest that implementations here perform considerably faster than theirs and also
than boost's:

![Benchmarks](https://github.com/cassioneri/dates/blob/master/benchmarks/benchmarks.png)

(See live [[3]](http://quick-bench.com/LybJ2bUF67numbQAryYTyRuJQVE),
[[4]](http://quick-bench.com/RDy_9sfmJDhZH7Nh5asYQJSvEjc),
[[5]](http://quick-bench.com/mCJ8dbzevMX0ssfee4mfgx4IMbA) and
[[6]](http://quick-bench.com/N49JDfWYLmAPYPdJyuiTHFiWrVo).)

**Disclaimer**: Benchmarks above compare implementations as of 2020-May-02. They have been slightly
edited, mainly to get consistent function signatures across implementations. Storage types for
years, months, days and day counts might have been changed for closer compliance with C++20
requirements.

Tests show correctness and compliance with the C++ Standard, that is, the algorithms are strictly
increasing 1-to-1 maps between dates in [-32768-Jan-01, 32767-Dec-31] and day counts in [-12687794,
11248737] with 1970-Jan-01 being mapped into 0.

Implementations are split into building blocks allowing configurable features. For those only
interested in the case specified by the C++ Standard (previous paragraph), ready to copy-and-paste
implementations can be seen in the benchmark links above.

The configurable features allow even better performance than showed by the benchmarks above at the
expense of diverging from the C++ Standard choices on types, ranges and epoch. The best performance
is achieved when only dates after 0000-Mar-01 are allowed.

## Design choices

The code does not implement a full date library. However, despite its limited usage, some design
choices were made with generality and performance in mind.

Configurable features include:

1. The epoch for the rata die.
2. The type used to store rata dies.
3. The type used to store years.

The ranges of possible dates and rata dies depend on the choices above. These ranges are calculated
by the code and are available for evaluations in `constexpr` contexts.

The most important functions are implemented in two template classes:

1. `ugregorian_t`
2. `gregorian_t`

The former assumes years and rata dies are positive and the epoch is 0000-Mar-01. The rationale is
that `unsigned` computations are generally faster than `signed` ones. (For instance, [division by
constants](https://godbolt.org/z/4JxB4J).) Therefore `ugregorian_t` gets better performance by
working only on `unsigned` integers.

The latter is more configurable than ugregorian_t allowing negative years and rata dies. It also
allows different epochs. (By default, the epoch is the Unix date 1970-Jan-01.) This is a thin but
not free layer class around `ugregorian_t`. Indeed, each function in `gregorian_t` simply adapts
inputs and outputs (generally through one addition and one subtraction) before/after delegating to a
corresponding function in `ugregorian_t`.

## Contents

1. `date.hpp`   : Implementations.
2. `tests.cpp`  : Tests. (Including exhaustive full-range round-trips taking around 30s to complete
on commodity hardware.)
3. `search.cpp` : Helper program that was used to find some coefficients used by the algorithms.

## References

[1] <span id="baum"> Peter Baum, *Date Algorithms*,
  https://www.researchgate.net/publication/316558298_Date_Algorithms<br>
[2] <span id="hinnant"> Howard Hinnant, *chrono-Compatible Low-Level Date Algorithms*,
  https://howardhinnant.github.io/date_algorithms.html<br>
[3] <span id="is_leap_year"> Cassio Neri, *`is_leap_year` benchmark*,
  http://quick-bench.com/LybJ2bUF67numbQAryYTyRuJQVE<br>
[4] <span id="last_day_of_month"> Cassio Neri, *`last_day_of_month` benchmark*,
  http://quick-bench.com/RDy_9sfmJDhZH7Nh5asYQJSvEjc<br>
[5] <span id="to_rata_die"> Cassio Neri, *`to_rata_die` benchmark*,
  http://quick-bench.com/mCJ8dbzevMX0ssfee4mfgx4IMbA<br>
[6] <span id="to_date"> Cassio Neri, *`to_date` benchmark*,
  http://quick-bench.com/N49JDfWYLmAPYPdJyuiTHFiWrVo<br>
