#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 9876
#define QUEUE_SIZE 6

struct board
{
    char f[3][8][8];
    int M[2];
    bool S, A;
    int p, q;
};

void new_game(struct board *x){

int i, j, p;
x->S=false;
x->A=true;
x->p=12;
x->q=12;
for(i=0; i<8 ;i++){
    p=i%2;
    for(j=0;j<8;j++)
    {
        if(i<3)
        {

            if(p==0){
                x->f[0][i][j]='0';
                p=1;
            }
            else{
                x->f[0][i][j]='p';
                p=0;
            }
        }
        else if(i>4){
            if(p==0){
                x->f[0][i][j]='0';
                p=1;
            }
            else{
                x->f[0][i][j]='q';
                p=0;
            }
        }
        else x->f[0][i][j]='0';
        x->f[1][i][j]='0';
        x->f[2][i][j]='0';
    }
}
}

void clear(char f[8][8]){
    int i, j;
    for(i=0; i<8 ;i++){
        for(j=0;j<8;j++){
            f[i][j]='0';
        }
    }

}

void check_move(struct board *b, int x, int y, char p){

    clear(b->f[1]);
    if(b->f[0][x][y]==p){
        if(y>0){
            if((p=='p')&&(b->f[0][x+1][y-1]=='0')) b->f[1][x+1][y-1]='m';
            if((p=='q')&&(b->f[0][x-1][y-1]=='0')) b->f[1][x-1][y-1]='m';
        }
        if(y<7){
            if((p=='p')&&(b->f[0][x+1][y+1]=='0')) b->f[1][x+1][y+1]='m';
            if((p=='q')&&(b->f[0][x-1][y+1]=='0')) b->f[1][x-1][y+1]='m';
        }
        b->M[0]=x;
        b->M[1]=y;
    }
    if(b->f[0][x][y]==p-32){
        int i, j;
        i=x-1;
        j=y+1;
        while((i>=0)&&(j<=7)&&(b->f[0][i][j]=='0')){
            b->f[1][i][j]='m';
            i--;
            j++;
        }
        i=x+1;
        j=y+1;
        while((i<=7)&&(j<=7)&&(b->f[0][i][j]=='0')){
            b->f[1][i][j]='m';
            i++;
            j++;
        }
        i=x+1;
        j=y-1;
        while((i<=7)&&(j>=0)&&(b->f[0][i][j]=='0')){
            b->f[1][i][j]='m';
            i++;
            j--;
        }
        i=x-1;
        j=y-1;
        while((i>=0)&&(j>=0)&&(b->f[0][i][j]=='0')){
            b->f[1][i][j]='m';
            i--;
            j--;
        }
        b->M[0]=x;
        b->M[1]=y;
    }

}

void move(struct board *b, int x, int y){//check
    char z;
    if(b->f[1][x][y]=='m'){
        z=b->f[0][b->M[0]][b->M[1]];
        b->f[0][b->M[0]][b->M[1]]='0';
        if((x==0)&&(z=='q')) z='Q';
        if((x==7)&&(z=='p')) z='P';
        b->f[0][x][y]=z;
        //clear_move(b);
        clear(b->f[1]);
        b->A=false;
    }
}

int check_attack(struct board *b, int x, int y, char p, char q){
    int c=0;
    int n, m, z;
    clear(b->f[1]);
    if(b->f[0][x][y]==p){
        if(x>1){
            if(y>1){
                if((b->f[0][x-2][y-2]=='0')&&((b->f[0][x-1][y-1]==q)||(b->f[0][x-1][y-1]==q-32))){
                    b->f[1][x-2][y-2]='a';
                    c=1;
                }
            }
            if(y<6){
                if((b->f[0][x-2][y+2]=='0')&&((b->f[0][x-1][y+1]==q)||(b->f[0][x-1][y+1]==q-32))){
                    b->f[1][x-2][y+2]='a';
                    c=1;
                }
            }
        }
        if(x<6){
            if(y>1){
                if((b->f[0][x+2][y-2]=='0')&&((b->f[0][x+1][y-1]==q)||(b->f[0][x+1][y-1]==q-32))){
                    b->f[1][x+2][y-2]='a';
                    c=1;
                }
            }
            if(y<6){
                if((b->f[0][x+2][y+2]=='0')&&((b->f[0][x+1][y+1]==q)||(b->f[0][x+1][y+1]==q-32))){
                    b->f[1][x+2][y+2]='a';
                    c=1;
                }
            }
        }
        b->M[0]=x;
        b->M[1]=y;
    }
    if(b->f[0][x][y]==p-32){
        n=x-1;
        m=y-1;
        z=0;
        while((n>0)&&(m>0)&&(z==0)){
            if(b->f[0][n][m]!='0'){
                z=1;
                if((b->f[0][n-1][m-1]=='0')&&((b->f[0][n][m]==q)||(b->f[0][n][m]==q-32))){
                    b->f[1][n-1][m-1]='a';
                    c=1;
                }
            }
            n--;
            m--;
        }
        n=x-1;
        m=y+1;
        z=0;
        while((n>0)&&(m<7)&&(z==0)){
            if(b->f[0][n][m]!='0'){
                z=1;
                if((b->f[0][n-1][m+1]=='0')&&((b->f[0][n][m]==q)||(b->f[0][n][m]==q-32))){
                    b->f[1][n-1][m+1]='a';
                    c=1;
                }
            }
            n--;
            m++;
        }
        n=x+1;
        m=y-1;
        z=0;
        while((n<7)&&(m>0)&&(z==0)){
            if(b->f[0][n][m]!='0'){
                z=1;
                if((b->f[0][n+1][m-1]=='0')&&((b->f[0][n][m]==q)||(b->f[0][n][m]==q-32))){
                    b->f[1][n+1][m-1]='a';
                    c=1;
                }
            }
            n++;
            m--;
        }
        n=x+1;
        m=y+1;
        z=0;
        while((n<7)&&(m<7)&&(z==0)){
            if(b->f[0][n][m]!='0'){
                z=1;
                if((b->f[0][n+1][m+1]=='0')&&((b->f[0][n][m]==q)||(b->f[0][n][m]==q-32))){
                    b->f[1][n+1][m+1]='a';
                    c=1;
                }
            }
            n++;
            m++;
        }
        b->M[0]=x;
        b->M[1]=y;
    }
    return c;
}

void search_attack(struct board *b, char p, char q){//TO DO!!!
int i, j, z;
clear(b->f[2]);
    for(i=0;i<=7;i++){
        for(j=0;j<=7;j++){
            z=check_attack(b,i,j,p,q);
            if(z==1){
                b->f[2][i][j]='s';
                b->S=true;
            }
        }
    }
    clear(b->f[1]);
}

void attack(struct board *b, int x, int y, char p, char q){
    char z;
    int n, m, c;
    if((b->f[1][x][y]=='a')&&(b->f[2][b->M[0]][b->M[1]]=='s')){
        z=b->f[0][b->M[0]][b->M[1]];
        n=b->M[0]-x;
        m=b->M[1]-y;
        if(n>0) n=1;
        else n=-1;
        if(m>0) m=1;
        else m=-1;
        b->f[0][b->M[0]][b->M[1]]='0';
        b->f[0][x+n][y+m]='0';
        if((x==0)&&(z=='q')) z='Q';
        if((x==7)&&(z=='p')) z='P';
        b->f[0][x][y]=z;
        clear(b->f[1]);
        clear(b->f[2]);
        if(q=='q') b->q=b->q-1;
        else b->p=b->p-1;
        c=check_attack(b,x,y,p,q);
        if(c==1){
            b->f[2][x][y]='s';
        }
        else{
            b->S=false;
            b->A=false;
        }
    }
}



struct thread_data_t
{
    int sd1, sd2;
};

int write_game(int sd, struct board *b, char p){
    int x, i, j, z;
    char buff[194];
    char R[1];
    x=0;
    for(z=0; z<3; z++){
        for(i=0; i<8; i++){
            for(j=0; j<8; j++){
                buff[x]=b->f[z][i][j];
                x++;
            }
        }
    }
    buff[x]=p;
    if(b->S==true) buff[x+1]='t';
    else buff[x+1]='f';
    write(sd, buff, sizeof(buff));
    x = read(sd, R, sizeof(R));
    return x;
}


void *ThreadBehavior(void *t_data)
{
    pthread_detach(pthread_self());
    struct thread_data_t *th_data = (struct thread_data_t*)t_data;
    struct board *b;
	bool match=true;
    char p1='q', p2='p', c;
    int x[2];
    char r[2];
    int G;
	int M, S;
    b = malloc (sizeof (struct board));
    new_game(b);
    //write b->f do p1 graj
    G=write_game(th_data->sd1, b, p1);
    if(G<1) {
		match=false;
		write_game(th_data->sd2, b, 'w');
	}
    //write b->f do p2 czekaj
    if(match)
    {
        G=write_game(th_data->sd2, b, p2);
        if(G<1) {
            match=false;
            write_game(th_data->sd1, b, 'w');
        }
        M=th_data->sd1;
        S=th_data->sd2;
    }
    while((b->p>0)&&(b->q>0)&&(match==true))
    {
        do
        {
			sleep(1);
            if(match){
                //write b->f do p1 graj
                G=write_game(M, b, p1);
                if(G<1) {
                    match=false;
                    write_game(S, b, 'w');
                }
            }
            if(match){
                //write b->f do p2 cekaj
                G=write_game(S, b, p1);
                if(G<1) {
                    match=false;
                    write_game(M, b, 'w');
                }
            }
            if(match){
                //read x od p1
                G=read(M, r, sizeof(r));
                if(G<1) {
                    match=false;
                    write_game(S, b, 'w');
                }
            }
            if(match){
                x[0]=(int)r[0]-48;
                x[1]=(int)r[1]-48;
                if(b->S){
                    attack(b, x[0], x[1], p1, p2);
                    if(b->A==true) check_attack(b, x[0], x[1], p1, p2);
                }
                else{
                    move(b, x[0], x[1]);
                    if(b->A==true) check_move(b, x[0], x[1], p1);
                }
            }
        }while((b->A)&&(match==true));
        if(match){
            if(M==th_data->sd1){
                M=th_data->sd2;
                S=th_data->sd1;
            }
            else{
                M=th_data->sd1;
                S=th_data->sd2;
            }
            b->A=true;
            c=p1;
            p1=p2;
            p2=c;
            search_attack(b, p1, p2);
        }
    }
    if(b->p==0){
            write_game(th_data->sd1, b, 'w');
            write_game(th_data->sd2, b, 'l');
    }
    if(b->q==0){
            write_game(th_data->sd1, b, 'l');
            write_game(th_data->sd2, b, 'w');
    }
    free(b);
    free(t_data);
    pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
    int server_sd;
    int connection_sd1;
    int connection_sd2;
    int create;
    char reuse_addr_val = 1;
    struct sockaddr_in server_address;

    memset(&server_address, 0, sizeof(struct sockaddr));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SERVER_PORT);

    server_sd=socket(AF_INET, SOCK_STREAM, 0);
    if (server_sd < 0){
       fprintf(stderr, "%s: blad gniazda..\n", argv[0]);
       exit(1);
    }

    setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

    create = bind(server_sd, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
    if (create < 0)
    {
        fprintf(stderr, "%s: Blad addresu i portu\n", argv[0]);
        exit(1);
    }

    create = listen(server_sd, QUEUE_SIZE);
    if (create < 0) {
    	fprintf(stderr, "%s: Blad kolejki\n", argv[0]);
        exit(1);
    }

	fprintf(stderr,"server rozpoczyna prace\n");	

    while(1){
        connection_sd1 = accept(server_sd, NULL, NULL);
        if (connection_sd1 < 0)
        {
            fprintf(stderr, "%s: Blad polaczenia 1\n", argv[0]);
            exit(1);
        }
		else fprintf(stderr, "gracz 1 gotowy\n");
        connection_sd2 = accept(server_sd, NULL, NULL);
        if (connection_sd2 < 0)
        {
            fprintf(stderr, "%s: Blad polaczenia 2\n", argv[0]);
            exit(1);
        }
		else fprintf(stderr, "gracz 2 gotowy\n");
        pthread_t thread1;
        struct thread_data_t *t_data;
        t_data = malloc (sizeof (struct thread_data_t));
        t_data->sd1=connection_sd1;
        t_data->sd2=connection_sd2;
        create=pthread_create(&thread1, NULL, ThreadBehavior, (void *)t_data);
        if (create){
            fprintf(stderr,"Blad watku: %d\n", create);
            exit(-1);
        }
        fprintf(stderr,"server rozpoczyna nowa gre\n");
    }
    close(server_sd);
    return 0;
}
