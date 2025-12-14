#ifndef SECURITY
#define SECURITY
#include<string>
#include <cstdlib>  
#include <ctime> 
using namespace std;
class security
{
   string salt;
   string password;  
   int count;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
   string hash(string pass)
   {  count=0;
      string newPass="";
      int i=0,j=0;
      for(;i<pass.size()&&j<16;)
      {  char x=pass[i];
         int a=x*x+i*x+j;
         a=a%127;
         char y=salt[j];
         int b=y*y+j*y+i;
         b=b%127;
         if(b>a) {newPass+=char(b); j++;}
         else {newPass+=char(a);i++;}
         
      }
      while(i<pass.size())
      {
       char x=pass[i];
         int a=x*x+i*x+j;
         a=a%127;
         newPass+=char(a);  
         i++;
      }
       while(j<16)
      {
       char x=salt[j];
         int a=x*x+j*x+i;
         a=a%127;
         newPass+=char(a);  
         j++;
      }
      return newPass;
   }
   string create_salt()
   {
      string s="";
      for(int i=0;i<16;i++)
      {
       srand(time(nullptr)+i);
       int random_num =rand() % 127;
       s+=char(random_num);
      }
      return s;
   }
   public:
   security(string pass)
   {
    salt=create_salt();
    password=hash(pass);
   }
  
   bool check(string s)
   {
      if(password==hash(s)) {return true;}
      else return false;
   }

};
#endif