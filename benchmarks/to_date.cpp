/*******************************************************************************
 *
 * to_date benchmarks
 *
 * Copyright (C) 2020 Cassio Neri
 *
 * This file is part of https://github.com/cassioneri/calendar.
 *
 * This file is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this file. If not,
 * see <https://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <cstdint>
#include <random>
#include <type_traits>

//------------------------------------------------------------------------------
// Config
//------------------------------------------------------------------------------

using year_t     = std::int16_t; // as in std::chrono::year
using month_t    = std::uint8_t; // as in std::chrono::month
using day_t      = std::uint8_t; // as in std::chrono::day
using rata_die_t = std::int32_t; // as in std::chrono::days

//------------------------------------------------------------------------------
// Implementations
//------------------------------------------------------------------------------

struct date_t {
  year_t  year;
  month_t month;
  day_t   day;
};

namespace neri {

  // https://github.com/cassioneri/calendar/blob/master/calendar.hpp

  std::pair<std::uint32_t, std::uint32_t> constexpr
  div_1461(std::uint32_t n) noexcept {
      auto constexpr a = std::uint64_t(1) << 32;
      auto constexpr b = std::uint32_t(a / 1461 + 1);
      auto const     p = std::uint64_t(b) * n;
      auto const     q = std::uint32_t(p / a);
      auto const     r = std::uint32_t(p % a) / b;
      return {q, r};
  }

  date_t constexpr
  to_date(rata_die_t n) noexcept {
    using rata_die_t     = std::make_unsigned_t<::rata_die_t>;
    auto constexpr z2    = rata_die_t(-1468000);
    auto constexpr n2_e3 = rata_die_t(536895458);
    auto const n2      = n + n2_e3;
    auto const p1      = 4 * n2 + 3;
    auto const q1      = p1 / 146097;
    auto const r1      = p1 % 146097;
    auto const p2      = r1 | 3;
    auto const [q2, r] = div_1461(p2);
    auto const r2      = r / 4;
    auto const p3      = 2141 * r2 + 197657;
    auto const m       = p3 / 65536;
    auto const d       = p3 % 65536 / 2141;
    auto const y       = 100 * q1 + q2;
    auto const j       = r2 > 305;
    auto const y1      = y + j;
    auto const m1      = j ? m - 12 : m;
    auto const d1      = d + 1;
    return { year_t(y1 + z2), month_t(m1), day_t(d1) };
  }

} // namespace neri

namespace baum {

  // https://www.researchgate.net/publication/316558298_Date_Algorithms

  // Section 6.2.1/3
  date_t static constexpr
  to_date(rata_die_t n) noexcept {
    auto const z  = std::uint32_t(n) + 719469; // adjusted to unix epoch
    auto const h  = 100 * z - 25;
    auto const a  = h / 3652425;
    auto const b  = a - a / 4;
    auto const y_ = (100 * b + h) / 36525;
    auto const c  = b + z - 365 * y_ - y_ / 4;
    auto const m_ = (535 * c + 48950) / 16384;
    auto const d  = c - (979 * m_ - 2918) / 32;
    auto const j  = m_ > 12;
    auto const y  = y_ + j;
    auto const m  = j ? m_ - 12 : m_;
    return { year_t(y), month_t(m), day_t(d) };
  }

} // namespace baum

namespace boost {

  // Code in this namespace is subject to the following terms.

  // Copyright (c) 2002,2003 CrystalClear Software, Inc.

  // Boost Software License - Version 1.0 - August 17th, 2003

  // Permission is hereby granted, free of charge, to any person or organization
  // obtaining a copy of the software and accompanying documentation covered by
  // this license (the "Software") to use, reproduce, display, distribute,
  // execute, and transmit the Software, and to prepare derivative works of the
  // Software, and to permit third-parties to whom the Software is furnished to
  // do so, all subject to the following:

  // The copyright notices in the Software and this entire statement, including
  // the above license grant, this restriction and the following disclaimer,
  // must be included in all copies of the Software, in whole or in part, and
  // all derivative works of the Software, unless such copies or derivative
  // works are solely in the form of machine-executable object code generated by
  // a source language processor.

  // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  // IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  // FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
  // SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
  // FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
  // ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  // DEALINGS IN THE SOFTWARE.

  // https://github.com/boostorg/date_time/blob/4e1b7cde45edf8fdda73ec5c60053c9257138292/include/boost/date_time/gregorian_calendar.ipp#L109
  date_t constexpr
  to_date(rata_die_t dayNumber) noexcept {
    rata_die_t a = dayNumber + 32044;
    rata_die_t b = (4*a + 3)/146097;
    rata_die_t c = a-((146097*b)/4);
    rata_die_t d = (4*c + 3)/1461;
    rata_die_t e = c - (1461*d)/4;
    rata_die_t m = (5*e + 2)/153;
    day_t day = static_cast<day_t>(e - ((153*m + 2)/5) + 1);
    month_t month = static_cast<month_t>(m + 3 - 12 * (m/10));
    year_t year = static_cast<year_t>(100*b + d - 4800 + (m/10));
    return date_t{static_cast<year_t>(year),month,day};
  }

} // namespace boost

namespace dotnet {

  // Code in this namespace is subject to the following terms.

  // The MIT License (MIT)

  // Copyright (c) .NET Foundation and Contributors

  // All rights reserved.

  // Permission is hereby granted, free of charge, to any person obtaining a copy
  // of this software and associated documentation files (the "Software"), to deal
  // in the Software without restriction, including without limitation the rights
  // to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  // copies of the Software, and to permit persons to whom the Software is
  // furnished to do so, subject to the following conditions:

  // The above copyright notice and this permission notice shall be included in all
  // copies or substantial portions of the Software.

  // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  // IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  // FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  // AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  // LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  // OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  // SOFTWARE.

  // https://github.com/dotnet/runtime/blob/master/src/libraries/System.Private.CoreLib/src/System/DateTime.cs#L102
  rata_die_t static constexpr s_daysToMonth365[] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
  rata_die_t static constexpr s_daysToMonth366[] = {
    0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

  // https://github.com/dotnet/runtime/blob/bddbb03b33162a758e99c14ae821665a647b77c7/src/libraries/System.Private.CoreLib/src/System/DateTime.cs#L64
  rata_die_t static constexpr DaysPerYear = 365;
  rata_die_t static constexpr DaysPer4Years = DaysPerYear * 4 + 1;       // 1461
  rata_die_t static constexpr DaysPer100Years = DaysPer4Years * 25 - 1;  // 36524
  rata_die_t static constexpr DaysPer400Years = DaysPer100Years * 4 + 1; // 146097

  // https://github.com/dotnet/runtime/blob/bddbb03b33162a758e99c14ae821665a647b77c7/src/libraries/System.Private.CoreLib/src/System/DateTime.cs#L938
  date_t static constexpr
  to_date(rata_die_t rata_die) noexcept {
    rata_die_t n = rata_die + 719162; // adjusted to unix epoch
    rata_die_t y400 = n / DaysPer400Years;
    n -= y400 * DaysPer400Years;
    rata_die_t y100 = n / DaysPer100Years;
    if (y100 == 4) y100 = 3;
    n -= y100 * DaysPer100Years;
    rata_die_t y4 = n / DaysPer4Years;
    n -= y4 * DaysPer4Years;
    rata_die_t y1 = n / DaysPerYear;
    if (y1 == 4) y1 = 3;
    year_t year = y400 * 400 + y100 * 100 + y4 * 4 + y1 + 1;
    n -= y1 * DaysPerYear;
    bool leapYear = y1 == 3 && (y4 != 24 || y100 == 3);
    rata_die_t const* days = leapYear ? s_daysToMonth366 : s_daysToMonth365;
    month_t m = (n >> 5) + 1;
    while (n >= days[m]) m++;
    month_t month = m;
    day_t day = n - days[m - 1] + 1;
    return date_t{year, month, day};
  }

} // namespace dotnet

namespace glibc {

  // Code in this namespace is subject to the following terms.

  // Copyright (C) 1993-2020 Free Software Foundation, Inc.

  // This section of the file is part of the GNU C Library.
  // Contributed by Paul Eggert <eggert@twinsun.com>.
  // The GNU C Library is free software; you can redistribute it and/or
  // modify it under the terms of the GNU Lesser General Public
  // License as published by the Free Software Foundation; either
  // version 2.1 of the License, or (at your option) any later version.

  // The GNU C Library is distributed in the hope that it will be useful,
  // but WITHOUT ANY WARRANTY; without even the implied warranty of
  // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  // Lesser General Public License for more details.

  // You should have received a copy of the GNU Lesser General Public
  // License along with the GNU C Library; if not, see
  // <https://www.gnu.org/licenses/>.

  // https://sourceware.org/git/?p=glibc.git;a=blob;f=time/mktime.c;h=63c82fc6a96848b1f1e34164e7ce696035635fc6;hb=HEAD#l173
  unsigned short int static constexpr __mon_yday[2][13] =
    {
      /* Normal years.  */
      { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
      /* Leap years.  */
      { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
    };

  // https://sourceware.org/git/?p=glibc.git;a=blob;f=time/time.h;h=015bc1c7f3b5d3db689f68de2a0c6ebbbc94f138#l179
  #define __isleap(year)	\
    ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

  // https://sourceware.org/git/?p=glibc.git;a=blob;f=time/offtime.c;h=1415b1b4013834f39e4b0c11f0479dd866aab617#l24
  date_t static constexpr
  to_date(rata_die_t days) noexcept {

    rata_die_t y = 1970;
    #define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
    #define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

    while (days < 0 || days >= (__isleap (y) ? 366 : 365))
      {
        /* Guess a corrected year, assuming 365 days per year.  */
        rata_die_t yg = y + days / 365 - (days % 365 < 0);

        /* Adjust DAYS and Y to match the guessed year.  */
        days -= ((yg - y) * 365
                + LEAPS_THRU_END_OF (yg - 1)
                - LEAPS_THRU_END_OF (y - 1));
        y = yg;
      }

    auto ip = __mon_yday[__isleap(y)];
    rata_die_t m = 0;
    for (m = 11; days < (long int) ip[m]; --m)
      continue;
    days -= ip[m];
    return date_t{year_t(y), month_t(m + 1), day_t(days + 1)};
  }

} // namespace glibc

namespace hatcher {

  // Algorithms by D. A. Hactcher as appeared in
  // E. G. Richards, Mapping Time, The calendar and its history, Oxford University Press, 1998.

  // Table 25.1, page 311.
  auto static constexpr y = rata_die_t(4716);
  auto static constexpr m = rata_die_t(3);
  auto static constexpr n = rata_die_t(12);
  auto static constexpr r = rata_die_t(4);
  auto static constexpr p = rata_die_t(1461);
  auto static constexpr v = rata_die_t(3);
  auto static constexpr u = rata_die_t(5);
  auto static constexpr s = rata_die_t(153);
  auto static constexpr w = rata_die_t(2);

  // Table 25.4, page 320.
  auto static constexpr B = rata_die_t(274277);
  auto static constexpr G = rata_die_t(-38);
  // Page 319
  auto static constexpr K = 36524;

  // Algorithm F, page 324.
  date_t static constexpr
  to_date(rata_die_t x) noexcept {
    auto const J  = x + rata_die_t(2440575); // adjusted to unix epoch
    auto const g  = 3 * ((4 * J + B) / (4 * K + 1)) / 4 + G;
    auto const j  = 1401 + g;
    auto const Jp = J + j + g;
    auto const Yp = (r * Jp + v) / p;
    auto const Tp = (r * Jp + v) % p / r;
    auto const Mp = (u * Tp + w) / s;
    auto const Dp = (u * Tp + w) % s / u;
    auto const D  = Dp + 1;
    auto const M  = (Mp + m - 1) % n + 1;
    auto const Y  = Yp - y + (n + m - 1 - M) / n;
    return date_t{year_t(Y), month_t(M), day_t(D)};
  }

} // namespace hatcher

namespace llvm {

  // Code in this namespace is subject to the following terms.

  // Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
  // See https://llvm.org/LICENSE.txt for license information.

  // https://github.com/llvm/llvm-project/blob/8e34be2f2511dfff7a8e3018bbd4188a93e446ea/libcxx/include/chrono#L2305
  date_t constexpr
  to_date(rata_die_t __d) noexcept {
    const int      __z = __d + 719468;
    const int      __era = (__z >= 0 ? __z : __z - 146096) / 146097;
    const unsigned __doe = static_cast<unsigned>(__z - __era * 146097);              // [0, 146096]
    const unsigned __yoe = (__doe - __doe/1460 + __doe/36524 - __doe/146096) / 365;  // [0, 399]
    const int      __yr = static_cast<int>(__yoe) + __era * 400;
    const unsigned __doy = __doe - (365 * __yoe + __yoe/4 - __yoe/100);              // [0, 365]
    const unsigned __mp = (5 * __doy + 2)/153;                                       // [0, 11]
    const unsigned __dy = __doy - (153 * __mp + 2)/5 + 1;                            // [1, 31]
    const unsigned __mth = __mp + (__mp < 10 ? 3 : -9);                              // [1, 12]
    return date_t{year_t(__yr + (__mth <= 2)), month_t(__mth), day_t(__dy)};
  }

} // namespace llvm

namespace reingold {

  // E. M. Reingold and N. Dershowitz, Calendrical Calculations, The Ultimate Edition, Cambridge
  // University Press, 2018.

  // Table 1.2, page 17.
  rata_die_t static constexpr gregorian_epoch = 1;

  // alt-fixed-from-gregorian, equation (2.28), page 65:
  rata_die_t static constexpr
  to_rata_die(date_t date) noexcept {

    auto const year  = rata_die_t(date.year );
    auto const month = rata_die_t(date.month);
    auto const day   = rata_die_t(date.day  );

    // In the book mp = (month - 3) mod 12, where mod denotes Euclidean remainder. When month < 3 we
    // have month - 3 < 0 and % does not match mod. The alternative below provides the intended
    // result even in this case and keeps the expected performance of the original formula.
    auto const mp = (month + 9) % 12;
    auto const yp = year - mp / 10;

    // Equation (1.42), page 28, with b = <4, 25, 4>, i.e., b0 = 4, b1 = 25 and b2 = 4 gives
    auto const a0 = (yp / 400);
    auto const a1 = (yp / 100) %  4;
    auto const a2 = (yp /   4) % 25;
    auto const a3 = (yp /   1) %  4;
    // On page 66, quantities above are denoted by n400, n100, n4 and n1.

    auto const n = gregorian_epoch - 1 - 306 + 365 * yp + 97 * a0 + 24 * a1 + 1 * a2 + 0 * a3 +
      (3 * mp + 2) / 5 + 30 * mp + day;
    return n - 719163; // adjusted to unix epoch
  }

  // gregorian-year-from-fixed, equation (2.21), page 61:
  rata_die_t static constexpr
  gregorian_year_from_fixed(rata_die_t date) noexcept {
    auto const d0   = date - gregorian_epoch;
    auto const n400 = d0 / 146097;
    auto const d1   = d0 % 146097;
    auto const n100 = d1 / 36524;
    auto const d2   = d1 % 36524;
    auto const n4   = d2 / 1461;
    auto const d3   = d2 % 1461;
    auto const n1   = d3 / 365;
    auto const year = 400 * n400 + 100 * n100 + 4 * n4 + n1;
    return (n100 == 4 | n1 == 4) ? year : year + 1;
  }

  // alt-fixed-from-gregorian, equation (2.28), page 65:
  rata_die_t static constexpr
  fixed_from_gregorian(date_t date) noexcept {
    return to_rata_die(date) + 719163;
  }

  rata_die_t static constexpr
  mod_1_12(rata_die_t month) noexcept {
    return month > 12 ? month - 12 : month;
  }

  // alt-gregorian-from-fixed, equation (2.29), page 66:
  date_t static constexpr
  to_date(rata_die_t date) noexcept {
    date = date + 719163; // adjusted to unix epoch
    auto const y          = gregorian_year_from_fixed(gregorian_epoch - 1 + date + 306);
    auto const prior_days = date - fixed_from_gregorian(date_t{year_t(y - 1), 3, 1});
    auto const month      = mod_1_12((5 * prior_days + 2) / 153 + 3);
    auto const year       = y - (month + 9) / 12;
    auto const day        = date - fixed_from_gregorian(date_t{year_t(year), month_t(month), 1})
      + 1;
    return { year_t(year), month_t(month), day_t(day) };
  }

} // namespace reingold

//------------------------------------------------------------------------------
// Benchmark data
//------------------------------------------------------------------------------

auto const rata_dies = [](){
  std::uniform_int_distribution<rata_die_t> uniform_dist(-146097, 146096);
  std::mt19937 rng;
  std::array<std::int32_t, 16384> rata_dies;
  for (auto& n : rata_dies)
    n = uniform_dist(rng);
  return rata_dies;
}();

//------------------------------------------------------------------------------
// Benchmark
//------------------------------------------------------------------------------

void Reingold(benchmark::State& state) {
  for (auto _ : state) {
    for (auto const n : rata_dies) {
      auto u = reingold::to_date(n);
      benchmark::DoNotOptimize(u);
    }
  }
}
BENCHMARK(Reingold);

void GLIBC(benchmark::State& state) {
  for (auto _ : state) {
    for (auto const n : rata_dies) {
      auto u = glibc::to_date(n);
      benchmark::DoNotOptimize(u);
    }
  }
}
BENCHMARK(GLIBC);

void DotNet(benchmark::State& state) {
  for (auto _ : state) {
    for (auto const n : rata_dies) {
      auto u = dotnet::to_date(n);
      benchmark::DoNotOptimize(u);
    }
  }
}
BENCHMARK(DotNet);

void Hatcher(benchmark::State& state) {
  for (auto _ : state) {
    for (auto const n : rata_dies) {
      auto u = hatcher::to_date(n);
      benchmark::DoNotOptimize(u);
    }
  }
}
BENCHMARK(Hatcher);

void Boost(benchmark::State& state) {
  for (auto _ : state) {
    for (auto const n : rata_dies) {
      auto u = boost::to_date(n);
      benchmark::DoNotOptimize(u);
    }
  }
}
BENCHMARK(Boost);

void LLVM(benchmark::State& state) {
  for (auto _ : state) {
    for (auto const n : rata_dies) {
      auto u = llvm::to_date(n);
      benchmark::DoNotOptimize(u);
    }
  }
}
BENCHMARK(LLVM);

void Baum(benchmark::State& state) {
  for (auto _ : state) {
    for (auto const n : rata_dies) {
      auto u = baum::to_date(n);
      benchmark::DoNotOptimize(u);
    }
  }
}
BENCHMARK(Baum);

void Neri(benchmark::State& state) {
  for (auto _ : state) {
    for (auto const n : rata_dies) {
      auto u = neri::to_date(n);
      benchmark::DoNotOptimize(u);
    }
  }
}
BENCHMARK(Neri);
