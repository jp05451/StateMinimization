#include "lib/StateMinimization.h"

int main(int argc,char *argv[])
{
    StateMinimization S(argv[1], argv[2]);
    S.KissToDot(argv[1]);
    return 0;
}