typedef static int Fp;

extern int jojo;

typedef int Iter_t;
typedef (*AE)();

const char * foo_ptr;
char * const foo_ptr2;

const int three = 3;

Iter_t *iters;
int (* funptr)(float f, int* q);

int c;

int j ;

int j = 3;

static int f;
static int f = 3;

static char foobar = 'g';

enum baz {
  a,b,c
};


enum baz barone;

struct bar {
  int c;
};

static struct bar q = {3},r,s;
 
typedef struct bar StructBar;
typedef StructBar *StructBarPtr;

StructBarPtr buzzsaw;


struct {
  int aleph;
  int bet;
} tester;



static int *jeepers(int test, int test2) {
  int k;
  int k = test;
  return &k;
}

void main()
{
  Iter_t french;
  static int p;
  static int p = 5;
  {
    int c = 4;
    static int f = 2;
    f+c;
  }
  jeepers();
  jojo = 3;
  return c + 2;
}

