// TQDM C++ port.
// Source: https://github.com/aminnj/cpptqdm
//

#ifndef TQDM_HPP_
#define TQDM_HPP_

#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

/// a mapping of string names to themes
const std::unordered_map<std::string, std::vector<std::string>> THEMES = {
    {"default",      {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"}},
    {"line",         {"─", "─", "─", "╾", "╾", "╾", "╾", "━", "═"}},
    {"braille",      {" ", "⡀", "⡄", "⡆", "⡇", "⡏", "⡟", "⡿", "⣿"}},
    {"braille_spin", {" ", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠇", "⠿"}},
    {"vertical",     {"▁", "▂", "▃", "▄", "▅", "▆", "▇", "█", "█"}},
    {"basic",        {" ", " ", " ", " ", " ", " ", " ", " ", "#"}}
};

/// the minimum value for a period
#define PERIOD_MIN 1.0
/// the maximum value for a period
#define PERIOD_MAX 5e5
/// the number of iterations to warmup before calculating the period
#define PERIOD_WARMUP 30

/// Return a timestamp string from the given seconds.
///
/// @param seconds the number of seconds to convert to a time string
/// @returns a time string of the the form "H*:MM:SS" if seconds is more than
///     an hour and of the form "MM:SS" otherwise
///
std::string timestr(int seconds) {
    std::ostringstream stream;
    int minutes = seconds / 60.0;
    if (minutes > 60) {  // more that one hour
        int hours = minutes / 60.0;
        stream << hours << ":" <<
            std::setfill('0') << std::setw(2) << minutes % 60 << ":" <<
            std::setfill('0') << std::setw(2) << seconds % 60;
    } else {  // less than an hour
        stream << std::setfill('0') << std::setw(2) << minutes % 60 << ":" <<
            std::setfill('0') << std::setw(2) << seconds % 60;
    }
    return stream.str();
}

/// A C++ port for tqdm progress bars
class tqdm {
 private:
    /// a typedef for the timestamp used internally
    typedef std::chrono::time_point<std::chrono::system_clock> Timepoint;

    /// the total number of iterations in the progress bar
    uint64_t total;
    /// the unit for the progress bar to use
    std::string unit;
    /// the current iteration
    uint64_t n;
    /// the n value at the last call to `update`
    uint64_t n_old;
    /// the number of calls to `update`
    uint64_t n_updates;
    /// the number of updates before outputting to stdout
    uint32_t period;
    /// the alpha value for the exponential moving average
    double alpha_ema;
    /// the average rate (exponential moving average)
    double average;
    /// the time that the progress bar was created
    Timepoint t_start;
    /// the time at the last call to `progress` for measuring step time
    Timepoint t_last;

    /// the progress bars to use (i.e., the theme for the bar)
    std::vector<std::string> bars;

    /// whether the progress bar is running in an instance of `screen`
    bool in_screen = system("test $STY") == 0;
    /// whether the progress bar is running in an interactive shell
    bool is_tty = isatty(1);
    /// the output width of the progress bar
    int width = 40;
    /// whether the progress bar is complete
    bool is_complete;

    /// Take measurements at a loop step.
    ///
    /// @returns the iterations difference in time
    ///
    double measure() {
        // get the time now
        auto t_now = std::chrono::system_clock::now();
        // get the difference in time between now and last measurement call
        double dt = ((std::chrono::duration<double>)(t_now - t_last)).count();
        // get the total difference in time
        double dT = ((std::chrono::duration<double>)(t_now - t_start)).count();
        // calculate the difference in iterations
        int dn = n - n_old;

        // set the last value of n to this n
        n_old = n;
        // set the last time to the time now
        t_last = t_now;

        if (average == 0) {  // first sample, set to current rate
            average = dn / dt;
        } else {  // arbitrary sample, use exponential moving average
            average = (1 - alpha_ema) * average + alpha_ema * dn / dt;
        }

        return dT;
    }

    /// Learn an appropriate period length to avoid spamming stdout and slowing
    /// down the loop, shoot for ~25Hz and smooth over 3 seconds.
    ///
    /// @param dt_tot the total change in time since the beginning of the loop
    /// @param smooth the number of seconds to smooth over
    ///
    void learn_period(double dt_tot, double freq=25, double smooth=3) {
        if (++n_updates > PERIOD_WARMUP)  // warmed up
            period = std::min(std::max((1.0 / freq) * n / dt_tot, PERIOD_MIN), PERIOD_MAX);
    }

 public:
    /// Create a new tqdm progress bar.
    ///
    /// @param total_ the total number of iterations in the loop
    /// @param unit_ the unit to use for the progress bar
    /// @param theme the name of the theme to use
    ///
    explicit tqdm(
        uint64_t total_,
        const std::string& unit_ = "it",
        const std::string& theme = "default"
    ) :
        total(total_),
        unit(unit_),
        n(0),
        n_old(n),
        n_updates(0),
        period(1),
        alpha_ema(0.1),
        average(0.0),
        t_start(std::chrono::system_clock::now()), // set the start time to now
        t_last(std::chrono::system_clock::now()),  // set the last time to now
        bars(THEMES.at(theme)),
        is_complete(false) { }

    /// Call the progress bar (shortcut to the `update` function).
    template<class ...Ts>
    inline void operator()(Ts... args) { update(&args...); }

    /// Update the progress bar.
    ///
    /// @param dn the number of steps to update the progress bar by
    ///
    void update(uint64_t dn = 1) {
        // don't update after completion
        if (is_complete) return;
        // update the iteration counter
        n += dn;
        // check if the terminal supports interactive output and whether this
        // call to update is in the next period and should output to console
        if (!is_tty || (n % period != 0)) return;
        // measure the average and determine the total difference in time
        auto dt_tot = measure();
        // learn the period value to prevent overhead from the bar
        learn_period(dt_tot);
        // determine how much time until the loop is complete
        double time_to_complete = (total - n) / average;
        // determine the completion percentage of the loop
        int percent_complete = n / (total * 0.01);
        if (total - n <= period) {  // progress is complete
            is_complete = true;
            time_to_complete = 0;
            percent_complete = 100;
            average = total / dt_tot;
            n = total;
        }

        // determine how much of the bar is filled
        double total_filled = static_cast<double>(n) / total * width;
        // determine how many complete pieces of the bar there are
        int pieces_filled = static_cast<int>(total_filled);

        // remove last bar (carriage return) and print the percent complete
        // and the start of the bar
        std::cout << "\r" << percent_complete << "%|";
        // print complete pieces
        for (int i = 0; i < pieces_filled; i++)
            std::cout << bars[8];
        // print the partial piece
        if (!in_screen && (n != total))
            std::cout << bars[8.0 * (total_filled - pieces_filled)];
        // print incomplete pieces
        for (int i = 0; i < width - pieces_filled - 1; i++)
            std::cout << bars[0];
        // print the end of the bar
        std::cout << "| ";
        // print the statistics for the progress bar
        std::cout << n << "/" << total << " " <<
            "[" <<
            timestr(dt_tot) << "<" << timestr(time_to_complete) << ", " <<
            average << unit << "/s" <<
            "]";

        // print a new line if complete
        if (is_complete) std::cout << std::endl;
        // flush cout
        if (total - n > period) std::cout << std::flush;
    }
};

// un-define macros used in this module
#undef PERIOD_MIN
#undef PERIOD_MAX
#undef PERIOD_WARMUP

#endif  // TQDM_HPP_
