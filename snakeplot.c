/*
    DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
                Version 0.1b, April 2013

Copyright (C) 2013 Henrique Miranda <miranda.henrique@gmail.com> 

Everyone is permitted to copy and distribute verbatim or modified 
copies of this license document, and changing it is allowed as long 
as the name is changed. 

        DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include "gnuplotcha.h"

#define NLINKS 1
#define TABLE(x,y) t->data[ t->nrows*y + x] 
/*
structures
*/

struct elo;

typedef struct {
    int x;
    int y;
} coor;

struct elo {
    coor c;
    struct elo *n; /* next link */
};

typedef struct {
    FILE *gp;
    struct elo *e; /*elo 0*/
    coor fruit;
    size_t s; /*size of the snake*/
    int *data;
    int ncols;
    int nrows;
} table;

typedef struct elo elo;

/*
functions
*/

coor set_coords( int x, int y )
{
    coor c;
    c.x = x;
    c.y = y;
    return c;
}

inline int com_coords( coor a, coor b )
{
    return ( a.x == b.x ) && ( a.y == b.y );
}

void put_fruit( table *t )
{
    coor c;

    do {
        c = set_coords(rand()%t->nrows, rand()%t->ncols);
    }
    while ( TABLE( c.x, c.y ) == 1 );

    t->fruit = c;
    TABLE(t->fruit.x, t->fruit.y) = 1; //draw fruit
}

table* init_table( int nrows, int ncols )
{
    table *t;

    t = malloc(sizeof(table));
    t->e    = malloc(sizeof(elo)); //link 1
    t->e->n = malloc(sizeof(elo)); //link 2
    t->e->n->n = t->e;
    t->s = 2;

    //set size of the window
    t->nrows = nrows;
    t->ncols = ncols;

    //set initial coords
    t->e->c = set_coords( nrows/2, ncols/2 );
    
    //malloc table
    t->data = malloc(sizeof(int)*nrows*ncols);
    size_t i;
    for ( i = 0; i < nrows*ncols; i++ )
    {
        t->data[i] = 0;
    }
    //memset (t->data,0,sizeof(int)*nrows*ncols);

    //init gnuplotcha
    t->gp = gnuplotcha_open();
    gnuplotcha_setrange(t->gp,'x',0,ncols);
    gnuplotcha_setrange(t->gp,'y',0,nrows);

    put_fruit( t );
    
    return t;
}

void add_link( table *t )
/** add a link to the snake*/
{
    elo *e;

    e = t->e->n;
    t->e->n = malloc( sizeof(elo));
    t->e->n->n = e;
    t->s++;
 }

void eat( table *t, coor c )
{
    size_t n;
    //if fruit and snake coincide eat
    if ( com_coords(t->e->c, t->fruit) )
    {
        for ( n = 0; n < NLINKS; n++ )
        {
            add_link( t );
        }
        put_fruit( t );
    }
}

int move_snake( table *t, char c )
{
    coor p;
    coor del;

    p = t->e->c;

    //decide direction
    switch(c)
    {
        case 'w': p.y++; break;
        case 'a': p.x--; break;
        case 's': p.y--; break;
        case 'd': p.x++; break;
        return 1;
    }

    //infinite screen
    p.x = (p.x + t->nrows) % t->nrows;
    p.y = (p.y + t->ncols) % t->ncols;
 
    //check colisions
    if ( TABLE( p.x, p.y) == -1 )
    {
        endwin(); exit(0);
    }
    
    eat( t, p );
    t->e = t->e->n; //move to next link
    del = t->e->c; //get coords from tail
    TABLE( del.x, del.y ) = 0; //remove tail
    t->e->c = p; //move tail to head
    TABLE( p.x, p.y ) = -1; //draw head

    return 0;
}

void update( table *t )
{
    FILE *gp = t->gp;
    const size_t N = t->ncols;
    const size_t M = t->nrows;
    gnuplotcha_plotadd(gp, M, N, "%d", GPCHA_Image | GPCHA_PlotLimits(0,0), "array=(%zu,%zu)", M, N );
    gnuplotcha_senddata1d(gp, t->data, N*M, sizeof(int));
}

int main()
{
    char c_new, c_old;
    table *t; /*table of the game*/
    int nrows = 30, ncols = 30;
    WINDOW *w;

    /*init window*/ 
    w = initscr(); //init screen
    noecho();      //dont print keystrokes
    cbreak();      //dont need to press enter

    /*init table*/
    t = init_table( nrows, ncols );
    refresh();
    wtimeout(w,100);
    c_old = c_new = 'w';

    while ( 1 )
    {
        c_new = wgetch( w ); //get new move

        if ( ((c_new == 'w') && (c_old == 's')) ||
             ((c_new == 's') && (c_old == 'w')) ||
             ((c_new == 'a') && (c_old == 'd')) ||
             ((c_new == 'd') && (c_old == 'a')) ||
             ( c_new == ERR ) ) //invalid move
        {
            c_new = c_old; //use old move
        }
        
        switch(c_new)
        {
            case 'w': case 's': case 'a': case 'd':
                c_old = c_new; //update move
                break;
            case 'q':
                endwin(); exit(0); //quit game
            case 'p':
                while ( getchar() != 'p' ); //pause game
                continue;
            default:
                c_new = c_old; //use old move
        }

        move_snake( t, c_new );
        update( t );
    }
    
    return 0;
}
