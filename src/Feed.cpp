#include "App.h"

// ==================== FEED CLASS ====================
Feed::Feed() : head(nullptr), count(0) {}

Feed::~Feed() {
    clear();
}

void Feed::clear() {
    FeedNode* current = head;
    while (current) {
        FeedNode* temp = current;
        current = current->next;
        delete temp;
    }
    head = nullptr;
    count = 0;
}

void Feed::insertSorted(Post* post) {
    FeedNode* newNode = new FeedNode(post);
    
    // Insert at head if empty or newer than head
    if (!head || post->timestamp.isNewer(head->post->timestamp)) {
        newNode->next = head;
        head = newNode;
    } else {
        // Find insertion point
        FeedNode* current = head;
        while (current->next && current->next->post->timestamp.isNewer(post->timestamp)) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
    
    count++;
}

void Feed::generateFeed(User* currentUser, PostDatabase* allPosts) {
    clear();
    
    if (!currentUser) return;
    
    // Collect posts from followed users
    Post* current = allPosts->getHead();
    while (current && count < 20) {
        // Include posts from users we follow
        if (currentUser->isFollowing(current->userID)) {
            insertSorted(current);
        }
        current = current->next;
    }
}