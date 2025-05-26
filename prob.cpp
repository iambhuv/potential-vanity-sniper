#include <iostream>

/*

*           *
* *       * *
*   *   *   *
*     *     *
*   *   *   *
* *       * *
*           *

*/
int main() {
  int layers = 3;

  

  for (int i = layers; i >= -layers; i--)
  {
    bool fill = abs(i) == layers;
    for (int j = 0; j <= layers-abs(i); j++)
    {
      std::cout << " ";
    }
    for (int j = 0; j <= abs(i); j++)
    {
      std::cout << (((!j || !(j-abs(i))) || fill) ? "* " : "  ");
    }
    std::cout << std::endl;
  }
}