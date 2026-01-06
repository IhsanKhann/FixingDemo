<<<<<<< HEAD
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

// Enum for notification types (lower value = higher priority)
enum NotificationType {
    COMMENT = 1,
    LIKE = 2,
    FOLLOW = 3
};

// Structure for timestamp
struct Timestamp {
    int day;
    int month;
    int year;
    int hour;
    int minute;
    
    // Compare timestamps (returns true if this timestamp is earlier)
    bool isEarlier(Timestamp other) {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        if (day != other.day) return day < other.day;
        if (hour != other.hour) return hour < other.hour;
        return minute < other.minute;
    }
    
    void display() {
        cout << day << "/" << month << "/" << year << " " 
             << hour << ":" << (minute < 10 ? "0" : "") << minute;
    }
};

// Structure for Notification
struct Notification {
    NotificationType type;
    string message;
    Timestamp timestamp;
    bool isRead;
    
    Notification() : type(FOLLOW), message(""), isRead(false) {
        timestamp.day = 1;
        timestamp.month = 1;
        timestamp.year = 2024;
        timestamp.hour = 0;
        timestamp.minute = 0;
    }
    
    Notification(NotificationType t, string msg, Timestamp ts) 
        : type(t), message(msg), timestamp(ts), isRead(false) {}
    
    // Compare notifications: first by priority, then by timestamp
    bool hasHigherPriority(Notification other) {
        if (type != other.type) {
            return type < other.type; // Lower enum value = higher priority
        }
        // Same priority, earlier timestamp comes first
        return timestamp.isEarlier(other.timestamp);
    }
    
    void display() {
        string typeStr;
        switch(type) {
            case COMMENT: typeStr = "[COMMENT]"; break;
            case LIKE: typeStr = "[LIKE]"; break;
            case FOLLOW: typeStr = "[FOLLOW]"; break;
        }
        
        cout << typeStr << " " << message << " - ";
        timestamp.display();
        cout << (isRead ? " [READ]" : " [UNREAD]") << endl;
    }
};

// Min Heap Priority Queue for Notifications
class NotificationQueue {
private:
    Notification* heap;
    int capacity;
    int size;
    
    // Get parent index
    int parent(int i) { return (i - 1) / 2; }
    
    // Get left child index
    int leftChild(int i) { return 2 * i + 1; }
    
    // Get right child index
    int rightChild(int i) { return 2 * i + 2; }
    
    // Swap two notifications
    void swap(int i, int j) {
        Notification temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }
    
    // Move element up to maintain heap property
    void heapifyUp(int index) {
        while (index > 0 && heap[index].hasHigherPriority(heap[parent(index)])) {
            swap(index, parent(index));
            index = parent(index);
        }
    }
    
    // Move element down to maintain heap property
    void heapifyDown(int index) {
        int smallest = index;
        int left = leftChild(index);
        int right = rightChild(index);
        
        if (left < size && heap[left].hasHigherPriority(heap[smallest])) {
            smallest = left;
        }
        
        if (right < size && heap[right].hasHigherPriority(heap[smallest])) {
            smallest = right;
        }
        
        if (smallest != index) {
            swap(index, smallest);
            heapifyDown(smallest);
        }
    }
    
public:
    NotificationQueue(int cap = 100) : capacity(cap), size(0) {
        heap = new Notification[capacity];
    }
    
    ~NotificationQueue() {
        delete[] heap;
    }
    
    // Insert a notification into the priority queue
    void insert(NotificationType type, string message, Timestamp timestamp) {
        if (size >= capacity) {
            cout << "Notification queue is full!" << endl;
            return;
        }
        
        Notification newNotif(type, message, timestamp);
        heap[size] = newNotif;
        heapifyUp(size);
        size++;
        
        cout << "\n>> Notification added successfully!" << endl;
    }
    
    // Extract the highest priority notification (Min)
    Notification extractMin() {
        if (size == 0) {
            cout << "No notifications available!" << endl;
            return Notification();
        }
        
        Notification minNotif = heap[0];
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);
        
        return minNotif;
    }
    
    // Display all notifications (highest priority first)
    void displayNotifications() {
        if (size == 0) {
            cout << "\n[No notifications to display]" << endl;
            return;
        }
        
        cout << "\n?? YOUR NOTIFICATIONS (Highest Priority First):\n";
        cout << "================================================\n";
        
        // Create a temporary array to store notifications
        Notification* temp = new Notification[size];
        int tempSize = size;
        
        // Extract all notifications
        for (int i = 0; i < tempSize; i++) {
            temp[i] = extractMin();
        }
        
        // Display notifications
        for (int i = 0; i < tempSize; i++) {
            cout << (i + 1) << ". ";
            temp[i].display();
        }
        
        // Restore notifications back to the queue
        for (int i = 0; i < tempSize; i++) {
            heap[size] = temp[i];
            heapifyUp(size);
            size++;
        }
        
        delete[] temp;
        cout << "================================================\n";
    }
    
    // Mark the highest priority notification as read
    void markAsRead() {
        if (size == 0) {
            cout << "No notifications to mark as read!" << endl;
            return;
        }
        
        Notification notif = extractMin();
        notif.isRead = true;
        
        cout << "\n? Marked as read: ";
        notif.display();
    }
    
    // Clear all notifications
    void clearAll() {
        size = 0;
        cout << "\n? All notifications cleared!" << endl;
    }
    
    // Get count of notifications
    int getCount() {
        return size;
    }
    
    // Check if queue is empty
    bool isEmpty() {
        return size == 0;
    }
};

// Helper function to create current timestamp
Timestamp getCurrentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    Timestamp ts;
    ts.day = ltm->tm_mday;
    ts.month = 1 + ltm->tm_mon;
    ts.year = 1900 + ltm->tm_year;
    ts.hour = ltm->tm_hour;
    ts.minute = ltm->tm_min;
    
    return ts;
}

// Main function with menu
void notificationMenu() {
    NotificationQueue notifQueue;
    
    cout << "============================================\n";
    cout << "   SOCIAL MEDIA NOTIFICATION SYSTEM\n";
    cout << "     Priority Queue Implementation\n";
    cout << "============================================\n\n";
    
    // Sample notifications for demonstration
    Timestamp ts1 = {5, 1, 2026, 10, 30};
    Timestamp ts2 = {5, 1, 2026, 11, 15};
    Timestamp ts3 = {5, 1, 2026, 9, 45};
    Timestamp ts4 = {5, 1, 2026, 12, 0};
    Timestamp ts5 = {5, 1, 2026, 8, 20};
    
    notifQueue.insert(LIKE, "Ahmed liked your post 'Beautiful sunset'", ts2);
    notifQueue.insert(COMMENT, "Sara commented: 'Great photo!'", ts1);
    notifQueue.insert(FOLLOW, "Ali started following you", ts3);
    notifQueue.insert(COMMENT, "Hassan commented: 'Amazing!'", ts5);
    notifQueue.insert(LIKE, "Fatima liked your post", ts4);
    
    int choice;
    do {
        cout << "\n-----------------------------------------\n";
        cout << "         NOTIFICATION MENU\n";
        cout << "-----------------------------------------\n";
        cout << " 1. Add New Notification\n";
        cout << " 2. Display All Notifications\n";
        cout << " 3. Mark Top Notification as Read\n";
        cout << " 4. Clear All Notifications\n";
        cout << " 5. Show Notification Count\n";
        cout << " 6. Exit\n";
        cout << "-----------------------------------------\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Clear newline from buffer
        
        switch(choice) {
            case 1: {
                int typeChoice;
                string message;
                
                cout << "\nSelect notification type:\n";
                cout << "1. Comment (Highest Priority)\n";
                cout << "2. Like (Medium Priority)\n";
                cout << "3. Follow (Lowest Priority)\n";
                cout << "Enter choice: ";
                cin >> typeChoice;
                cin.ignore();
                
                NotificationType type;
                switch(typeChoice) {
                    case 1: type = COMMENT; break;
                    case 2: type = LIKE; break;
                    case 3: type = FOLLOW; break;
                    default: 
                        cout << "Invalid choice! Defaulting to FOLLOW.\n";
                        type = FOLLOW;
                }
                
                cout << "Enter notification message: ";
                getline(cin, message);
                
                Timestamp currentTime = getCurrentTime();
                notifQueue.insert(type, message, currentTime);
                break;
            }
            
            case 2:
                notifQueue.displayNotifications();
                break;
            
            case 3:
                notifQueue.markAsRead();
                break;
            
            case 4:
                notifQueue.clearAll();
                break;
            
            case 5:
                cout << "\n?? Total notifications: " << notifQueue.getCount() << endl;
                break;
            
            case 6:
                cout << "\n?? Thank you for using the Notification System!\n";
                break;
            
            default:
                cout << "\n? Invalid choice! Please try again.\n";
        }
        
    } while(choice != 6);
    
=======
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

// Enum for notification types (lower value = higher priority)
enum NotificationType {
    COMMENT = 1,
    LIKE = 2,
    FOLLOW = 3
};

// Structure for timestamp
struct Timestamp {
    int day;
    int month;
    int year;
    int hour;
    int minute;
    
    // Compare timestamps (returns true if this timestamp is earlier)
    bool isEarlier(Timestamp other) {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        if (day != other.day) return day < other.day;
        if (hour != other.hour) return hour < other.hour;
        return minute < other.minute;
    }
    
    void display() {
        cout << day << "/" << month << "/" << year << " " 
             << hour << ":" << (minute < 10 ? "0" : "") << minute;
    }
};

// Structure for Notification
struct Notification {
    NotificationType type;
    string message;
    Timestamp timestamp;
    bool isRead;
    
    Notification() : type(FOLLOW), message(""), isRead(false) {
        timestamp.day = 1;
        timestamp.month = 1;
        timestamp.year = 2024;
        timestamp.hour = 0;
        timestamp.minute = 0;
    }
    
    Notification(NotificationType t, string msg, Timestamp ts) 
        : type(t), message(msg), timestamp(ts), isRead(false) {}
    
    // Compare notifications: first by priority, then by timestamp
    bool hasHigherPriority(Notification other) {
        if (type != other.type) {
            return type < other.type; // Lower enum value = higher priority
        }
        // Same priority, earlier timestamp comes first
        return timestamp.isEarlier(other.timestamp);
    }
    
    void display() {
        string typeStr;
        switch(type) {
            case COMMENT: typeStr = "[COMMENT]"; break;
            case LIKE: typeStr = "[LIKE]"; break;
            case FOLLOW: typeStr = "[FOLLOW]"; break;
        }
        
        cout << typeStr << " " << message << " - ";
        timestamp.display();
        cout << (isRead ? " [READ]" : " [UNREAD]") << endl;
    }
};

// Min Heap Priority Queue for Notifications
class NotificationQueue {
private:
    Notification* heap;
    int capacity;
    int size;
    
    // Get parent index
    int parent(int i) { return (i - 1) / 2; }
    
    // Get left child index
    int leftChild(int i) { return 2 * i + 1; }
    
    // Get right child index
    int rightChild(int i) { return 2 * i + 2; }
    
    // Swap two notifications
    void swap(int i, int j) {
        Notification temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }
    
    // Move element up to maintain heap property
    void heapifyUp(int index) {
        while (index > 0 && heap[index].hasHigherPriority(heap[parent(index)])) {
            swap(index, parent(index));
            index = parent(index);
        }
    }
    
    // Move element down to maintain heap property
    void heapifyDown(int index) {
        int smallest = index;
        int left = leftChild(index);
        int right = rightChild(index);
        
        if (left < size && heap[left].hasHigherPriority(heap[smallest])) {
            smallest = left;
        }
        
        if (right < size && heap[right].hasHigherPriority(heap[smallest])) {
            smallest = right;
        }
        
        if (smallest != index) {
            swap(index, smallest);
            heapifyDown(smallest);
        }
    }
    
public:
    NotificationQueue(int cap = 100) : capacity(cap), size(0) {
        heap = new Notification[capacity];
    }
    
    ~NotificationQueue() {
        delete[] heap;
    }
    
    // Insert a notification into the priority queue
    void insert(NotificationType type, string message, Timestamp timestamp) {
        if (size >= capacity) {
            cout << "Notification queue is full!" << endl;
            return;
        }
        
        Notification newNotif(type, message, timestamp);
        heap[size] = newNotif;
        heapifyUp(size);
        size++;
        
        cout << "\n>> Notification added successfully!" << endl;
    }
    
    // Extract the highest priority notification (Min)
    Notification extractMin() {
        if (size == 0) {
            cout << "No notifications available!" << endl;
            return Notification();
        }
        
        Notification minNotif = heap[0];
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);
        
        return minNotif;
    }
    
    // Display all notifications (highest priority first)
    void displayNotifications() {
        if (size == 0) {
            cout << "\n[No notifications to display]" << endl;
            return;
        }
        
        cout << "\n?? YOUR NOTIFICATIONS (Highest Priority First):\n";
        cout << "================================================\n";
        
        // Create a temporary array to store notifications
        Notification* temp = new Notification[size];
        int tempSize = size;
        
        // Extract all notifications
        for (int i = 0; i < tempSize; i++) {
            temp[i] = extractMin();
        }
        
        // Display notifications
        for (int i = 0; i < tempSize; i++) {
            cout << (i + 1) << ". ";
            temp[i].display();
        }
        
        // Restore notifications back to the queue
        for (int i = 0; i < tempSize; i++) {
            heap[size] = temp[i];
            heapifyUp(size);
            size++;
        }
        
        delete[] temp;
        cout << "================================================\n";
    }
    
    // Mark the highest priority notification as read
    void markAsRead() {
        if (size == 0) {
            cout << "No notifications to mark as read!" << endl;
            return;
        }
        
        Notification notif = extractMin();
        notif.isRead = true;
        
        cout << "\n? Marked as read: ";
        notif.display();
    }
    
    // Clear all notifications
    void clearAll() {
        size = 0;
        cout << "\n? All notifications cleared!" << endl;
    }
    
    // Get count of notifications
    int getCount() {
        return size;
    }
    
    // Check if queue is empty
    bool isEmpty() {
        return size == 0;
    }
};

// Helper function to create current timestamp
Timestamp getCurrentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    Timestamp ts;
    ts.day = ltm->tm_mday;
    ts.month = 1 + ltm->tm_mon;
    ts.year = 1900 + ltm->tm_year;
    ts.hour = ltm->tm_hour;
    ts.minute = ltm->tm_min;
    
    return ts;
}

// Main function with menu
int main() {
    NotificationQueue notifQueue;
    
    cout << "============================================\n";
    cout << "   SOCIAL MEDIA NOTIFICATION SYSTEM\n";
    cout << "     Priority Queue Implementation\n";
    cout << "============================================\n\n";
    
    // Sample notifications for demonstration
    Timestamp ts1 = {5, 1, 2026, 10, 30};
    Timestamp ts2 = {5, 1, 2026, 11, 15};
    Timestamp ts3 = {5, 1, 2026, 9, 45};
    Timestamp ts4 = {5, 1, 2026, 12, 0};
    Timestamp ts5 = {5, 1, 2026, 8, 20};
    
    notifQueue.insert(LIKE, "Ahmed liked your post 'Beautiful sunset'", ts2);
    notifQueue.insert(COMMENT, "Sara commented: 'Great photo!'", ts1);
    notifQueue.insert(FOLLOW, "Ali started following you", ts3);
    notifQueue.insert(COMMENT, "Hassan commented: 'Amazing!'", ts5);
    notifQueue.insert(LIKE, "Fatima liked your post", ts4);
    
    int choice;
    do {
        cout << "\n-----------------------------------------\n";
        cout << "         NOTIFICATION MENU\n";
        cout << "-----------------------------------------\n";
        cout << " 1. Add New Notification\n";
        cout << " 2. Display All Notifications\n";
        cout << " 3. Mark Top Notification as Read\n";
        cout << " 4. Clear All Notifications\n";
        cout << " 5. Show Notification Count\n";
        cout << " 6. Exit\n";
        cout << "-----------------------------------------\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Clear newline from buffer
        
        switch(choice) {
            case 1: {
                int typeChoice;
                string message;
                
                cout << "\nSelect notification type:\n";
                cout << "1. Comment (Highest Priority)\n";
                cout << "2. Like (Medium Priority)\n";
                cout << "3. Follow (Lowest Priority)\n";
                cout << "Enter choice: ";
                cin >> typeChoice;
                cin.ignore();
                
                NotificationType type;
                switch(typeChoice) {
                    case 1: type = COMMENT; break;
                    case 2: type = LIKE; break;
                    case 3: type = FOLLOW; break;
                    default: 
                        cout << "Invalid choice! Defaulting to FOLLOW.\n";
                        type = FOLLOW;
                }
                
                cout << "Enter notification message: ";
                getline(cin, message);
                
                Timestamp currentTime = getCurrentTime();
                notifQueue.insert(type, message, currentTime);
                break;
            }
            
            case 2:
                notifQueue.displayNotifications();
                break;
            
            case 3:
                notifQueue.markAsRead();
                break;
            
            case 4:
                notifQueue.clearAll();
                break;
            
            case 5:
                cout << "\n?? Total notifications: " << notifQueue.getCount() << endl;
                break;
            
            case 6:
                cout << "\n?? Thank you for using the Notification System!\n";
                break;
            
            default:
                cout << "\n? Invalid choice! Please try again.\n";
        }
        
    } while(choice != 6);
    
    return 0;
>>>>>>> 49c375b172a9182ca3aba706e8fc45549a772c81
}