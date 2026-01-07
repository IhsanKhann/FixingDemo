#include "App.h"

// ==================== HISTORY CLASS ====================
History::History() : top(nullptr), count(0) {}

History::~History() {
    clear();
}

void History::push(int userID, int postID) {
    HistoryNode* newNode = new HistoryNode(userID, postID);
    newNode->next = top;
    top = newNode;
    count++;
    
    // Maintain max history limit
    if (count > MAX_HISTORY) {
        // Traverse to second-to-last node
        HistoryNode* current = top;
        for (int i = 1; i < MAX_HISTORY; i++) {
            if (!current->next) break;
            current = current->next;
        }
        
        // Delete nodes beyond limit
        HistoryNode* temp = current->next;
        current->next = nullptr;
        
        while (temp) {
            HistoryNode* next = temp->next;
            delete temp;
            temp = next;
        }
        
        count = MAX_HISTORY;
    }
}

bool History::pop(int& userID, int& postID) {
    if (!top) return false;
    
    HistoryNode* temp = top;
    userID = temp->userID;
    postID = temp->postID;
    top = top->next;
    delete temp;
    count--;
    
    return true;
}

bool History::peek(int& userID, int& postID) {
    if (!top) return false;
    
    userID = top->userID;
    postID = top->postID;
    return true;
}

void History::clear() {
    while (top) {
        HistoryNode* temp = top;
        top = top->next;
        delete temp;
    }
    count = 0;
}