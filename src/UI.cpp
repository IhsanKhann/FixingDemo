#include "App.h"

// ==================== UI CLASS ====================
UI::UI(UserDatabase* users, PostDatabase* posts, NotificationQueue* notifs, History* hist)
    : currentScreen(LOGIN_SCREEN), currentUser(nullptr), viewingUser(nullptr),
      viewingPost(nullptr), userDatabase(users), postDatabase(posts),
      notifications(notifs), history(hist), searchResultCount(0), showError(false) {
    
    memset(usernameInput, 0, sizeof(usernameInput));
    memset(passwordInput, 0, sizeof(passwordInput));
    memset(bioInput, 0, sizeof(bioInput));
    memset(postInput, 0, sizeof(postInput));
    memset(commentInput, 0, sizeof(commentInput));
    memset(searchInput, 0, sizeof(searchInput));
    memset(errorMessage, 0, sizeof(errorMessage));
    
    feed = new Feed();
}

UI::~UI() {
    delete feed;
}

Timestamp UI::getCurrentTime() {
    return getCurrentTimestamp();
}

string UI::timestampToString(const Timestamp& ts) {
    return ::timestampToString(ts);
}

void UI::showErrorMessage(const char* msg) {
    strncpy(errorMessage, msg, 255);
    errorMessage[255] = '\0';
    showError = true;
}

void UI::logout() {
    currentUser = nullptr;
    currentScreen = LOGIN_SCREEN;
    memset(usernameInput, 0, sizeof(usernameInput));
    memset(passwordInput, 0, sizeof(passwordInput));
}

void UI::setScreen(Screen screen) {
    currentScreen = screen;
}

void UI::render() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    
    ImGui::Begin("Social Media App", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    
    // Error popup
    if (showError) {
        ImGui::OpenPopup("Error");
        if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", errorMessage);
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                showError = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    
    // Render current screen
    switch (currentScreen) {
        case LOGIN_SCREEN: renderLoginScreen(); break;
        case FEED_SCREEN: renderFeedScreen(); break;
        case PROFILE_SCREEN: renderProfileScreen(); break;
        case NOTIFICATIONS_SCREEN: renderNotificationsScreen(); break;
        case SEARCH_SCREEN: renderSearchScreen(); break;
        case CREATE_POST_SCREEN: renderCreatePostScreen(); break;
        case POST_DETAIL_SCREEN: renderPostDetailScreen(); break;
    }
    
    ImGui::End();
}

void UI::renderLoginScreen() {
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() / 2 - 150, 150));
    ImGui::BeginGroup();
    
    ImGui::Text("Social Media Login");
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Text("Username:");
    ImGui::InputText("##username", usernameInput, 64);
    
    ImGui::Text("Password:");
    ImGui::InputText("##password", passwordInput, 64, ImGuiInputTextFlags_Password);
    
    ImGui::Spacing();
    
    if (ImGui::Button("Login", ImVec2(145, 30))) {
        if (strlen(usernameInput) == 0 || strlen(passwordInput) == 0) {
            showErrorMessage("Please enter username and password");
        } else {
            User* user = userDatabase->login(usernameInput, passwordInput);
            if (user) {
                currentUser = user;
                feed->generateFeed(currentUser, postDatabase);
                setScreen(FEED_SCREEN);
                memset(usernameInput, 0, sizeof(usernameInput));
                memset(passwordInput, 0, sizeof(passwordInput));
            } else {
                showErrorMessage("Invalid username or password");
            }
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Register", ImVec2(145, 30))) {
        if (strlen(usernameInput) < 3 || strlen(usernameInput) > 20) {
            showErrorMessage("Username must be 3-20 characters");
        } else if (strlen(passwordInput) < 6) {
            showErrorMessage("Password must be at least 6 characters");
        } else {
            User* user = userDatabase->registerUser(usernameInput, passwordInput);
            if (user) {
                currentUser = user;
                setScreen(FEED_SCREEN);
                memset(usernameInput, 0, sizeof(usernameInput));
                memset(passwordInput, 0, sizeof(passwordInput));
            } else {
                showErrorMessage("Username already exists or invalid");
            }
        }
    }
    
    ImGui::EndGroup();
}

void UI::renderFeedScreen() {
    // Top bar
    ImGui::Text("Home Feed - Welcome, %s!", currentUser->username.c_str());
    ImGui::SameLine(ImGui::GetWindowWidth() - 250);
    
    if (ImGui::Button("Create Post")) setScreen(CREATE_POST_SCREEN);
    ImGui::SameLine();
    if (ImGui::Button("Refresh")) feed->generateFeed(currentUser, postDatabase);
    
    ImGui::Separator();
    
    // Navigation buttons
    if (ImGui::Button("Feed")) setScreen(FEED_SCREEN);
    ImGui::SameLine();
    if (ImGui::Button("Profile")) {
        viewingUser = currentUser;
        setScreen(PROFILE_SCREEN);
    }
    ImGui::SameLine();
    if (ImGui::Button("Notifications")) setScreen(NOTIFICATIONS_SCREEN);
    ImGui::SameLine();
    if (ImGui::Button("Search")) setScreen(SEARCH_SCREEN);
    ImGui::SameLine();
    if (ImGui::Button("Logout")) logout();
    
    ImGui::Separator();
    
    // Feed display
    ImGui::BeginChild("FeedScroll", ImVec2(0, 0), true);
    
    FeedNode* node = feed->getHead();
    if (!node) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                          "No posts to show. Follow users to see their posts!");
    }
    
    while (node) {
        Post* post = node->post;
        
        ImGui::PushID(post->postID);
        ImGui::BeginGroup();
        
        // Username button
        if (ImGui::Button(post->username.c_str())) {
            viewingUser = userDatabase->searchByID(post->userID);
            setScreen(PROFILE_SCREEN);
        }
        
        // Post content
        ImGui::TextWrapped("%s", post->content.c_str());
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
                          "%s", timestampToString(post->timestamp).c_str());
        
        ImGui::Text("Likes: %d | Comments: %d", post->likes, post->commentCount);
        
        // Like button
        if (post->userID != currentUser->userID) {
            if (ImGui::SmallButton("Like")) {
                post->addLike();
                notifications->addNotification(LIKE, currentUser->userID,
                                              currentUser->username, post->postID,
                                              currentUser->username + " liked your post",
                                              getCurrentTime());
            }
            ImGui::SameLine();
        }
        
        // View details button
        if (ImGui::SmallButton("View Details")) {
            viewingPost = post;
            setScreen(POST_DETAIL_SCREEN);
        }
        
        ImGui::EndGroup();
        ImGui::Separator();
        ImGui::PopID();
        
        node = node->next;
    }
    
    ImGui::EndChild();
}

void UI::renderProfileScreen() {
    if (!viewingUser) {
        setScreen(FEED_SCREEN);
        return;
    }
    
    ImGui::Text("Profile: %s", viewingUser->username.c_str());
    
    // Navigation
    if (ImGui::Button("Back to Feed")) setScreen(FEED_SCREEN);
    ImGui::SameLine();
    if (ImGui::Button("Search")) setScreen(SEARCH_SCREEN);
    ImGui::SameLine();
    if (ImGui::Button("Logout")) logout();
    
    ImGui::Separator();
    
    // Profile info
    ImGui::Text("Username: %s", viewingUser->username.c_str());
    ImGui::Text("Bio: %s", viewingUser->bio.empty() ? "No bio" : viewingUser->bio.c_str());
    ImGui::Text("Followers: %d | Following: %d",
                viewingUser->followerCount, viewingUser->followingCount);
    
    ImGui::Spacing();
    
    // Follow/Unfollow button
    if (viewingUser->userID != currentUser->userID) {
        if (currentUser->isFollowing(viewingUser->userID)) {
            if (ImGui::Button("Unfollow")) {
                currentUser->removeFollowing(viewingUser->userID);
                viewingUser->removeFollower(currentUser->userID);
                feed->generateFeed(currentUser, postDatabase);
            }
        } else {
            if (ImGui::Button("Follow")) {
                currentUser->addFollowing(viewingUser->userID);
                viewingUser->addFollower(currentUser->userID);
                notifications->addNotification(FOLLOW, currentUser->userID,
                                              currentUser->username, 0,
                                              currentUser->username + " followed you",
                                              getCurrentTime());
                feed->generateFeed(currentUser, postDatabase);
            }
        }
    } else {
        // Edit bio for own profile
        ImGui::Text("Edit Bio:");
        ImGui::InputTextMultiline("##bio", bioInput, 256, ImVec2(-1, 60));
        if (ImGui::Button("Update Bio")) {
            viewingUser->bio = string(bioInput);
            memset(bioInput, 0, sizeof(bioInput));
            showErrorMessage("Bio updated successfully!");
        }
    }
    
    ImGui::Separator();
    
    // User's posts
    ImGui::Text("Posts:");
    ImGui::BeginChild("ProfilePosts", ImVec2(0, 0), true);
    
    Post* post = postDatabase->getHead();
    bool foundPosts = false;
    
    while (post) {
        if (post->userID == viewingUser->userID) {
            foundPosts = true;
            ImGui::PushID(post->postID);
            
            ImGui::TextWrapped("%s", post->content.c_str());
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
                              "%s", timestampToString(post->timestamp).c_str());
            ImGui::Text("Likes: %d | Comments: %d", post->likes, post->commentCount);
            
            if (ImGui::SmallButton("View Details")) {
                viewingPost = post;
                setScreen(POST_DETAIL_SCREEN);
            }
            
            if (post->userID == currentUser->userID) {
                ImGui::SameLine();
                if (ImGui::SmallButton("Delete")) {
                    postDatabase->deletePost(post->postID);
                    feed->generateFeed(currentUser, postDatabase);
                }
            }
            
            ImGui::Separator();
            ImGui::PopID();
        }
        post = post->next;
    }
    
    if (!foundPosts) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No posts yet");
    }
    
    ImGui::EndChild();
}

void UI::renderNotificationsScreen() {
    ImGui::Text("Notifications");
    
    if (ImGui::Button("Back to Feed")) setScreen(FEED_SCREEN);
    ImGui::SameLine();
    if (ImGui::Button("Clear All")) notifications->clearAll();
    
    ImGui::Separator();
    
    ImGui::BeginChild("NotificationScroll", ImVec2(0, 0), true);
    
    Notification* notifArr[200];
    int count = 0;
    notifications->getAllNotifications(notifArr, count);
    
    if (count == 0) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No notifications");
    }
    
    for (int i = 0; i < count; i++) {
        Notification* n = notifArr[i];
        
        // Don't show self-notifications
        if (n->fromUserID == currentUser->userID) continue;
        
        ImGui::PushID(n->notificationID);
        
        const char* typeStr = "";
        ImVec4 color;
        
        switch (n->type) {
            case COMMENT:
                typeStr = "[COMMENT]";
                color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
                break;
            case LIKE:
                typeStr = "[LIKE]";
                color = ImVec4(1.0f, 0.7f, 0.0f, 1.0f);
                break;
            case FOLLOW:
                typeStr = "[FOLLOW]";
                color = ImVec4(0.0f, 0.7f, 1.0f, 1.0f);
                break;
        }
        
        ImGui::TextColored(color, "%s", typeStr);
        ImGui::SameLine();
        ImGui::Text("%s", n->message.c_str());
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
                          "%s", timestampToString(n->timestamp).c_str());
        
        if (!n->isRead && ImGui::SmallButton("Mark Read")) {
            n->isRead = true;
        }
        
        ImGui::Separator();
        ImGui::PopID();
    }
    
    ImGui::EndChild();
}

void UI::renderSearchScreen() {
    ImGui::Text("Search Users");
    
    if (ImGui::Button("Back to Feed")) setScreen(FEED_SCREEN);
    
    ImGui::Separator();
    
    ImGui::Text("Search:");
    ImGui::InputText("##search", searchInput, 64);
    ImGui::SameLine();
    
    if (ImGui::Button("Search")) {
        searchResultCount = 0;
        if (strlen(searchInput) > 0) {
            User* allUsers[100];
            int userCount = 0;
            userDatabase->getAllUsers(allUsers, userCount);
            
            string searchTerm = searchInput;
            transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), ::tolower);
            
            for (int i = 0; i < userCount; i++) {
                string username = allUsers[i]->username;
                transform(username.begin(), username.end(), username.begin(), ::tolower);
                
                if (username.find(searchTerm) != string::npos) {
                    searchResults[searchResultCount++] = allUsers[i];
                }
            }
        }
    }
    
    ImGui::Separator();
    ImGui::BeginChild("SearchResults", ImVec2(0, 0), true);
    
    if (searchResultCount == 0) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                          "No results. Try searching for a username.");
    }
    
    for (int i = 0; i < searchResultCount; i++) {
        ImGui::PushID(i);
        
        if (ImGui::Button(searchResults[i]->username.c_str())) {
            viewingUser = searchResults[i];
            setScreen(PROFILE_SCREEN);
        }
        
        ImGui::SameLine();
        ImGui::Text("- Followers: %d", searchResults[i]->followerCount);
        
        ImGui::PopID();
    }
    
    ImGui::EndChild();
}

void UI::renderCreatePostScreen() {
    ImGui::Text("Create New Post");
    
    if (ImGui::Button("Cancel")) {
        setScreen(FEED_SCREEN);
        memset(postInput, 0, sizeof(postInput));
    }
    
    ImGui::Separator();
    
    ImGui::Text("What's on your mind?");
    ImGui::InputTextMultiline("##post", postInput, 512, ImVec2(-1, 200));
    
    int len = strlen(postInput);
    ImGui::Text("Characters: %d / 280", len);
    
    if (len > 280) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                          "Post is too long!");
    }
    
    ImGui::Spacing();
    
    if (ImGui::Button("Post", ImVec2(120, 30))) {
        if (len == 0) {
            showErrorMessage("Post cannot be empty");
        } else if (len > 280) {
            showErrorMessage("Post is too long (max 280 characters)");
        } else {
            postDatabase->createPost(currentUser->userID, currentUser->username,
                                    postInput, getCurrentTime());
            feed->generateFeed(currentUser, postDatabase);
            memset(postInput, 0, sizeof(postInput));
            setScreen(FEED_SCREEN);
        }
    }
}

void UI::renderPostDetailScreen() {
    if (!viewingPost) {
        setScreen(FEED_SCREEN);
        return;
    }
    
    ImGui::Text("Post Details");
    
    if (ImGui::Button("Back")) setScreen(FEED_SCREEN);
    
    ImGui::Separator();
    
    // Post content
    ImGui::BeginChild("PostDetail", ImVec2(0, 250), true);
    
    if (ImGui::Button(viewingPost->username.c_str())) {
        viewingUser = userDatabase->searchByID(viewingPost->userID);
        setScreen(PROFILE_SCREEN);
    }
    
    ImGui::TextWrapped("%s", viewingPost->content.c_str());
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
                      "%s", timestampToString(viewingPost->timestamp).c_str());
    ImGui::Text("Likes: %d | Comments: %d", viewingPost->likes, viewingPost->commentCount);
    
    if (viewingPost->userID != currentUser->userID) {
        if (ImGui::Button("Like")) {
            viewingPost->addLike();
            notifications->addNotification(LIKE, currentUser->userID,
                                          currentUser->username, viewingPost->postID,
                                          currentUser->username + " liked your post",
                                          getCurrentTime());
        }
    }
    
    ImGui::EndChild();
    
    // Comments section
    ImGui::Text("Comments:");
    ImGui::BeginChild("Comments", ImVec2(0, 200), true);
    
    Comment* comment = viewingPost->getComments();
    if (!comment) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No comments yet");
    }
    
    while (comment) {
        ImGui::PushID(comment->commentID);
        
        ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f),
                          "%s:", comment->username.c_str());
        ImGui::SameLine();
        ImGui::TextWrapped("%s", comment->content.c_str());
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                          "%s", timestampToString(comment->timestamp).c_str());
        ImGui::Separator();
        
        ImGui::PopID();
        comment = comment->next;
    }
    
    ImGui::EndChild();
    
    // Add comment
    if (viewingPost->userID != currentUser->userID) {
        ImGui::Text("Add Comment:");
        ImGui::InputTextMultiline("##comment", commentInput, 512, ImVec2(-1, 60));
        
        if (ImGui::Button("Post Comment")) {
            if (strlen(commentInput) > 0) {
                viewingPost->addComment(currentUser->userID, currentUser->username,
                                       commentInput, getCurrentTime());
                notifications->addNotification(COMMENT, currentUser->userID,
                                              currentUser->username, viewingPost->postID,
                                              currentUser->username + " commented on your post",
                                              getCurrentTime());
                memset(commentInput, 0, sizeof(commentInput));
            }
        }
    }
}