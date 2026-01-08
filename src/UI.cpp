#include "App.h"

// ==================== UI CLASS ====================
UI::UI(UserDatabase* users, PostDatabase* posts, NotificationQueue* notifs, History* hist)
    : currentScreen(LOGIN_SCREEN),
      currentUser(nullptr),
      viewingUser(nullptr),
      viewingPost(nullptr),
      userDatabase(users),
      postDatabase(posts),
      notifications(notifs),
      history(hist),
      showError(false),
      searchResultCount(0) {

    // Initialize input buffers
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

std::string UI::timestampToString(const Timestamp& ts) {
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

// Modern gradient button
bool GradientButton(const char* label, const ImVec2& size) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.3f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.4f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.2f, 0.7f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    bool result = ImGui::Button(label, size);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    return result;
}

void UI::render() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    
    // Dark theme background
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.12f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    
    ImGui::Begin("Social Media App", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoScrollbar);
    
    // Gradient header
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 headerStart = ImGui::GetCursorScreenPos();
    ImVec2 headerEnd = ImVec2(headerStart.x + ImGui::GetWindowWidth(), headerStart.y + 60);
    drawList->AddRectFilledMultiColor(
        headerStart, headerEnd,
        IM_COL32(100, 80, 200, 255),   // Top left - purple
        IM_COL32(200, 100, 150, 255),  // Top right - pink
        IM_COL32(255, 150, 100, 255),  // Bottom right - orange
        IM_COL32(150, 100, 200, 255)   // Bottom left - purple-pink
    );
    
    // App title in header
    ImGui::SetCursorPos(ImVec2(20, 15));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text("Social Media App");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();
    
    // Error popup
    if (showError) {
        ImGui::OpenPopup("Error");
        ImGui::SetNextWindowSize(ImVec2(300, 120));
        if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_NoResize)) {
            ImGui::SetCursorPosY(30);
            ImGui::TextWrapped("%s", errorMessage);
            ImGui::SetCursorPosY(80);
            ImGui::SetCursorPosX((300 - 120) / 2);
            if (GradientButton("OK", ImVec2(120, 30))) {
                showError = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    
    // Content area (between header and bottom nav)
    ImGui::SetCursorPos(ImVec2(0, 60));
    ImGui::BeginChild("ContentArea", ImVec2(0, -70), false, ImGuiWindowFlags_NoScrollbar);
    
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
    
    ImGui::EndChild();
    
    // Bottom navigation
    if (currentUser != nullptr) {
        ImGui::SetCursorPos(ImVec2(0, ImGui::GetWindowHeight() - 70));
        
        ImVec2 pos = ImGui::GetCursorScreenPos();
        drawList->AddRectFilled(
            pos,
            ImVec2(pos.x + ImGui::GetWindowWidth(), pos.y + 70),
            IM_COL32(25, 25, 35, 255),
            8.0f
        );
        
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 60);
        
        float buttonWidth = ImGui::GetWindowWidth() / 5.0f;
        float iconSize = 30.0f;
        
        // Home button
        ImGui::SetCursorPosX(buttonWidth * 0 + (buttonWidth - iconSize) / 2);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_Text, currentScreen == FEED_SCREEN ? 
                              ImVec4(0.5f, 0.4f, 1.0f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        if (ImGui::Button("H", ImVec2(iconSize, iconSize))) setScreen(FEED_SCREEN);
        ImGui::PopStyleColor(3);
        
        // Search button
        ImGui::SameLine();
        ImGui::SetCursorPosX(buttonWidth * 1 + (buttonWidth - iconSize) / 2);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_Text, currentScreen == SEARCH_SCREEN ? 
                              ImVec4(0.5f, 0.4f, 1.0f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        if (ImGui::Button("S", ImVec2(iconSize, iconSize))) setScreen(SEARCH_SCREEN);
        ImGui::PopStyleColor(3);
        
        // Create Post (center button)
        ImGui::SameLine();
        ImGui::SetCursorPosX(buttonWidth * 2 + (buttonWidth - iconSize) / 2);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.3f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.4f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, iconSize / 2);
        if (ImGui::Button("+", ImVec2(iconSize, iconSize))) setScreen(CREATE_POST_SCREEN);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        
        // Notifications button
        ImGui::SameLine();
        ImGui::SetCursorPosX(buttonWidth * 3 + (buttonWidth - iconSize) / 2);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_Text, currentScreen == NOTIFICATIONS_SCREEN ? 
                              ImVec4(0.5f, 0.4f, 1.0f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        if (ImGui::Button("N", ImVec2(iconSize, iconSize))) setScreen(NOTIFICATIONS_SCREEN);
        ImGui::PopStyleColor(3);
        
        // Profile button
        ImGui::SameLine();
        ImGui::SetCursorPosX(buttonWidth * 4 + (buttonWidth - iconSize) / 2);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_Text, currentScreen == PROFILE_SCREEN ? 
                              ImVec4(0.5f, 0.4f, 1.0f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        if (ImGui::Button("P", ImVec2(iconSize, iconSize))) {
            viewingUser = currentUser;
            setScreen(PROFILE_SCREEN);
        }
        ImGui::PopStyleColor(3);
    }
    
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void UI::renderLoginScreen() {

    float windowWidth = ImGui::GetWindowWidth();
    float contentWidth = 350;
    
    ImGui::SetCursorPos(ImVec2((windowWidth - contentWidth) / 2, 120));
    ImGui::BeginGroup();
    
    // Title
    ImGui::SetWindowFontScale(1.8f);
    ImGui::Text("Login / Register");
    ImGui::SetWindowFontScale(1.0f);
    
    ImGui::Dummy(ImVec2(0, 20));
    
    // Username input
    ImGui::Text("Username");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
    ImGui::SetNextItemWidth(contentWidth);
    ImGui::InputText("##username", usernameInput, 64);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    
    ImGui::Dummy(ImVec2(0, 10));
    
    // Password input
    ImGui::Text("Password");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
    ImGui::SetNextItemWidth(contentWidth);
    ImGui::InputText("##password", passwordInput, 64, ImGuiInputTextFlags_Password);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    
    ImGui::Dummy(ImVec2(0, 30));
    
    // Login button
    if (GradientButton("Login", ImVec2((contentWidth - 10) / 2, 40))) {
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
    
    // Register button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.15f, 0.25f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    if (ImGui::Button("Register", ImVec2((contentWidth - 10) / 2, 40))) {
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
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    
    ImGui::EndGroup();
}

void UI::renderFeedScreen() {
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::Text("Home Feed");
    
    ImGui::SetCursorPos(ImVec2(20, 50));
    ImGui::BeginChild("FeedScroll", ImVec2(0, 0), false);
    
    FeedNode* node = feed->getHead();
    if (!node) {
        ImGui::SetCursorPos(ImVec2(20, 100));
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                          "No posts to show.\nFollow users to see their posts!");
    }
    
    while (node) {
        Post* post = node->post;
        
        ImGui::PushID(post->postID);
        
        // Post card background
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 cardStart = ImGui::GetCursorScreenPos();
        ImVec2 cardEnd = ImVec2(cardStart.x + ImGui::GetWindowWidth() - 40, cardStart.y + 140);
        
        drawList->AddRectFilled(cardStart, cardEnd, IM_COL32(20, 20, 30, 255), 12.0f);
        
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Indent(15);
        
        // Username button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        if (ImGui::Button(post->username.c_str())) {
            viewingUser = userDatabase->searchByID(post->userID);
            setScreen(PROFILE_SCREEN);
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        
        // Post content
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() - 70);
        ImGui::Text("%s", post->content.c_str());
        ImGui::PopTextWrapPos();
        
        ImGui::Dummy(ImVec2(0, 5));
        
        // Timestamp
        ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f),
                          "%s", timestampToString(post->timestamp).c_str());
        
        ImGui::Dummy(ImVec2(0, 5));
        
        // Stats
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
                          "Likes: %d  |  Comments: %d", post->likes, post->commentCount);
        
        ImGui::Dummy(ImVec2(0, 8));
        
        // Action buttons
        if (post->userID != currentUser->userID) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.3f, 0.9f, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.4f, 1.0f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.7f, 1.0f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
            if (ImGui::SmallButton("Like")) {
                post->addLike();
                notifications->addNotification(LIKE, currentUser->userID,
                                              currentUser->username, post->postID,
                                              currentUser->username + " liked your post",
                                              getCurrentTime());
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
        }
        
        // Comment button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.4f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.5f, 0.7f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        if (ImGui::SmallButton("Comment")) {
            viewingPost = post;
            setScreen(POST_DETAIL_SCREEN);
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
        
        ImGui::Unindent(15);
        ImGui::Dummy(ImVec2(0, 15));
        
        ImGui::PopID();
        
        node = node->next;
    }
    
    ImGui::Dummy(ImVec2(0, 20));
    
    ImGui::EndChild();
}

void UI::renderProfileScreen() {
    if (!viewingUser) {
        setScreen(FEED_SCREEN);
        return;
    }
    
    ImGui::SetCursorPos(ImVec2(20, 20));
    
    // Back button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    if (ImGui::Button("< Back", ImVec2(80, 30))) setScreen(FEED_SCREEN);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
    
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.5f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    if (ImGui::Button("Logout", ImVec2(80, 30))) logout();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
    
    ImGui::SetCursorPos(ImVec2(20, 70));
    ImGui::BeginChild("ProfileScroll", ImVec2(0, 0), false);
    
    // Profile header card
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 headerStart = ImGui::GetCursorScreenPos();
    ImVec2 headerEnd = ImVec2(headerStart.x + ImGui::GetWindowWidth() - 40, headerStart.y + 200);
    drawList->AddRectFilled(headerStart, headerEnd, IM_COL32(20, 20, 30, 255), 12.0f);
    
    ImGui::Dummy(ImVec2(0, 20));
    ImGui::Indent(20);
    
    // Username
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text("%s", viewingUser->username.c_str());
    ImGui::SetWindowFontScale(1.0f);
    
    ImGui::Dummy(ImVec2(0, 10));
    
    // Bio
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                      "%s", viewingUser->bio.empty() ? "No bio" : viewingUser->bio.c_str());
    
    ImGui::Dummy(ImVec2(0, 15));
    
    // Stats
    ImGui::Text("Followers: %d  |  Following: %d",
                viewingUser->followerCount, viewingUser->followingCount);
    
    ImGui::Dummy(ImVec2(0, 20));
    
    // Follow/Unfollow or Edit Bio
    if (viewingUser->userID != currentUser->userID) {
        if (currentUser->isFollowing(viewingUser->userID)) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.5f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
            if (ImGui::Button("Unfollow", ImVec2(150, 35))) {
                currentUser->removeFollowing(viewingUser->userID);
                viewingUser->removeFollower(currentUser->userID);
                feed->generateFeed(currentUser, postDatabase);
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(2);
        } else {
            if (GradientButton("Follow", ImVec2(150, 35))) {
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
        ImGui::Text("Edit Bio:");
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
        ImGui::InputTextMultiline("##bio", bioInput, 256, ImVec2(ImGui::GetWindowWidth() - 80, 60));
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        if (GradientButton("Update Bio", ImVec2(150, 35))) {
            viewingUser->bio = string(bioInput);
            memset(bioInput, 0, sizeof(bioInput));
            showErrorMessage("Bio updated successfully!");
        }
    }
    
    ImGui::Unindent(20);
    ImGui::Dummy(ImVec2(0, 30));
    
    // Posts section
    ImGui::Indent(20);
    ImGui::SetWindowFontScale(1.2f);
    ImGui::Text("Posts");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Dummy(ImVec2(0, 10));
    ImGui::Unindent(20);
    
    Post* post = postDatabase->getHead();
    bool foundPosts = false;
    
    while (post) {
        if (post->userID == viewingUser->userID) {
            foundPosts = true;
            ImGui::PushID(post->postID);
            
            // Post card
            ImVec2 cardStart = ImGui::GetCursorScreenPos();
            ImVec2 cardEnd = ImVec2(cardStart.x + ImGui::GetWindowWidth() - 40, cardStart.y + 120);
            drawList->AddRectFilled(cardStart, cardEnd, IM_COL32(20, 20, 30, 255), 12.0f);
            
            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Indent(20);
            
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() - 80);
            ImGui::Text("%s", post->content.c_str());
            ImGui::PopTextWrapPos();
            
            ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f),
                              "%s", timestampToString(post->timestamp).c_str());
            ImGui::Text("Likes: %d | Comments: %d", post->likes, post->commentCount);
            
            ImGui::Dummy(ImVec2(0, 5));
            
            if (ImGui::SmallButton("View")) {
                viewingPost = post;
                setScreen(POST_DETAIL_SCREEN);
            }
            
            if (post->userID == currentUser->userID) {
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 0.7f));
                if (ImGui::SmallButton("Delete")) {
                    postDatabase->deletePost(post->postID);
                    feed->generateFeed(currentUser, postDatabase);
                }
                ImGui::PopStyleColor(2);
            }
            
            ImGui::Unindent(20);
            ImGui::Dummy(ImVec2(0, 15));
            
            ImGui::PopID();
        }
        post = post->next;
    }
    
    if (!foundPosts) {
        ImGui::Indent(20);
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No posts yet");
        ImGui::Unindent(20);
    }
    
    ImGui::Dummy(ImVec2(0, 20));
    ImGui::EndChild();
}

void UI::renderNotificationsScreen() {
    // Header
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::SetWindowFontScale(1.3f);
    ImGui::Text("Notifications");
    ImGui::SetWindowFontScale(1.0f);
    
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f,0.3f,0.4f,1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f,0.4f,0.5f,1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    if (ImGui::Button("Clear All", ImVec2(80,30))) notifications->clearAll();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    // Scrollable notifications
    ImGui::SetCursorPos(ImVec2(20,70));
    ImGui::BeginChild("NotificationScroll", ImVec2(0,0), false);

    Notification* notifArr[200];
    int count = 0;
    notifications->getAllNotifications(notifArr, count);

    if (count == 0) {
        ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1.0f), "No notifications");
    }

    for (int i = 0; i < count; i++) {
        Notification* n = notifArr[i];
        if (n->fromUserID == currentUser->userID) continue;

        ImGui::PushID(n->notificationID);

        // Draw notification card
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 start = ImGui::GetCursorScreenPos();
        ImVec2 end = ImVec2(start.x + ImGui::GetContentRegionAvail().x, start.y + 100);

        ImU32 cardColor = IM_COL32(25,25,35,255);
        switch (n->type) {
            case COMMENT: cardColor = IM_COL32(40,25,35,255); break;
            case LIKE:    cardColor = IM_COL32(40,35,25,255); break;
            case FOLLOW:  cardColor = IM_COL32(25,35,40,255); break;
        }
        drawList->AddRectFilled(start, end, cardColor, 12.0f);

        ImGui::Dummy(ImVec2(0,10));
        ImGui::Indent(15);

        // Type badge + message
        const char* typeStr = "";
        ImVec4 typeColor;
        switch (n->type) {
            case COMMENT: typeStr="COMMENT"; typeColor=ImVec4(1,0.4f,0.4f,1); break;
            case LIKE:    typeStr="LIKE";    typeColor=ImVec4(1,0.8f,0.2f,1); break;
            case FOLLOW:  typeStr="FOLLOW";  typeColor=ImVec4(0.3f,0.7f,1,1); break;
        }

        ImGui::TextColored(typeColor, "[%s]", typeStr);
        ImGui::SameLine();
        ImGui::Text("%s", n->message.c_str());

        ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1), "%s", timestampToString(n->timestamp).c_str());

        // Mark as read button
        if (!n->isRead) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f,0.3f,0.8f,0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f,0.4f,0.9f,0.7f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
            if (ImGui::SmallButton("Mark Read")) n->isRead = true;
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(2);
        }

        ImGui::Unindent(15);
        ImGui::Dummy(ImVec2(0,15));
        ImGui::PopID();
    }

    ImGui::Dummy(ImVec2(0,20));
    ImGui::EndChild();
}

void UI::renderSearchScreen() {
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::SetWindowFontScale(1.3f);
    ImGui::Text("Search");
    ImGui::SetWindowFontScale(1.0f);
    
    ImGui::SetCursorPos(ImVec2(20, 70));
    
    // Search bar
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
    ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 140);
    ImGui::InputTextWithHint("##search", "Search user...", searchInput, 64);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    
    ImGui::SameLine();
    if (GradientButton("Search", ImVec2(90, 32))) {
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
    
    ImGui::SetCursorPos(ImVec2(20, 120));
    ImGui::BeginChild("SearchResults", ImVec2(0, 0), false);
    
    if (searchResultCount == 0) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                          "No results.\nTry searching for a username.");
    }
    
    for (int i = 0; i < searchResultCount; i++) {
        ImGui::PushID(i);
        
        // User card
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 cardStart = ImGui::GetCursorScreenPos();
        ImVec2 cardEnd = ImVec2(cardStart.x + ImGui::GetWindowWidth() - 40, cardStart.y + 80);
        drawList->AddRectFilled(cardStart, cardEnd, IM_COL32(20, 20, 30, 255), 12.0f);
        
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Indent(15);
        
        // Username button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.5f, 1.0f, 1.0f));
        ImGui::SetWindowFontScale(1.2f);
        if (ImGui::Button(searchResults[i]->username.c_str())) {
            viewingUser = searchResults[i];
            setScreen(PROFILE_SCREEN);
        }
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor(3);
        
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
                          "Followers: %d", searchResults[i]->followerCount);
        
        ImGui::Unindent(15);
        ImGui::Dummy(ImVec2(0, 15));
        
        ImGui::PopID();
    }
    
    ImGui::Dummy(ImVec2(0, 20));
    ImGui::EndChild();
}

void UI::renderCreatePostScreen() {
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::SetWindowFontScale(1.3f);
    ImGui::Text("Create Post");
    ImGui::SetWindowFontScale(1.0f);
    
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.5f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    if (ImGui::Button("Cancel", ImVec2(80, 30))) {
        setScreen(FEED_SCREEN);
        memset(postInput, 0, sizeof(postInput));
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
    
    ImGui::SetCursorPos(ImVec2(20, 80));
    ImGui::BeginChild("CreatePostArea", ImVec2(0, 0), false);
    
    // Post input card
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 cardStart = ImGui::GetCursorScreenPos();
    ImVec2 cardEnd = ImVec2(cardStart.x + ImGui::GetWindowWidth() - 40, cardStart.y + 300);
    drawList->AddRectFilled(cardStart, cardEnd, IM_COL32(20, 20, 30, 255), 12.0f);
    
    ImGui::Dummy(ImVec2(0, 15));
    ImGui::Indent(15);
    
    ImGui::Text("What's on your mind?");
    ImGui::Dummy(ImVec2(0, 10));
    
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.18f, 1.0f));
    ImGui::InputTextMultiline("##post", postInput, 512, 
                              ImVec2(ImGui::GetWindowWidth() - 70, 200));
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    
    ImGui::Dummy(ImVec2(0, 10));
    
    int len = strlen(postInput);
    ImVec4 charColor = len > 280 ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
    ImGui::TextColored(charColor, "Characters: %d / 280", len);
    
    ImGui::Unindent(15);
    ImGui::Dummy(ImVec2(0, 15));
    
    // Post button
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) / 2);
    if (GradientButton("Post", ImVec2(200, 45))) {
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
    
    ImGui::EndChild();
}

void UI::renderPostDetailScreen() {
    if (!viewingPost) {
        setScreen(FEED_SCREEN);
        return;
    }
    
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    if (ImGui::Button("< Back", ImVec2(80, 30))) setScreen(FEED_SCREEN);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
    
    ImGui::SetCursorPos(ImVec2(20, 70));
    ImGui::BeginChild("PostDetailScroll", ImVec2(0, 0), false);
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Post card
    ImVec2 postStart = ImGui::GetCursorScreenPos();
    ImVec2 postEnd = ImVec2(postStart.x + ImGui::GetWindowWidth() - 40, postStart.y + 200);
    drawList->AddRectFilled(postStart, postEnd, IM_COL32(20, 20, 30, 255), 12.0f);
    
    ImGui::Dummy(ImVec2(0, 15));
    ImGui::Indent(15);
    
    // Username
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.5f, 1.0f, 1.0f));
    ImGui::SetWindowFontScale(1.2f);
    if (ImGui::Button(viewingPost->username.c_str())) {
        viewingUser = userDatabase->searchByID(viewingPost->userID);
        setScreen(PROFILE_SCREEN);
    }
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor(3);
    
    ImGui::Dummy(ImVec2(0, 10));
    
    // Post content
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() - 70);
    ImGui::Text("%s", viewingPost->content.c_str());
    ImGui::PopTextWrapPos();
    
    ImGui::Dummy(ImVec2(0, 10));
    
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                      "%s", timestampToString(viewingPost->timestamp).c_str());
    
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                      "Likes: %d  |  Comments: %d", viewingPost->likes, viewingPost->commentCount);
    
    ImGui::Dummy(ImVec2(0, 15));
    
    // Like button
    if (viewingPost->userID != currentUser->userID) {
        if (GradientButton("Like", ImVec2(100, 35))) {
            viewingPost->addLike();
            notifications->addNotification(LIKE, currentUser->userID,
                                          currentUser->username, viewingPost->postID,
                                          currentUser->username + " liked your post",
                                          getCurrentTime());
        }
    }
    
    ImGui::Unindent(15);
    ImGui::Dummy(ImVec2(0, 20));
    
    // Comments section header
    ImGui::Indent(15);
    ImGui::SetWindowFontScale(1.2f);
    ImGui::Text("Comments");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Unindent(15);
    ImGui::Dummy(ImVec2(0, 10));
    
    // Comments list
    Comment* comment = viewingPost->getComments();
    if (!comment) {
        ImGui::Indent(15);
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No comments yet");
        ImGui::Unindent(15);
    }
    
    while (comment) {
        ImGui::PushID(comment->commentID);
        
        // Comment card
        ImVec2 commentStart = ImGui::GetCursorScreenPos();
        ImVec2 commentEnd = ImVec2(commentStart.x + ImGui::GetWindowWidth() - 40, commentStart.y + 90);
        drawList->AddRectFilled(commentStart, commentEnd, IM_COL32(25, 25, 35, 255), 10.0f);
        
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Indent(15);
        
        ImGui::TextColored(ImVec4(0.4f, 0.6f, 1.0f, 1.0f), "%s:", comment->username.c_str());
        
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() - 70);
        ImGui::Text("%s", comment->content.c_str());
        ImGui::PopTextWrapPos();
        
        ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f),
                          "%s", timestampToString(comment->timestamp).c_str());
        
        ImGui::Unindent(15);
        ImGui::Dummy(ImVec2(0, 15));
        
        ImGui::PopID();
        comment = comment->next;
    }
    
    // Add comment section
    if (viewingPost->userID != currentUser->userID) {
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Indent(15);
        ImGui::Text("Add Comment:");
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
        ImGui::InputTextMultiline("##comment", commentInput, 512, 
                                  ImVec2(ImGui::GetWindowWidth() - 70, 80));
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        
        ImGui::Dummy(ImVec2(0, 10));
        
        if (GradientButton("Post Comment", ImVec2(150, 35))) {
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
        ImGui::Unindent(15);
    }
    
    ImGui::Dummy(ImVec2(0, 30));
    ImGui::EndChild();
}

// ==================== DUMMY DATA INITIALIZATION ====================
void UI::initializeDummyData() {
    // Generate dummy users and their follower relationships
    userDatabase->generateDummyUsers();
    
    // Generate dummy posts with likes and comments
    postDatabase->generateDummyPosts(userDatabase);
    
    // Generate some notifications for alice (the default test user)
    User* alice = userDatabase->searchByUsername("alice");
    if (alice) {
        User* bob = userDatabase->searchByUsername("bob");
        User* charlie = userDatabase->searchByUsername("charlie");
        User* eve = userDatabase->searchByUsername("eve");
        
        Timestamp now = getCurrentTimestamp();
        
        // Helper for minutes ago
        auto minutesAgo = [&](int minutes) {
            Timestamp ts = now;
            ts.minute -= minutes;
            if (ts.minute < 0) {
                ts.minute += 60;
                ts.hour -= 1;
            }
            return ts;
        };
        
        if (bob) {
            notifications->addNotification(LIKE, bob->userID, bob->username, 1005,
                "bob liked your post", minutesAgo(30));
            notifications->addNotification(COMMENT, bob->userID, bob->username, 1005,
                "bob commented on your post", minutesAgo(25));
        }
        
        if (charlie) {
            notifications->addNotification(FOLLOW, charlie->userID, charlie->username, 0,
                "charlie followed you", minutesAgo(45));
            notifications->addNotification(COMMENT, charlie->userID, charlie->username, 1010,
                "charlie commented on your post", minutesAgo(20));
        }
        
        if (eve) {
            notifications->addNotification(LIKE, eve->userID, eve->username, 1010,
                "eve liked your post", minutesAgo(60));
        }
    }
}