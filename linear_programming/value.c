#include <stdio.h>
#include <stdlib.h>

typedef struct Fractional{
    long numerator;
    long denominator;
}Fractional;

// Uses gcd algorithm to set the lowest values to numerator and denominator
void gcd_shrink(Fractional *f){
    if(f->numerator == 0){
        f->denominator = 1;
        return;
    }

    long gcd = 1;
    long x = f->numerator;
    long y = f->denominator;
    if(x < 0){
        x = 0 - x;
    }
    if (x < y){
        long tmp = x;
        x = y;
        y = tmp;
    }

    // INVARIANT:
    // numerator + denominator == (x + y) * gcd
    while(y != 0){
        gcd = y;
        y = x % gcd;
        x = gcd;
    }
    f->numerator = f->numerator / gcd;
    f->denominator = f->denominator / gcd;
}

// Addition of two fractional values, returns a new Fractional with the result
Fractional fadd(const Fractional *a, const Fractional *b){
    long numerator = (a->numerator*b->denominator)+(b->numerator*a->denominator);
    long denominator = a->denominator*b->denominator;
    Fractional res = {numerator, denominator};
    gcd_shrink(&res);
    return res;
}

// Subtraction of two fractional values, returns a new Fractional with the result
Fractional fsub(const Fractional *a, const Fractional *b){
    long numerator = (a->numerator*b->denominator)-(b->numerator*a->denominator);
    long denominator = a->denominator*b->denominator;
    Fractional res = {numerator, denominator};
    gcd_shrink(&res);
    return res;
}

Fractional fmul(const Fractional *a, const Fractional *b){
    long numerator = a->numerator*b->numerator;
    long denominator = a->denominator*b->denominator;
    Fractional res = {numerator, denominator};
    gcd_shrink(&res);
    return res;
}

// a / b = res
Fractional fdiv(const Fractional *a, const Fractional *b){
    if(b->numerator == 0){
        fprintf(stderr, "Error: Division by zero.\n");
        exit(EXIT_FAILURE);
    }
    long numerator = a->numerator*b->denominator;
    long denominator = a->denominator*b->numerator;
    Fractional res = {numerator, denominator};
    gcd_shrink(&res);
    return res;
}

double get_double_value(const Fractional *f){
    return ((double)f->numerator/f->denominator);
}


/**
 * TESTS
*/

void test_fadd(){
    Fractional value_a = {1,2};
    Fractional value_b = {1,2};
    Fractional sum = fadd(&value_a,&value_b);
    printf("%ld/%ld\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

    value_a.numerator = 4;
    value_a.denominator = 1;
    value_b.numerator = 3;
    value_b.denominator = 2;
    sum = fadd(&value_a,&value_b);
    printf("%ld/%ld\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

    value_a.numerator = 4;
    value_a.denominator = 3;
    value_b.numerator = 5;
    value_b.denominator = 3;
    sum = fadd(&value_a,&value_b);
    printf("%ld/%ld\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

}

void test_fsub(){
    Fractional value_a = {1,2};
    Fractional value_b = {1,2};
    printf("Testing: (%ld/%ld) - (%ld/%ld)",value_a.numerator,value_a.denominator,value_b.numerator,value_b.denominator);
    Fractional sum = fsub(&value_a,&value_b);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

    value_a.numerator = 4;
    value_a.denominator = 1;
    value_b.numerator = 3;
    value_b.denominator = 2;
    printf("Testing: (%ld/%ld) - (%ld/%ld)",value_a.numerator,value_a.denominator,value_b.numerator,value_b.denominator);
    sum = fsub(&value_a,&value_b);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

    value_a.numerator = 4;
    value_a.denominator = 3;
    value_b.numerator = 5;
    value_b.denominator = 3;
    printf("Testing: (%ld/%ld) - (%ld/%ld)",value_a.numerator,value_a.denominator,value_b.numerator,value_b.denominator);
    sum = fsub(&value_a,&value_b);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));
}

void test_fmul(){
    Fractional value_a = {1,2};
    Fractional value_b = {1,2};
    printf("Testing: (%ld/%ld) * (%ld/%ld)",value_a.numerator,value_a.denominator,value_b.numerator,value_b.denominator);
    Fractional sum = fmul(&value_a,&value_b);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

    value_a.numerator = 4;
    value_a.denominator = 1;
    value_b.numerator = 3;
    value_b.denominator = 2;
    printf("Testing: (%ld/%ld) * (%ld/%ld)",value_a.numerator,value_a.denominator,value_b.numerator,value_b.denominator);
    sum = fmul(&value_a,&value_b);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

    value_a.numerator = 4;
    value_a.denominator = 3;
    value_b.numerator = 5;
    value_b.denominator = 3;
    printf("Testing: (%ld/%ld) * (%ld/%ld)",value_a.numerator,value_a.denominator,value_b.numerator,value_b.denominator);
    sum = fmul(&value_a,&value_b);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));
}
void test_fdiv(){
    Fractional value_a = {1,2};
    Fractional value_b = {1,2};
    printf("Testing: (%ld/%ld) / (%ld/%ld)",value_a.numerator,value_a.denominator,value_b.numerator,value_b.denominator);
    Fractional sum = fdiv(&value_a,&value_b);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

    value_a.numerator = 4;
    value_a.denominator = 1;
    value_b.numerator = 3;
    value_b.denominator = 2;
    printf("Testing: (%ld/%ld) / (%ld/%ld)",value_a.numerator,value_a.denominator,value_b.numerator,value_b.denominator);
    sum = fdiv(&value_a,&value_b);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

    value_a.numerator = 4;
    value_a.denominator = 3;
    value_b.numerator = 5;
    value_b.denominator = 3;
    printf("Testing: (%ld/%ld) / (%ld/%ld)",value_a.numerator,value_a.denominator,value_b.numerator,value_b.denominator);
    sum = fdiv(&value_a,&value_b);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));
}

void test_error(){
    Fractional value_a = {1,2};
    Fractional value_b = {1,2};
    Fractional sum = fadd(&value_a,&value_b);
    printf("%ld/%ld\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

    printf("Testing: (%ld/%ld) - (%ld/%ld)",sum.numerator,sum.denominator,value_b.numerator,value_b.denominator);
    sum = fsub(&sum,&value_b);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

    printf("Testing: (%ld/%ld) - (%ld/%ld)",sum.numerator,sum.denominator,value_b.numerator,value_b.denominator);
    sum = fsub(&sum,&value_b);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));

    printf("Testing: (%ld/%ld) / (%ld/%ld)",value_b.numerator,value_b.denominator,sum.numerator,sum.denominator);
    sum = fdiv(&value_b,&sum);
    printf(" = (%ld/%ld)\n",sum.numerator,sum.denominator);
    printf("Double: %.2f\n",get_double_value(&sum));
}

int main(int argc, char **argv){
    // test_fadd();
    // test_fsub();
    // test_fmul();
    // test_fdiv();
    // test_error();
}