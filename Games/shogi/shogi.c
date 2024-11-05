#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<ev.h>
#include<string.h>

#define ROW 9
#define COLUMN 9
#define MAXSTACK 200
#define true 1
#define false 0

FILE* fp;

typedef int BOOL;
BOOL check = true;//check keyboard input
BOOL systemstart = true;//whole system switch
BOOL gamestart = false;// time counting loop start
BOOL gamereset = true;//use go back to first step
BOOL timereset = false;//time loop reset

 ev_io stdin_watcher;
 ev_timer time_watcher;

int now;
int clk1 = 0;
int clk2 = 0;
int counter = 1;//check who's turn

char board[ROW][COLUMN] = {
							{'C', 'H', 'S', 'G', 'K', 'G', 'S', 'H', 'C'}, 
							{' ', 'A', ' ', ' ', ' ', ' ', ' ', 'F', ' '},
							{'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
							{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
							{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
							{'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
							{' ', 'f', ' ', ' ', ' ', ' ', ' ', 'a', ' '},
							{'c', 'h', 's', 'g', 'k', 'g', 's', 'h', 'c'}
						  }; /*initialize the shogi board*/
char s_board[ROW][COLUMN];

void chessboard(){
    for(int i = 8;i >= 0;i--){
        printf(" %d  ",i+1);
    }
    printf("\n");
    for(int i = 8;i >= 0;i--)
	{

		for(int j = 0;j < COLUMN ; j++){
            switch(board[i][j]) {//categorize
                case' ': printf("    ");break;
                case'k': printf("\033[31m 王 \033[0m");break;
                case'g': printf("\033[31m 金 \033[0m");break;
                case's': printf("\033[31m 銀 \033[0m");break;
                case'h': printf("\033[31m 桂 \033[0m");break;
                case'c': printf("\033[31m 香 \033[0m");break;
                case'a': printf("\033[31m 角 \033[0m");break;
                case'f': printf("\033[31m 飛 \033[0m");break;
                case'p': printf("\033[31m 步 \033[0m");break;
                case'K': printf("\033[34m 王 \033[0m");break;
                case'G': printf("\033[34m 金 \033[0m");break;
                case'S': printf("\033[34m 銀 \033[0m");break;
                case'H': printf("\033[34m 桂 \033[0m");break;
                case'C': printf("\033[34m 香 \033[0m");break;
                case'A': printf("\033[34m 角 \033[0m");break;
                case'F': printf("\033[34m 飛 \033[0m");break;
                case'P': printf("\033[34m 步 \033[0m");break;
            }
        }
        switch(i){
            case 8 : printf("一\n");break;
            case 7 : printf("二\n");break;
            case 6 : printf("三\n");break;
            case 5 : printf("四\n");break;
            case 4 : printf("五\n");break;
            case 3 : printf("六\n");break;
            case 2 : printf("七\n");break;
            case 1 : printf("八\n");break;
            case 0 : printf("九\n");break;
        }
    }
}
struct data
{
	int from_s[2];
	int to_s[2];
	char f_chess;
	char t_chess;
}movement[MAXSTACK];//step restoration

int top=-1;//counter for maxstack

int isEmpty()
{
	if(top==-1) 
	{
        gamereset = false;
		return 1;
	}
	else
	{
		return 0;
	}
}

int push(int from1, int from2, int to1,int to2)//stack
{
	if(top>=MAXSTACK)
	{
		printf("the stack is full\n");
		return 0;
	}
	else
	{
		top++;		
		movement[top].from_s[0] = from1;
		movement[top].from_s[1] = from2;
		movement[top].to_s[0] = to1;
		movement[top].to_s[1] = to2;
		movement[top].f_chess = board[from1][from2];
		movement[top].t_chess = board[to1][to2];
		return 1;
	}
}

int goback()//pop
{
	if(isEmpty())
	{
		return -1;
	}	
	else
	{
		board[movement[top].from_s[0]][movement[top].from_s[1]] = movement[top].f_chess;
		board[movement[top].to_s[0]][movement[top].to_s[1]] = movement[top].t_chess;
		top--;
		return 0;
	}
}

void record_Board(void)	//for replay
{
	for(int i = 0;i<ROW;i++)
	{
		for(int j = 0;j<COLUMN;j++)
		{
			s_board[i][j] = board[i][j];
		}	
	}
}


void replay(void){//review step
    int step = 0;
    char option[10];
    check = true;
    for(int i = 0;i<ROW;i++)
	{
		for(int j = 0;j<COLUMN;j++)
		{
			board[i][j] = s_board[i][j];
		}
	}
    board[movement[step].to_s[0]][movement[step].to_s[1]] = board[movement[step].from_s[0]][movement[step].from_s[1]];
	board[movement[step].from_s[0]][movement[step].from_s[1]] = ' ';
	printf("**********%d**********\n", step+1);
    chessboard();
    while(check){
    printf("please enter 'f' to forward, 'b' to backward, 'q' to quit:"); 
    scanf(" %s", option);
    if(strlen(option)!=1){
			printf("invalid input, please try again\n");
		}
    else{
        if(option[0] == 'b' || option[0] == 'B'){//go back function
            if(step==0){
                printf("failed to backward\n");
            }
            else{
	            board[movement[step].from_s[0]][movement[step].from_s[1]] = movement[step].f_chess;
		        board[movement[step].to_s[0]][movement[step].to_s[1]] = movement[step].t_chess;
                step--;
	            printf("**********%d**********\n", step+1);
                chessboard();
            }
        }
        else if(option[0] == 'f' || option[0] == 'F'){//gamecheck function to update chessboard
            if(step == top){
                printf("failed to forward\n");
            }
            else{
                step++;
                board[movement[step].to_s[0]][movement[step].to_s[1]] = board[movement[step].from_s[0]][movement[step].from_s[1]];
                board[movement[step].from_s[0]][movement[step].from_s[1]] = ' ';
	            printf("**********%d**********\n", step+1);
                chessboard();
            }
        }
        else if(option[0] == 'q' || option[0] == 'Q'){
            check = false;
            break;
        }
    }
    }
}
int writefile(){//binary file write in
    char filename[50];
    printf("name for saving file(binary file as '.dat'):");
    scanf("%s",filename);
    if(!(fp = fopen(filename,"wb"))){
        return 0;
    }
    else{
        fwrite(movement,sizeof(movement[MAXSTACK]),MAXSTACK,fp);
        fclose(fp);
        return 1;
    }
}
int readfile(){//read file
    char filename[50];
    printf("name for saving file(binary file as '.dat'):");
    scanf("%s",filename);
    if(!(fp = fopen(filename,"rb"))){
        return 0;
    }
    else{
        fread(movement,sizeof(movement[MAXSTACK]),MAXSTACK,fp);
        fclose(fp);
        return 1;
    }
}
int Choose_Option(){//main page in system
    check = true;
    gamereset = true;
    char mode[20];
    fflush(stdin);
    while(check){
        printf("enter 'n' for new game, 'r' for shogi manual, 's' for saving the previous game, amd 'q' to quit:");
        scanf(" %s", mode);
        if(strlen(mode)!=1){
			printf("invalid input, please try again\n");
		}
        else{
            if(mode[0] == 'n' || mode[0] == 'N'){//reset chessboard and start time loop
            printf("Game starts!\n");
            while(gamereset){
                goback();
            }
            counter = 1;
            check = false;
            gamestart = true;
            }
            else if(mode[0] == 'r' || mode[0] == 'R'){//review game or call binary file
                printf("enter 'o' to open saved files(it will recover the previous game, suggesting saving the game first.) or 'p' to review previous game:");
                scanf(" %s", mode);
                if(strlen(mode)!=1){
			printf("invalid input, please try again\n");
		    }
            else{
                if(mode[0] == 'o' || mode[0] == 'O'){//call binary file
                    if(!readfile()){
                        printf("no file or wrong filename! cannot open the file!\n");
                    }
                    else{
                        replay();
                    }
                }
                else if(mode[0] == 'p' || mode[0] == 'P'){//run savings in struct
                        if(top!=-1){
                          replay();  
                    }
                else{
                        printf("no data!\n");
                    }
                }
            }
            }
            else if(mode[0] == 'q' || mode[0] == 'Q'){//system quit
            systemstart = false;
            check = false;
            }
            else if(mode[0] == 's' || mode[0] == 'S'){//save movement to binary file
                if(top == -1){
                    printf("no data can be restored!\n");
                }
                else{
                    if(!writefile()){
                       printf("wrong filename! cannot open the file!\n");
                    }
                    else{
                        printf("successfully restored!\n");
                    }
                }
            }
            else{
            printf("invalid input, please try again\n");
            }
        }
    }
}

int game_check(int start_row, int start_column, int end_row, int end_column){//shogi rule to check the step is right
    int unit1 = 0, unit2 = 0;
	check = false;
    //make sure the inputs are between 1 to 9
    if(start_row >= 1 && start_row <= 9 && start_column >= 1 && 
       start_column <= 9 && end_row >= 1 && end_row <= 9 && end_column >= 1 && end_column <= 9){
       start_row = 9 - start_row;
       start_column = 9 - start_column;
       end_row = 9 - end_row;
       end_column = 9 - end_column;
       //check the turn is right and the destination doesn't have the same group of chess
       if(counter%2 == 1 && isupper(board[start_row][start_column]) && !isupper(board[end_row][end_column])) 
		{
		
		}
	   else if(counter%2 == 0 && islower(board[start_row][start_column]) && !islower(board[end_row][end_column])) 
		{
		
		}
		else
		{
			printf("it's not your turn!! / wrong step!!\n");
			return 1;
		}
    }
    else
	{
		printf("invalid input!please try again!\n");
		return 1;
	}

    switch(board[start_row][start_column]){
    case'K': 
	case'k': /*step of king*/
	{
		if(abs(end_row - start_row) <= 1 && abs(end_column - start_column) <= 1)
		{
				
		}
		else
		{
			check = true;
			break;
		}
		break;
	}
    case'G': 
	case'g':/*step of gold*/
    {
       if(abs(end_row - start_row) <= 1 && abs(end_column - start_column) <= 1)
		{
			if(counter%2 == 1 && end_row - start_row < 0 && end_column - start_column != 0){
                check = true;
			    break;
            }	
            if(counter%2 == 0 && end_row - start_row > 0 && end_column - start_column != 0){
                check = true;
			    break;
            }	  
		} 
        else
		{
			check = true;
			break;
		}
		break;
    }
    case'S': 
	case's':/*step of silver*/
    {
        if(abs(end_row - start_row) == 1 && abs(end_row - start_row) == abs(end_column - start_column)){

        }
        else if(counter%2 == 1 && end_row - start_row == 1 && end_column - start_column == 0){

        }
        else if(counter%2 == 0 && end_row - start_row == -1 && end_column - start_column == 0){
            
        }
        else
		{
			check = true;
			break;
		}
		break;
    }
    case'H': 
	case'h':/*step of horse*/
    {
        if(counter%2 == 1 && end_row - start_row == 2 && abs(end_column - start_column) == 1){  

        }
        else if(counter%2 == 0 && end_row - start_row == -2 && abs(end_column - start_column) == 1){

        }
        else
		{
			check = true;
			break;
		}
		break;
    }
    case'C': 
	case'c':/*step of car*/
    {
        if(end_column - start_column == 0){
            if(counter%2 == 1 && end_row - start_row >= 1){
                unit1 = (end_row - start_row) / abs(end_row - start_row);
                for (int i = 1; i< abs((end_row - start_row)); i++) 
			{
				if (isalpha(board[start_row+((unit1)*i)][start_column])) 
				{
					check = true;
					break;
				}
            }
        }
            else if(counter%2 == 0 && end_row - start_row <= -1){
                unit1 = (end_row - start_row) / abs(end_row - start_row);
                for (int i = 1; i< abs((end_row - start_row)); i++) 
			{
				if (isalpha(board[start_row+((unit1)*i)][start_column])) 
				{
					check = true;
					break;
				}
            }
        }
            else{
			check = true;
			break;
        }
    }
        else
		{
			check = true;
			break;
		}
        break;
    }
    case'A': 
	case'a':/*step of angle*/
    {
        if(abs(end_row - start_row) == abs(end_column - start_column))
		{
			unit1 = (end_row - start_row)/abs(end_row - start_row);
			unit2 = (end_column - start_column)/abs(end_column - start_column);
			for(int i = 1;i < abs(end_column - start_column);i++)
			{
				if(isalpha(board[start_row + (unit1*i)][start_column + (unit2*i)]))
				{
					check = true;
					break;
				}
			}
		}
		else
		{
			check = true;
			break;
		}
		break;
    }
    case'F': 
	case'f':/*step of flying car*/
    {
        if((end_row - start_row) == 0 || (end_column - start_column) == 0) {
            if(end_row - start_row == 0){
				unit2 = (end_column - start_column)/abs(end_column - start_column);
            }
			else{
				unit1 = (end_row - start_row)/abs(end_row - start_row);
            }
            for(int i = 1;i < (abs(end_row - start_row) + abs(end_column - start_column));i++)
			{
				if(isalpha(board[start_row + (unit1*i)][start_column + (unit2*i)]))
				{
					check = true;
					break;
				}
			}
        }
        else
		{
			check = true;
			break;
		}
		break;
    }
    case'P':
    case'p':/*step of ping*/
    {
        if(end_column - start_column == 0){
            if(counter%2 == 1 && end_row - start_row == 1){

            }
            else if(counter%2 == 0 && end_row - start_row == -1){

            }
            else
		{
			check = true;
			break;
		}
        }
        else
		{
			check = true;
			break;
		}
        break;
    }
    default:
		check = true;
		printf("Failed!");
}
if(check)
	{										
		printf("wrong step!!\n");
		return 1;
	}
else if(board[end_row][end_column] == 'k' || board[end_row][end_column] == 'K') /*condition to end game*/
	{	
        push(start_row, start_column, end_row, end_column);	
        board[end_row][end_column] = board[start_row][start_column];	
		board[start_row][start_column] = ' ';
        chessboard();
		printf("%s wins !!\nend game\n", counter%2? "blue":"red");
        gamestart = false;
	}
	else
	{
		push(start_row, start_column, end_row, end_column);
		board[end_row][end_column] = board[start_row][start_column];	
		board[start_row][start_column] = ' ';
        counter++;
        printf("action finishes! now it's %s's turn \n", counter%2? "blue":"red");
		return 0;
	}
}
int input(){//feed input
    int start_row, start_column, end_row, end_column;
    scanf("%d",&start_row);
    if (start_row == 0){
        if(goback())
			{
				printf("can't go back\n");
                return 1;
			}
			else
			{
				counter--;
                printf("action finishes! now it's %s's turn \n", counter%2? "blue":"red");
                return 1;

			}
    }
    printf("enter the origin of column(筋, x-axis):");
    scanf("%d",&start_column);
    printf("enter the destination of row(段, y-axis):");
    scanf("%d",&end_row);
    printf("enter the destination of column(筋, x-axis):");
    scanf("%d",&end_column);
    game_check(start_row, start_column, end_row, end_column);
}
static void stdin_cb (EV_P_ ev_io *w, int revents){//loop i/o switch
    int total = (int)ev_now(loop)-now;
    if(input()){
        total = (int)ev_now(loop)-now;
        chessboard();
        if(counter%2 == 1){
            clk1 = total - clk2;
            printf("the total time of the game is %d sec, and the total time the %s spend is %d sec.\n", total, counter%2? "blue":"red",clk1);
        }
        else{
            clk2 = total - clk1;
            printf("the total time of the game is %d sec, and the total time the %s spend is %d sec.\n", total, counter%2? "blue":"red",clk2);
        }
    }
    else{
        total = (int)ev_now(loop)-now;
        chessboard();
        if(counter%2 == 0){
            clk1 = total - clk2;
            printf("the total time of the game is %d sec, and the total time the %s spend is %d sec.\n", total, (counter-1)%2? "blue":"red",clk1);
        }
        else{
            clk2 = total - clk1;
            printf("the total time of the game is %d sec, and the total time the %s spend is %d sec.\n", total, (counter-1)%2? "blue":"red",clk2);
        }
    }
    if(!gamestart){
        clk1 = clk2 = 0;
        ev_io_stop (EV_A_ w);
        ev_timer_stop (loop, &time_watcher);
        ev_break(EV_A_ EVBREAK_ALL);
    }
    //else
}
static void timeout_cb (EV_P_ ev_timer *w, int revents){//time loop
    while(timereset){
        now = ev_now(loop);
        timereset = false;
    }
    int total = (int)ev_now(loop)-now;
	if(counter%2 == 1){
            clk1 = total - clk2;
            if(total%5 == 0){
            chessboard();
            printf("%s's turn\n", counter%2?"blue":"red");
            printf("the total time of the game is %d sec, and the total time the blue spend is %d sec.\n",total,clk1);
            printf("enter the origin of row(段, y-axis) or enter 0 to go back to last step:\n");
        }
    }
    else{
            clk2 = total - clk1;
            if(total%5 == 0){
            chessboard();
            printf("%s's turn\n", counter%2?"blue":"red");
            printf("the total time of the game is %d sec, and the total time the red spend is %d sec.\n",total,clk2);
            printf("enter the origin of row(段, y-axis) or enter 0 to go back to last step:\n");
        }

    }
}

int main(){//system loop
    record_Board();
    while(systemstart){
       Choose_Option();
       while(gamestart){
       timereset = true;
       struct ev_loop *loop = EV_DEFAULT;
       ev_timer_set (&time_watcher,0,1);
	   now = ev_now(loop);
       ev_io_init (&stdin_watcher, stdin_cb,0, EV_READ);
	   ev_io_start (loop, &stdin_watcher);
       ev_timer_init (&time_watcher, timeout_cb, 0, 1);
       ev_timer_start (loop, &time_watcher);
       ev_run (loop, 0);
       }
    }
    return 0;
}