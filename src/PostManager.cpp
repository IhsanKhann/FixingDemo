#include "PostManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

Timestamp getCurrentTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    Timestamp ts;
    ts.year = 1900 + ltm->tm_year;
    ts.month = 1 + ltm->tm_mon;
    ts.day = ltm->tm_mday;
    ts.hour = ltm->tm_hour;
    ts.minute = ltm->tm_min;
    ts.second = ltm->tm_sec;
    return ts;
}

PostManager::PostManager() : head(nullptr), tail(nullptr), nextPostID(1001), nextCommentID(1) {}

PostManager::~PostManager() {
    Post* current = head;
    while (current) {
        // Delete all comments first
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
}

int PostManager::createPost(int userID, const string& content) {
    if (content.empty() || content.length() > 280) {
        cout << "Error: Post content must be 1-280 characters!\n";
        return -1;
    }

    Timestamp ts = getCurrentTimestamp();
    Post* newPost = new Post(nextPostID++, userID, content, ts);

    // Insert at head (newest first)
    if (!head) {
        head = tail = newPost;
    } else {
        newPost->next = head;
        head->prev = newPost;
        head = newPost;
    }

    cout << "Post created successfully! Post ID: " << newPost->postID << "\n";
    return newPost->postID;
}

bool PostManager::editPost(int postID, int userID, const string& newContent) {
    if (newContent.empty() || newContent.length() > 280) {
        cout << "Error: Post content must be 1-280 characters!\n";
        return false;
    }

    Post* post = findPost(postID);
    if (!post) {
        cout << "Error: Post not found!\n";
        return false;
    }

    if (post->userID != userID) {
        cout << "Error: You can only edit your own posts!\n";
        return false;
    }

    post->content = newContent;
    cout << "Post edited successfully!\n";
    return true;
}

bool PostManager::deletePost(int postID, int userID) {
    Post* post = findPost(postID);
    if (!post) {
        cout << "Error: Post not found!\n";
        return false;
    }

    if (post->userID != userID) {
        cout << "Error: You can only delete your own posts!\n";
        return false;
    }

    // Delete all comments
    Comment* comment = post->comments;
    while (comment) {
        Comment* temp = comment;
        comment = comment->next;
        delete temp;
    }

    // Remove from list
    if (post->prev) {
        post->prev->next = post->next;
    } else {
        head = post->next;
    }

    if (post->next) {
        post->next->prev = post->prev;
    } else {
        tail = post->prev;
    }

    delete post;
    cout << "Post deleted successfully!\n";
    return true;
}

void PostManager::displayUserPosts(int userID) {
    Post* current = head;
    bool found = false;

    cout << "\n========== Posts by User " << userID << " ==========\n";

    while (current) {
        if (current->userID == userID) {
            displayPost(current);
            found = true;
        }
        current = current->next;
    }

    if (!found) {
        cout << "No posts found for this user.\n";
    }
}

void PostManager::displayPost(Post* post) {
    if (!post) return;

    cout << "\n--- Post ID: " << post->postID << " ---\n";
    cout << "User ID: " << post->userID << "\n";
    cout << "Content: " << post->content << "\n";
    cout << "Time: " << post->timestamp.year << "-" << post->timestamp.month << "-" 
         << post->timestamp.day << " " << post->timestamp.hour << ":" 
         << post->timestamp.minute << ":" << post->timestamp.second << "\n";
    cout << "Likes: " << post->likes << "\n";

    // Count comments
    int commentCount = 0;
    Comment* c = post->comments;
    while (c) {
        commentCount++;
        c = c->next;
    }
    cout << "Comments: " << commentCount << "\n";
}

Post* PostManager::findPost(int postID) {
    Post* current = head;
    while (current) {
        if (current->postID == postID) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

bool PostManager::likePost(int postID, int userID) {
    Post* post = findPost(postID);
    if (!post) {
        cout << "Error: Post not found!\n";
        return false;
    }

    if (post->userID == userID) {
        cout << "Error: You cannot like your own post!\n";
        return false;
    }

    post->likes++;
    cout << "Post liked! Total likes: " << post->likes << "\n";
    return true;
}

bool PostManager::unlikePost(int postID, int userID) {
    Post* post = findPost(postID);
    if (!post) {
        cout << "Error: Post not found!\n";
        return false;
    }

    if (post->likes > 0) {
        post->likes--;
        cout << "Post unliked! Total likes: " << post->likes << "\n";
        return true;
    } else {
        cout << "Error: Post has no likes!\n";
        return false;
    }
}

bool PostManager::addComment(int postID, int userID, const string& content) {
    if (content.empty()) {
        cout << "Error: Comment cannot be empty!\n";
        return false;
    }

    Post* post = findPost(postID);
    if (!post) {
        cout << "Error: Post not found!\n";
        return false;
    }

    Timestamp ts = getCurrentTimestamp();
    Comment* newComment = new Comment(nextCommentID++, userID, content, ts);

    // Add to end of comment list
    if (!post->comments) {
        post->comments = newComment;
    } else {
        Comment* current = post->comments;
        while (current->next) {
            current = current->next;
        }
        current->next = newComment;
    }

    cout << "Comment added successfully!\n";
    return true;
}

void PostManager::displayComments(Post* post) {
    if (!post || !post->comments) {
        cout << "No comments on this post.\n";
        return;
    }

    cout << "\n--- Comments ---\n";
    Comment* current = post->comments;
    while (current) {
        cout << "User " << current->userID << ": " << current->content << "\n";
        current = current->next;
    }
}

void PostManager::saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file for writing!\n";
        return;
    }

    Post* current = head;
    while (current) {
        file << current->postID << "|" << current->userID << "|" 
             << current->content << "|"
             << current->timestamp.year << "-" << current->timestamp.month << "-" 
             << current->timestamp.day << " "
             << current->timestamp.hour << ":" << current->timestamp.minute << ":" 
             << current->timestamp.second << "|"
             << current->likes << "\n";
        current = current->next;
    }

    file.close();
}

void PostManager::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return; // File doesn't exist yet
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string pidStr, uidStr, content, tsStr, likesStr;

        getline(ss, pidStr, '|');
        getline(ss, uidStr, '|');
        getline(ss, content, '|');
        getline(ss, tsStr, '|');
        getline(ss, likesStr, '|');

        int pid = stoi(pidStr);
        int uid = stoi(uidStr);
        int likes = stoi(likesStr);

        // Parse timestamp
        stringstream tsStream(tsStr);
        string dateStr, timeStr;
        getline(tsStream, dateStr, ' ');
        getline(tsStream, timeStr);

        Timestamp ts;
        sscanf(dateStr.c_str(), "%d-%d-%d", &ts.year, &ts.month, &ts.day);
        sscanf(timeStr.c_str(), "%d:%d:%d", &ts.hour, &ts.minute, &ts.second);

        Post* newPost = new Post(pid, uid, content, ts);
        newPost->likes = likes;

        // Add to tail (preserve order from file)
        if (!head) {
            head = tail = newPost;
        } else {
            tail->next = newPost;
            newPost->prev = tail;
            tail = newPost;
        }

        if (pid >= nextPostID) {
            nextPostID = pid + 1;
        }
    }

    file.close();
}