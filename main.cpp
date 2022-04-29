#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#define compareBoxes(box1, box2, box3) ((board[box1] == board[box2]) && (board[box2] == board[box3]) && (board[box1] != 0)) //Checkes if three items are the same, and makes sure they're not 0's.
#define numberToLetter(x) ((x > 0) ? (x == 1) ? 'X' : 'O' : ' ') //Takes the number and turns it into the letter or space.

using namespace std;

int board[9] = {0,0,0,0,0,0,0,0,0}; //Starts empty board.
Mix_Chunk* welcome = NULL;
Mix_Chunk *Oturn = NULL;
Mix_Chunk *Xturn = NULL;
Mix_Chunk *getStart = NULL;
Mix_Chunk *Owin = NULL;
Mix_Chunk *Xwin = NULL;
Mix_Chunk *draw = NULL;
Mix_Chunk *intro = NULL;

SDL_Window* window;
SDL_Renderer* renderer;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const string WINDOW_TITLE = "Tic Tac Toe by Pham Minh Quang";
//****************************************************************
void logSDLError(std::ostream& os,
                 const std::string &msg, bool fatal = false);
void initSDL(SDL_Window* &window, SDL_Renderer* &renderer);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
void waitUntilKeyPressed(); SDL_Texture* loadTexture(string path, SDL_Renderer* renderer);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);
void loadMenu();
void renderO(int pos);
void renderX();
int convertToNumber(int x, int y);
void printXTurn();
void printOTurn();
void printXWon();
void printOWon();
void printTie();
void printGameOver();
int getWinner(int board[9]);
bool gameOver(int board[9]);
int willWin(int board[9], int player);
void getHumanMove();
void getComputerMove();


int main(int argc, char* argv[])
{
    //loadMenu();
    gameLoop:
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1){
        printf("%s", Mix_GetError());
    }
    loadMenu();
    SDL_Window* window;
    SDL_Renderer* renderer;
    int possibleWinner;
    while (true) {
		//Player X decides what move they'll do.
		getHumanMove();
		//Decides whether or not the game continues.
		if (gameOver(board) > 0) {
			break;
		}
		//Player O decides which move they'll do.
		//SDL_Delay(500);
		bool good = false;
		for (int x = 2; x > 0; x--){
			possibleWinner = willWin(board, x);
			if (possibleWinner != -1) {
				board[possibleWinner] = 2;
				renderO(possibleWinner);
				good = true;
				break;
			}
		}
		if (good);
		else if (board[4] == 0){
            board[4] = 2; //Middle.
            renderO(4);
		}
		else if (board[4] != 0) getComputerMove();

		//Decides whether or not the game continues.
		if(gameOver(board)) break;
	}
	for(int i = 0; i<9; i++){
        board[i] = 0;
    }
    printGameOver();
    SDL_Delay(1000);
    SDL_Event e;
    while(true){
        if(SDL_WaitEvent(&e)==0) SDL_Delay(100);
        else if (e.type == SDL_QUIT) break;
        else if (e.type == SDL_KEYDOWN){
            if(e.key.keysym.sym == SDLK_RETURN){
                    goto gameLoop;
            }
            else if(e.key.keysym.sym == SDLK_BACKSPACE){
                quitSDL(window, renderer);
            }
        }
    }
    SDL_Delay(5000);
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
void renderO(int pos) // computer
{
    if(pos == 0){
        SDL_Texture* O = loadTexture("images/O.png", renderer);
        renderTexture(O, renderer, 50, 75);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 1){
        SDL_Texture* O = loadTexture("images/O.png", renderer);
        renderTexture(O, renderer, 160, 75);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 2){
        SDL_Texture* O = loadTexture("images/O.png", renderer);
        renderTexture(O, renderer, 270, 75);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 3){
        SDL_Texture* O = loadTexture("images/O.png", renderer);
        renderTexture(O, renderer, 50, 185);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 4){
        SDL_Texture* X = loadTexture("images/O.png", renderer);
        renderTexture(X, renderer, 160, 185);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 5){
        SDL_Texture* O = loadTexture("images/O.png", renderer);
        renderTexture(O, renderer, 270, 185);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 6){
        SDL_Texture* O = loadTexture("images/O.png", renderer);
        renderTexture(O, renderer, 50, 295);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 7){
        SDL_Texture* O = loadTexture("images/O.png", renderer);
        renderTexture(O, renderer, 160, 295);
        SDL_RenderPresent(renderer);
    }
    else if(pos == 8){
        SDL_Texture* O = loadTexture("images/O.png", renderer);
        renderTexture(O, renderer, 270, 295);
        SDL_RenderPresent(renderer);
    }
}
void renderX() //human
{
    SDL_Event ev;
    while(true){
        if(SDL_WaitEvent(&ev)==0) SDL_Delay(100);
        else if (ev.type == SDL_QUIT) break;
        else if (ev.type == SDL_MOUSEBUTTONDOWN){
            if(ev.button.x < 143 && ev.button.y < 166){
                    SDL_Texture* X = loadTexture("images/X.png", renderer);
                    renderTexture(X, renderer, 50, 75);
                    SDL_RenderPresent(renderer);
                    break;
            }

            else if(ev.button.x < 252 && ev.button.x > 143 && ev.button.y < 166){
                    SDL_Texture* X = loadTexture("images/X.png", renderer);
                    renderTexture(X, renderer, 160, 75);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 363 && ev.button.x > 252 && ev.button.y < 166){
                    SDL_Texture* X = loadTexture("images/X.png", renderer);
                    renderTexture(X, renderer, 270, 75);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if( ev.button.x < 143 && ev.button.y < 276 && ev.button.y >166){
                    SDL_Texture* X = loadTexture("images/X.png", renderer);
                    renderTexture(X, renderer, 50, 185);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 252 && ev.button.x > 143 && ev.button.y < 276 && ev.button.y >166){
                    SDL_Texture* X = loadTexture("images/X.png", renderer);
                    renderTexture(X, renderer, 160, 185);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 363 && ev.button.x > 252 && ev.button.y < 276 && ev.button.y >166){
                    SDL_Texture* X = loadTexture("images/X.png", renderer);
                    renderTexture(X, renderer, 270, 185);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 143  && ev.button.y < 363 && ev.button.y >276){
                    SDL_Texture* X = loadTexture("images/X.png", renderer);
                    renderTexture(X, renderer, 50, 295);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 252 && ev.button.x > 143 && ev.button.y < 363 && ev.button.y > 276){
                    SDL_Texture* X = loadTexture("images/X.png", renderer);
                    renderTexture(X, renderer, 160, 295);
                    SDL_RenderPresent(renderer);
                    break;
                }

            else if(ev.button.x < 363 && ev.button.x > 252 && ev.button.y < 363 && ev.button.y > 276){
                    SDL_Texture* X = loadTexture("images/X.png", renderer);
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
    welcome = Mix_LoadWAV("audio/welcome.wav");
    Mix_PlayChannel(-1, welcome, 0);
    intro = Mix_LoadWAV("audio/intro.wav");
    Mix_PlayChannel(-1, intro, 0);
    loadGrid();
    getStart = Mix_LoadWAV("audio/get start.wav");
    Mix_PlayChannel(-1, getStart, 0);
    SDL_Delay(1000);
}

void printXTurn()
{
    Xturn = Mix_LoadWAV("audio/X-turn.wav");
    Mix_PlayChannel(-1, Xturn, 0);
//    SDL_Texture* Xturn = loadTexture("images/Player X_s Turn.png", renderer);
//    renderTexture(Xturn, renderer, 400, 15);
//    SDL_RenderPresent(renderer);

}

void printOTurn()
{
    Oturn = Mix_LoadWAV("audio/O-turn .wav");
    Mix_PlayChannel(-1, Oturn, 0);
//    SDL_Texture* OTurn = loadTexture("images/Player O_s Turn.png", renderer);
//    renderTexture(OTurn, renderer, 400, 15);
//    SDL_RenderPresent(renderer);
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

void printGameOver()
{
    SDL_RenderClear(renderer);
    SDL_Texture* over = loadTexture("images/Game Over.png", renderer);
    SDL_RenderCopy(renderer, over, NULL, NULL);
    SDL_RenderPresent(renderer);
}


int getWinner(int board[9]) {
	//Finds winner of game, if there is no winner, returns 0.
	int winner = 0;
	for (int x = 0; x < 3; x++) {
		if (compareBoxes(3*x, 3*x+1, 3*x+2)) { //Chekcs rows.
			winner = board[3*x];
			break;
		} else if (compareBoxes(x, x+3, x+6)) { //Checks columns.
			winner = board[x];
			break;

		} else if (compareBoxes(2*x, 4, 8-2*x) && (x < 2)) { //Checks diagonals. Doesn't check if x == 2.
			winner = board[4];
			break;
		}
	}
	return winner;
}
bool gameOver(int board[9]){
	//Checks if game is over, and announces who won, or if it was a tie.
	int winner = getWinner(board);
	if (winner > 0) {
        if(numberToLetter(winner) == 'X'){
            printXWon();
            Xwin = Mix_LoadWAV("audio/X-win.wav");
            Mix_PlayChannel(-1, Xwin, 0);

        }
        else if(numberToLetter(winner) == 'O'){
            printOWon();
            Owin = Mix_LoadWAV("audio/O-win.wav");
            Mix_PlayChannel(-1, Owin, 0);
        }
		return true;
	}
	for (int x = 0; x < 9; x++) {
		if (board[x] == 0) return false;
	}
	printTie();
	draw = Mix_LoadWAV("audio/draw.wav");
    Mix_PlayChannel(-1, draw, 0);
	return true;
}

int willWin(int board[9], int player) {
	//Checks if a given player could win in the next plank.
	for (int x = 0; x < 9; x++) {
		int tempBoard[9];
		memcpy(tempBoard, board, 36);
		if (board[x] > 0) continue;
		tempBoard[x] = player;
		if(getWinner(tempBoard) == player) return x;
	}
	return -1;
}

void getHumanMove()
{
    printXTurn();
    SDL_Event ev;
    while(true){
        if(SDL_WaitEvent(&ev)==0) SDL_Delay(100);
        else if (ev.type == SDL_QUIT) break;
        else if (ev.type == SDL_MOUSEBUTTONDOWN){
            int pos1 = convertToNumber(ev.button.x, ev.button.y);
            if(board[pos1] == 1 || board[pos1] == 2){
                getHumanMove();
            }
            else{
                renderX();
                board[pos1] = 1;
                break;
            }
        }
    }
    SDL_Delay(1000);
    printOTurn();
    SDL_Delay(1000);
}


void getComputerMove()
{

    srand(time(NULL));
	int pos2 = rand() % (8 - 0 + 1) + 0;
	if(board[pos2] == 1 || board[pos2] == 2){
        getComputerMove();
	}
    else{
        renderO(pos2);
        board[pos2] = 2;
    }
}
















