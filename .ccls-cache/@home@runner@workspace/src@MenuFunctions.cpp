#include "MasterHeader.h"
#include "PostManager.h"
#include "FeedManager.h"
#include <iostream>
#include <vector>

using namespace std;

// External references
extern PostManager postManager;
extern FeedManager feedManager;

// Assumed helper function to get user's following list
extern vector<int> getUserFollowingList(int userID);

void postMenu() {
    int choice;
    cout << "\n========== Post Management ==========\n";
    cout << "1. Create Post\n";
    cout << "2. Edit Post\n";
    cout << "3. Delete Post\n";
    cout << "4. Display User's Posts\n";
    cout << "5. View Post Details\n";
    cout << "6. Back\n";
    cout << "Choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
        case 1: {
            int userID;
            string content;
            cout << "Your User ID: ";
            cin >> userID;
            cin.ignore();
            cout << "Post content (max 280 chars): ";
            getline(cin, content);
            int postID = postManager.createPost(userID, content);
            if (postID != -1) {
                postManager.saveToFile("data/posts.txt");
            }
            break;
        }
        case 2: {
            int postID, userID;
            string newContent;
            cout << "Post ID: ";
            cin >> postID;
            cout << "Your User ID: ";
            cin >> userID;
            cin.ignore();
            cout << "New content: ";
            getline(cin, newContent);
            if (postManager.editPost(postID, userID, newContent)) {
                postManager.saveToFile("data/posts.txt");
            }
            break;
        }
        case 3: {
            int postID, userID;
            cout << "Post ID: ";
            cin >> postID;
            cout << "Your User ID: ";
            cin >> userID;
            if (postManager.deletePost(postID, userID)) {
                postManager.saveToFile("data/posts.txt");
            }
            break;
        }
        case 4: {
            int userID;
            cout << "User ID: ";
            cin >> userID;
            postManager.displayUserPosts(userID);
            break;
        }
        case 5: {
            int postID;
            cout << "Post ID: ";
            cin >> postID;
            Post* post = postManager.findPost(postID);
            if (post) {
                postManager.displayPost(post);
                postManager.displayComments(post);
            } else {
                cout << "Post not found!\n";
            }
            break;
        }
        case 6:
            return;
        default:
            cout << "Invalid choice!\n";
    }
}

void interactionMenu() {
    int choice;
    cout << "\n========== Interactions ==========\n";
    cout << "1. Like Post\n";
    cout << "2. Unlike Post\n";
    cout << "3. Add Comment\n";
    cout << "4. Back\n";
    cout << "Choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
        case 1: {
            int postID, userID;
            cout << "Post ID: ";
            cin >> postID;
            cout << "Your User ID: ";
            cin >> userID;
            if (postManager.likePost(postID, userID)) {
                postManager.saveToFile("data/posts.txt");
            }
            break;
        }
        case 2: {
            int postID, userID;
            cout << "Post ID: ";
            cin >> postID;
            cout << "Your User ID: ";
            cin >> userID;
            if (postManager.unlikePost(postID, userID)) {
                postManager.saveToFile("data/posts.txt");
            }
            break;
        }
        case 3: {
            int postID, userID;
            string content;
            cout << "Post ID: ";
            cin >> postID;
            cout << "Your User ID: ";
            cin >> userID;
            cin.ignore();
            cout << "Comment: ";
            getline(cin, content);
            if (postManager.addComment(postID, userID, content)) {
                postManager.saveToFile("data/posts.txt");
            }
            break;
        }
        case 4:
            return;
        default:
            cout << "Invalid choice!\n";
    }
}

void feedMenu() {
    int userID;
    cout << "Enter your User ID: ";
    cin >> userID;

    vector<int> followingList = getUserFollowingList(userID);
    feedManager.generateFeed(followingList, postManager);
    feedManager.displayFeed(20);
}