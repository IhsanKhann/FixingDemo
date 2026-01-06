#include <iostream>
#include "MasterHeader.h" // This links the other files

int main() {
    int choice;
    std::cout << "--- SOCIAL MEDIA SYSTEM ---\n";
    std::cout << "1. User Management\n2. Notifications\n3. History\n4. Exit\n";
    std::cout << "Enter choice: ";
    std::cin >> choice;

    if (choice == 1) userMenu();
    else if (choice == 2) notificationMenu();
    else if (choice == 3) historyMenu();
    
    return 0;
}