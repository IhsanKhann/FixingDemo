#include "Post.h"

bool isNewer(const Timestamp& a, const Timestamp& b) {
    if (a.year != b.year) return a.year > b.year;
    if (a.month != b.month) return a.month > b.month;
    if (a.day != b.day) return a.day > b.day;
    if (a.hour != b.hour) return a.hour > b.hour;
    if (a.minute != b.minute) return a.minute > b.minute;
    return a.second > b.second;
}