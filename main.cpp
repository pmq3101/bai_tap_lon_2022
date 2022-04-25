#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;

// game funtions

int grid[9];


// SDL functions
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event* click;




const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const string WINDOW_TITLE = "Tic Tac Toe by Pham Minh Quang";
//****************************************************************
void logSDLError(std::ostream& os,
                 const std::string &msg, bool fatal = false);
void initSDL(SDL_Window* &window, SDL_Renderer* &renderer);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
void waitUntilKeyPressed();
void loadMenu();
void renderX(int pos);
void renderO();
int convertToNumber(int x, int y);
void printXTurn();
void printOTurn();
void printXWon();
void printOWon();
void printTie();
void initGrid();
//void click_on_cell();
char check();
void getHumanMove();
void getComputerMove();



SDL_Texture* loadTexture(string path, SDL_Renderer* renderer);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);
int main(int argc, char* argv[])
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    char done;
    done = ' ';
    loadMenu();
    initGrid();
    do {
    getHumanMove();
    done = check(); /* check winner */
    if(done!= ' ') break; /* if winner found...*/
    getComputerMove();
    done = check(); /* check for winner again */
  } while(done == ' ');
    if(done == 'X') printXWon();
    else if(done == 'O') printOWon();
    SDL_Delay(100000);

    quitSDL(window, renderer);
    return 0;
}

SDL_Texture* loadTexture(string path, SDL_Renderer* renderer)
{
    SDL_Texture* newTexture =  nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
        cout << "Unable to load image "<< path << "SDL_image error: " << IMG_GetError()<< endl;
    else{
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if(newTexture == nullptr)
            cout<< "Unable to create texture form "<<path<< "SDL Error: "<<SDL_GetError() << endl;
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}
void logSDLError(std::ostream& os,const std::string &msg, bool fatal)
{
    os << msg << " Error: " << SDL_GetError() << std::endl;
    if (fatal) {
        SDL_Quit();
        exit(1);
    }
}

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(std::cout, "SDL_Init", true);

    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
       SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    //window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
    //   SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (window == nullptr) logSDLError(std::cout, "CreateWindow", true);


    //Khi thông thường chạy với môi trường bình thường ở nhà
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                              SDL_RENDERER_PRESENTVSYNC);
    //Khi chạy ở máy thực hành WinXP ở trường (máy ảo)
    //renderer = SDL_CreateSoftwareRenderer(SDL_GetWindowSurface(window));
    if (renderer == nullptr) logSDLError(std::cout, "CreateRenderer", true);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
        if ( SDL_WaitEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y)
{
	//Thiết lập hình chữ nhật đích mà chúng ta muốn vẽ ảnh vào trong
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	//Truy vẫn texture để lấy chiều rộng và cao (vào chiều rộng và cao tương ứng của hình chữ nhật đích)
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    //Đưa toàn bộ ảnh trong texture vào hình chữ nhật đích
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

void loadGrid()
{
    SDL_Event e;
    while(true){
        if(SDL_WaitEvent(&e)==0) SDL_Delay(100);
        else if (e.type == SDL_QUIT) break;
        else if (e.type == SDL_KEYDOWN){
            if(e.key.keysym.sym == SDLK_RETURN){
                    SDL_RenderClear(renderer);
                    SDL_Texture* board = loadTexture("images/Tic Tac Toe grid.png", renderer);
                    SDL_RenderCopy(renderer, board, NULL, NULL);
                    SDL_RenderPresent(renderer);
                    break;
            }
        }
    }
}
int convertToNumber(int x, int y)
{
    int ans;
    if(x < 143 && y < 166){
        ans = 0;
        }

    else if(x < 252 && x > 143 && y < 166){
        ans = 1;

        }

    else if(x < 363 && x > 252 && y < 166){
        ans = 2;

        }

    else if( x <143 && y < 276 && y >166){
        ans = 3;

        }

    else if(x < 252 && x > 143 && y < 276 && y >166){
        ans = 4;

        }

    else if(x < 363 && x > 252 && y < 276 && y >166){
        ans = 5;

        }

    else if(x < 143  && y < 363 && y >276){
        ans = 6;

        }

    else if(x < 252 && x > 143 && y < 363 && y > 276){
        ans = 7;

        }

    else if(x < 363 && x > 252 && y < 363 && y > 276){
        ans = 8;

        }
        return ans;
    }
void renderX(int pos) // computer
{
    if(pos == 0){
        SDL_Texture* X = loadTexture("images/X.png", renderer);
        renderTexture(X, renderer, 50, 75);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 1){
        SDL_Texture* X = loadTexture("images/X.png", renderer);
        renderTexture(X, renderer, 160, 75);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 2){
        SDL_Texture* X = loadTexture("images/X.png", renderer);
        renderTexture(X, renderer, 270, 75);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 3){
        SDL_Texture* X = loadTexture("images/X.png", renderer);
        renderTexture(X, renderer, 50, 185);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 4){
        SDL_Texture* X = loadTexture("images/X.png", renderer);
        renderTexture(X, renderer, 160, 185);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 5){
        SDL_Texture* X = loadTexture("images/X.png", renderer);
        renderTexture(X, renderer, 270, 185);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 6){
        SDL_Texture* X = loadTexture("images/X.png", renderer);
        renderTexture(X, renderer, 50, 295);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 7){
        SDL_Texture* X = loadTexture("images/X.png", renderer);
        renderTexture(X, renderer, 160, 295);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 8){
        SDL_Texture* X = loadTexture("images/X.png", renderer);
        renderTexture(X, renderer, 270, 295);
        SDL_RenderPresent(renderer);
    }
}
void renderO() //human
{
    SDL_Event ev;
    while(true){
        if(SDL_WaitEvent(&ev)==0) SDL_Delay(100);
        else if (ev.type == SDL_QUIT) break;
        else if (ev.type == SDL_MOUSEBUTTONDOWN){
            if(ev.button.x < 143 && ev.button.y < 166){
                    SDL_Texture* X = loadTexture("images/O.png", renderer);
                    renderTexture(X, renderer, 50, 75);
                    SDL_RenderPresent(renderer);
                    break;
            }

            else if(ev.button.x < 252 && ev.button.x > 143 && ev.button.y < 166){
                    SDL_Texture* X = loadTexture("images/O.png", renderer);
                    renderTexture(X, renderer, 160, 75);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 363 && ev.button.x > 252 && ev.button.y < 166){
                    SDL_Texture* X = loadTexture("images/O.png", renderer);
                    renderTexture(X, renderer, 270, 75);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if( ev.button.x < 143 && ev.button.y < 276 && ev.button.y >166){
                    SDL_Texture* X = loadTexture("images/O.png", renderer);
                    renderTexture(X, renderer, 50, 185);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 252 && ev.button.x > 143 && ev.button.y < 276 && ev.button.y >166){
                    SDL_Texture* X = loadTexture("images/O.png", renderer);
                    renderTexture(X, renderer, 160, 185);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 363 && ev.button.x > 252 && ev.button.y < 276 && ev.button.y >166){
                    SDL_Texture* X = loadTexture("images/O.png", renderer);
                    renderTexture(X, renderer, 270, 185);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 143  && ev.button.y < 363 && ev.button.y >276){
                    SDL_Texture* X = loadTexture("images/O.png", renderer);
                    renderTexture(X, renderer, 50, 295);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 252 && ev.button.x > 143 && ev.button.y < 363 && ev.button.y > 276){
                    SDL_Texture* X = loadTexture("images/O.png", renderer);
                    renderTexture(X, renderer, 160, 295);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 363 && ev.button.x > 252 && ev.button.y < 363 && ev.button.y > 276){
                    SDL_Texture* X = loadTexture("images/O.png", renderer);
                    renderTexture(X, renderer, 270, 295);
                    SDL_RenderPresent(renderer);
                    break;
                }
            }
        }
}


void loadMenu(){
    initSDL(window, renderer);

    SDL_Texture* menu = loadTexture("images/title screen.png", renderer);
    SDL_RenderCopy(renderer, menu, NULL, NULL);
    SDL_RenderPresent(renderer);
    loadGrid();
}

void printXTurn()
{
    SDL_Texture* Xturn = loadTexture("images/Player X_s Turn.png", renderer);
                    renderTexture(Xturn, renderer, 400, 15);
                    SDL_RenderPresent(renderer);
}

void printOTurn()
{
    SDL_Texture* OTurn = loadTexture("images/Player O_s Turn.png", renderer);
                    renderTexture(OTurn, renderer, 400, 15);
                    SDL_RenderPresent(renderer);
}

void printXWon()
{
   SDL_Texture* XWon = loadTexture("images/Player X Wins.png", renderer);
                    renderTexture(XWon, renderer, 420, 10);
                    SDL_RenderPresent(renderer);
}

void printOWon()
{
    SDL_Texture* OWon = loadTexture("images/Player O Wins.png", renderer);
                    renderTexture(OWon, renderer, 420, 10);
                    SDL_RenderPresent(renderer);
}
void printTie()
{
    SDL_Texture* Tie = loadTexture("images/tie.png", renderer);
                    renderTexture(Tie, renderer, 420, 10);
                    SDL_RenderPresent(renderer);
}
void printChange()
{
    SDL_Texture* Change = loadTexture("images/change.png", renderer);
                    renderTexture(Change, renderer, 420, 10);
                    SDL_RenderPresent(renderer);
}


char check()
{

    if((grid[0]=='X' && grid[1]=='X' && grid[2]=='X') || (grid[3]=='X' && grid[4]== 'X' && grid[5]=='X') || (grid[6]=='X' && grid[7]=='X' && grid[8]=='X')
       || (grid[0]=='X' && grid[3]=='X' && grid[6]=='X') || (grid[1]=='X' && grid[4]=='X' && grid[7]=='X') || (grid[2]=='X' && grid[5]=='X' && grid[6]=='X')
       || (grid[0]=='X' && grid[4]=='X' && grid[8]=='X') || (grid[2]=='X' && grid[4]=='X' && grid[6]=='X'))
        {
           return 'X';
        }
    else if((grid[0]=='O' && grid[1]=='O' && grid[2]=='O') || (grid[3]=='O'&& grid[4]== 'O' && grid[5]=='O') || (grid[6]=='O' && grid[7]=='O' && grid[8]=='O')
       || (grid[0]=='O' && grid[3]=='O' && grid[6]=='O') || (grid[1]=='O' && grid[4]=='O' && grid[7]=='O') || (grid[2]=='O' && grid[5]=='O' && grid[8]=='O')
       || (grid[0]=='O' && grid[4]=='O' && grid[8]=='O') || (grid[2]=='O' && grid[4]=='O' && grid[6]=='O'))
        {
           return 'O';
        }
    return ' ';
}

//void click_on_cell()
//{
//    int running = 1, turn = 0, finish = 0;
//    bool check = false;
//    for(int i=0; i<9; i++){
//        grid[i] = EMPTY;
//    }
//    SDL_Event event;
//    while(running)
//    {
//        SDL_WaitEvent(&event);
//        switch(event.type)
//        {
//            case SDL_QUIT:
//                running=0;
//                break;
//            case SDL_MOUSEBUTTONDOWN:
//                    if(turn)
//                    {
//                        int pos1 = convertToNumber(event.button.x, event.button.y);
//                        cout << event.button.x << " " << event.button.y;
//                        if(grid[pos1] == EMPTY ) renderO(event.button.x, event.button.y);
//                        grid[pos1] = CIRCLE;
//                        turn=0;
//                    }
//                    else if(!turn)
//                    {
//                        srand(time(NULL));
//                        int pos2 = rand() % (8 - 0 + 1) + 0;
//                        if(grid[pos2] == EMPTY){
//                            renderX(pos2);
//                            grid[pos2] = CROSS;
//                            turn = 1;
//
//                }
//            }
//        }
//    }
//}
void initGrid()
{
    for(int i=0; i<9; i++){
        grid[i] == ' ';
    }
}
void getHumanMove()
{
    SDL_Event ev;
    while(true){
        if(SDL_WaitEvent(&ev)==0) SDL_Delay(100);
        else if (ev.type == SDL_QUIT) break;
        else if (ev.type == SDL_MOUSEBUTTONDOWN){
            int pos1 = convertToNumber(ev.button.x, ev.button.y);
            if(grid[pos1] == 'X' || grid[pos1] == 'O'){
                getHumanMove();
            }
            else{
                renderO();
                grid[pos1] = 'O';
                break;
            }
        }
    }
}


void getComputerMove()
{
    SDL_Delay(2000);
    srand(time(NULL));
	int pos2 = rand() % (8 - 0 + 1) + 0;
	if(grid[pos2] == 'X' || grid[pos2] == 'O'){
        getComputerMove();
	}
    else{
        renderX(pos2);
        grid[pos2] = 'X';

    }

}














