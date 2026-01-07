#include "App.h"

// ==================== COMMENT CLASS ====================
Comment::Comment(int id, int uid, const string& uname, const string& text, Timestamp ts)
    : commentID(id), userID(uid), username(uname), content(text), timestamp(ts), next(nullptr) {}

// ==================== POST CLASS ====================
Post::Post(int pid, int uid, const string& uname, const string& text, Timestamp ts)
    : postID(pid), userID(uid), username(uname), content(text), timestamp(ts),
      likes(0), commentCount(0), comments(nullptr), prev(nullptr), next(nullptr) {}

Post::~Post() {
    // Delete all comments
    Comment* current = comments;
    while (current) {
        Comment* temp = current;
        current = current->next;
        delete temp;
    }
}

void Post::addLike() {
    likes++;
}

void Post::addComment(int uid, const string& uname, const string& text, Timestamp ts) {
    Comment* newComment = new Comment(commentCount++, uid, uname, text, ts);
    
    if (!comments) {
        comments = newComment;
    } else {
        Comment* current = comments;
        while (current->next) {
            current = current->next;
        }
        current->next = newComment;
    }
}

// ==================== POST DATABASE CLASS ====================
PostDatabase::PostDatabase() : head(nullptr), tail(nullptr), nextPostID(1001), nextCommentID(1) {}

PostDatabase::~PostDatabase() {
    Post* current = head;
    while (current) {
        Post* temp = current;
        current = current->next;
        delete temp;
    }
}

Post* PostDatabase::createPost(int userID, const string& username, const string& content, Timestamp ts) {
    if (content.empty() || content.length() > 280) {
        return nullptr;
    }
    
    Post* newPost = new Post(nextPostID++, userID, username, content, ts);
    
    // Insert at head (newest first)
    if (!head) {
        head = tail = newPost;
    } else {
        newPost->next = head;
        head->prev = newPost;
        head = newPost;
    }
    
    return newPost;
}

bool PostDatabase::deletePost(int postID) {
    Post* post = findPost(postID);
    if (!post) return false;
    
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
    return true;
}

Post* PostDatabase::findPost(int postID) {
    Post* current = head;
    while (current) {
        if (current->postID == postID) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// ==================== UTILITY FUNCTIONS ====================
Timestamp getCurrentTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    return Timestamp(1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
                    ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
}

string timestampToString(const Timestamp& ts) {
    char buffer[64];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
            ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second);
    return string(buffer);
}