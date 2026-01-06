#ifndef RAWEENAAMIR_HEADERFILE_H
#define RAWEENAAMIR_HEADERFILE_H



#include <string>
using namespace std;

// ---------------- Timestamp ----------------
struct Timestamp {
    int year, month, day;
    int hour, minute, second;
};

inline bool isNewer(const Timestamp& a, const Timestamp& b) {
    if (a.year != b.year) return a.year > b.year;
    if (a.month != b.month) return a.month > b.month;
    if (a.day != b.day) return a.day > b.day;
    if (a.hour != b.hour) return a.hour > b.hour;
    if (a.minute != b.minute) return a.minute > b.minute;
    return a.second > b.second;
}

// ---------------- Comment (Singly Linked List) ----------------
struct Comment {
    int commentID;
    int userID;
    string content;
    Timestamp timestamp;
    Comment* next;

    Comment(int id, int uid, const string& text, Timestamp ts)
        : commentID(id), userID(uid), content(text), timestamp(ts), next(nullptr) {}
};

// ---------------- Post ----------------
struct Post {
    int postID;
    int userID;
    string content;
    Timestamp timestamp;
    int likes;
    Comment* comments;

    Post* prev;
    Post* next;

    Post(int pid, int uid, const string& text, Timestamp ts)
        : postID(pid), userID(uid), content(text), timestamp(ts),
          likes(0), comments(nullptr), prev(nullptr), next(nullptr) {}
};

#endif
