#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(){
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();                                                     //初始化glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  //告訴 GLFW 3.3 是我們想要使用的 OpenGL 版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                  //告訴 GLFW 3.3 是我們想要使用的 OpenGL 版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //明確告訴GLFW我們使用的是核心模式(Core-profile)。明確告訴GLFW我們需要使用核心模式意味著我們只能使用OpenGL功能的子集（沒有我們已不再需要的向後相容特性）

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);        // Mac OS X 上才需要加入這串指令
    #endif

    // 接下來我們需要建立一個視窗物件。此視窗物件保存所有視窗數據，並且是 GLFW 的大多數其他函數所需要的。
    // glfwCreateWindow函數需要視窗寬度和高度分別作為其前兩個參數。
    // 第三個參數允許我們為視窗建立一個名稱；現在我們這樣稱呼它"LearnOpenGL"，當然你可以隨意命名它。
    // 最後兩個參數我們暫時忽略。
    // 該函數傳回一個GLFW視窗我們稍後將需要它來進行其他 GLFW 操作。
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //我們告訴 GLFW 將視窗上下文設定為目前執行緒的主上下文。
    glfwMakeContextCurrent(window);
    //告訴GLFW每次調整視窗大小時，會呼叫該函數。
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // 我們向 GLAD 傳遞函數來載入特定於作業系統的 OpenGL 函數指標的位址。 
    // GLFW 定義了 glfwGetProcAddress，它根據我們的作業系統定義了正確的函數
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // 我們希望應用程式繼續繪製圖像並處理用戶輸入，直到程式被明確告知停止。
    // 因此，我們必須建立一個 while 循環，我們現在稱之為渲染循環(render loop)，它會繼續執行，直到我們告訴 GLFW 停止。
    // glfwWindowShouldClose函數在每次循環迭代開始時檢查 GLFW 是否已被指示關閉。如果是，函數會返回true
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);



        // render
        // ------
        // 在幀開始時我們想要清除螢幕。否則，我們仍然會看到前一幀的結果
        // 我們可以使用glClear清除螢幕的顏色緩衝區，我們傳入緩衝區位元來指定我們要清除哪個緩衝區
        // GL_COLOR_BUFFER_BIT、GL_DEPTH_BUFFER_BIT和GL_STENCIL_BUFFER_BIT。現在我們只關心顏色值，所以我們只清除顏色緩衝區。
        // rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);       //我們使用glClearColor指定清除螢幕的顏色    //每當我們call glClear並清除顏色緩衝區時，整個顏色緩衝區將填滿由glClearColor配置的顏色
        glClear(GL_COLOR_BUFFER_BIT);



        // glfwPollEvents函數檢查是否觸發了任何事件（例如鍵盤輸入或滑鼠移動事件），更新視窗狀態，並呼叫相應的函數（which we can register via callback methods）
        // glfwSwapBuffers將交換在此渲染迭代期間用於渲染的顏色緩衝區（一個大型 2D 緩衝區，其中包含 GLFW 視窗中每個像素的顏色值）並將其顯示為螢幕的輸出。
        // 雙緩衝區
            // 當應用程式在單一緩衝區中繪製時，產生的影像可能會出現閃爍問題。
            // 這是因為產生的輸出影像不是立即繪製的，而是逐像素繪製的，通常是從左到右、從上到下。
            // 由於該影像在渲染過程中不會立即顯示給用戶，因此結果可能包含偽影。為了避免這些問題，視窗應用程式應用雙緩衝區進行渲染。
            // 前緩衝區包含螢幕上顯示的最終輸出影像，而所有渲染命令都繪製到後緩衝區。
            // 一旦所有渲染命令完成，我們就將後台緩衝區交換到前台緩衝區，以便可以顯示圖像而無需渲染，從而消除所有上述偽影。
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 一旦退出渲染循環，我們就希望正確清理/刪除所有已分配的 GLFW 資源。我們可以透過以下方式做到這一點
    // glfwTerminate將清理所有資源並正確退出應用程式。
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// 當使用者調整視窗大小時，ViewPort也應該調整。
// 我們在視窗上註冊一個回呼函數(Callback Function)，每次調整視窗大小時都會呼叫該函數。
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// 我們也希望在 GLFW 中有某種形式的輸入控制，我們可以用好幾個 GLFW 的輸入函數來實現這一點。
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    // 我們使用 glfwGetKey 將視窗與按鍵一起作為輸入的函數。函數傳回目前是否按下該鍵。
    // 這裡我們檢查使用者是否按下了ESC鍵（如果沒有按下，glfwGetKey回傳GLFW_RELEASE）。
    // 如果使用者確實按下了ESC鍵，我們透過將WindowShouldClose設為true來關閉視窗
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
