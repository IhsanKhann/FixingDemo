#include "App.h"

// ==================== NOTIFICATION CLASS ====================
Notification::Notification()
    : notificationID(0), type(FOLLOW), fromUserID(0), fromUsername(""),
      postID(0), message(""), isRead(false) {}

Notification::Notification(int id, NotificationType t, int fromID, const string& fromUser,
                          int pID, const string& msg, Timestamp ts)
    : notificationID(id), type(t), fromUserID(fromID), fromUsername(fromUser),
      postID(pID), message(msg), timestamp(ts), isRead(false) {}

bool Notification::hasHigherPriority(const Notification& other) const {
    if (type != other.type) {
        return type < other.type;
    }
    return timestamp.isEarlier(other.timestamp);
}

// ==================== NOTIFICATION QUEUE CLASS ====================
NotificationQueue::NotificationQueue(int cap)
    : capacity(cap), size(0), nextID(1) {
    heap = new Notification*[capacity];
}

NotificationQueue::~NotificationQueue() {
    for (int i = 0; i < size; i++) {
        delete heap[i];
    }
    delete[] heap;
}

void NotificationQueue::swap(int i, int j) {
    Notification* temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

void NotificationQueue::heapifyUp(int index) {
    while (index > 0 && heap[index]->hasHigherPriority(*heap[parent(index)])) {
        swap(index, parent(index));
        index = parent(index);
    }
}

void NotificationQueue::heapifyDown(int index) {
    int smallest = index;
    int left = leftChild(index);
    int right = rightChild(index);
    
    if (left < size && heap[left]->hasHigherPriority(*heap[smallest])) {
        smallest = left;
    }
    
    if (right < size && heap[right]->hasHigherPriority(*heap[smallest])) {
        smallest = right;
    }
    
    if (smallest != index) {
        swap(index, smallest);
        heapifyDown(smallest);
    }
}

void NotificationQueue::addNotification(NotificationType type, int fromUserID,
                                       const string& fromUsername, int postID,
                                       const string& message, Timestamp timestamp) {
    if (size >= capacity) {
        return; // Queue full
    }
    
    Notification* newNotif = new Notification(nextID++, type, fromUserID,
                                             fromUsername, postID, message, timestamp);
    heap[size] = newNotif;
    heapifyUp(size);
    size++;
}

void NotificationQueue::getAllNotifications(Notification** arr, int& count) {
    count = size;
    
    // Create temporary copy to preserve heap
    Notification** temp = new Notification*[size];
    int tempSize = size;
    
    // Extract all
    for (int i = 0; i < tempSize; i++) {
        temp[i] = heap[0];
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);
    }
    
    // Copy to output array
    for (int i = 0; i < tempSize; i++) {
        arr[i] = temp[i];
    }
    
    // Restore heap
    for (int i = 0; i < tempSize; i++) {
        heap[size] = temp[i];
        heapifyUp(size);
        size++;
    }
    
    delete[] temp;
}

void NotificationQueue::clearAll() {
    for (int i = 0; i < size; i++) {
        delete heap[i];
    }
    size = 0;
}