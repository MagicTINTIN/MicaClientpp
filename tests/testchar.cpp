#include <string>
#include <iostream>

int main(int argc, char const *argv[])
{
    //unsigned char c = 139;
    //std::cout << "Un caractere en plein milieu d'un m" << c << "ot" << std::endl;
    for (size_t i = 127; i < 160; i++)
    {
        unsigned char c = i;
        std::cout << "Un caractere en plein milieu d'un m" << c << "ot" << std::endl;
    }

    return 0;
}
