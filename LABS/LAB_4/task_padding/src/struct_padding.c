#include <stdio.h>
    
struct test {
    char a; // 0{0 0 0} <- padding
    int b;  // 0 0 0 0
    int c;  // 0 0 0 0
};

struct test_2 {
    char a_1; // 0 <- packed
    int b_1;  // 0 0 0 0
    int c_1;  // 0 0 0 0
    char d_1; // 0

} __attribute__((packed));

int main (void)
{
    char str[12] = {'A', 0, 0, 0,'B', 0, 0, 0, 0, 0, 0, 0};
    
    const struct test *ptr_1 = (struct test *) str;
    const struct test_2 *ptr_2 = (struct test_2 *) str;
    
    printf("size of unpacked struct test is %zu\n", sizeof(struct test));

    printf("size of packed struct test is %zu\n\n", sizeof(struct test_2));

    // {|'A', 0, 0, 0,| 'B', 0, 0, 0, 0, 0, 0, 0 } - only 'A' is visible.
    printf("pointer of unpacked struct test points to %c\n", ptr_1->a);
    // {|'A', 0, 0, 0,| 0, 'B', 0, 0, 0, 0 } - only 'B' is visible.
    printf("pointer of unpacked struct test points to %d\n", ptr_1->b);
    // {'A', 0, 0, 0, |0, 'B', 0, 0,| 0, 0 } - some random garbage value 16896
    printf("pointer of unpacked struct test points to %d\n\n", ptr_1->c);

     // {'A', 0, 0, 0, 0, 'B', 0, 0,| 0, 0 ...}- 2 zeroes and random numbers = garbage value.

    printf("pointer of packed struct test_2 points to %c\n", ptr_2->a_1);

    // {|'A'|, 0, 0, 0, 0, 'B', 0, 0, 0, 0 } - only 'A' is visible. No padding.

    printf("pointer of packed struct test_2 points to %d\n", ptr_2->b_1);

     // {'A',| 0, 0, 0, 0|, 'B', 0, 0, 0, 0 } - b_1 takes up all 4 zeroes. No padding.

    printf("pointer of packed struct test_2 points to %c\n", ptr_2->c_1);

    // {'A',0, 0, 0, 0,| 'B', 0, 0, 0,| 0 } - c_1 takes 'B' and 3 zeroes. No padding. 'B' is shown.

     printf("pointer of packed struct test_2 points to %d\n", ptr_2->d_1);

    // {'A',0, 0, 0, 0,'B', 0, 0, 0,| 0 |} - d_1 takes 1 zeroe. No padding.

    return 0;
}
