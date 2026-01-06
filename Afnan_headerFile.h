#ifndef FEED_TYPES_H
#define FEED_TYPES_H

#include <string>
using namespace std;

// Timestamp (as required by document)
struct Timestamp {
    int year, month, day;
    int hour, minute, second;
};

bool isNewer(const Timestamp& a, const Timestamp& b);

// Minimal Post reference (feed does not own posts)
struct Post {
    int postID;
    int userID;
    string content;
    Timestamp timestamp;
    int likes;
};

#endif
