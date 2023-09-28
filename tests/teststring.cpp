#include <iostream>
#include <string>

int main(int argc, char const *argv[])
{
    std::string str1 = "Test";
    std::string str2 = str1;
    str2 = "Changed";
    std::cout << str1 << " " << str2 << std::endl;
    return 0;
}
