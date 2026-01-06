#ifndef POST_MANAGER_H
#define POST_MANAGER_H

#include "Post.h"
#include <string>

using namespace std;

class PostManager {
private:
    Post* head;
    Post* tail;
    int nextPostID;
    int nextCommentID;

public:
    PostManager();
    ~PostManager();

    // Post operations
    int createPost(int userID, const string& content);
    bool editPost(int postID, int userID, const string& newContent);
    bool deletePost(int postID, int userID);
    void displayUserPosts(int userID);
    void displayPost(Post* post);
    Post* findPost(int postID);

    // Interaction operations
    bool likePost(int postID, int userID);
    bool unlikePost(int postID, int userID);
    bool addComment(int postID, int userID, const string& content);
    void displayComments(Post* post);

    // File operations
    void saveToFile(const string& filename);
    void loadFromFile(const string& filename);

    // Feed generation helper
    Post* getHead() { return head; }
};

#endif