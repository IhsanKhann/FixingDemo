#include "Afnan_headerFile.h"
#include <iostream>
using namespace std;

// Feed node (Linked List)
struct FeedNode {
    Post* post;
    FeedNode* next;

    FeedNode(Post* p) : post(p), next(nullptr) {}
};

// Compare timestamps
bool isNewer(const Timestamp& a, const Timestamp& b) {
    if (a.year != b.year) return a.year > b.year;
    if (a.month != b.month) return a.month > b.month;
    if (a.day != b.day) return a.day > b.day;
    if (a.hour != b.hour) return a.hour > b.hour;
    if (a.minute != b.minute) return a.minute > b.minute;
    return a.second > b.second;
}

// Feed Generator
class FeedGenerator {
private:
    FeedNode* head;
    int count;

public:
    FeedGenerator() : head(nullptr), count(0) {}

    // Insert post in sorted order (newest first)
    void insertPost(Post* p) {
        if (count >= 20) return; // top 20 only

        FeedNode* node = new FeedNode(p);

        if (!head || isNewer(p->timestamp, head->post->timestamp)) {
            node->next = head;
            head = node;
        } else {
            FeedNode* curr = head;
            while (curr->next &&
                   !isNewer(p->timestamp, curr->next->post->timestamp)) {
                curr = curr->next;
            }
            node->next = curr->next;
            curr->next = node;
        }
        count++;
    }

    // Display feed
    void displayFeed() {
        FeedNode* curr = head;
        while (curr) {
            cout << "PostID: " << curr->post->postID << endl;
            cout << curr->post->content << endl;
            cout << "Likes: " << curr->post->likes << endl << endl;
            curr = curr->next;
        }
    }
};
