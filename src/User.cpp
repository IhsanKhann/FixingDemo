#include "../include/App.h"

// ==================== USER CLASS ====================
User::User(int id, const string& uname, const string& pass, const string& userBio)
    : userID(id), username(uname), password(pass), bio(userBio),
      followingCount(0), followerCount(0), followingCapacity(10), followersCapacity(10) {
    followingList = new int[followingCapacity];
    followersList = new int[followersCapacity];
}

User::~User() {
    delete[] followingList;
    delete[] followersList;
}

bool User::addFollowing(int targetID) {
    // Check if already following
    for (int i = 0; i < followingCount; i++) {
        if (followingList[i] == targetID) return false;
    }
    
    // Resize if needed
    if (followingCount >= followingCapacity) {
        followingCapacity *= 2;
        int* newList = new int[followingCapacity];
        for (int i = 0; i < followingCount; i++) {
            newList[i] = followingList[i];
        }
        delete[] followingList;
        followingList = newList;
    }
    
    followingList[followingCount++] = targetID;
    return true;
}

bool User::removeFollowing(int targetID) {
    for (int i = 0; i < followingCount; i++) {
        if (followingList[i] == targetID) {
            // Shift elements
            for (int j = i; j < followingCount - 1; j++) {
                followingList[j] = followingList[j + 1];
            }
            followingCount--;
            return true;
        }
    }
    return false;
}

bool User::addFollower(int followerID) {
    // Check if already a follower
    for (int i = 0; i < followerCount; i++) {
        if (followersList[i] == followerID) return false;
    }
    
    // Resize if needed
    if (followerCount >= followersCapacity) {
        followersCapacity *= 2;
        int* newList = new int[followersCapacity];
        for (int i = 0; i < followerCount; i++) {
            newList[i] = followersList[i];
        }
        delete[] followersList;
        followersList = newList;
    }
    
    followersList[followerCount++] = followerID;
    return true;
}

bool User::removeFollower(int followerID) {
    for (int i = 0; i < followerCount; i++) {
        if (followersList[i] == followerID) {
            // Shift elements
            for (int j = i; j < followerCount - 1; j++) {
                followersList[j] = followersList[j + 1];
            }
            followerCount--;
            return true;
        }
    }
    return false;
}

bool User::isFollowing(int targetID) const {
    for (int i = 0; i < followingCount; i++) {
        if (followingList[i] == targetID) return true;
    }
    return false;
}

// ==================== USER DATABASE CLASS ====================
UserDatabase::UserDatabase() : root(nullptr), nextUserID(1001) {}

UserDatabase::~UserDatabase() {
    destroyTree(root);
}

void UserDatabase::destroyTree(UserNode* node) {
    if (!node) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node->user;
    delete node;
}

UserNode* UserDatabase::insertNode(UserNode* node, User* user) {
    if (!node) return new UserNode(user);
    
    if (user->userID < node->user->userID) {
        node->left = insertNode(node->left, user);
    } else if (user->userID > node->user->userID) {
        node->right = insertNode(node->right, user);
    }
    
    return node;
}

UserNode* UserDatabase::searchByID(UserNode* node, int userID) {
    if (!node || node->user->userID == userID) return node;
    
    if (userID < node->user->userID) {
        return searchByID(node->left, userID);
    }
    return searchByID(node->right, userID);
}

User* UserDatabase::searchByUsername(UserNode* node, const string& username) {
    if (!node) return nullptr;
    
    if (node->user->username == username) {
        return node->user;
    }
    
    User* leftResult = searchByUsername(node->left, username);
    if (leftResult) return leftResult;
    
    return searchByUsername(node->right, username);
}

void UserDatabase::collectUsers(UserNode* node, User** arr, int& index) {
    if (!node) return;
    collectUsers(node->left, arr, index);
    arr[index++] = node->user;
    collectUsers(node->right, arr, index);
}

User* UserDatabase::registerUser(const string& username, const string& password, const string& bio) {
    // Validate username
    if (username.length() < 3 || username.length() > 20) {
        return nullptr;
    }
    
    // Validate password
    if (password.length() < 6) {
        return nullptr;
    }
    
    // Check if username exists
    if (searchByUsername(root, username)) {
        return nullptr;
    }
    
    // Create new user
    User* newUser = new User(nextUserID++, username, password, bio);
    root = insertNode(root, newUser);
    
    return newUser;
}

User* UserDatabase::login(const string& username, const string& password) {
    User* user = searchByUsername(root, username);
    if (!user) return nullptr;
    
    if (user->password == password) {
        return user;
    }
    
    return nullptr;
}

User* UserDatabase::searchByID(int userID) {
    UserNode* node = searchByID(root, userID);
    return node ? node->user : nullptr;
}

User* UserDatabase::searchByUsername(const string& username) {
    return searchByUsername(root, username);
}

void UserDatabase::getAllUsers(User** arr, int& count) {
    count = 0;
    collectUsers(root, arr, count);
}