#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>

using namespace std;

struct Connection {
    int userID;
    Connection* next;
    Connection(int id) : userID(id), next(nullptr) {}
};

struct User {
    int userID;
    string username;
    string password;
    string bio;
    Connection* followersList;
    Connection* followingList;
    int followerCount;
    int followingCount;

    User(int id, string uname, string pass, string userBio = "")
        : userID(id), username(uname), password(pass), bio(userBio),
          followersList(nullptr), followingList(nullptr),
          followerCount(0), followingCount(0) {}
};

struct UserNode {
    User* userData;
    UserNode* left;
    UserNode* right;
    UserNode(User* user) : userData(user), left(nullptr), right(nullptr) {}
};

class UserManagement {
private:
    UserNode* root;
    int nextUserID;
    User* currentLoggedInUser;

    UserNode* insertNode(UserNode* node, User* user) {
        if (!node) return new UserNode(user);
        if (user->userID < node->userData->userID)
            node->left = insertNode(node->left, user);
        else if (user->userID > node->userData->userID)
            node->right = insertNode(node->right, user);
        return node;
    }

    UserNode* searchByID(UserNode* node, int userID) {
        if (!node || node->userData->userID == userID) return node;
        if (userID < node->userData->userID) 
            return searchByID(node->left, userID);
        return searchByID(node->right, userID);
    }

    User* searchByUsernameHelper(UserNode* node, const string& username) {
        if (!node) return nullptr;
        if (node->userData->username == username) return node->userData;
        User* leftResult = searchByUsernameHelper(node->left, username);
        if (leftResult) return leftResult;
        return searchByUsernameHelper(node->right, username);
    }

    void inOrderTraversal(UserNode* node, vector<User*>& users) {
        if (!node) return;
        inOrderTraversal(node->left, users);
        users.push_back(node->userData);
        inOrderTraversal(node->right, users);
    }

    void destroyTree(UserNode* node) {
        if (!node) return;
        destroyTree(node->left);
        destroyTree(node->right);
        deleteConnectionList(node->userData->followersList);
        deleteConnectionList(node->userData->followingList);
        delete node->userData;
        delete node;
    }

    void deleteConnectionList(Connection* head) {
        while (head) {
            Connection* temp = head;
            head = head->next;
            delete temp;
        }
    }

    bool addConnection(Connection*& head, int userID) {
        Connection* temp = head;
        while (temp) {
            if (temp->userID == userID) return false;
            temp = temp->next;
        }
        Connection* newConn = new Connection(userID);
        newConn->next = head;
        head = newConn;
        return true;
    }

    bool removeConnection(Connection*& head, int userID) {
        if (!head) return false;
        if (head->userID == userID) {
            Connection* temp = head;
            head = head->next;
            delete temp;
            return true;
        }
        Connection* current = head;
        while (current->next) {
            if (current->next->userID == userID) {
                Connection* temp = current->next;
                current->next = current->next->next;
                delete temp;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    bool connectionExists(Connection* head, int userID) {
        Connection* temp = head;
        while (temp) {
            if (temp->userID == userID) return true;
            temp = temp->next;
        }
        return false;
    }

    vector<int> getConnectionIDs(Connection* head) {
        vector<int> ids;
        Connection* temp = head;
        while (temp) {
            ids.push_back(temp->userID);
            temp = temp->next;
        }
        return ids;
    }

public:
    UserManagement() : root(nullptr), nextUserID(1001), currentLoggedInUser(nullptr) {}
    ~UserManagement() { destroyTree(root); }

    int registerUser(const string& username, const string& password, const string& bio = "") {
        if (username.length() < 3 || username.length() > 20 || username.find(' ') != string::npos)
            return 2;
        if (password.length() < 6) return 3;
        if (searchByUsernameHelper(root, username)) return 1;

        User* newUser = new User(nextUserID, username, password, bio);
        root = insertNode(root, newUser);
        nextUserID++;
        cout << "User registered: @" << username << " (ID: " << newUser->userID << ")\n";
        return 0;
    }

    User* loginUser(const string& username, const string& password) {
        User* user = searchByUsernameHelper(root, username);
        if (!user) { 
            cout << "Error: Username not found\n"; 
            return nullptr; 
        }
        if (user->password != password) { 
            cout << "Error: Incorrect password\n"; 
            return nullptr; 
        }
        currentLoggedInUser = user;
        cout << "Login successful: Welcome @" << username << "\n";
        return user;
    }

    void logoutUser() {
        if (currentLoggedInUser) {
            cout << "User logged out: @" << currentLoggedInUser->username << "\n";
            currentLoggedInUser = nullptr;
        }
    }

    User* getCurrentUser() {
        return currentLoggedInUser;
    }

    User* getUserByID(int userID) { 
        UserNode* node = searchByID(root, userID);
        return node ? node->userData : nullptr;
    }

    User* getUserByUsername(const string& username) { 
        return searchByUsernameHelper(root, username); 
    }

    vector<User*> getAllUsers() {
        vector<User*> users;
        inOrderTraversal(root, users);
        return users;
    }

    bool updateBio(int userID, const string& newBio) {
        User* user = getUserByID(userID);
        if (!user) { 
            cout << "Error: User not found\n"; 
            return false; 
        }
        user->bio = newBio;
        cout << "Bio updated\n";
        return true;
    }

    bool updateUsername(int userID, const string& newUsername) {
        if (newUsername.length() < 3 || newUsername.length() > 20) { 
            cout << "Error: Invalid username\n"; 
            return false; 
        }
        User* existing = searchByUsernameHelper(root, newUsername);
        if (existing && existing->userID != userID) { 
            cout << "Error: Username taken\n"; 
            return false; 
        }
        User* user = getUserByID(userID);
        if (!user) { 
            cout << "Error: User not found\n"; 
            return false; 
        }
        user->username = newUsername;
        cout << "Username updated\n";
        return true;
    }

    bool updatePassword(int userID, const string& oldPassword, const string& newPassword) {
        User* user = getUserByID(userID);
        if (!user) { 
            cout << "Error: User not found\n"; 
            return false; 
        }
        if (user->password != oldPassword) { 
            cout << "Error: Incorrect old password\n"; 
            return false; 
        }
        if (newPassword.length() < 6) { 
            cout << "Error: Password too short\n"; 
            return false; 
        }
        user->password = newPassword;
        cout << "Password updated\n";
        return true;
    }

    int followUser(int followerID, int targetID) {
        if (followerID == targetID) { 
            cout << "Error: Cannot follow yourself\n"; 
            return 1; 
        }
        User* follower = getUserByID(followerID);
        User* target = getUserByID(targetID);
        if (!follower || !target) { 
            cout << "Error: User not found\n"; 
            return 3; 
        }
        if (connectionExists(follower->followingList, targetID)) { 
            cout << "Already following\n"; 
            return 2; 
        }
        
        addConnection(follower->followingList, targetID);
        follower->followingCount++;
        addConnection(target->followersList, followerID);
        target->followerCount++;
        
        cout << "@" << follower->username << " is now following @" << target->username << "\n";
        return 0;
    }

    bool unfollowUser(int followerID, int targetID) {
        User* follower = getUserByID(followerID);
        User* target = getUserByID(targetID);
        if (!follower || !target) { 
            cout << "Error: User not found\n"; 
            return false; 
        }
        if (!removeConnection(follower->followingList, targetID)) { 
            cout << "Not following\n"; 
            return false; 
        }
        
        follower->followingCount--;
        removeConnection(target->followersList, followerID);
        target->followerCount--;
        
        cout << "@" << follower->username << " unfollowed @" << target->username << "\n";
        return true;
    }

    bool isFollowing(int followerID, int targetID) {
        User* follower = getUserByID(followerID);
        if (!follower) return false;
        return connectionExists(follower->followingList, targetID);
    }

    vector<User*> getFollowers(int userID) {
        User* user = getUserByID(userID);
        vector<User*> followers;
        if (!user) return followers;
        for (int id : getConnectionIDs(user->followersList)) {
            User* u = getUserByID(id);
            if (u) followers.push_back(u);
        }
        return followers;
    }

    vector<User*> getFollowing(int userID) {
        User* user = getUserByID(userID);
        vector<User*> following;
        if (!user) return following;
        for (int id : getConnectionIDs(user->followingList)) {
            User* u = getUserByID(id);
            if (u) following.push_back(u);
        }
        return following;
    }

    vector<int> getFollowingIDs(int userID) {
        User* user = getUserByID(userID);
        if (!user) return vector<int>();
        return getConnectionIDs(user->followingList);
    }

    void displayUserProfile(int userID) {
        User* user = getUserByID(userID);
        if (!user) { 
            cout << "User not found\n"; 
            return; 
        }
        cout << "\n=== USER PROFILE ===\n";
        cout << "ID: " << user->userID << "\n";
        cout << "Username: @" << user->username << "\n";
        cout << "Bio: " << (user->bio.empty() ? "No bio" : user->bio) << "\n";
        cout << "Followers: " << user->followerCount << "\n";
        cout << "Following: " << user->followingCount << "\n";
        cout << "====================\n";
    }

    void displayAllUsers() {
        vector<User*> users = getAllUsers();
        if (users.empty()) {
            cout << "\nNo users registered.\n";
            return;
        }
        cout << "\n=== ALL USERS ===\n";
        for (User* u : users) {
            cout << "ID: " << u->userID << " | @" << u->username
                 << " | Followers: " << u->followerCount 
                 << " | Following: " << u->followingCount << "\n";
        }
        cout << "=================\n";
    }

    vector<User*> searchUsers(const string& searchTerm) {
        vector<User*> allUsers = getAllUsers();
        vector<User*> results;
        
        string lowerSearch = searchTerm;
        transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
        
        for (User* user : allUsers) {
            string lowerUsername = user->username;
            transform(lowerUsername.begin(), lowerUsername.end(), lowerUsername.begin(), ::tolower);
            
            if (lowerUsername.find(lowerSearch) != string::npos) {
                results.push_back(user);
            }
        }
        return results;
    }

    bool saveUsersToFile(const string& filename = "users.txt") {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Cannot save users\n";
            return false;
        }
        
        vector<User*> users = getAllUsers();
        for (User* user : users) {
            file << user->userID << "|" 
                 << user->username << "|" 
                 << user->password << "|" 
                 << user->bio << "|"
                 << user->followerCount << "|"
                 << user->followingCount << "\n";
        }
        
        file.close();
        cout << "Users saved to " << filename << "\n";
        return true;
    }

    bool saveConnectionsToFile(const string& filename = "connections.txt") {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Cannot save connections\n";
            return false;
        }
        
        vector<User*> users = getAllUsers();
        for (User* user : users) {
            vector<int> followingIDs = getConnectionIDs(user->followingList);
            for (int targetID : followingIDs) {
                file << user->userID << "," << targetID << "\n";
            }
        }
        
        file.close();
        cout << "Connections saved to " << filename << "\n";
        return true;
    }

    bool loadUsersFromFile(const string& filename = "users.txt") {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "No existing user file. Starting fresh.\n";
            return false;
        }
        
        string line;
        int maxID = 1000;
        
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 4) {
                int userID = stoi(tokens[0]);
                string username = tokens[1];
                string password = tokens[2];
                string bio = tokens[3];
                
                User* user = new User(userID, username, password, bio);
                root = insertNode(root, user);
                
                if (userID > maxID) {
                    maxID = userID;
                }
            }
        }
        
        nextUserID = maxID + 1;
        file.close();
        cout << "Users loaded from " << filename << "\n";
        return true;
    }

    bool loadConnectionsFromFile(const string& filename = "connections.txt") {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "No existing connections file.\n";
            return false;
        }
        
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string followerStr, targetStr;
            
            if (getline(ss, followerStr, ',') && getline(ss, targetStr, ',')) {
                int followerID = stoi(followerStr);
                int targetID = stoi(targetStr);
                
                User* follower = getUserByID(followerID);
                User* target = getUserByID(targetID);
                if (follower && target && !connectionExists(follower->followingList, targetID)) {
                    addConnection(follower->followingList, targetID);
                    follower->followingCount++;
                    addConnection(target->followersList, followerID);
                    target->followerCount++;
                }
            }
        }
        
        file.close();
        cout << "Connections loaded from " << filename << "\n";
        return true;
    }
};

void displayUserMainMenu() {
    cout << "\n=== SOCIAL MEDIA SYSTEM ===\n";
    cout << "1. Register\n";
    cout << "2. Login\n";
    cout << "3. View All Users\n";
    cout << "4. Save Data\n";
    cout << "5. Load Data\n";
    cout << "6. Exit\n";
    cout << "Choice: ";
}

void displayUserMenu(const string& username) {
    cout << "\n=== Logged in as @" << username << " ===\n";
    cout << "1. View Profile\n";
    cout << "2. Update Bio\n";
    cout << "3. Update Username\n";
    cout << "4. Update Password\n";
    cout << "5. Follow User\n";
    cout << "6. Unfollow User\n";
    cout << "7. View Followers\n";
    cout << "8. View Following\n";
    cout << "9. Search Users\n";
    cout << "10. Logout\n";
    cout << "Choice: ";
}

void userMenu() {
    UserManagement userManager;
    int choice;

    cout << "\n=================================\n";
    cout << "  USER MANAGEMENT SYSTEM\n";
    cout << "=================================\n";

    userManager.loadUsersFromFile();
    userManager.loadConnectionsFromFile();

    while (true) {
        displayUserMainMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input!\n";
            continue;
        }

        if (choice == 1) {
            string username, password, bio;
            cout << "\n--- REGISTER ---\n";
            cout << "Username: ";
            cin >> username;
            cout << "Password: ";
            cin >> password;
            cin.ignore();
            cout << "Bio (optional): ";
            getline(cin, bio);
            
            int status = userManager.registerUser(username, password, bio);
            if (status == 1) cout << "Username exists!\n";
            else if (status == 2) cout << "Invalid username!\n";
            else if (status == 3) cout << "Password too short!\n";
            
        } else if (choice == 2) {
            string username, password;
            cout << "\n--- LOGIN ---\n";
            cout << "Username: ";
            cin >> username;
            cout << "Password: ";
            cin >> password;
            
            User* currentUser = userManager.loginUser(username, password);
            if (!currentUser) continue;

            int userChoice;
            do {
                displayUserMenu(currentUser->username);
                cin >> userChoice;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input!\n";
                    continue;
                }

                switch(userChoice) {
                    case 1:
                        userManager.displayUserProfile(currentUser->userID);
                        break;
                        
                    case 2: {
                        cin.ignore();
                        string bio;
                        cout << "New bio: ";
                        getline(cin, bio);
                        userManager.updateBio(currentUser->userID, bio);
                        break;
                    }
                    
                    case 3: {
                        string newUsername;
                        cout << "New username: ";
                        cin >> newUsername;
                        userManager.updateUsername(currentUser->userID, newUsername);
                        break;
                    }
                    
                    case 4: {
                        string oldPass, newPass;
                        cout << "Old password: ";
                        cin >> oldPass;
                        cout << "New password: ";
                        cin >> newPass;
                        userManager.updatePassword(currentUser->userID, oldPass, newPass);
                        break;
                    }
                    
                    case 5: {
                        string targetUsername;
                        cout << "Username to follow: ";
                        cin >> targetUsername;
                        User* target = userManager.getUserByUsername(targetUsername);
                        if (target) {
                            userManager.followUser(currentUser->userID, target->userID);
                        } else {
                            cout << "User not found!\n";
                        }
                        break;
                    }
                    
                    case 6: {
                        string targetUsername;
                        cout << "Username to unfollow: ";
                        cin >> targetUsername;
                        User* target = userManager.getUserByUsername(targetUsername);
                        if (target) {
                            userManager.unfollowUser(currentUser->userID, target->userID);
                        } else {
                            cout << "User not found!\n";
                        }
                        break;
                    }
                    
                    case 7: {
                        vector<User*> followers = userManager.getFollowers(currentUser->userID);
                        if (followers.empty()) {
                            cout << "\nNo followers yet.\n";
                        } else {
                            cout << "\n--- FOLLOWERS (" << followers.size() << ") ---\n";
                            for (auto u : followers) {
                                cout << "  @" << u->username << " (ID: " << u->userID << ")\n";
                            }
                        }
                        break;
                    }
                    
                    case 8: {
                        vector<User*> following = userManager.getFollowing(currentUser->userID);
                        if (following.empty()) {
                            cout << "\nNot following anyone.\n";
                        } else {
                            cout << "\n--- FOLLOWING (" << following.size() << ") ---\n";
                            for (auto u : following) {
                                cout << "  @" << u->username << " (ID: " << u->userID << ")\n";
                            }
                        }
                        break;
                    }
                    
                    case 9: {
                        string searchTerm;
                        cout << "Search: ";
                        cin >> searchTerm;
                        vector<User*> results = userManager.searchUsers(searchTerm);
                        if (results.empty()) {
                            cout << "\nNo users found.\n";
                        } else {
                            cout << "\n--- RESULTS (" << results.size() << ") ---\n";
                            for (User* u : results) {
                                cout << "  @" << u->username << " (ID: " << u->userID 
                                     << ") | Followers: " << u->followerCount << "\n";
                            }
                        }
                        break;
                    }
                    
                    case 10:
                        userManager.logoutUser();
                        break;
                        
                    default:
                        if (userChoice != 10) {
                            cout << "Invalid choice!\n";
                        }
                }
            } while (userChoice != 10);
            
        } else if (choice == 3) {
            userManager.displayAllUsers();
            
        } else if (choice == 4) {
            userManager.saveUsersToFile();
            userManager.saveConnectionsToFile();
            
        } else if (choice == 5) {
            userManager.loadUsersFromFile();
            userManager.loadConnectionsFromFile();
            
        } else if (choice == 6) {
            cout << "\nAuto-saving...\n";
            userManager.saveUsersToFile();
            userManager.saveConnectionsToFile();
            cout << "\nGoodbye!\n\n";
            break;
            
        } else {
            cout << "Invalid choice!\n";
        }
    }

}

// Global function to get user's following list (stub - returns empty for now since user management is session-local)
vector<int> getUserFollowingList(int userID) {
    // Note: Full integration would require shared user management instance
    return vector<int>();
}