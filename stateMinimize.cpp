#include "lib/StateMinimization.h"

using namespace std;

int main(int argc, char *argv[])
{
    StateMinimization S(argv[1], argv[2], argv[3]);
    //S.KissToDot(argv[1]);
    S.begin();
}
