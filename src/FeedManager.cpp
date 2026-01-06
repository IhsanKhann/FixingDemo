#include "FeedManager.h"
#include "PostManager.h"
#include <iostream>

using namespace std;

FeedManager::FeedManager() : feedHead(nullptr), feedTail(nullptr) {}

FeedManager::~FeedManager() {
    clearFeed();
}

void FeedManager::clearFeed() {
    Post* current = feedHead;
    while (current) {
        // Delete comments
        Comment* comment = current->comments;
        while (comment) {
            Comment* temp = comment;
            comment = comment->next;
            delete temp;
        }

        Post* temp = current;
        current = current->next;
        delete temp;
    }
    feedHead = feedTail = nullptr;
}

Post* FeedManager::copyPost(Post* original) {
    if (!original) return nullptr;

    Post* copy = new Post(original->postID, original->userID, 
                          original->content, original->timestamp);
    copy->likes = original->likes;

    // Copy comments
    Comment* origComment = original->comments;
    Comment* prevCopy = nullptr;

    while (origComment) {
        Comment* commentCopy = new Comment(origComment->commentID, 
                                           origComment->userID,
                                           origComment->content, 
                                           origComment->timestamp);
        
        if (!copy->comments) {
            copy->comments = commentCopy;
        } else {
            prevCopy->next = commentCopy;
        }
        
        prevCopy = commentCopy;
        origComment = origComment->next;
    }

    return copy;
}

void FeedManager::insertSorted(Post* newPost) {
    if (!feedHead || isNewer(newPost->timestamp, feedHead->timestamp)) {
        // Insert at head
        newPost->next = feedHead;
        if (feedHead) {
            feedHead->prev = newPost;
        } else {
            feedTail = newPost;
        }
        feedHead = newPost;
        return;
    }

    // Find position
    Post* current = feedHead;
    while (current->next && isNewer(current->next->timestamp, newPost->timestamp)) {
        current = current->next;
    }

    // Insert after current
    newPost->next = current->next;
    newPost->prev = current;

    if (current->next) {
        current->next->prev = newPost;
    } else {
        feedTail = newPost;
    }

    current->next = newPost;
}

void FeedManager::generateFeed(const vector<int>& followingList, PostManager& postMgr) {
    clearFeed();

    if (followingList.empty()) {
        cout << "You are not following anyone. Your feed is empty.\n";
        return;
    }

    // Collect posts from followed users
    Post* post = postMgr.getHead();
    while (post) {
        for (int followedID : followingList) {
            if (post->userID == followedID) {
                Post* feedPost = copyPost(post);
                insertSorted(feedPost);
                break;
            }
        }
        post = post->next;
    }
}

void FeedManager::displayFeed(int limit) {
    if (!feedHead) {
        cout << "\nYour feed is empty. Follow users to see their posts!\n";
        return;
    }

    cout << "\n========== YOUR FEED (Top " << limit << ") ==========\n";
    Post* current = feedHead;
    int count = 0;

    while (current && count < limit) {
        cout << "\n--- Post #" << (count + 1) << " ---\n";
        cout << "Post ID: " << current->postID << "\n";
        cout << "User ID: " << current->userID << "\n";
        cout << "Content: " << current->content << "\n";
        cout << "Time: " << current->timestamp.year << "-" << current->timestamp.month << "-"
             << current->timestamp.day << " " << current->timestamp.hour << ":"
             << current->timestamp.minute << ":" << current->timestamp.second << "\n";
        cout << "Likes: " << current->likes << " | ";

        int commentCount = 0;
        Comment* c = current->comments;
        while (c) {
            commentCount++;
            c = c->next;
        }
        cout << "Comments: " << commentCount << "\n";

        current = current->next;
        count++;
    }

    cout << "\n===============================================\n";
}