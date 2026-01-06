#include <iostream>
#include <string>
#include <ctime>
using namespace std;

enum NotificationType {
    COMMENT = 1,
    LIKE = 2,
    FOLLOW = 3
};

struct Timestamp {
    int day;
    int month;
    int year;
    int hour;
    int minute;
    
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
    
    bool hasHigherPriority(Notification other) {
        if (type != other.type) {
            return type < other.type;
        }
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

class NotificationQueue {
private:
    Notification* heap;
    int capacity;
    int size;
    
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }
    
    void swap(int i, int j) {
        Notification temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }
    
    void heapifyUp(int index) {
        while (index > 0 && heap[index].hasHigherPriority(heap[parent(index)])) {
            swap(index, parent(index));
            index = parent(index);
        }
    }
    
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
    
    void displayNotifications() {
        if (size == 0) {
            cout << "\n[No notifications to display]" << endl;
            return;
        }
        
        cout << "\nYOUR NOTIFICATIONS (Highest Priority First):\n";
        cout << "================================================\n";
        
        Notification* temp = new Notification[size];
        int tempSize = size;
        
        for (int i = 0; i < tempSize; i++) {
            temp[i] = extractMin();
        }
        
        for (int i = 0; i < tempSize; i++) {
            cout << (i + 1) << ". ";
            temp[i].display();
        }
        
        for (int i = 0; i < tempSize; i++) {
            heap[size] = temp[i];
            heapifyUp(size);
            size++;
        }
        
        delete[] temp;
        cout << "================================================\n";
    }
    
    void markAsRead() {
        if (size == 0) {
            cout << "No notifications to mark as read!" << endl;
            return;
        }
        
        Notification notif = extractMin();
        notif.isRead = true;
        
        cout << "\nMarked as read: ";
        notif.display();
    }
    
    void clearAll() {
        size = 0;
        cout << "\nAll notifications cleared!" << endl;
    }
    
    int getCount() {
        return size;
    }
    
    bool isEmpty() {
        return size == 0;
    }
};

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

void notificationMenu() {
    NotificationQueue notifQueue;
    
    cout << "============================================\n";
    cout << "   SOCIAL MEDIA NOTIFICATION SYSTEM\n";
    cout << "     Priority Queue Implementation\n";
    cout << "============================================\n\n";
    
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
        cin.ignore();
        
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
                cout << "\nTotal notifications: " << notifQueue.getCount() << endl;
                break;
            
            case 6:
                cout << "\nThank you for using the Notification System!\n";
                break;
            
            default:
                cout << "\nInvalid choice! Please try again.\n";
        }
        
    } while(choice != 6);
}
