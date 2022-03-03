#include <stdio.h>
#ifndef XMAX
#define XMAX 20
#define YMAX 40
#endif
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

struct point
{
    int x;
    int y;
};
    
struct rect
{
    struct point new_pt_1;
    struct point new_pt_2;
};


struct point make_point (int x, int y)
{
    struct point temp;
    
    temp.x = x;
    temp.y = y;
        
    return temp;
}

struct point add_point (struct point p1, struct point p2)
{
    p1.x += p2.x;
    p1.y += p2.y;

    return p1;
}

int p_in_rect(struct point, struct rect);

struct rect canon_rect (struct rect);

int main(void)
{
    struct rect screen;
    struct rect canonized_rect;
    struct point middle;
    struct point added_point;
    struct point make_point (int, int);
    struct point p;

    screen.new_pt_1 = make_point(50,60);

    printf ("the lower angle is at (%d, %d)\n", screen.new_pt_1.x, screen.new_pt_1.y);

    screen.new_pt_2 = make_point(XMAX, YMAX);

    printf ("the upper angle is at (%d, %d)\n", screen.new_pt_2.x, screen.new_pt_2.y);

    middle = make_point(((screen.new_pt_1.x + screen.new_pt_2.x) / 2), ((screen.new_pt_1.y + screen.new_pt_2.y) / 2));

    printf ("middle: %d %d\n", middle.x, middle.y);

    added_point = add_point(screen.new_pt_1, screen.new_pt_2);

    printf ("added point is : x is at %d y is at %d\n", added_point.x, added_point.y);

    p = make_point(10, 10);

    printf ("point is at %d\n", p_in_rect(p, screen));

    /*-------------------------------------------------*/

    canonized_rect = canon_rect(screen);

    printf ("canonized rectancle: x is at (%d, %d) - y is at (%d, %d)\n", canonized_rect.new_pt_1.x, canonized_rect.new_pt_1.y, canonized_rect.new_pt_2.x, canonized_rect.new_pt_2.y);

    //-------------------------------------------------

    struct point origin = {100, 200};
    struct point *ptr = &origin;

    printf ("origin is %d - %d\n", ptr-> x, ptr-> y);

    printf ("origin is %d - %d (with old pointers) \n", (*ptr).x, (*ptr).y);

    struct rect rectangle = {make_point(20, 30), make_point(50, 60)};
    struct rect *rectangle_ptr = &rectangle;

    printf ("p 'x' of rectangle is at (%d,%d)\n", rectangle_ptr-> new_pt_1.x, rectangle_ptr-> new_pt_1.y);
    printf ("p 'y' of rectangle is at (%d,%d)\n", rectangle_ptr-> new_pt_2.x, rectangle_ptr-> new_pt_2.y);

    return 0;
}


int p_in_rect(struct point p, struct rect r)
{
    return p.x >= r.new_pt_1.x && p.x < r.new_pt_2.x && p.y >= r.new_pt_1.y && p.y < r.new_pt_2.y;
};

struct rect canon_rect (struct rect r)
{
    struct rect temp;

    temp.new_pt_1.x = min(r.new_pt_1.x, r.new_pt_2.x);
    temp.new_pt_1.y = min(r.new_pt_1.y, r.new_pt_2.y);
    temp.new_pt_2.x = max(r.new_pt_1.x, r.new_pt_2.x);
    temp.new_pt_2.y = max(r.new_pt_1.y, r.new_pt_2.y);

    return temp;
}