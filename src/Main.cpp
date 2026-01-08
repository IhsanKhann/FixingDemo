#include "App.h"

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }

    // OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Social Media App", NULL, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Create application state
    UserDatabase userDB;
    PostDatabase postDB;
    NotificationQueue notifQueue;
    History history;
    
    // Load existing data from files
    cout << "Loading data from files..." << endl;
    userDB.loadFromFile("users.txt");
    userDB.loadConnectionsFromFile("connections.txt");
    postDB.loadFromFile("posts.txt");
    
    // If no data exists, generate dummy data
    User* testUser = userDB.searchByUsername("alice");
    if (!testUser) {
        cout << "No existing data found. Generating dummy data..." << endl;
        userDB.generateDummyUsers();
        postDB.generateDummyPosts(&userDB);
        
        // Save dummy data immediately
        userDB.saveToFile("users.txt");
        userDB.saveConnectionsToFile("connections.txt");
        postDB.saveToFile("posts.txt");
    }
    
    UI ui(&userDB, &postDB, &notifQueue, &history);
    
    // Initialize notifications for alice if exists
    User* alice = userDB.searchByUsername("alice");
    if (alice) {
        ui.initializeDummyData(); // This only adds notifications
    }

    // Auto-save timer
    auto lastSaveTime = std::chrono::steady_clock::now();
    const int AUTOSAVE_INTERVAL_SECONDS = 30; // Save every 30 seconds

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Auto-save periodically
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastSaveTime).count();
        if (elapsed >= AUTOSAVE_INTERVAL_SECONDS) {
            cout << "Auto-saving data..." << endl;
            userDB.saveToFile("users.txt");
            userDB.saveConnectionsToFile("connections.txt");
            postDB.saveToFile("posts.txt");
            lastSaveTime = currentTime;
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render UI
        ui.render();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Final save before exit
    cout << "Saving data before exit..." << endl;
    userDB.saveToFile("users.txt");
    userDB.saveConnectionsToFile("connections.txt");
    postDB.saveToFile("posts.txt");

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}