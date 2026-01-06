#include <iostream>
#include "RaweenAamir_headerFile.h"
#include <string>
using namespace std;


// ---------------- Post List ----------------
class PostList {
private:
    Post* head;

public:
    PostList() : head(nullptr) {}

    Post* getHead() { return head; }

    // Create post (newest first)
    bool createPost(int postID, int userID, const string& text, Timestamp ts) {
        if (text.empty() || text.length() > 280) return false;

        Post* newPost = new Post(postID, userID, text, ts);

        if (!head || isNewer(ts, head->timestamp)) {
            newPost->next = head;
            if (head) head->prev = newPost;
            head = newPost;
            return true;
        }

        Post* curr = head;
        while (curr->next && !isNewer(ts, curr->next->timestamp)) {
            curr = curr->next;
        }

        newPost->next = curr->next;
        newPost->prev = curr;
        if (curr->next) curr->next->prev = newPost;
        curr->next = newPost;
        return true;
    }

    // Edit post (owner only)
    bool editPost(int postID, int userID, const string& newText) {
        if (newText.empty() || newText.length() > 280) return false;

        Post* curr = head;
        while (curr) {
            if (curr->postID == postID && curr->userID == userID) {
                curr->content = newText;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    // Delete post (owner only)
    bool deletePost(int postID, int userID) {
        Post* curr = head;
        while (curr) {
            if (curr->postID == postID && curr->userID == userID) {
                if (curr->prev) curr->prev->next = curr->next;
                else head = curr->next;

                if (curr->next) curr->next->prev = curr->prev;
                delete curr;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    // Display posts by user
    void displayUserPosts(int userID) {
        Post* curr = head;
        while (curr) {
            if (curr->userID == userID) {
                cout << "PostID: " << curr->postID << "\n";
                cout << curr->content << "\n";
                cout << "Likes: " << curr->likes << "\n\n";
            }
            curr = curr->next;
        }
    }
};
