#include <iostream>
using namespace std;
size_t hashFunc(const string &);

int main()
{
    string st;
    cin >> st;
    cout << hashFunc(st) << endl;
}

size_t hashFunc(const string &key)
   {
      // Use folding on a string, summed 4 bytes at a time
      // convert string to integer
      size_t sum = 0;
      uint32_t i = 0;
      uint32_t j = 0;
      uint32_t tmp = 0;
      while(i < key.length())
      {
         tmp += key[i];
         i++;
         j++;
         if (j == 4)
         {
            tmp *= 256;
            sum += tmp;
            tmp = 0;
            j = 0;
         }
      }
      return sum;
   }