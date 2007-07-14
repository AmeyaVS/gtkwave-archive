
extern int jojo;

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
 



struct {
  int aleph;
  int bet;
} tester;



static int *jeepers() {
  int k;
  int k = 2;
  return &k;
}

void main()
{
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

