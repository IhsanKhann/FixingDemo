#include <iostream>
#include <string>
using namespace std;

enum ResourceType {
    PROFILE,
    POST
};

struct HistoryEntry {
    ResourceType type;
    string targetId;
    string username;
    string postTitle;
    
    HistoryEntry() : type(PROFILE), targetId(""), username(""), postTitle("") {}
    
    HistoryEntry(ResourceType t, string id, string user, string title = "")
        : type(t), targetId(id), username(user), postTitle(title) {}
    
    void display() {
        cout << "Type: " << (type == PROFILE ? "PROFILE" : "POST") << endl;
        cout << "ID: " << targetId << endl;
        if (type == PROFILE) {
            cout << "Username: " << username << endl;
        } else {
            cout << "Post Title: " << postTitle << endl;
            cout << "Author: " << username << endl;
        }
    }
};

class BrowsingHistory {
private:
    HistoryEntry stack[100];
    int top;
    int maxSize;
    
    bool isDuplicate(HistoryEntry entry) {
        if (top == -1) return false;
        return stack[top].targetId == entry.targetId && 
               stack[top].type == entry.type;
    }
    
public:
    BrowsingHistory() : top(-1), maxSize(100) {}
    
    void push(HistoryEntry entry) {
        if (isDuplicate(entry)) {
            cout << "[INFO] Same page, not adding to history." << endl;
            return;
        }
        
        if (top >= maxSize - 1) {
            cout << "[INFO] History full. Removing oldest." << endl;
            for (int i = 0; i < maxSize - 1; i++) {
                stack[i] = stack[i + 1];
            }
            top--;
        }
        
        stack[++top] = entry;
        cout << "[SUCCESS] Added to history. Total: " << (top + 1) << endl;
    }
    
    bool pop(HistoryEntry& entry) {
        if (isEmpty()) {
            cout << "[ERROR] No history to go back to." << endl;
            return false;
        }
        entry = stack[top--];
        cout << "[SUCCESS] Went back. Remaining: " << (top + 1) << endl;
        return true;
    }
    
    bool peek(HistoryEntry& entry) {
        if (isEmpty()) {
            cout << "[ERROR] History is empty." << endl;
            return false;
        }
        entry = stack[top];
        return true;
    }
    
    void displayAll() {
        if (isEmpty()) {
            cout << "\n[HISTORY] No browsing history." << endl;
            return;
        }
        
        cout << "\n========== BROWSING HISTORY ==========" << endl;
        cout << "Total Entries: " << (top + 1) << "\n" << endl;
        
        for (int i = top; i >= 0; i--) {
            cout << "[" << (top - i + 1) << "]" << endl;
            stack[i].display();
            cout << endl;
        }
        cout << "======================================" << endl;
    }
    
    void clear() {
        top = -1;
        cout << "[SUCCESS] History cleared." << endl;
    }
    
    bool isEmpty() { return top == -1; }
    int size() { return top + 1; }
};

void historyMenu() {
    BrowsingHistory history;
    
    cout << "========================================" << endl;
    cout << "    BROWSING HISTORY SYSTEM" << endl;
    cout << "========================================\n" << endl;
    
    history.push(HistoryEntry(PROFILE, "alice_123", "alice_wonderland"));
    history.push(HistoryEntry(POST, "post_001", "alice_wonderland", "My first post!"));
    history.push(HistoryEntry(PROFILE, "bob_456", "bob_builder"));
    history.push(HistoryEntry(POST, "post_002", "bob_builder", "Sunset photo"));
    
    int choice;
    while (true) {
        cout << "\n----- MENU -----" << endl;
        cout << "1. Visit Profile" << endl;
        cout << "2. Visit Post" << endl;
        cout << "3. Go Back" << endl;
        cout << "4. View History" << endl;
        cout << "5. Clear History" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();
        
        if (choice == 1) {
            string profileId, username;
            cout << "Enter profile ID: ";
            getline(cin, profileId);
            cout << "Enter username: ";
            getline(cin, username);
            
            history.push(HistoryEntry(PROFILE, profileId, username));
            
        } else if (choice == 2) {
            string postId, title, author;
            cout << "Enter post ID: ";
            getline(cin, postId);
            cout << "Enter post title: ";
            getline(cin, title);
            cout << "Enter author: ";
            getline(cin, author);
            
            history.push(HistoryEntry(POST, postId, author, title));
            
        } else if (choice == 3) {
            HistoryEntry entry;
            if (history.pop(entry)) {
                cout << "\nGoing back to:" << endl;
                HistoryEntry current;
                if (history.peek(current)) {
                    current.display();
                } else {
                    cout << "No more history. At home page." << endl;
                }
            }
            
        } else if (choice == 4) {
            history.displayAll();
            
        } else if (choice == 5) {
            history.clear();
            
        } else if (choice == 6) {
            cout << "\nGoodbye!" << endl;
            break;
            
        } else {
            cout << "Invalid choice!" << endl;
        }
    }
}
