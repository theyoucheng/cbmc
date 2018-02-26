int BLOCK=0;
int main () {
int inp1, inp2, inp3; int least = inp1;
int most = inp1;

int x;
if (x == 0)
x += 1; // irrelevant - should be sliced out!

if (most < inp2)
most = inp2; // C1

if (most < inp3)
most = inp3; // C2

if (least > inp2)
most = inp2; // C3 (fault#1)

if (least > inp3)
least = inp3; // C4

assert(least<=most); //

}
