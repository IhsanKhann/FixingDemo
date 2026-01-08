#ifndef APP_H
#define APP_H

// ==================== STANDARD LIBRARIES ====================
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>  // ADD THIS LINE

// ==================== IMGUI ====================
#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_glfw.h"
#include "../imgui/backends/imgui_impl_opengl3.h"

// ==================== GLFW ====================
#include <GLFW/glfw3.h>

using namespace std;

// ==================== ENUMS ====================
enum NotificationType {
    COMMENT = 1,
    LIKE = 2,
    FOLLOW = 3
};

enum Screen {
    LOGIN_SCREEN,
    FEED_SCREEN,
    PROFILE_SCREEN,
    NOTIFICATIONS_SCREEN,
    SEARCH_SCREEN,
    CREATE_POST_SCREEN,
    POST_DETAIL_SCREEN
};

// ==================== TIMESTAMP STRUCT ====================
struct Timestamp {
    int year, month, day;
    int hour, minute, second;

    Timestamp() : year(2025), month(1), day(1), hour(0), minute(0), second(0) {}
    Timestamp(int y, int m, int d, int h, int min, int s)
        : year(y), month(m), day(d), hour(h), minute(min), second(s) {}

    bool isEarlier(const Timestamp& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        if (day != other.day) return day < other.day;
        if (hour != other.hour) return hour < other.hour;
        if (minute != other.minute) return minute < other.minute;
        return second < other.second;
    }

    bool isNewer(const Timestamp& other) const {
        return !isEarlier(other) && !equals(other);
    }

    bool equals(const Timestamp& other) const {
        return year == other.year && month == other.month && day == other.day &&
               hour == other.hour && minute == other.minute && second == other.second;
    }
};

// ==================== FORWARD DECLARATIONS ====================
class User;
class Post;
class Comment;
class Feed;
class History;
class Notification;
class NotificationQueue;
class UserDatabase;
class PostDatabase;
class UI;

// ==================== COMMENT CLASS ====================
class Comment {
public:
    int commentID;
    int userID;
    string username;
    string content;
    Timestamp timestamp;
    Comment* next;

    Comment(int id, int uid, const string& uname, const string& text, Timestamp ts);
};

// ==================== POST CLASS ====================
class Post {
public:
    int postID;
    int userID;
    string username;
    string content;
    Timestamp timestamp;
    int likes;
    int commentCount;
    Comment* comments;
    Post* prev;
    Post* next;

    Post(int pid, int uid, const string& uname, const string& text, Timestamp ts);
    ~Post();

    void addLike();
    void addComment(int uid, const string& uname, const string& text, Timestamp ts);
    Comment* getComments() { return comments; }
};

// ==================== USER CLASS ====================
class User {
public:
    int userID;
    string username;
    string password;
    string bio;
    int* followingList;
    int* followersList;
    int followingCount;
    int followerCount;
    int followingCapacity;
    int followersCapacity;

    User(int id, const string& uname, const string& pass, const string& userBio = "");
    ~User();

    bool addFollowing(int targetID);
    bool removeFollowing(int targetID);
    bool addFollower(int followerID);
    bool removeFollower(int followerID);
    bool isFollowing(int targetID) const;
};

// ==================== FEED CLASS ====================
class FeedNode {
public:
    Post* post;
    FeedNode* next;

    FeedNode(Post* p) : post(p), next(nullptr) {}
};

class Feed {
private:
    FeedNode* head;
    int count;

public:
    Feed();
    ~Feed();

    void clear();
    void insertSorted(Post* post);
    void generateFeed(User* currentUser, PostDatabase* allPosts);
    FeedNode* getHead() { return head; }
};

// ==================== HISTORY CLASS ====================
class HistoryNode {
public:
    int userID;
    int postID;
    HistoryNode* next;

    HistoryNode(int uid, int pid) : userID(uid), postID(pid), next(nullptr) {}
};

class History {
private:
    HistoryNode* top;
    int count;
    static const int MAX_HISTORY = 50;

public:
    History();
    ~History();

    void push(int userID, int postID);
    bool pop(int& userID, int& postID);
    bool peek(int& userID, int& postID);
    void clear();
    bool isEmpty() { return top == nullptr; }
};

// ==================== NOTIFICATION CLASS ====================
class Notification {
public:
    int notificationID;
    NotificationType type;
    int fromUserID;
    string fromUsername;
    int postID;
    string message;
    Timestamp timestamp;
    bool isRead;

    Notification();
    Notification(int id, NotificationType t, int fromID, const string& fromUser,
                 int pID, const string& msg, Timestamp ts);

    bool hasHigherPriority(const Notification& other) const;
};

class NotificationQueue {
private:
    Notification** heap;
    int capacity;
    int size;
    int nextID;

    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }
    void swap(int i, int j);
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    NotificationQueue(int cap = 200);
    ~NotificationQueue();

    void addNotification(NotificationType type, int fromUserID, const string& fromUsername,
                         int postID, const string& message, Timestamp timestamp);
    void getAllNotifications(Notification** arr, int& count);
    void clearAll();
    bool isEmpty() { return size == 0; }
};

// ==================== USER DATABASE CLASS ====================
class UserNode {
public:
    User* user;
    UserNode* left;
    UserNode* right;

    UserNode(User* u) : user(u), left(nullptr), right(nullptr) {}
};

class UserDatabase {
private:
    UserNode* root;
    int nextUserID;

    UserNode* insertNode(UserNode* node, User* user);
    UserNode* searchByID(UserNode* node, int userID);
    User* searchByUsername(UserNode* node, const string& username);
    void destroyTree(UserNode* node);
    void collectUsers(UserNode* node, User** arr, int& index);

public:
    UserDatabase();
    ~UserDatabase();

    User* registerUser(const string& username, const string& password, const string& bio = "");
    User* login(const string& username, const string& password);
    User* searchByID(int userID);
    User* searchByUsername(const string& username);
    void getAllUsers(User** arr, int& count);
    void generateDummyUsers();
    
    // ADD THESE FILE HANDLING METHODS:
    void saveToFile(const string& filename);
    void loadFromFile(const string& filename);
    void saveConnectionsToFile(const string& filename);
    void loadConnectionsFromFile(const string& filename);
};

// ==================== POST DATABASE CLASS ====================
class PostDatabase {
private:
    Post* head;
    Post* tail;
    int nextPostID;
    int nextCommentID;

public:
    PostDatabase();
    ~PostDatabase();

    Post* createPost(int userID, const string& username, const string& content, Timestamp ts);
    bool deletePost(int postID);
    Post* findPost(int postID);
    Post* getHead() { return head; }
    int getNextCommentID() { return nextCommentID++; }
    void generateDummyPosts(UserDatabase* userDB);
    void clearAll();
    
    // ADD THESE FILE HANDLING METHODS:
    void saveToFile(const string& filename);
    void loadFromFile(const string& filename);
};

// ==================== UI CLASS ====================
class UI {
private:
    Screen currentScreen;
    User* currentUser;
    User* viewingUser;
    Post* viewingPost;

    UserDatabase* userDatabase;
    PostDatabase* postDatabase;
    NotificationQueue* notifications;
    History* history;
    Feed* feed;

    char usernameInput[65];
    char passwordInput[65];
    char bioInput[257];
    char postInput[513];
    char commentInput[513];
    char searchInput[65];
    char errorMessage[256];
    bool showError;

    User* searchResults[100];
    int searchResultCount;

    void showErrorMessage(const char* msg);
    Timestamp getCurrentTime();
    string timestampToString(const Timestamp& ts);

public:
    UI(UserDatabase* users, PostDatabase* posts, NotificationQueue* notifs, History* hist);
    ~UI();

    void render();
    void logout();
    void setScreen(Screen screen);

    void renderLoginScreen();
    void renderFeedScreen();
    void renderProfileScreen();
    void renderNotificationsScreen();
    void renderSearchScreen();
    void renderCreatePostScreen();
    void renderPostDetailScreen();
    void initializeDummyData();  // ADD THIS LINE
    void populateDummyFeed();   // <<< declaration fixes the error
};

// ==================== UTILITY FUNCTIONS ====================
Timestamp getCurrentTimestamp();
string timestampToString(const Timestamp& ts);

#endif // APP_H
