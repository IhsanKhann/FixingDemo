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

// ==================== DUMMY DATA GENERATION ====================
void UserDatabase::generateDummyUsers() {
    // Create 5 dummy users
    User* alice = registerUser("alice", "password123", "Love coding and coffee! ☕");
    User* bob = registerUser("bob", "password123", "Gaming enthusiast 🎮");
    User* charlie = registerUser("charlie", "password123", "Traveling the world 🌍");
    User* diana = registerUser("diana", "password123", "Photography lover 📷");
    User* eve = registerUser("eve", "password123", "Music is life 🎵");
    
    if (!alice || !bob || !charlie || !diana || !eve) return;
    
    // Set up follower relationships
    // Alice follows Bob, Charlie, Diana
    alice->addFollowing(bob->userID);
    bob->addFollower(alice->userID);
    
    alice->addFollowing(charlie->userID);
    charlie->addFollower(alice->userID);
    
    alice->addFollowing(diana->userID);
    diana->addFollower(alice->userID);
    
    // Bob follows Alice, Eve
    bob->addFollowing(alice->userID);
    alice->addFollower(bob->userID);
    
    bob->addFollowing(eve->userID);
    eve->addFollower(bob->userID);
    
    // Charlie follows Alice, Diana, Eve
    charlie->addFollowing(alice->userID);
    alice->addFollower(charlie->userID);
    
    charlie->addFollowing(diana->userID);
    diana->addFollower(charlie->userID);
    
    charlie->addFollowing(eve->userID);
    eve->addFollower(charlie->userID);
    
    // Diana follows Bob, Charlie
    diana->addFollowing(bob->userID);
    bob->addFollower(diana->userID);
    
    diana->addFollowing(charlie->userID);
    charlie->addFollower(diana->userID);
    
    // Eve follows Alice, Bob, Diana
    eve->addFollowing(alice->userID);
    alice->addFollower(eve->userID);
    
    eve->addFollowing(bob->userID);
    bob->addFollower(eve->userID);
    
    eve->addFollowing(diana->userID);
    diana->addFollower(eve->userID);
}

// ==================== FILE HANDLING ====================
void UserDatabase::saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open " << filename << " for writing" << endl;
        return;
    }
    
    User* users[100];
    int count = 0;
    getAllUsers(users, count);
    
    file << count << endl; // Save total number of users
    
    for (int i = 0; i < count; i++) {
        User* u = users[i];
        file << u->userID << endl;
        file << u->username << endl;
        file << u->password << endl;
        file << u->bio << endl;
        file << u->followerCount << endl;
        file << u->followingCount << endl;
    }
    
    file.close();
    cout << "Saved " << count << " users to " << filename << endl;
}

void UserDatabase::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "No existing users file found. Starting fresh." << endl;
        return;
    }
    
    int count;
    file >> count;
    file.ignore(); // Skip newline
    
    for (int i = 0; i < count; i++) {
        int userID;
        string username, password, bio;
        int followerCount, followingCount;
        
        file >> userID;
        file.ignore();
        getline(file, username);
        getline(file, password);
        getline(file, bio);
        file >> followerCount >> followingCount;
        file.ignore();
        
        // Create user directly with ID
        User* newUser = new User(userID, username, password, bio);
        root = insertNode(root, newUser);
        
        // Update nextUserID
        if (userID >= nextUserID) {
            nextUserID = userID + 1;
        }
    }
    
    file.close();
    cout << "Loaded " << count << " users from " << filename << endl;
}

void UserDatabase::saveConnectionsToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open " << filename << " for writing" << endl;
        return;
    }
    
    User* users[100];
    int count = 0;
    getAllUsers(users, count);
    
    // Save following relationships
    for (int i = 0; i < count; i++) {
        User* u = users[i];
        for (int j = 0; j < u->followingCount; j++) {
            file << u->userID << " " << u->followingList[j] << endl;
        }
    }
    
    file.close();
    cout << "Saved connections to " << filename << endl;
}

void UserDatabase::loadConnectionsFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "No existing connections file found." << endl;
        return;
    }
    
    int userID, followingID;
    while (file >> userID >> followingID) {
        User* user = searchByID(userID);
        User* target = searchByID(followingID);
        
        if (user && target) {
            user->addFollowing(followingID);
            target->addFollower(userID);
        }
    }
    
    file.close();
    cout << "Loaded connections from " << filename << endl;
}