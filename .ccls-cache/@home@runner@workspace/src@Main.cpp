#include <iostream>
#include "MasterHeader.h"
#include "PostManager.h"
#include "FeedManager.h"

using namespace std;

// Global instances
PostManager postManager;
FeedManager feedManager;

void displayMainMenu() {
    cout << "\n=======================================\n";
    cout << "    SOCIAL MEDIA FEED MANAGER\n";
    cout << "=======================================\n";
    cout << "1. User Management\n";
    cout << "2. Post Management\n";
    cout << "3. Feed Generation\n";
    cout << "4. Interactions (Like/Comment)\n";
    cout << "5. Notifications\n";
    cout << "6. Browsing History\n";
    cout << "7. Exit\n";
    cout << "=======================================\n";
    cout << "Choice: ";
}

int main() {
    // Load data
    postManager.loadFromFile("data/posts.txt");
    cout << "System initialized successfully!\n";

    int choice;
    while (true) {
        displayMainMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                userMenu();
                break;
            case 2:
                postMenu();
                break;
            case 3:
                feedMenu();
                break;
            case 4:
                interactionMenu();
                break;
            case 5:
                notificationMenu();
                break;
            case 6:
                historyMenu();
                break;
            case 7:
                cout << "Saving data...\n";
                postManager.saveToFile("data/posts.txt");
                cout << "Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice!\n";
        }
    }

    return 0;
}