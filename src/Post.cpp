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

// ==================== DUMMY DATA GENERATION ====================
void PostDatabase::generateDummyPosts(UserDatabase* userDB) {
    // Get dummy users
    User* alice = userDB->searchByUsername("alice");
    User* bob = userDB->searchByUsername("bob");
    User* charlie = userDB->searchByUsername("charlie");
    User* diana = userDB->searchByUsername("diana");
    User* eve = userDB->searchByUsername("eve");
    
    if (!alice || !bob || !charlie || !diana || !eve) return;
    
    // Create timestamps for posts (going back in time)
    Timestamp now = getCurrentTimestamp();
    
    // Helper to create timestamp X hours ago
    auto hoursAgo = [&](int hours) {
        Timestamp ts = now;
        ts.hour -= hours;
        if (ts.hour < 0) {
            ts.hour += 24;
            ts.day -= 1;
        }
        return ts;
    };
    
    // Create posts (most recent first)
    Post* post1 = createPost(eve->userID, eve->username, 
        "Just discovered this amazing Lo-Fi playlist! Perfect for coding sessions 🎧", 
        hoursAgo(1));
    
    Post* post2 = createPost(diana->userID, diana->username, 
        "Golden hour is the best time for photography! Just captured an amazing sunset 🌅", 
        hoursAgo(2));
    
    Post* post3 = createPost(charlie->userID, charlie->username, 
        "Travel tip: Always pack light! You'll thank yourself later ✈️", 
        hoursAgo(3));
    
    Post* post4 = createPost(bob->userID, bob->username, 
        "Any gamers here? Just finished an epic raid! Looking for team members 🎮", 
        hoursAgo(4));
    
    Post* post5 = createPost(alice->userID, alice->username, 
        "Learning C++ GUI with ImGui is actually really fun! Who knew? 💻", 
        hoursAgo(5));
    
    Post* post6 = createPost(eve->userID, eve->username, 
        "Music recommendation: Check out some synthwave for productivity!", 
        hoursAgo(6));
    
    Post* post7 = createPost(diana->userID, diana->username, 
        "Photography tip: Rule of thirds changes everything! 📸", 
        hoursAgo(8));
    
    Post* post8 = createPost(charlie->userID, charlie->username, 
        "Exploring the mountains today! The view is absolutely breathtaking 🏔️", 
        hoursAgo(10));
    
    Post* post9 = createPost(bob->userID, bob->username, 
        "Just finished a new game level! That boss fight was intense! 🔥", 
        hoursAgo(12));
    
    Post* post10 = createPost(alice->userID, alice->username, 
        "Hello everyone! This is my first post on this platform. Excited to be here! 👋", 
        hoursAgo(15));
    
    // Add some likes to posts
    if (post1) post1->addLike();
    if (post2) { post2->addLike(); post2->addLike(); }
    if (post3) post3->addLike();
    if (post4) { post4->addLike(); post4->addLike(); post4->addLike(); }
    if (post5) { post5->addLike(); post5->addLike(); }
    if (post7) post7->addLike();
    if (post10) { post10->addLike(); post10->addLike(); post10->addLike(); post10->addLike(); }
    
    // Add some comments
    if (post10) {
        post10->addComment(bob->userID, bob->username, 
            "Welcome Alice! Glad to have you here!", hoursAgo(14));
        post10->addComment(charlie->userID, charlie->username, 
            "Hey Alice! Looking forward to your posts!", hoursAgo(14));
        post10->addComment(eve->userID, eve->username, 
            "Welcome to the community! 🎉", hoursAgo(13));
    }
    
    if (post5) {
        post5->addComment(bob->userID, bob->username, 
            "ImGui is awesome! What are you building?", hoursAgo(4));
    }
    
    if (post4) {
        post4->addComment(charlie->userID, charlie->username, 
            "I'm interested! What game?", hoursAgo(3));
    }
    
    if (post2) {
        post2->addComment(eve->userID, eve->username, 
            "Beautiful shot! What camera do you use?", hoursAgo(1));
    }
}

// ==================== FILE HANDLING ====================
void PostDatabase::saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open " << filename << " for writing" << endl;
        return;
    }
    
    int postCount = 0;
    Post* current = head;
    while (current) {
        postCount++;
        current = current->next;
    }
    
    file << postCount << endl; // Save total number of posts
    
    current = head;
    while (current) {
        file << current->postID << endl;
        file << current->userID << endl;
        file << current->username << endl;
        file << current->content << endl;
        file << current->timestamp.year << " " << current->timestamp.month << " " 
             << current->timestamp.day << " " << current->timestamp.hour << " "
             << current->timestamp.minute << " " << current->timestamp.second << endl;
        file << current->likes << endl;
        file << current->commentCount << endl;
        
        // Save comments
        Comment* comment = current->comments;
        while (comment) {
            file << "COMMENT" << endl;
            file << comment->commentID << endl;
            file << comment->userID << endl;
            file << comment->username << endl;
            file << comment->content << endl;
            file << comment->timestamp.year << " " << comment->timestamp.month << " "
                 << comment->timestamp.day << " " << comment->timestamp.hour << " "
                 << comment->timestamp.minute << " " << comment->timestamp.second << endl;
            comment = comment->next;
        }
        file << "END_COMMENTS" << endl;
        
        current = current->next;
    }
    
    file.close();
    cout << "Saved " << postCount << " posts to " << filename << endl;
}

void PostDatabase::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "No existing posts file found. Starting fresh." << endl;
        return;
    }
    
    int postCount;
    file >> postCount;
    file.ignore();
    
    for (int i = 0; i < postCount; i++) {
        int postID, userID, likes, commentCount;
        string username, content, line;
        Timestamp ts;
        
        file >> postID;
        file.ignore();
        file >> userID;
        file.ignore();
        getline(file, username);
        getline(file, content);
        file >> ts.year >> ts.month >> ts.day >> ts.hour >> ts.minute >> ts.second;
        file >> likes >> commentCount;
        file.ignore();
        
        // Create post directly
        Post* post = new Post(postID, userID, username, content, ts);
        post->likes = likes;
        
        // Insert at tail to maintain order
        if (!head) {
            head = tail = post;
        } else {
            tail->next = post;
            post->prev = tail;
            tail = post;
        }
        
        // Update nextPostID
        if (postID >= nextPostID) {
            nextPostID = postID + 1;
        }
        
        // Load comments
        while (true) {
            getline(file, line);
            if (line == "END_COMMENTS") break;
            if (line == "COMMENT") {
                int commentID, commentUserID;
                string commentUsername, commentContent;
                Timestamp commentTS;
                
                file >> commentID;
                file.ignore();
                file >> commentUserID;
                file.ignore();
                getline(file, commentUsername);
                getline(file, commentContent);
                file >> commentTS.year >> commentTS.month >> commentTS.day 
                     >> commentTS.hour >> commentTS.minute >> commentTS.second;
                file.ignore();
                
                post->addComment(commentUserID, commentUsername, commentContent, commentTS);
            }
        }
    }
    
    file.close();
    cout << "Loaded " << postCount << " posts from " << filename << endl;
}

void PostDatabase::clearAll() {
    Post* current = head;
    while (current) {
        Post* temp = current;
        current = current->next;
        delete temp;
    }
    head = tail = nullptr;
    nextPostID = 1001;
}