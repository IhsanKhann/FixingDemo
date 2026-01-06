#ifndef FEED_MANAGER_H
#define FEED_MANAGER_H

#include "Post.h"
#include <vector>

using namespace std;

// Forward declaration
class PostManager;

class FeedManager {
private:
    Post* feedHead;
    Post* feedTail;

    void clearFeed();
    void insertSorted(Post* newPost);
    Post* copyPost(Post* original);

public:
    FeedManager();
    ~FeedManager();

    void generateFeed(const vector<int>& followingList, PostManager& postMgr);
    void displayFeed(int limit = 20);
};

#endif