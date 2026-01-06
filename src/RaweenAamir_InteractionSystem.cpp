#include <iostream>
#include "RaweenAamir_headerFile.h"
#include <string>
using namespace std;


// ---------------- Interaction System ----------------
class InteractionSystem {
public:
    // Like post (cannot like own post)
    bool likePost(Post& post, int userID) {
        if (post.userID == userID) return false;
        post.likes++;
        // notifyLike(post.userID);
        return true;
    }

    // Unlike post
    bool unlikePost(Post& post, int userID) {
        if (post.likes <= 0) return false;
        post.likes--;
        return true;
    }

    // Add comment (singly linked list)
    bool addComment(Post& post, int commentID, int userID,
                    const string& text, Timestamp ts) {
        if (text.empty()) return false;

        Comment* newComment = new Comment(commentID, userID, text, ts);

        if (!post.comments) {
            post.comments = newComment;
        } else {
            Comment* curr = post.comments;
            while (curr->next) curr = curr->next;
            curr->next = newComment;
        }

        // notifyComment(post.userID);
        return true;
    }

    // Display comments
    void displayComments(const Post& post) {
        Comment* curr = post.comments;
        while (curr) {
            cout << "User " << curr->userID << ": ";
            cout << curr->content << "\n";
            curr = curr->next;
        }
    }
};
